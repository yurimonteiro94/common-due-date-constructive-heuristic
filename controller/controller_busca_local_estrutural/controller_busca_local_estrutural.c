#include "controller_busca_local_estrutural.h"

#include <stddef.h>
#include <stdlib.h>

static Boolean controllerBuscaLocalEstruturalParametrosSaoValidos(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalEstrutural *resultado) {
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

    if(resultado == NULL) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalEstruturalCopiarSolucao(const Solucao *origem,Solucao *destino) {
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

static Boolean controllerBuscaLocalEstruturalCopiarConteudo(const Solucao *origem,Solucao *destino) {
    QuantidadeDeTarefas posicao;

    if(origem == NULL || destino == NULL) {
        return FALSO;
    }

    if((*origem).sequenciaDeTarefas == NULL || (*destino).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    if((*origem).quantidadeDeTarefas != (*destino).quantidadeDeTarefas) {
        return FALSO;
    }

    for(posicao = 0;posicao < (*origem).quantidadeDeTarefas;posicao++) {
        (*destino).sequenciaDeTarefas[posicao] = (*origem).sequenciaDeTarefas[posicao];
    }

    (*destino).quantidadeDeTarefasAlocadas = (*origem).quantidadeDeTarefasAlocadas;

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalEstruturalMontarMapaDeTarefas(const Instancia *instancia,const Tarefa **tarefasPorIdentificador) {
    QuantidadeDeTarefas indiceDaTarefa;
    IdentificadorDeTarefa identificadorDaTarefa;

    if(instancia == NULL || tarefasPorIdentificador == NULL) {
        return FALSO;
    }

    for(indiceDaTarefa = 0;indiceDaTarefa <= (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        tarefasPorIdentificador[indiceDaTarefa] = NULL;
    }

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        identificadorDaTarefa = (*instancia).tarefas[indiceDaTarefa].identificador;

        if(identificadorDaTarefa == 0 || identificadorDaTarefa > (*instancia).quantidadeDeTarefas) {
            return FALSO;
        }

        tarefasPorIdentificador[identificadorDaTarefa] = &((*instancia).tarefas[indiceDaTarefa]);
    }

    for(identificadorDaTarefa = 1;identificadorDaTarefa <= (*instancia).quantidadeDeTarefas;identificadorDaTarefa++) {
        if(tarefasPorIdentificador[identificadorDaTarefa] == NULL) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalEstruturalAvaliarSolucaoRapida(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custo) {
    QuantidadeDeTarefas posicao;
    QuantidadeDeTarefas posicaoReversa;
    IdentificadorDeTarefa identificadorDaTarefa;
    const Tarefa *tarefa;
    InteiroPositivoDe32Bits somaDosTempos;
    InteiroPositivoDe32Bits instanteInicial;
    InteiroPositivoDe32Bits instanteAtual;
    InteiroPositivoDe32Bits adiantamento;
    InteiroPositivoDe32Bits atraso;
    long long inclinacao;
    Custo custoTotal;

    if(solucao == NULL || tarefasPorIdentificador == NULL || temposPrefixados == NULL || custo == NULL) {
        return FALSO;
    }

    if((*solucao).sequenciaDeTarefas == NULL || (*solucao).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    somaDosTempos = 0;
    inclinacao = 0;

    for(posicao = 0;posicao < (*solucao).quantidadeDeTarefas;posicao++) {
        identificadorDaTarefa = (*solucao).sequenciaDeTarefas[posicao];

        if(identificadorDaTarefa == 0 || identificadorDaTarefa > (*solucao).quantidadeDeTarefas) {
            return FALSO;
        }

        tarefa = tarefasPorIdentificador[identificadorDaTarefa];

        if(tarefa == NULL) {
            return FALSO;
        }

        somaDosTempos += (*tarefa).tempoProcessamento;
        temposPrefixados[posicao] = somaDosTempos;
        inclinacao -= (long long) (*tarefa).penalidadeAdiantamento;
    }

    instanteInicial = 0;

    for(posicaoReversa = (*solucao).quantidadeDeTarefas;posicaoReversa > 0;posicaoReversa--) {
        posicao = (QuantidadeDeTarefas) (posicaoReversa - 1);
        identificadorDaTarefa = (*solucao).sequenciaDeTarefas[posicao];
        tarefa = tarefasPorIdentificador[identificadorDaTarefa];
        inclinacao += (long long) (*tarefa).penalidadeAdiantamento + (long long) (*tarefa).penalidadeAtraso;

        if(inclinacao >= 0) {
            if(temposPrefixados[posicao] <= dataDeEntregaComum) {
                instanteInicial = dataDeEntregaComum - temposPrefixados[posicao];
            }

            break;
        }
    }

    instanteAtual = instanteInicial;
    custoTotal = 0;

    for(posicao = 0;posicao < (*solucao).quantidadeDeTarefas;posicao++) {
        identificadorDaTarefa = (*solucao).sequenciaDeTarefas[posicao];
        tarefa = tarefasPorIdentificador[identificadorDaTarefa];
        instanteAtual += (*tarefa).tempoProcessamento;

        if(instanteAtual < dataDeEntregaComum) {
            adiantamento = dataDeEntregaComum - instanteAtual;
            custoTotal += ((Custo) (*tarefa).penalidadeAdiantamento) * ((Custo) adiantamento);
        }
        else if(instanteAtual > dataDeEntregaComum) {
            atraso = instanteAtual - dataDeEntregaComum;
            custoTotal += ((Custo) (*tarefa).penalidadeAtraso) * ((Custo) atraso);
        }
    }

    (*custo) = custoTotal;

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalEstruturalMontarVizinhoPorBloco(const Solucao *solucaoCorrente,Solucao *solucaoCandidata,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas tamanhoDoBloco,QuantidadeDeTarefas posicaoDestinoReduzida) {
    QuantidadeDeTarefas quantidadeDeTarefas;
    QuantidadeDeTarefas quantidadeReduzida;
    QuantidadeDeTarefas posicaoFinal;
    QuantidadeDeTarefas posicaoOriginal;
    QuantidadeDeTarefas indiceDoBloco;

    if(solucaoCorrente == NULL || solucaoCandidata == NULL) {
        return FALSO;
    }

    if((*solucaoCorrente).sequenciaDeTarefas == NULL || (*solucaoCandidata).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    quantidadeDeTarefas = (*solucaoCorrente).quantidadeDeTarefas;

    if(tamanhoDoBloco == 0 || tamanhoDoBloco >= quantidadeDeTarefas) {
        return FALSO;
    }

    if(posicaoOrigem + tamanhoDoBloco > quantidadeDeTarefas) {
        return FALSO;
    }

    quantidadeReduzida = (QuantidadeDeTarefas) (quantidadeDeTarefas - tamanhoDoBloco);

    if(posicaoDestinoReduzida > quantidadeReduzida) {
        return FALSO;
    }

    posicaoOriginal = 0;
    indiceDoBloco = 0;

    for(posicaoFinal = 0;posicaoFinal < quantidadeDeTarefas;posicaoFinal++) {
        if(posicaoFinal >= posicaoDestinoReduzida && posicaoFinal < posicaoDestinoReduzida + tamanhoDoBloco) {
            (*solucaoCandidata).sequenciaDeTarefas[posicaoFinal] = (*solucaoCorrente).sequenciaDeTarefas[posicaoOrigem + indiceDoBloco];
            indiceDoBloco++;
        }
        else {
            while(posicaoOriginal >= posicaoOrigem && posicaoOriginal < posicaoOrigem + tamanhoDoBloco) {
                posicaoOriginal++;
            }

            if(posicaoOriginal >= quantidadeDeTarefas) {
                return FALSO;
            }

            (*solucaoCandidata).sequenciaDeTarefas[posicaoFinal] = (*solucaoCorrente).sequenciaDeTarefas[posicaoOriginal];
            posicaoOriginal++;
        }
    }

    (*solucaoCandidata).quantidadeDeTarefasAlocadas = quantidadeDeTarefas;

    return VERDADEIRO;
}

static QuantidadeDeTarefas controllerBuscaLocalEstruturalCalcularPrimeiroDestino(QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas raio) {
    if(posicaoOrigem > raio) {
        return (QuantidadeDeTarefas) (posicaoOrigem - raio);
    }

    return 0;
}

static QuantidadeDeTarefas controllerBuscaLocalEstruturalCalcularUltimoDestino(QuantidadeDeTarefas quantidadeReduzida,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas raio) {
    if(posicaoOrigem >= quantidadeReduzida) {
        if(raio >= posicaoOrigem - quantidadeReduzida) {
            return quantidadeReduzida;
        }

        return (QuantidadeDeTarefas) (posicaoOrigem + raio);
    }

    if(raio > quantidadeReduzida - posicaoOrigem) {
        return quantidadeReduzida;
    }

    return (QuantidadeDeTarefas) (posicaoOrigem + raio);
}

static Boolean controllerBuscaLocalEstruturalTentarPrimeiraMelhoriaPorBloco(Solucao *solucaoCorrente,Solucao *solucaoCandidata,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custoCorrente,QuantidadeDeTarefas tamanhoDoBloco,QuantidadeDeTarefas raio,InteiroPositivoDe32Bits *quantidadeDeVizinhos,InteiroPositivoDe32Bits *quantidadeDeMelhorias,Boolean *houveMelhoria) {
    QuantidadeDeTarefas quantidadeDeTarefas;
    QuantidadeDeTarefas quantidadeReduzida;
    QuantidadeDeTarefas posicaoOrigem;
    QuantidadeDeTarefas posicaoDestino;
    QuantidadeDeTarefas primeiroDestino;
    QuantidadeDeTarefas ultimoDestino;
    Custo custoCandidato;

    if(solucaoCorrente == NULL || solucaoCandidata == NULL || custoCorrente == NULL || houveMelhoria == NULL) {
        return FALSO;
    }

    quantidadeDeTarefas = (*solucaoCorrente).quantidadeDeTarefas;

    if(tamanhoDoBloco >= quantidadeDeTarefas) {
        (*houveMelhoria) = FALSO;

        return VERDADEIRO;
    }

    quantidadeReduzida = (QuantidadeDeTarefas) (quantidadeDeTarefas - tamanhoDoBloco);
    (*houveMelhoria) = FALSO;

    for(posicaoOrigem = 0;posicaoOrigem + tamanhoDoBloco <= quantidadeDeTarefas && (*houveMelhoria) == FALSO;posicaoOrigem++) {
        primeiroDestino = controllerBuscaLocalEstruturalCalcularPrimeiroDestino(posicaoOrigem,raio);
        ultimoDestino = controllerBuscaLocalEstruturalCalcularUltimoDestino(quantidadeReduzida,posicaoOrigem,raio);

        for(posicaoDestino = primeiroDestino;posicaoDestino <= ultimoDestino && (*houveMelhoria) == FALSO;posicaoDestino++) {
            if(posicaoDestino == posicaoOrigem) {
                continue;
            }

            if(controllerBuscaLocalEstruturalMontarVizinhoPorBloco(solucaoCorrente,solucaoCandidata,posicaoOrigem,tamanhoDoBloco,posicaoDestino) == FALSO) {
                return FALSO;
            }

            if(controllerBuscaLocalEstruturalAvaliarSolucaoRapida(solucaoCandidata,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                return FALSO;
            }

            (*quantidadeDeVizinhos)++;

            if(custoCandidato < (*custoCorrente)) {
                if(controllerBuscaLocalEstruturalCopiarConteudo(solucaoCandidata,solucaoCorrente) == FALSO) {
                    return FALSO;
                }

                (*custoCorrente) = custoCandidato;
                (*quantidadeDeMelhorias)++;
                (*houveMelhoria) = VERDADEIRO;
            }
        }
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalEstruturalExecutarBuscaComposta(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalEstrutural *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca) {
    ResultadoBuscaLocal resultadoBuscaComposta;

    resultadoBuscaComposta = criarResultadoBuscaLocalVazio();

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(instancia,fatorH,solucaoInicial,solucaoFinal,&resultadoBuscaComposta,raioDeReinsercao,raioDeTroca) == FALSO) {
        return FALSO;
    }

    (*resultado).quantidadeDeChamadasDaBuscaComposta++;
    (*resultado).quantidadeDeVizinhosDaBuscaComposta += resultadoBuscaComposta.quantidadeDeVizinhosAvaliados;

    if((*resultado).quantidadeDeChamadasDaBuscaComposta == 1) {
        (*resultado).custoInicial = resultadoBuscaComposta.custoInicial;
        (*resultado).custoAposPrimeiraBuscaComposta = resultadoBuscaComposta.custoFinal;
    }

    return VERDADEIRO;
}

ResultadoBuscaLocalEstrutural criarResultadoBuscaLocalEstruturalVazio(void) {
    ResultadoBuscaLocalEstrutural resultado;

    resultado.custoInicial = 0;
    resultado.custoAposPrimeiraBuscaComposta = 0;
    resultado.custoFinal = 0;
    resultado.quantidadeDeCiclosEstruturais = 0;
    resultado.quantidadeDeChamadasDaBuscaComposta = 0;
    resultado.quantidadeDeVizinhosDaBuscaComposta = 0;
    resultado.quantidadeDeVizinhosDeBlocoDois = 0;
    resultado.quantidadeDeVizinhosDeBlocoTres = 0;
    resultado.quantidadeDeMelhoriasDeBlocoDois = 0;
    resultado.quantidadeDeMelhoriasDeBlocoTres = 0;

    return resultado;
}

Boolean controllerBuscaLocalEstruturalMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalEstrutural *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca,QuantidadeDeTarefas raioDeBlocos) {
    Solucao solucaoCorrente;
    Solucao solucaoCandidata;
    Solucao solucaoOtimizada;
    const Tarefa **tarefasPorIdentificador;
    InteiroPositivoDe32Bits *temposPrefixados;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoCorrente;
    Boolean houveMelhoria;

    if(controllerBuscaLocalEstruturalParametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultado) == FALSO) {
        return FALSO;
    }

    if(raioDeReinsercao == 0 || raioDeTroca == 0 || raioDeBlocos == 0) {
        return FALSO;
    }

    (*resultado) = criarResultadoBuscaLocalEstruturalVazio();
    solucaoCorrente = criarSolucaoVazia();
    solucaoCandidata = criarSolucaoVazia();
    solucaoOtimizada = criarSolucaoVazia();
    tarefasPorIdentificador = NULL;
    temposPrefixados = NULL;

    if(controllerBuscaLocalEstruturalExecutarBuscaComposta(instancia,fatorH,solucaoInicial,&solucaoCorrente,resultado,raioDeReinsercao,raioDeTroca) == FALSO) {
        return FALSO;
    }

    if(inicializarSolucao(&solucaoCandidata,solucaoCorrente.quantidadeDeTarefas) == FALSO) {
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    tarefasPorIdentificador = (const Tarefa **) malloc(sizeof(Tarefa *) * ((size_t) solucaoCorrente.quantidadeDeTarefas + 1u));

    if(tarefasPorIdentificador == NULL) {
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    temposPrefixados = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * solucaoCorrente.quantidadeDeTarefas);

    if(temposPrefixados == NULL) {
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(controllerBuscaLocalEstruturalMontarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(controllerBuscaLocalEstruturalAvaliarSolucaoRapida(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    houveMelhoria = VERDADEIRO;

    while(houveMelhoria == VERDADEIRO) {
        houveMelhoria = FALSO;
        (*resultado).quantidadeDeCiclosEstruturais++;

        if(controllerBuscaLocalEstruturalTentarPrimeiraMelhoriaPorBloco(&solucaoCorrente,&solucaoCandidata,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,2,raioDeBlocos,&((*resultado).quantidadeDeVizinhosDeBlocoDois),&((*resultado).quantidadeDeMelhoriasDeBlocoDois),&houveMelhoria) == FALSO) {
            free(temposPrefixados);
            free(tarefasPorIdentificador);
            liberarSolucao(&solucaoCandidata);
            liberarSolucao(&solucaoCorrente);

            return FALSO;
        }

        if(houveMelhoria == FALSO) {
            if(controllerBuscaLocalEstruturalTentarPrimeiraMelhoriaPorBloco(&solucaoCorrente,&solucaoCandidata,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,3,raioDeBlocos,&((*resultado).quantidadeDeVizinhosDeBlocoTres),&((*resultado).quantidadeDeMelhoriasDeBlocoTres),&houveMelhoria) == FALSO) {
                free(temposPrefixados);
                free(tarefasPorIdentificador);
                liberarSolucao(&solucaoCandidata);
                liberarSolucao(&solucaoCorrente);

                return FALSO;
            }
        }

        if(houveMelhoria == VERDADEIRO) {
            solucaoOtimizada = criarSolucaoVazia();

            if(controllerBuscaLocalEstruturalExecutarBuscaComposta(instancia,fatorH,&solucaoCorrente,&solucaoOtimizada,resultado,raioDeReinsercao,raioDeTroca) == FALSO) {
                free(temposPrefixados);
                free(tarefasPorIdentificador);
                liberarSolucao(&solucaoCandidata);
                liberarSolucao(&solucaoCorrente);

                return FALSO;
            }

            liberarSolucao(&solucaoCorrente);
            solucaoCorrente = solucaoOtimizada;

            if(controllerBuscaLocalEstruturalAvaliarSolucaoRapida(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente) == FALSO) {
                free(temposPrefixados);
                free(tarefasPorIdentificador);
                liberarSolucao(&solucaoCandidata);
                liberarSolucao(&solucaoCorrente);

                return FALSO;
            }
        }
    }

    (*resultado).custoFinal = custoCorrente;

    if(controllerBuscaLocalEstruturalCopiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    free(temposPrefixados);
    free(tarefasPorIdentificador);
    liberarSolucao(&solucaoCandidata);
    liberarSolucao(&solucaoCorrente);

    return VERDADEIRO;
}