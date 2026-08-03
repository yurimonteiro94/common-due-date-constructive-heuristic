#include "controller_busca_local.h"

#include <stddef.h>
#include <stdlib.h>

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

    if(solucao == NULL) {
        return FALSO;
    }

    if((*solucao).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    if((*solucao).quantidadeDeTarefas == 0) {
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

static Boolean controllerBuscaLocalMontarMapaDeTarefas(const Instancia *instancia,const Tarefa **tarefasPorIdentificador) {
    QuantidadeDeTarefas indiceDaTarefa;
    IdentificadorDeTarefa identificadorDaTarefa;

    if(instancia == NULL) {
        return FALSO;
    }

    if(tarefasPorIdentificador == NULL) {
        return FALSO;
    }

    for(indiceDaTarefa = 0;indiceDaTarefa <= (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        tarefasPorIdentificador[indiceDaTarefa] = NULL;
    }

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        identificadorDaTarefa = (*instancia).tarefas[indiceDaTarefa].identificador;

        if(identificadorDaTarefa == 0) {
            return FALSO;
        }

        if(identificadorDaTarefa > (*instancia).quantidadeDeTarefas) {
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

static Boolean controllerBuscaLocalAvaliarSolucaoRapida(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custo) {
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

    if(solucao == NULL) {
        return FALSO;
    }

    if(tarefasPorIdentificador == NULL) {
        return FALSO;
    }

    if(temposPrefixados == NULL) {
        return FALSO;
    }

    if(custo == NULL) {
        return FALSO;
    }

    if((*solucao).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    if((*solucao).quantidadeDeTarefas == 0) {
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
            else {
                instanteInicial = 0;
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

static QuantidadeDeTarefas controllerBuscaLocalCalcularPrimeiraPosicaoDestino(QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas raioDeReinsercao) {
    if(posicaoOrigem > raioDeReinsercao) {
        return (QuantidadeDeTarefas) (posicaoOrigem - raioDeReinsercao);
    }

    return 0;
}

static QuantidadeDeTarefas controllerBuscaLocalCalcularUltimaPosicaoDestino(QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas raioDeReinsercao) {
    QuantidadeDeTarefas ultimaPosicao;
    QuantidadeDeTarefas ultimaPosicaoDoRaio;

    ultimaPosicao = (QuantidadeDeTarefas) (quantidadeDeTarefas - 1);

    if(raioDeReinsercao > ultimaPosicao - posicaoOrigem) {
        return ultimaPosicao;
    }

    ultimaPosicaoDoRaio = (QuantidadeDeTarefas) (posicaoOrigem + raioDeReinsercao);

    return ultimaPosicaoDoRaio;
}

ResultadoBuscaLocal criarResultadoBuscaLocalVazio(void) {
    ResultadoBuscaLocal resultadoBuscaLocal;

    resultadoBuscaLocal.custoInicial = 0;
    resultadoBuscaLocal.custoFinal = 0;
    resultadoBuscaLocal.quantidadeDeIteracoes = 0;
    resultadoBuscaLocal.quantidadeDeVizinhosAvaliados = 0;

    return resultadoBuscaLocal;
}

static Boolean controllerBuscaLocalExecutarReinsercao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao) {
    Solucao solucaoCorrente;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoCorrente;
    Custo custoCandidato;
    QuantidadeDeTarefas posicaoOrigem;
    QuantidadeDeTarefas posicaoDestino;
    QuantidadeDeTarefas primeiraPosicaoDestino;
    QuantidadeDeTarefas ultimaPosicaoDestino;
    QuantidadeDeTarefas raioEfetivo;
    Boolean houveMelhoria;
    const Tarefa **tarefasPorIdentificador;
    InteiroPositivoDe32Bits *temposPrefixados;

    if(controllerBuscaLocalParametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO) {
        return FALSO;
    }

    if(raioDeReinsercao == 0) {
        return FALSO;
    }

    (*resultadoBuscaLocal) = criarResultadoBuscaLocalVazio();
    solucaoCorrente = criarSolucaoVazia();
    tarefasPorIdentificador = NULL;
    temposPrefixados = NULL;

    if(controllerBuscaLocalCopiarSolucao(solucaoInicial,&solucaoCorrente) == FALSO) {
        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    tarefasPorIdentificador = (const Tarefa **) malloc(sizeof(Tarefa *) * ((size_t) solucaoCorrente.quantidadeDeTarefas + 1u));

    if(tarefasPorIdentificador == NULL) {
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    temposPrefixados = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * solucaoCorrente.quantidadeDeTarefas);

    if(temposPrefixados == NULL) {
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(controllerBuscaLocalMontarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(controllerBuscaLocalAvaliarSolucaoRapida(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(raioDeReinsercao >= solucaoCorrente.quantidadeDeTarefas) {
        raioEfetivo = (QuantidadeDeTarefas) (solucaoCorrente.quantidadeDeTarefas - 1);
    }
    else {
        raioEfetivo = raioDeReinsercao;
    }

    (*resultadoBuscaLocal).custoInicial = custoCorrente;
    houveMelhoria = VERDADEIRO;

    while(houveMelhoria == VERDADEIRO) {
        houveMelhoria = FALSO;
        (*resultadoBuscaLocal).quantidadeDeIteracoes++;

        for(posicaoOrigem = 0;posicaoOrigem < solucaoCorrente.quantidadeDeTarefas && houveMelhoria == FALSO;posicaoOrigem++) {
            primeiraPosicaoDestino = controllerBuscaLocalCalcularPrimeiraPosicaoDestino(posicaoOrigem,raioEfetivo);
            ultimaPosicaoDestino = controllerBuscaLocalCalcularUltimaPosicaoDestino(solucaoCorrente.quantidadeDeTarefas,posicaoOrigem,raioEfetivo);

            for(posicaoDestino = primeiraPosicaoDestino;posicaoDestino <= ultimaPosicaoDestino && houveMelhoria == FALSO;posicaoDestino++) {
                if(posicaoDestino != posicaoOrigem) {
                    if(controllerBuscaLocalReinserirTarefa(&solucaoCorrente,posicaoOrigem,posicaoDestino) == FALSO) {
                        free(temposPrefixados);
                        free(tarefasPorIdentificador);
                        liberarSolucao(&solucaoCorrente);

                        return FALSO;
                    }

                    if(controllerBuscaLocalAvaliarSolucaoRapida(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                        free(temposPrefixados);
                        free(tarefasPorIdentificador);
                        liberarSolucao(&solucaoCorrente);

                        return FALSO;
                    }

                    (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;

                    if(custoCandidato < custoCorrente) {
                        custoCorrente = custoCandidato;
                        houveMelhoria = VERDADEIRO;
                    }
                    else {
                        if(controllerBuscaLocalReinserirTarefa(&solucaoCorrente,posicaoDestino,posicaoOrigem) == FALSO) {
                            free(temposPrefixados);
                            free(tarefasPorIdentificador);
                            liberarSolucao(&solucaoCorrente);

                            return FALSO;
                        }
                    }
                }
            }
        }
    }

    (*resultadoBuscaLocal).custoFinal = custoCorrente;

    if(controllerBuscaLocalCopiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    free(temposPrefixados);
    free(tarefasPorIdentificador);
    liberarSolucao(&solucaoCorrente);

    return VERDADEIRO;
}

Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal) {
    if(solucaoInicial == NULL) {
        return FALSO;
    }

    if((*solucaoInicial).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    return controllerBuscaLocalExecutarReinsercao(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal,(QuantidadeDeTarefas) ((*solucaoInicial).quantidadeDeTarefas - 1));
}

Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercaoLimitada(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao) {
    return controllerBuscaLocalExecutarReinsercao(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal,raioDeReinsercao);
}