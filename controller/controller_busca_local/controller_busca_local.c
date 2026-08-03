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

    if(posicaoOrigem >= (*solucao).quantidadeDeTarefas || posicaoDestino >= (*solucao).quantidadeDeTarefas) {
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

static Boolean controllerBuscaLocalTrocarTarefas(Solucao *solucao,QuantidadeDeTarefas primeiraPosicao,QuantidadeDeTarefas segundaPosicao) {
    IdentificadorDeTarefa identificadorTemporario;

    if(solucao == NULL) {
        return FALSO;
    }

    if((*solucao).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    if(primeiraPosicao >= (*solucao).quantidadeDeTarefas || segundaPosicao >= (*solucao).quantidadeDeTarefas) {
        return FALSO;
    }

    if(primeiraPosicao == segundaPosicao) {
        return VERDADEIRO;
    }

    identificadorTemporario = (*solucao).sequenciaDeTarefas[primeiraPosicao];
    (*solucao).sequenciaDeTarefas[primeiraPosicao] = (*solucao).sequenciaDeTarefas[segundaPosicao];
    (*solucao).sequenciaDeTarefas[segundaPosicao] = identificadorTemporario;

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalMontarMapaDeTarefas(const Instancia *instancia,const Tarefa **tarefasPorIdentificador) {
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

static Boolean controllerBuscaLocalAvaliarSolucaoRapida(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custo,QuantidadeDeTarefas *quantidadeDeTarefasAdiantadas) {
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

    if(quantidadeDeTarefasAdiantadas != NULL) {
        (*quantidadeDeTarefasAdiantadas) = 0;
    }

    for(posicao = 0;posicao < (*solucao).quantidadeDeTarefas;posicao++) {
        identificadorDaTarefa = (*solucao).sequenciaDeTarefas[posicao];
        tarefa = tarefasPorIdentificador[identificadorDaTarefa];
        instanteAtual += (*tarefa).tempoProcessamento;

        if(instanteAtual < dataDeEntregaComum) {
            adiantamento = dataDeEntregaComum - instanteAtual;
            custoTotal += ((Custo) (*tarefa).penalidadeAdiantamento) * ((Custo) adiantamento);

            if(quantidadeDeTarefasAdiantadas != NULL) {
                (*quantidadeDeTarefasAdiantadas)++;
            }
        }
        else if(instanteAtual > dataDeEntregaComum) {
            atraso = instanteAtual - dataDeEntregaComum;
            custoTotal += ((Custo) (*tarefa).penalidadeAtraso) * ((Custo) atraso);
        }
        else if(quantidadeDeTarefasAdiantadas != NULL) {
            (*quantidadeDeTarefasAdiantadas)++;
        }
    }

    (*custo) = custoTotal;

    return VERDADEIRO;
}

static QuantidadeDeTarefas controllerBuscaLocalCalcularPrimeiraPosicaoDestino(QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas raio) {
    if(posicaoOrigem > raio) {
        return (QuantidadeDeTarefas) (posicaoOrigem - raio);
    }

    return 0;
}

static QuantidadeDeTarefas controllerBuscaLocalCalcularUltimaPosicaoDestino(QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas raio) {
    QuantidadeDeTarefas ultimaPosicao;

    ultimaPosicao = (QuantidadeDeTarefas) (quantidadeDeTarefas - 1);

    if(raio > ultimaPosicao - posicaoOrigem) {
        return ultimaPosicao;
    }

    return (QuantidadeDeTarefas) (posicaoOrigem + raio);
}

static Boolean controllerBuscaLocalMovimentoCruzaFronteira(QuantidadeDeTarefas primeiraPosicao,QuantidadeDeTarefas segundaPosicao,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas) {
    Boolean primeiraEhAdiantada;
    Boolean segundaEhAdiantada;

    primeiraEhAdiantada = primeiraPosicao < quantidadeDeTarefasAdiantadas ? VERDADEIRO : FALSO;
    segundaEhAdiantada = segundaPosicao < quantidadeDeTarefasAdiantadas ? VERDADEIRO : FALSO;

    return primeiraEhAdiantada != segundaEhAdiantada ? VERDADEIRO : FALSO;
}

ResultadoBuscaLocal criarResultadoBuscaLocalVazio(void) {
    ResultadoBuscaLocal resultadoBuscaLocal;

    resultadoBuscaLocal.custoInicial = 0;
    resultadoBuscaLocal.custoFinal = 0;
    resultadoBuscaLocal.quantidadeDeIteracoes = 0;
    resultadoBuscaLocal.quantidadeDeVizinhosAvaliados = 0;
    resultadoBuscaLocal.quantidadeDeVizinhosPorReinsercao = 0;
    resultadoBuscaLocal.quantidadeDeVizinhosPorTroca = 0;
    resultadoBuscaLocal.quantidadeDeMelhoriasPorReinsercao = 0;
    resultadoBuscaLocal.quantidadeDeMelhoriasPorTroca = 0;

    return resultadoBuscaLocal;
}

static Boolean controllerBuscaLocalTentarPrimeiraMelhoriaPorReinsercao(Solucao *solucaoCorrente,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custoCorrente,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raio,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas,Boolean somenteCruzandoFronteira,Boolean *houveMelhoria) {
    QuantidadeDeTarefas posicaoOrigem;
    QuantidadeDeTarefas posicaoDestino;
    QuantidadeDeTarefas primeiraPosicaoDestino;
    QuantidadeDeTarefas ultimaPosicaoDestino;
    Custo custoCandidato;

    (*houveMelhoria) = FALSO;

    for(posicaoOrigem = 0;posicaoOrigem < (*solucaoCorrente).quantidadeDeTarefas && (*houveMelhoria) == FALSO;posicaoOrigem++) {
        primeiraPosicaoDestino = controllerBuscaLocalCalcularPrimeiraPosicaoDestino(posicaoOrigem,raio);
        ultimaPosicaoDestino = controllerBuscaLocalCalcularUltimaPosicaoDestino((*solucaoCorrente).quantidadeDeTarefas,posicaoOrigem,raio);

        for(posicaoDestino = primeiraPosicaoDestino;posicaoDestino <= ultimaPosicaoDestino && (*houveMelhoria) == FALSO;posicaoDestino++) {
            if(posicaoDestino == posicaoOrigem) {
                continue;
            }

            if(somenteCruzandoFronteira == VERDADEIRO && controllerBuscaLocalMovimentoCruzaFronteira(posicaoOrigem,posicaoDestino,quantidadeDeTarefasAdiantadas) == FALSO) {
                continue;
            }

            if(controllerBuscaLocalReinserirTarefa(solucaoCorrente,posicaoOrigem,posicaoDestino) == FALSO) {
                return FALSO;
            }

            if(controllerBuscaLocalAvaliarSolucaoRapida(solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato,NULL) == FALSO) {
                return FALSO;
            }

            (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
            (*resultadoBuscaLocal).quantidadeDeVizinhosPorReinsercao++;

            if(custoCandidato < (*custoCorrente)) {
                (*custoCorrente) = custoCandidato;
                (*resultadoBuscaLocal).quantidadeDeMelhoriasPorReinsercao++;
                (*houveMelhoria) = VERDADEIRO;
            }
            else if(controllerBuscaLocalReinserirTarefa(solucaoCorrente,posicaoDestino,posicaoOrigem) == FALSO) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalTentarPrimeiraMelhoriaPorTroca(Solucao *solucaoCorrente,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custoCorrente,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raio,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas,Boolean somenteCruzandoFronteira,Boolean *houveMelhoria) {
    QuantidadeDeTarefas primeiraPosicao;
    QuantidadeDeTarefas segundaPosicao;
    QuantidadeDeTarefas ultimaPosicao;
    Custo custoCandidato;

    (*houveMelhoria) = FALSO;

    for(primeiraPosicao = 0;primeiraPosicao < (*solucaoCorrente).quantidadeDeTarefas && (*houveMelhoria) == FALSO;primeiraPosicao++) {
        ultimaPosicao = controllerBuscaLocalCalcularUltimaPosicaoDestino((*solucaoCorrente).quantidadeDeTarefas,primeiraPosicao,raio);

        for(segundaPosicao = (QuantidadeDeTarefas) (primeiraPosicao + 1);segundaPosicao <= ultimaPosicao && (*houveMelhoria) == FALSO;segundaPosicao++) {
            if(somenteCruzandoFronteira == VERDADEIRO && controllerBuscaLocalMovimentoCruzaFronteira(primeiraPosicao,segundaPosicao,quantidadeDeTarefasAdiantadas) == FALSO) {
                continue;
            }

            if(controllerBuscaLocalTrocarTarefas(solucaoCorrente,primeiraPosicao,segundaPosicao) == FALSO) {
                return FALSO;
            }

            if(controllerBuscaLocalAvaliarSolucaoRapida(solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato,NULL) == FALSO) {
                return FALSO;
            }

            (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
            (*resultadoBuscaLocal).quantidadeDeVizinhosPorTroca++;

            if(custoCandidato < (*custoCorrente)) {
                (*custoCorrente) = custoCandidato;
                (*resultadoBuscaLocal).quantidadeDeMelhoriasPorTroca++;
                (*houveMelhoria) = VERDADEIRO;
            }
            else if(controllerBuscaLocalTrocarTarefas(solucaoCorrente,primeiraPosicao,segundaPosicao) == FALSO) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalPrepararExecucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoCorrente,const Tarefa ***tarefasPorIdentificador,InteiroPositivoDe32Bits **temposPrefixados,DataDeEntregaComum *dataDeEntregaComum,Custo *custoCorrente) {
    if(controllerBuscaLocalCopiarSolucao(solucaoInicial,solucaoCorrente) == FALSO) {
        return FALSO;
    }

    (*dataDeEntregaComum) = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if((*dataDeEntregaComum) == 0) {
        liberarSolucao(solucaoCorrente);

        return FALSO;
    }

    (*tarefasPorIdentificador) = (const Tarefa **) malloc(sizeof(Tarefa *) * ((size_t) (*solucaoCorrente).quantidadeDeTarefas + 1u));

    if((*tarefasPorIdentificador) == NULL) {
        liberarSolucao(solucaoCorrente);

        return FALSO;
    }

    (*temposPrefixados) = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * (*solucaoCorrente).quantidadeDeTarefas);

    if((*temposPrefixados) == NULL) {
        free((void *) (*tarefasPorIdentificador));
        liberarSolucao(solucaoCorrente);

        return FALSO;
    }

    if(controllerBuscaLocalMontarMapaDeTarefas(instancia,*tarefasPorIdentificador) == FALSO) {
        free(*temposPrefixados);
        free((void *) (*tarefasPorIdentificador));
        liberarSolucao(solucaoCorrente);

        return FALSO;
    }

    if(controllerBuscaLocalAvaliarSolucaoRapida(solucaoCorrente,*tarefasPorIdentificador,*dataDeEntregaComum,*temposPrefixados,custoCorrente,NULL) == FALSO) {
        free(*temposPrefixados);
        free((void *) (*tarefasPorIdentificador));
        liberarSolucao(solucaoCorrente);

        return FALSO;
    }

    return VERDADEIRO;
}

static void controllerBuscaLocalLiberarExecucao(Solucao *solucaoCorrente,const Tarefa **tarefasPorIdentificador,InteiroPositivoDe32Bits *temposPrefixados) {
    free(temposPrefixados);
    free((void *) tarefasPorIdentificador);
    liberarSolucao(solucaoCorrente);
}

static QuantidadeDeTarefas controllerBuscaLocalCalcularRaioEfetivo(QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas raio) {
    if(raio >= quantidadeDeTarefas) {
        return (QuantidadeDeTarefas) (quantidadeDeTarefas - 1);
    }

    return raio;
}

static Boolean controllerBuscaLocalExecutarReinsercao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao) {
    Solucao solucaoCorrente;
    const Tarefa **tarefasPorIdentificador;
    InteiroPositivoDe32Bits *temposPrefixados;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoCorrente;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;
    QuantidadeDeTarefas raioEfetivo;
    Boolean houveMelhoria;

    if(controllerBuscaLocalParametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO || raioDeReinsercao == 0) {
        return FALSO;
    }

    (*resultadoBuscaLocal) = criarResultadoBuscaLocalVazio();
    solucaoCorrente = criarSolucaoVazia();

    if(controllerBuscaLocalPrepararExecucao(instancia,fatorH,solucaoInicial,&solucaoCorrente,&tarefasPorIdentificador,&temposPrefixados,&dataDeEntregaComum,&custoCorrente) == FALSO) {
        return FALSO;
    }

    raioEfetivo = controllerBuscaLocalCalcularRaioEfetivo(solucaoCorrente.quantidadeDeTarefas,raioDeReinsercao);
    (*resultadoBuscaLocal).custoInicial = custoCorrente;
    houveMelhoria = VERDADEIRO;

    while(houveMelhoria == VERDADEIRO) {
        (*resultadoBuscaLocal).quantidadeDeIteracoes++;

        if(controllerBuscaLocalAvaliarSolucaoRapida(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO) {
            controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

            return FALSO;
        }

        if(controllerBuscaLocalTentarPrimeiraMelhoriaPorReinsercao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,resultadoBuscaLocal,raioEfetivo,quantidadeDeTarefasAdiantadas,FALSO,&houveMelhoria) == FALSO) {
            controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

            return FALSO;
        }
    }

    (*resultadoBuscaLocal).custoFinal = custoCorrente;

    if(controllerBuscaLocalCopiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
        controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

        return FALSO;
    }

    controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

    return VERDADEIRO;
}

Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal) {
    if(solucaoInicial == NULL || (*solucaoInicial).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    return controllerBuscaLocalExecutarReinsercao(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal,(QuantidadeDeTarefas) ((*solucaoInicial).quantidadeDeTarefas - 1));
}

Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercaoLimitada(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao) {
    return controllerBuscaLocalExecutarReinsercao(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal,raioDeReinsercao);
}

Boolean controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca) {
    Solucao solucaoCorrente;
    const Tarefa **tarefasPorIdentificador;
    InteiroPositivoDe32Bits *temposPrefixados;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoCorrente;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;
    QuantidadeDeTarefas raioEfetivoDeReinsercao;
    QuantidadeDeTarefas raioEfetivoDeTroca;
    Boolean houveMelhoria;

    if(controllerBuscaLocalParametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO) {
        return FALSO;
    }

    if(raioDeReinsercao == 0 || raioDeTroca == 0) {
        return FALSO;
    }

    (*resultadoBuscaLocal) = criarResultadoBuscaLocalVazio();
    solucaoCorrente = criarSolucaoVazia();

    if(controllerBuscaLocalPrepararExecucao(instancia,fatorH,solucaoInicial,&solucaoCorrente,&tarefasPorIdentificador,&temposPrefixados,&dataDeEntregaComum,&custoCorrente) == FALSO) {
        return FALSO;
    }

    raioEfetivoDeReinsercao = controllerBuscaLocalCalcularRaioEfetivo(solucaoCorrente.quantidadeDeTarefas,raioDeReinsercao);
    raioEfetivoDeTroca = controllerBuscaLocalCalcularRaioEfetivo(solucaoCorrente.quantidadeDeTarefas,raioDeTroca);
    (*resultadoBuscaLocal).custoInicial = custoCorrente;
    houveMelhoria = VERDADEIRO;

    while(houveMelhoria == VERDADEIRO) {
        houveMelhoria = FALSO;
        (*resultadoBuscaLocal).quantidadeDeIteracoes++;

        if(controllerBuscaLocalAvaliarSolucaoRapida(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO) {
            controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

            return FALSO;
        }

        if(controllerBuscaLocalTentarPrimeiraMelhoriaPorReinsercao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,resultadoBuscaLocal,raioEfetivoDeReinsercao,quantidadeDeTarefasAdiantadas,VERDADEIRO,&houveMelhoria) == FALSO) {
            controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

            return FALSO;
        }

        if(houveMelhoria == VERDADEIRO) {
            continue;
        }

        if(controllerBuscaLocalTentarPrimeiraMelhoriaPorTroca(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,resultadoBuscaLocal,raioEfetivoDeTroca,quantidadeDeTarefasAdiantadas,VERDADEIRO,&houveMelhoria) == FALSO) {
            controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

            return FALSO;
        }

        if(houveMelhoria == VERDADEIRO) {
            continue;
        }

        if(controllerBuscaLocalTentarPrimeiraMelhoriaPorReinsercao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,resultadoBuscaLocal,raioEfetivoDeReinsercao,quantidadeDeTarefasAdiantadas,FALSO,&houveMelhoria) == FALSO) {
            controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

            return FALSO;
        }

        if(houveMelhoria == VERDADEIRO) {
            continue;
        }

        if(controllerBuscaLocalTentarPrimeiraMelhoriaPorTroca(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,resultadoBuscaLocal,raioEfetivoDeTroca,quantidadeDeTarefasAdiantadas,FALSO,&houveMelhoria) == FALSO) {
            controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

            return FALSO;
        }
    }

    (*resultadoBuscaLocal).custoFinal = custoCorrente;

    if(controllerBuscaLocalCopiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
        controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

        return FALSO;
    }

    controllerBuscaLocalLiberarExecucao(&solucaoCorrente,tarefasPorIdentificador,temposPrefixados);

    return VERDADEIRO;
}