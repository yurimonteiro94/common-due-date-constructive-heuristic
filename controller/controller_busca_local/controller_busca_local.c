#include "controller_busca_local.h"

#include "../../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stddef.h>

static Boolean controllerBuscaLocalParametrosSaoValidos(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal) {
    if(instanciaEhValida(instancia) == FALSO) {
        return FALSO;
    }

    if(fatorH != FATOR_H_02 && fatorH != FATOR_H_04 && fatorH != FATOR_H_06 && fatorH != FATOR_H_08) {
        return FALSO;
    }

    if(solucaoEhValida(solucaoInicial) == FALSO) {
        return FALSO;
    }

    if(solucaoFinal == NULL) {
        return FALSO;
    }

    if((*solucaoFinal).sequenciaDeTarefas != NULL) {
        return FALSO;
    }

    if(resultadoBuscaLocal == NULL) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalCopiarSolucao(const Solucao *origem,Solucao *destino) {
    QuantidadeDeTarefas posicao;

    if(solucaoEhValida(origem) == FALSO) {
        return FALSO;
    }

    if(destino == NULL) {
        return FALSO;
    }

    if((*destino).sequenciaDeTarefas != NULL) {
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

static Boolean controllerBuscaLocalReinserirTarefa(Solucao *solucao,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas posicaoDestino) {
    IdentificadorDeTarefa tarefaMovida;
    QuantidadeDeTarefas posicao;

    if(solucaoEhValida(solucao) == FALSO) {
        return FALSO;
    }

    if(posicaoOrigem >= (*solucao).quantidadeDeTarefas) {
        return FALSO;
    }

    if(posicaoDestino >= (*solucao).quantidadeDeTarefas) {
        return FALSO;
    }

    if(posicaoOrigem == posicaoDestino) {
        return VERDADEIRO;
    }

    tarefaMovida = (*solucao).sequenciaDeTarefas[posicaoOrigem];

    if(posicaoOrigem < posicaoDestino) {
        for(posicao = posicaoOrigem;posicao < posicaoDestino;posicao++) {
            (*solucao).sequenciaDeTarefas[posicao] = (*solucao).sequenciaDeTarefas[posicao + 1];
        }
    }
    else {
        for(posicao = posicaoOrigem;posicao > posicaoDestino;posicao--) {
            (*solucao).sequenciaDeTarefas[posicao] = (*solucao).sequenciaDeTarefas[posicao - 1];
        }
    }

    (*solucao).sequenciaDeTarefas[posicaoDestino] = tarefaMovida;

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalAvaliarSolucao(const Instancia *instancia,const Solucao *solucao,DataDeEntregaComum dataDeEntregaComum,Custo *custo) {
    if(custo == NULL) {
        return FALSO;
    }

    (*custo) = 0;

    if(gerenciadorDeCustosCalcularCustoDaSolucao(instancia,solucao,dataDeEntregaComum,custo) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

ResultadoBuscaLocal criarResultadoBuscaLocalVazio(void) {
    ResultadoBuscaLocal resultadoBuscaLocal;

    resultadoBuscaLocal.custoInicial = 0;
    resultadoBuscaLocal.custoFinal = 0;
    resultadoBuscaLocal.quantidadeDeIteracoes = 0;
    resultadoBuscaLocal.quantidadeDeVizinhosAvaliados = 0;

    return resultadoBuscaLocal;
}

Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal) {
    Solucao solucaoCorrente;
    Solucao solucaoCandidata;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoCorrente;
    Custo custoCandidato;
    QuantidadeDeTarefas posicaoOrigem;
    QuantidadeDeTarefas posicaoDestino;
    Boolean houveMelhoria;

    if(controllerBuscaLocalParametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO) {
        return FALSO;
    }

    (*resultadoBuscaLocal) = criarResultadoBuscaLocalVazio();

    solucaoCorrente = criarSolucaoVazia();

    if(controllerBuscaLocalCopiarSolucao(solucaoInicial,&solucaoCorrente) == FALSO) {
        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(controllerBuscaLocalAvaliarSolucao(instancia,&solucaoCorrente,dataDeEntregaComum,&custoCorrente) == FALSO) {
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    (*resultadoBuscaLocal).custoInicial = custoCorrente;

    houveMelhoria = VERDADEIRO;

    while(houveMelhoria == VERDADEIRO) {
        houveMelhoria = FALSO;
        (*resultadoBuscaLocal).quantidadeDeIteracoes++;

        for(posicaoOrigem = 0;posicaoOrigem < solucaoCorrente.quantidadeDeTarefas && houveMelhoria == FALSO;posicaoOrigem++) {
            for(posicaoDestino = 0;posicaoDestino < solucaoCorrente.quantidadeDeTarefas && houveMelhoria == FALSO;posicaoDestino++) {
                if(posicaoOrigem != posicaoDestino) {
                    solucaoCandidata = criarSolucaoVazia();

                    if(controllerBuscaLocalCopiarSolucao(&solucaoCorrente,&solucaoCandidata) == FALSO) {
                        liberarSolucao(&solucaoCorrente);

                        return FALSO;
                    }

                    if(controllerBuscaLocalReinserirTarefa(&solucaoCandidata,posicaoOrigem,posicaoDestino) == FALSO) {
                        liberarSolucao(&solucaoCandidata);
                        liberarSolucao(&solucaoCorrente);

                        return FALSO;
                    }

                    if(controllerBuscaLocalAvaliarSolucao(instancia,&solucaoCandidata,dataDeEntregaComum,&custoCandidato) == FALSO) {
                        liberarSolucao(&solucaoCandidata);
                        liberarSolucao(&solucaoCorrente);

                        return FALSO;
                    }

                    (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;

                    if(custoCandidato < custoCorrente) {
                        liberarSolucao(&solucaoCorrente);
                        solucaoCorrente = solucaoCandidata;
                        custoCorrente = custoCandidato;
                        houveMelhoria = VERDADEIRO;
                    }
                    else {
                        liberarSolucao(&solucaoCandidata);
                    }
                }
            }
        }
    }

    (*resultadoBuscaLocal).custoFinal = custoCorrente;

    if(controllerBuscaLocalCopiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    liberarSolucao(&solucaoCorrente);

    return VERDADEIRO;
}
