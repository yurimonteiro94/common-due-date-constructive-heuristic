#include "controller_busca_local_inversao.h"

#include <controller/controller_busca_local_hibrida/controller_busca_local_hibrida.h>
#include "../../model/entidades/resultado_busca_local_hibrida/resultado_busca_local_hibrida.h"

#include <stddef.h>
#include <stdlib.h>

static Boolean parametrosSaoValidos(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalInversao *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca,QuantidadeDeTarefas raioDeInversao) {
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

    if(raioDeReinsercao == 0 || raioDeTroca == 0 || raioDeInversao == 0) {
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

static Boolean avaliarSolucaoRapida(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custo) {
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

        inclinacao += (long long) (*tarefa).penalidadeAdiantamento;
        inclinacao += (long long) (*tarefa).penalidadeAtraso;

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

static Boolean inverterTrecho(Solucao *solucao,QuantidadeDeTarefas primeiraPosicao,QuantidadeDeTarefas ultimaPosicao) {
    IdentificadorDeTarefa identificadorTemporario;

    if(solucao == NULL || (*solucao).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    if(primeiraPosicao >= (*solucao).quantidadeDeTarefas || ultimaPosicao >= (*solucao).quantidadeDeTarefas) {
        return FALSO;
    }

    if(primeiraPosicao > ultimaPosicao) {
        return FALSO;
    }

    while(primeiraPosicao < ultimaPosicao) {
        identificadorTemporario = (*solucao).sequenciaDeTarefas[primeiraPosicao];
        (*solucao).sequenciaDeTarefas[primeiraPosicao] = (*solucao).sequenciaDeTarefas[ultimaPosicao];
        (*solucao).sequenciaDeTarefas[ultimaPosicao] = identificadorTemporario;

        primeiraPosicao++;
        ultimaPosicao--;
    }

    return VERDADEIRO;
}

static QuantidadeDeTarefas calcularUltimaPosicao(QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas primeiraPosicao,QuantidadeDeTarefas raioDeInversao) {
    QuantidadeDeTarefas ultimaPosicaoDaSolucao;

    ultimaPosicaoDaSolucao = (QuantidadeDeTarefas) (quantidadeDeTarefas - 1);

    if(raioDeInversao > ultimaPosicaoDaSolucao - primeiraPosicao) {
        return ultimaPosicaoDaSolucao;
    }

    return (QuantidadeDeTarefas) (primeiraPosicao + raioDeInversao);
}

static Boolean executarDescidaPorInversao(Solucao *solucaoCorrente,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custoCorrente,ResultadoBuscaLocalInversao *resultado,QuantidadeDeTarefas raioDeInversao,Boolean *houveMelhoriaNaDescida) {
    QuantidadeDeTarefas primeiraPosicao;
    QuantidadeDeTarefas ultimaPosicao;
    QuantidadeDeTarefas ultimaPosicaoPermitida;
    QuantidadeDeTarefas melhorPrimeiraPosicao;
    QuantidadeDeTarefas melhorUltimaPosicao;
    Custo custoCandidato;
    Custo melhorCusto;
    Boolean encontrouMelhoria;

    (*houveMelhoriaNaDescida) = FALSO;
    encontrouMelhoria = VERDADEIRO;

    while(encontrouMelhoria == VERDADEIRO) {
        encontrouMelhoria = FALSO;
        melhorCusto = (*custoCorrente);
        melhorPrimeiraPosicao = 0;
        melhorUltimaPosicao = 0;
        (*resultado).quantidadeDeIteracoesDeInversao++;

        for(primeiraPosicao = 0;primeiraPosicao + 1 < (*solucaoCorrente).quantidadeDeTarefas;primeiraPosicao++) {
            ultimaPosicaoPermitida = calcularUltimaPosicao((*solucaoCorrente).quantidadeDeTarefas,primeiraPosicao,raioDeInversao);

            for(ultimaPosicao = (QuantidadeDeTarefas) (primeiraPosicao + 1);ultimaPosicao <= ultimaPosicaoPermitida;ultimaPosicao++) {
                if(inverterTrecho(solucaoCorrente,primeiraPosicao,ultimaPosicao) == FALSO) {
                    return FALSO;
                }

                if(avaliarSolucaoRapida(solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                    return FALSO;
                }

                (*resultado).quantidadeDeVizinhosPorInversao++;

                if(custoCandidato < melhorCusto) {
                    melhorCusto = custoCandidato;
                    melhorPrimeiraPosicao = primeiraPosicao;
                    melhorUltimaPosicao = ultimaPosicao;
                    encontrouMelhoria = VERDADEIRO;
                }

                if(inverterTrecho(solucaoCorrente,primeiraPosicao,ultimaPosicao) == FALSO) {
                    return FALSO;
                }
            }
        }

        if(encontrouMelhoria == VERDADEIRO) {
            if(inverterTrecho(solucaoCorrente,melhorPrimeiraPosicao,melhorUltimaPosicao) == FALSO) {
                return FALSO;
            }

            (*custoCorrente) = melhorCusto;
            (*resultado).quantidadeDeInversoesAceitas++;
            (*houveMelhoriaNaDescida) = VERDADEIRO;
        }
    }

    return VERDADEIRO;
}

static void acumularResultadoDaBuscaComposta(ResultadoBuscaLocalInversao *resultado,const ResultadoBuscaLocal *resultadoDaBuscaComposta) {
    (*resultado).quantidadeDeChamadasDaBuscaComposta++;
    (*resultado).quantidadeDeVizinhosDaBuscaComposta += (*resultadoDaBuscaComposta).quantidadeDeVizinhosAvaliados;
    (*resultado).quantidadeDeMelhoriasPorReinsercao += (*resultadoDaBuscaComposta).quantidadeDeMelhoriasPorReinsercao;
    (*resultado).quantidadeDeMelhoriasPorTroca += (*resultadoDaBuscaComposta).quantidadeDeMelhoriasPorTroca;
}

Boolean controllerBuscaLocalInversaoMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalInversao *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca,QuantidadeDeTarefas raioDeInversao) {
    Solucao solucaoHibrida;
    Solucao solucaoCorrente;
    Solucao solucaoAposBuscaComposta;
    ResultadoBuscaLocalHibrida resultadoHibrido;
    ResultadoBuscaLocal resultadoDaBuscaComposta;
    const Tarefa **tarefasPorIdentificador;
    InteiroPositivoDe32Bits *temposPrefixados;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoAntesDoCiclo;
    Custo custoCorrente;
    Boolean houveMelhoriaPorInversao;
    Boolean continuar;

    if(parametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultado,raioDeReinsercao,raioDeTroca,raioDeInversao) == FALSO) {
        return FALSO;
    }

    (*resultado) = criarResultadoBuscaLocalInversaoVazio();
    solucaoHibrida = criarSolucaoVazia();
    solucaoCorrente = criarSolucaoVazia();
    solucaoAposBuscaComposta = criarSolucaoVazia();
    resultadoHibrido = criarResultadoBuscaLocalHibridaVazio();
    resultadoDaBuscaComposta = criarResultadoBuscaLocalVazio();
    tarefasPorIdentificador = NULL;
    temposPrefixados = NULL;

    if(controllerBuscaLocalHibridaMelhorarSolucao(instancia,fatorH,solucaoInicial,&solucaoHibrida,&resultadoHibrido,raioDeReinsercao,raioDeTroca) == FALSO) {
        return FALSO;
    }

    (*resultado).custoInicial = resultadoHibrido.custoInicial;
    (*resultado).custoAposBuscaHibrida = resultadoHibrido.custoFinal;

    if(copiarSolucao(&solucaoHibrida,&solucaoCorrente) == FALSO) {
        liberarSolucao(&solucaoHibrida);

        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        liberarSolucao(&solucaoCorrente);
        liberarSolucao(&solucaoHibrida);

        return FALSO;
    }

    tarefasPorIdentificador = (const Tarefa **) malloc(sizeof(Tarefa *) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));

    if(tarefasPorIdentificador == NULL) {
        liberarSolucao(&solucaoCorrente);
        liberarSolucao(&solucaoHibrida);

        return FALSO;
    }

    temposPrefixados = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * (*instancia).quantidadeDeTarefas);

    if(temposPrefixados == NULL) {
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);
        liberarSolucao(&solucaoHibrida);

        return FALSO;
    }

    if(montarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);
        liberarSolucao(&solucaoHibrida);

        return FALSO;
    }

    if(avaliarSolucaoRapida(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);
        liberarSolucao(&solucaoHibrida);

        return FALSO;
    }

    continuar = VERDADEIRO;

    while(continuar == VERDADEIRO) {
        continuar = FALSO;
        custoAntesDoCiclo = custoCorrente;
        (*resultado).quantidadeDeCiclos++;

        if(executarDescidaPorInversao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,resultado,raioDeInversao,&houveMelhoriaPorInversao) == FALSO) {
            free(temposPrefixados);
            free(tarefasPorIdentificador);
            liberarSolucao(&solucaoCorrente);
            liberarSolucao(&solucaoHibrida);

            return FALSO;
        }

        solucaoAposBuscaComposta = criarSolucaoVazia();
        resultadoDaBuscaComposta = criarResultadoBuscaLocalVazio();

        if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(instancia,fatorH,&solucaoCorrente,&solucaoAposBuscaComposta,&resultadoDaBuscaComposta,raioDeReinsercao,raioDeTroca) == FALSO) {
            free(temposPrefixados);
            free(tarefasPorIdentificador);
            liberarSolucao(&solucaoAposBuscaComposta);
            liberarSolucao(&solucaoCorrente);
            liberarSolucao(&solucaoHibrida);

            return FALSO;
        }

        acumularResultadoDaBuscaComposta(resultado,&resultadoDaBuscaComposta);

        if(resultadoDaBuscaComposta.custoFinal < custoCorrente) {
            liberarSolucao(&solucaoCorrente);
            solucaoCorrente = solucaoAposBuscaComposta;
            solucaoAposBuscaComposta = criarSolucaoVazia();
            custoCorrente = resultadoDaBuscaComposta.custoFinal;
        }

        liberarSolucao(&solucaoAposBuscaComposta);

        if(custoCorrente < custoAntesDoCiclo) {
            continuar = VERDADEIRO;
        }
    }

    (*resultado).custoFinal = custoCorrente;

    if(copiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);
        liberarSolucao(&solucaoHibrida);

        return FALSO;
    }

    free(temposPrefixados);
    free(tarefasPorIdentificador);
    liberarSolucao(&solucaoCorrente);
    liberarSolucao(&solucaoHibrida);

    return VERDADEIRO;
}