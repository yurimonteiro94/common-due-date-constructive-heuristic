#include "controller_busca_local_melhor_melhoria.h"

#include <stddef.h>
#include <stdlib.h>

#define TIPO_MOVIMENTO_NENHUM 0
#define TIPO_MOVIMENTO_REINSERCAO 1
#define TIPO_MOVIMENTO_TROCA 2

typedef struct MelhorMovimentoBuscaLocal {
    InteiroPositivoDe8Bits tipo;
    QuantidadeDeTarefas primeiraPosicao;
    QuantidadeDeTarefas segundaPosicao;
    Custo custo;
} MelhorMovimentoBuscaLocal;

static MelhorMovimentoBuscaLocal criarMelhorMovimentoBuscaLocalVazio(Custo custoAtual) {
    MelhorMovimentoBuscaLocal movimento;

    movimento.tipo = TIPO_MOVIMENTO_NENHUM;
    movimento.primeiraPosicao = 0;
    movimento.segundaPosicao = 0;
    movimento.custo = custoAtual;

    return movimento;
}

static Boolean parametrosSaoValidos(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal) {
    if(instanciaEhValida(instancia) == FALSO) {
        return FALSO;
    }

    if(fatorH != FATOR_H_02 && fatorH != FATOR_H_04 && fatorH != FATOR_H_06 && fatorH != FATOR_H_08) {
        return FALSO;
    }

    if(solucaoEhValida(solucaoInicial) == FALSO) {
        return FALSO;
    }

    if(solucaoFinal == NULL || resultadoBuscaLocal == NULL) {
        return FALSO;
    }

    if((*solucaoFinal).sequenciaDeTarefas != NULL) {
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

static Boolean reinserirTarefa(Solucao *solucao,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas posicaoDestino) {
    IdentificadorDeTarefa tarefaMovida;
    QuantidadeDeTarefas posicao;

    if(solucao == NULL || (*solucao).sequenciaDeTarefas == NULL) {
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

static Boolean trocarTarefas(Solucao *solucao,QuantidadeDeTarefas primeiraPosicao,QuantidadeDeTarefas segundaPosicao) {
    IdentificadorDeTarefa identificadorTemporario;

    if(solucao == NULL || (*solucao).sequenciaDeTarefas == NULL) {
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

static Boolean montarMapaDeTarefas(const Instancia *instancia,const Tarefa **tarefasPorIdentificador) {
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

static Boolean avaliarSolucao(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custo) {
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

static QuantidadeDeTarefas calcularRaioEfetivo(QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas raio) {
    if(raio >= quantidadeDeTarefas) {
        return (QuantidadeDeTarefas) (quantidadeDeTarefas - 1);
    }

    return raio;
}

static QuantidadeDeTarefas calcularPrimeiraPosicaoDestino(QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas raio) {
    if(posicaoOrigem > raio) {
        return (QuantidadeDeTarefas) (posicaoOrigem - raio);
    }

    return 0;
}

static QuantidadeDeTarefas calcularUltimaPosicaoDestino(QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas raio) {
    QuantidadeDeTarefas ultimaPosicao;

    ultimaPosicao = (QuantidadeDeTarefas) (quantidadeDeTarefas - 1);

    if(raio > ultimaPosicao - posicaoOrigem) {
        return ultimaPosicao;
    }

    return (QuantidadeDeTarefas) (posicaoOrigem + raio);
}

static Boolean avaliarReinsercoes(Solucao *solucaoCorrente,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,QuantidadeDeTarefas raio,MelhorMovimentoBuscaLocal *melhorMovimento,ResultadoBuscaLocal *resultadoBuscaLocal) {
    QuantidadeDeTarefas posicaoOrigem;
    QuantidadeDeTarefas posicaoDestino;
    QuantidadeDeTarefas primeiraPosicaoDestino;
    QuantidadeDeTarefas ultimaPosicaoDestino;
    Custo custoCandidato;

    for(posicaoOrigem = 0;posicaoOrigem < (*solucaoCorrente).quantidadeDeTarefas;posicaoOrigem++) {
        primeiraPosicaoDestino = calcularPrimeiraPosicaoDestino(posicaoOrigem,raio);
        ultimaPosicaoDestino = calcularUltimaPosicaoDestino((*solucaoCorrente).quantidadeDeTarefas,posicaoOrigem,raio);

        for(posicaoDestino = primeiraPosicaoDestino;posicaoDestino <= ultimaPosicaoDestino;posicaoDestino++) {
            if(posicaoDestino == posicaoOrigem) {
                continue;
            }

            if(reinserirTarefa(solucaoCorrente,posicaoOrigem,posicaoDestino) == FALSO) {
                return FALSO;
            }

            if(avaliarSolucao(solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                return FALSO;
            }

            (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
            (*resultadoBuscaLocal).quantidadeDeVizinhosPorReinsercao++;

            if(custoCandidato < (*melhorMovimento).custo) {
                (*melhorMovimento).tipo = TIPO_MOVIMENTO_REINSERCAO;
                (*melhorMovimento).primeiraPosicao = posicaoOrigem;
                (*melhorMovimento).segundaPosicao = posicaoDestino;
                (*melhorMovimento).custo = custoCandidato;
            }

            if(reinserirTarefa(solucaoCorrente,posicaoDestino,posicaoOrigem) == FALSO) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

static Boolean avaliarTrocas(Solucao *solucaoCorrente,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,QuantidadeDeTarefas raio,MelhorMovimentoBuscaLocal *melhorMovimento,ResultadoBuscaLocal *resultadoBuscaLocal) {
    QuantidadeDeTarefas primeiraPosicao;
    QuantidadeDeTarefas segundaPosicao;
    QuantidadeDeTarefas ultimaPosicao;
    Custo custoCandidato;

    for(primeiraPosicao = 0;primeiraPosicao < (*solucaoCorrente).quantidadeDeTarefas;primeiraPosicao++) {
        ultimaPosicao = calcularUltimaPosicaoDestino((*solucaoCorrente).quantidadeDeTarefas,primeiraPosicao,raio);

        for(segundaPosicao = (QuantidadeDeTarefas) (primeiraPosicao + 1);segundaPosicao <= ultimaPosicao;segundaPosicao++) {
            if(trocarTarefas(solucaoCorrente,primeiraPosicao,segundaPosicao) == FALSO) {
                return FALSO;
            }

            if(avaliarSolucao(solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                return FALSO;
            }

            (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
            (*resultadoBuscaLocal).quantidadeDeVizinhosPorTroca++;

            if(custoCandidato < (*melhorMovimento).custo) {
                (*melhorMovimento).tipo = TIPO_MOVIMENTO_TROCA;
                (*melhorMovimento).primeiraPosicao = primeiraPosicao;
                (*melhorMovimento).segundaPosicao = segundaPosicao;
                (*melhorMovimento).custo = custoCandidato;
            }

            if(trocarTarefas(solucaoCorrente,primeiraPosicao,segundaPosicao) == FALSO) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

static Boolean aplicarMelhorMovimento(Solucao *solucaoCorrente,const MelhorMovimentoBuscaLocal *melhorMovimento,ResultadoBuscaLocal *resultadoBuscaLocal) {
    if((*melhorMovimento).tipo == TIPO_MOVIMENTO_REINSERCAO) {
        if(reinserirTarefa(solucaoCorrente,(*melhorMovimento).primeiraPosicao,(*melhorMovimento).segundaPosicao) == FALSO) {
            return FALSO;
        }

        (*resultadoBuscaLocal).quantidadeDeMelhoriasPorReinsercao++;

        return VERDADEIRO;
    }

    if((*melhorMovimento).tipo == TIPO_MOVIMENTO_TROCA) {
        if(trocarTarefas(solucaoCorrente,(*melhorMovimento).primeiraPosicao,(*melhorMovimento).segundaPosicao) == FALSO) {
            return FALSO;
        }

        (*resultadoBuscaLocal).quantidadeDeMelhoriasPorTroca++;

        return VERDADEIRO;
    }

    return FALSO;
}

Boolean controllerBuscaLocalMelhorarSolucaoComMelhorMelhoria(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca) {
    Solucao solucaoCorrente;
    const Tarefa **tarefasPorIdentificador;
    InteiroPositivoDe32Bits *temposPrefixados;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoCorrente;
    QuantidadeDeTarefas raioEfetivoDeReinsercao;
    QuantidadeDeTarefas raioEfetivoDeTroca;
    MelhorMovimentoBuscaLocal melhorMovimento;
    Boolean houveMelhoria;

    if(parametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO) {
        return FALSO;
    }

    if(raioDeReinsercao == 0 || raioDeTroca == 0) {
        return FALSO;
    }

    (*resultadoBuscaLocal) = criarResultadoBuscaLocalVazio();
    solucaoCorrente = criarSolucaoVazia();
    tarefasPorIdentificador = NULL;
    temposPrefixados = NULL;

    if(copiarSolucao(solucaoInicial,&solucaoCorrente) == FALSO) {
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

    if(montarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(avaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    raioEfetivoDeReinsercao = calcularRaioEfetivo(solucaoCorrente.quantidadeDeTarefas,raioDeReinsercao);
    raioEfetivoDeTroca = calcularRaioEfetivo(solucaoCorrente.quantidadeDeTarefas,raioDeTroca);
    (*resultadoBuscaLocal).custoInicial = custoCorrente;
    houveMelhoria = VERDADEIRO;

    while(houveMelhoria == VERDADEIRO) {
        (*resultadoBuscaLocal).quantidadeDeIteracoes++;
        melhorMovimento = criarMelhorMovimentoBuscaLocalVazio(custoCorrente);

        if(avaliarReinsercoes(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,raioEfetivoDeReinsercao,&melhorMovimento,resultadoBuscaLocal) == FALSO) {
            free(temposPrefixados);
            free(tarefasPorIdentificador);
            liberarSolucao(&solucaoCorrente);

            return FALSO;
        }

        if(avaliarTrocas(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,raioEfetivoDeTroca,&melhorMovimento,resultadoBuscaLocal) == FALSO) {
            free(temposPrefixados);
            free(tarefasPorIdentificador);
            liberarSolucao(&solucaoCorrente);

            return FALSO;
        }

        houveMelhoria = melhorMovimento.tipo != TIPO_MOVIMENTO_NENHUM ? VERDADEIRO : FALSO;

        if(houveMelhoria == VERDADEIRO) {
            if(aplicarMelhorMovimento(&solucaoCorrente,&melhorMovimento,resultadoBuscaLocal) == FALSO) {
                free(temposPrefixados);
                free(tarefasPorIdentificador);
                liberarSolucao(&solucaoCorrente);

                return FALSO;
            }

            custoCorrente = melhorMovimento.custo;
        }
    }

    (*resultadoBuscaLocal).custoFinal = custoCorrente;

    if(copiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
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