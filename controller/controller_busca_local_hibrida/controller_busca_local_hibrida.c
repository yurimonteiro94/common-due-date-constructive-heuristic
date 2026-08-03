#include "controller_busca_local_hibrida.h"

#include "../controller_busca_local_melhor_melhoria/controller_busca_local_melhor_melhoria.h"

#include <stddef.h>

static Boolean parametrosSaoValidos(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalHibrida *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca) {
    if(instanciaEhValida(instancia) == FALSO) {
        return FALSO;
    }

    if(fatorH != FATOR_H_02 && fatorH != FATOR_H_04 && fatorH != FATOR_H_06 && fatorH != FATOR_H_08) {
        return FALSO;
    }

    if(solucaoEhValida(solucaoInicial) == FALSO) {
        return FALSO;
    }

    if(solucaoFinal == NULL || resultado == NULL) {
        return FALSO;
    }

    if((*solucaoFinal).sequenciaDeTarefas != NULL) {
        return FALSO;
    }

    if(raioDeReinsercao == 0 || raioDeTroca == 0) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean copiarSolucao(const Solucao *origem,Solucao *destino) {
    QuantidadeDeTarefas posicao;

    if(solucaoEhValida(origem) == FALSO) {
        return FALSO;
    }

    if(destino == NULL || (*destino).sequenciaDeTarefas != NULL) {
        return FALSO;
    }

    if(inicializarSolucao(destino,(*origem).quantidadeDeTarefas) == FALSO) {
        return FALSO;
    }

    for(posicao = 0;posicao < (*origem).quantidadeDeTarefas;posicao++) {
        if(solucaoDefinirTarefaNaPosicao(destino,posicao,(*origem).sequenciaDeTarefas[posicao]) == FALSO) {
            liberarSolucao(destino);

            return FALSO;
        }
    }

    return VERDADEIRO;
}

static void acumularResultado(ResultadoBuscaLocalHibrida *resultado,const ResultadoBuscaLocal *resultadoDaEtapa) {
    (*resultado).quantidadeDeChamadasDeBuscaLocal++;
    (*resultado).quantidadeTotalDeVizinhosAvaliados += resultadoDaEtapa->quantidadeDeVizinhosAvaliados;
    (*resultado).quantidadeTotalDeIteracoes += resultadoDaEtapa->quantidadeDeIteracoes;
    (*resultado).quantidadeTotalDeMelhoriasPorReinsercao += resultadoDaEtapa->quantidadeDeMelhoriasPorReinsercao;
    (*resultado).quantidadeTotalDeMelhoriasPorTroca += resultadoDaEtapa->quantidadeDeMelhoriasPorTroca;
}

static Boolean executarPrimeiraMelhoria(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalHibrida *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca,Custo *custoFinal) {
    ResultadoBuscaLocal resultadoDaEtapa;

    resultadoDaEtapa = criarResultadoBuscaLocalVazio();

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(instancia,fatorH,solucaoInicial,solucaoFinal,&resultadoDaEtapa,raioDeReinsercao,raioDeTroca) == FALSO) {
        return FALSO;
    }

    acumularResultado(resultado,&resultadoDaEtapa);
    (*custoFinal) = resultadoDaEtapa.custoFinal;

    return VERDADEIRO;
}

static Boolean executarMelhorMelhoria(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalHibrida *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca,Custo *custoFinal) {
    ResultadoBuscaLocal resultadoDaEtapa;

    resultadoDaEtapa = criarResultadoBuscaLocalVazio();

    if(controllerBuscaLocalMelhorarSolucaoComMelhorMelhoria(instancia,fatorH,solucaoInicial,solucaoFinal,&resultadoDaEtapa,raioDeReinsercao,raioDeTroca) == FALSO) {
        return FALSO;
    }

    acumularResultado(resultado,&resultadoDaEtapa);
    (*custoFinal) = resultadoDaEtapa.custoFinal;

    return VERDADEIRO;
}

static void liberarSolucoes(Solucao *solucaoPrimeiraIsolada,Solucao *solucaoMelhorIsolada,Solucao *solucaoAposMelhorDaTrajetoriaA,Solucao *solucaoFinalDaTrajetoriaA,Solucao *solucaoAposPrimeiraDaTrajetoriaB,Solucao *solucaoFinalDaTrajetoriaB) {
    liberarSolucao(solucaoPrimeiraIsolada);
    liberarSolucao(solucaoMelhorIsolada);
    liberarSolucao(solucaoAposMelhorDaTrajetoriaA);
    liberarSolucao(solucaoFinalDaTrajetoriaA);
    liberarSolucao(solucaoAposPrimeiraDaTrajetoriaB);
    liberarSolucao(solucaoFinalDaTrajetoriaB);
}

Boolean controllerBuscaLocalHibridaMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalHibrida *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca) {
    Solucao solucaoPrimeiraIsolada;
    Solucao solucaoMelhorIsolada;
    Solucao solucaoAposMelhorDaTrajetoriaA;
    Solucao solucaoFinalDaTrajetoriaA;
    Solucao solucaoAposPrimeiraDaTrajetoriaB;
    Solucao solucaoFinalDaTrajetoriaB;
    Custo custoTemporario;

    if(parametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultado,raioDeReinsercao,raioDeTroca) == FALSO) {
        return FALSO;
    }

    (*resultado) = criarResultadoBuscaLocalHibridaVazio();
    solucaoPrimeiraIsolada = criarSolucaoVazia();
    solucaoMelhorIsolada = criarSolucaoVazia();
    solucaoAposMelhorDaTrajetoriaA = criarSolucaoVazia();
    solucaoFinalDaTrajetoriaA = criarSolucaoVazia();
    solucaoAposPrimeiraDaTrajetoriaB = criarSolucaoVazia();
    solucaoFinalDaTrajetoriaB = criarSolucaoVazia();
    custoTemporario = 0;

    if(executarPrimeiraMelhoria(instancia,fatorH,solucaoInicial,&solucaoPrimeiraIsolada,resultado,raioDeReinsercao,raioDeTroca,&resultado->custoPrimeiraMelhoriaIsolada) == FALSO) {
        liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

        return FALSO;
    }

    resultado->custoInicial = resultado->custoPrimeiraMelhoriaIsolada;

    if(executarMelhorMelhoria(instancia,fatorH,solucaoInicial,&solucaoMelhorIsolada,resultado,raioDeReinsercao,raioDeTroca,&resultado->custoMelhorMelhoriaIsolada) == FALSO) {
        liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

        return FALSO;
    }

    if(executarMelhorMelhoria(instancia,fatorH,&solucaoPrimeiraIsolada,&solucaoAposMelhorDaTrajetoriaA,resultado,raioDeReinsercao,raioDeTroca,&custoTemporario) == FALSO) {
        liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

        return FALSO;
    }

    if(executarPrimeiraMelhoria(instancia,fatorH,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,resultado,raioDeReinsercao,raioDeTroca,&resultado->custoTrajetoriaPrimeiraMelhorPrimeira) == FALSO) {
        liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

        return FALSO;
    }

    if(executarPrimeiraMelhoria(instancia,fatorH,&solucaoMelhorIsolada,&solucaoAposPrimeiraDaTrajetoriaB,resultado,raioDeReinsercao,raioDeTroca,&custoTemporario) == FALSO) {
        liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

        return FALSO;
    }

    if(executarMelhorMelhoria(instancia,fatorH,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB,resultado,raioDeReinsercao,raioDeTroca,&resultado->custoTrajetoriaMelhorPrimeiraMelhor) == FALSO) {
        liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

        return FALSO;
    }

    if(resultado->custoTrajetoriaPrimeiraMelhorPrimeira <= resultado->custoTrajetoriaMelhorPrimeiraMelhor) {
        if(copiarSolucao(&solucaoFinalDaTrajetoriaA,solucaoFinal) == FALSO) {
            liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

            return FALSO;
        }

        resultado->custoFinal = resultado->custoTrajetoriaPrimeiraMelhorPrimeira;
        resultado->trajetoriaSelecionada = TRAJETORIA_HIBRIDA_PRIMEIRA_MELHOR_PRIMEIRA;
    }
    else {
        if(copiarSolucao(&solucaoFinalDaTrajetoriaB,solucaoFinal) == FALSO) {
            liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

            return FALSO;
        }

        resultado->custoFinal = resultado->custoTrajetoriaMelhorPrimeiraMelhor;
        resultado->trajetoriaSelecionada = TRAJETORIA_HIBRIDA_MELHOR_PRIMEIRA_MELHOR;
    }

    liberarSolucoes(&solucaoPrimeiraIsolada,&solucaoMelhorIsolada,&solucaoAposMelhorDaTrajetoriaA,&solucaoFinalDaTrajetoriaA,&solucaoAposPrimeiraDaTrajetoriaB,&solucaoFinalDaTrajetoriaB);

    return VERDADEIRO;
}