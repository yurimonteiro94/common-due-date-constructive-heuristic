#include "controller_busca_local_troca_k_particao.h"

#include <stddef.h>
#include <stdlib.h>

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

    if(solucaoEhValida(origem) == FALSO || destino == NULL || (*destino).sequenciaDeTarefas != NULL) {
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

static Boolean copiarSequencia(const Solucao *origem,Solucao *destino) {
    QuantidadeDeTarefas posicao;

    if(solucaoEhValida(origem) == FALSO || destino == NULL || (*destino).sequenciaDeTarefas == NULL || (*destino).quantidadeDeTarefas != (*origem).quantidadeDeTarefas) {
        return FALSO;
    }

    for(posicao = 0;posicao < (*origem).quantidadeDeTarefas;posicao++) {
        (*destino).sequenciaDeTarefas[posicao] = (*origem).sequenciaDeTarefas[posicao];
    }

    return VERDADEIRO;
}

static Boolean montarMapaDeTarefas(const Instancia *instancia,const Tarefa **tarefasPorIdentificador) {
    QuantidadeDeTarefas indice;
    IdentificadorDeTarefa identificador;

    if(instancia == NULL || tarefasPorIdentificador == NULL) {
        return FALSO;
    }

    for(indice = 0;indice <= (*instancia).quantidadeDeTarefas;indice++) {
        tarefasPorIdentificador[indice] = NULL;
    }

    for(indice = 0;indice < (*instancia).quantidadeDeTarefas;indice++) {
        identificador = (*instancia).tarefas[indice].identificador;

        if(identificador == 0 || identificador > (*instancia).quantidadeDeTarefas) {
            return FALSO;
        }

        tarefasPorIdentificador[identificador] = &((*instancia).tarefas[indice]);
    }

    for(identificador = 1;identificador <= (*instancia).quantidadeDeTarefas;identificador++) {
        if(tarefasPorIdentificador[identificador] == NULL) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

static Boolean tarefaDeveVirAntesNoLadoAdiantado(const Tarefa *primeiraTarefa,const Tarefa *segundaTarefa) {
    InteiroPositivoDe32Bits primeiroProduto;
    InteiroPositivoDe32Bits segundoProduto;

    primeiroProduto = ((InteiroPositivoDe32Bits) (*primeiraTarefa).tempoProcessamento) * ((InteiroPositivoDe32Bits) (*segundaTarefa).penalidadeAdiantamento);
    segundoProduto = ((InteiroPositivoDe32Bits) (*segundaTarefa).tempoProcessamento) * ((InteiroPositivoDe32Bits) (*primeiraTarefa).penalidadeAdiantamento);

    if(primeiroProduto > segundoProduto) {
        return VERDADEIRO;
    }

    if(primeiroProduto < segundoProduto) {
        return FALSO;
    }

    return (*primeiraTarefa).identificador < (*segundaTarefa).identificador ? VERDADEIRO : FALSO;
}

static Boolean tarefaDeveVirAntesNoLadoAtrasado(const Tarefa *primeiraTarefa,const Tarefa *segundaTarefa) {
    InteiroPositivoDe32Bits primeiroProduto;
    InteiroPositivoDe32Bits segundoProduto;

    primeiroProduto = ((InteiroPositivoDe32Bits) (*primeiraTarefa).tempoProcessamento) * ((InteiroPositivoDe32Bits) (*segundaTarefa).penalidadeAtraso);
    segundoProduto = ((InteiroPositivoDe32Bits) (*segundaTarefa).tempoProcessamento) * ((InteiroPositivoDe32Bits) (*primeiraTarefa).penalidadeAtraso);

    if(primeiroProduto < segundoProduto) {
        return VERDADEIRO;
    }

    if(primeiroProduto > segundoProduto) {
        return FALSO;
    }

    return (*primeiraTarefa).identificador < (*segundaTarefa).identificador ? VERDADEIRO : FALSO;
}

static void ordenarIdentificadores(IdentificadorDeTarefa *ordem,QuantidadeDeTarefas quantidade,const Tarefa **tarefasPorIdentificador,Boolean ladoAdiantado) {
    QuantidadeDeTarefas indice;
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificadorAtual;
    Boolean deveVirAntes;

    for(indice = 1;indice < quantidade;indice++) {
        identificadorAtual = ordem[indice];
        posicao = indice;

        while(posicao > 0) {
            if(ladoAdiantado == VERDADEIRO) {
                deveVirAntes = tarefaDeveVirAntesNoLadoAdiantado(tarefasPorIdentificador[identificadorAtual],tarefasPorIdentificador[ordem[posicao - 1]]);
            }
            else {
                deveVirAntes = tarefaDeveVirAntesNoLadoAtrasado(tarefasPorIdentificador[identificadorAtual],tarefasPorIdentificador[ordem[posicao - 1]]);
            }

            if(deveVirAntes == FALSO) {
                break;
            }

            ordem[posicao] = ordem[posicao - 1];
            posicao--;
        }

        ordem[posicao] = identificadorAtual;
    }
}

static Boolean construirOrdens(const Instancia *instancia,const Tarefa **tarefasPorIdentificador,IdentificadorDeTarefa *ordemAdiantada,IdentificadorDeTarefa *ordemAtrasada) {
    QuantidadeDeTarefas indice;
    IdentificadorDeTarefa identificador;

    if(instancia == NULL || tarefasPorIdentificador == NULL || ordemAdiantada == NULL || ordemAtrasada == NULL) {
        return FALSO;
    }

    for(indice = 0;indice < (*instancia).quantidadeDeTarefas;indice++) {
        identificador = (*instancia).tarefas[indice].identificador;
        ordemAdiantada[indice] = identificador;
        ordemAtrasada[indice] = identificador;
    }

    ordenarIdentificadores(ordemAdiantada,(*instancia).quantidadeDeTarefas,tarefasPorIdentificador,VERDADEIRO);
    ordenarIdentificadores(ordemAtrasada,(*instancia).quantidadeDeTarefas,tarefasPorIdentificador,FALSO);

    return VERDADEIRO;
}

static Boolean avaliarSolucao(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custo,QuantidadeDeTarefas *quantidadeDeTarefasAdiantadas) {
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

    if(solucao == NULL || tarefasPorIdentificador == NULL || temposPrefixados == NULL || custo == NULL || quantidadeDeTarefasAdiantadas == NULL) {
        return FALSO;
    }

    if((*solucao).sequenciaDeTarefas == NULL || (*solucao).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    somaDosTempos = 0;
    inclinacao = 0;

    for(posicao = 0;posicao < (*solucao).quantidadeDeTarefas;posicao++) {
        identificadorDaTarefa = (*solucao).sequenciaDeTarefas[posicao];
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
    (*quantidadeDeTarefasAdiantadas) = 0;

    for(posicao = 0;posicao < (*solucao).quantidadeDeTarefas;posicao++) {
        identificadorDaTarefa = (*solucao).sequenciaDeTarefas[posicao];
        tarefa = tarefasPorIdentificador[identificadorDaTarefa];
        instanteAtual += (*tarefa).tempoProcessamento;

        if(instanteAtual < dataDeEntregaComum) {
            adiantamento = dataDeEntregaComum - instanteAtual;
            custoTotal += ((Custo) (*tarefa).penalidadeAdiantamento) * ((Custo) adiantamento);
            (*quantidadeDeTarefasAdiantadas)++;
        }
        else if(instanteAtual > dataDeEntregaComum) {
            atraso = instanteAtual - dataDeEntregaComum;
            custoTotal += ((Custo) (*tarefa).penalidadeAtraso) * ((Custo) atraso);
        }
        else {
            (*quantidadeDeTarefasAdiantadas)++;
        }
    }

    (*custo) = custoTotal;

    return VERDADEIRO;
}

static Boolean construirSequenciaDaParticao(Solucao *solucao,const Boolean *tarefasAdiantadas,const IdentificadorDeTarefa *ordemAdiantada,const IdentificadorDeTarefa *ordemAtrasada) {
    QuantidadeDeTarefas indice;
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificador;

    if(solucao == NULL || tarefasAdiantadas == NULL || ordemAdiantada == NULL || ordemAtrasada == NULL || (*solucao).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    posicao = 0;

    for(indice = 0;indice < (*solucao).quantidadeDeTarefas;indice++) {
        identificador = ordemAdiantada[indice];

        if(tarefasAdiantadas[identificador] == VERDADEIRO) {
            (*solucao).sequenciaDeTarefas[posicao] = identificador;
            posicao++;
        }
    }

    for(indice = 0;indice < (*solucao).quantidadeDeTarefas;indice++) {
        identificador = ordemAtrasada[indice];

        if(tarefasAdiantadas[identificador] == FALSO) {
            (*solucao).sequenciaDeTarefas[posicao] = identificador;
            posicao++;
        }
    }

    return posicao == (*solucao).quantidadeDeTarefas ? VERDADEIRO : FALSO;
}

static Boolean tarefaMaisInadequadaNoLadoAdiantado(const Tarefa *primeiraTarefa,const Tarefa *segundaTarefa) {
    InteiroPositivoDe32Bits primeiroProduto;
    InteiroPositivoDe32Bits segundoProduto;

    primeiroProduto = ((InteiroPositivoDe32Bits) (*primeiraTarefa).penalidadeAdiantamento) * ((InteiroPositivoDe32Bits) (*segundaTarefa).penalidadeAtraso);
    segundoProduto = ((InteiroPositivoDe32Bits) (*segundaTarefa).penalidadeAdiantamento) * ((InteiroPositivoDe32Bits) (*primeiraTarefa).penalidadeAtraso);

    if(primeiroProduto > segundoProduto) {
        return VERDADEIRO;
    }

    if(primeiroProduto < segundoProduto) {
        return FALSO;
    }

    return (*primeiraTarefa).identificador < (*segundaTarefa).identificador ? VERDADEIRO : FALSO;
}

static Boolean tarefaMaisInadequadaNoLadoAtrasado(const Tarefa *primeiraTarefa,const Tarefa *segundaTarefa) {
    InteiroPositivoDe32Bits primeiroProduto;
    InteiroPositivoDe32Bits segundoProduto;

    primeiroProduto = ((InteiroPositivoDe32Bits) (*primeiraTarefa).penalidadeAtraso) * ((InteiroPositivoDe32Bits) (*segundaTarefa).penalidadeAdiantamento);
    segundoProduto = ((InteiroPositivoDe32Bits) (*segundaTarefa).penalidadeAtraso) * ((InteiroPositivoDe32Bits) (*primeiraTarefa).penalidadeAdiantamento);

    if(primeiroProduto > segundoProduto) {
        return VERDADEIRO;
    }

    if(primeiroProduto < segundoProduto) {
        return FALSO;
    }

    return (*primeiraTarefa).identificador < (*segundaTarefa).identificador ? VERDADEIRO : FALSO;
}

static void ordenarIdentificadoresPorInadequacao(IdentificadorDeTarefa *ordem,QuantidadeDeTarefas quantidade,const Tarefa **tarefasPorIdentificador,Boolean ladoAdiantado) {
    QuantidadeDeTarefas indice;
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificadorAtual;
    Boolean deveVirAntes;

    for(indice = 1;indice < quantidade;indice++) {
        identificadorAtual = ordem[indice];
        posicao = indice;

        while(posicao > 0) {
            if(ladoAdiantado == VERDADEIRO) {
                deveVirAntes = tarefaMaisInadequadaNoLadoAdiantado(tarefasPorIdentificador[identificadorAtual],tarefasPorIdentificador[ordem[posicao - 1]]);
            }
            else {
                deveVirAntes = tarefaMaisInadequadaNoLadoAtrasado(tarefasPorIdentificador[identificadorAtual],tarefasPorIdentificador[ordem[posicao - 1]]);
            }

            if(deveVirAntes == FALSO) {
                break;
            }

            ordem[posicao] = ordem[posicao - 1];
            posicao--;
        }

        ordem[posicao] = identificadorAtual;
    }
}

static Boolean construirOrdensDeInadequacao(const Instancia *instancia,const Tarefa **tarefasPorIdentificador,IdentificadorDeTarefa *ordemInadequacaoAdiantada,IdentificadorDeTarefa *ordemInadequacaoAtrasada) {
    QuantidadeDeTarefas indice;
    IdentificadorDeTarefa identificador;

    if(instancia == NULL || tarefasPorIdentificador == NULL || ordemInadequacaoAdiantada == NULL || ordemInadequacaoAtrasada == NULL) {
        return FALSO;
    }

    for(indice = 0;indice < (*instancia).quantidadeDeTarefas;indice++) {
        identificador = (*instancia).tarefas[indice].identificador;
        ordemInadequacaoAdiantada[indice] = identificador;
        ordemInadequacaoAtrasada[indice] = identificador;
    }

    ordenarIdentificadoresPorInadequacao(ordemInadequacaoAdiantada,(*instancia).quantidadeDeTarefas,tarefasPorIdentificador,VERDADEIRO);
    ordenarIdentificadoresPorInadequacao(ordemInadequacaoAtrasada,(*instancia).quantidadeDeTarefas,tarefasPorIdentificador,FALSO);

    return VERDADEIRO;
}

static QuantidadeDeTarefas obterLimiteDeCandidatosParaDuasTrocas(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 100) {
        return LIMITE_CANDIDATOS_DUAS_TROCAS_100;
    }

    if(quantidadeDeTarefas <= 200) {
        return LIMITE_CANDIDATOS_DUAS_TROCAS_200;
    }

    if(quantidadeDeTarefas <= 500) {
        return LIMITE_CANDIDATOS_DUAS_TROCAS_500;
    }

    return LIMITE_CANDIDATOS_DUAS_TROCAS_1000;
}

static QuantidadeDeTarefas obterLimiteDeCandidatosParaTresTrocas(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 20) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_20;
    }

    if(quantidadeDeTarefas <= 50) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_50;
    }

    if(quantidadeDeTarefas <= 100) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_100;
    }

    if(quantidadeDeTarefas <= 200) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_200;
    }

    if(quantidadeDeTarefas <= 500) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_500;
    }

    return LIMITE_CANDIDATOS_TRES_TROCAS_1000;
}

static QuantidadeDeTarefas prepararCandidatosDaParticao(const IdentificadorDeTarefa *ordemInadequacaoAdiantada,const IdentificadorDeTarefa *ordemInadequacaoAtrasada,const Boolean *tarefasAdiantadas,QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas limitePorLado,IdentificadorDeTarefa *candidatos) {
    QuantidadeDeTarefas indice;
    QuantidadeDeTarefas quantidadeDeCandidatos;
    QuantidadeDeTarefas quantidadeDeCandidatosDoLado;
    IdentificadorDeTarefa identificador;

    quantidadeDeCandidatos = 0;
    quantidadeDeCandidatosDoLado = 0;

    for(indice = 0;indice < quantidadeDeTarefas && quantidadeDeCandidatosDoLado < limitePorLado;indice++) {
        identificador = ordemInadequacaoAdiantada[indice];

        if(tarefasAdiantadas[identificador] == VERDADEIRO) {
            candidatos[quantidadeDeCandidatos] = identificador;
            quantidadeDeCandidatos++;
            quantidadeDeCandidatosDoLado++;
        }
    }

    quantidadeDeCandidatosDoLado = 0;

    for(indice = 0;indice < quantidadeDeTarefas && quantidadeDeCandidatosDoLado < limitePorLado;indice++) {
        identificador = ordemInadequacaoAtrasada[indice];

        if(tarefasAdiantadas[identificador] == FALSO) {
            candidatos[quantidadeDeCandidatos] = identificador;
            quantidadeDeCandidatos++;
            quantidadeDeCandidatosDoLado++;
        }
    }

    return quantidadeDeCandidatos;
}

static void alternarTarefa(Boolean *tarefasAdiantadas,IdentificadorDeTarefa identificador) {
    tarefasAdiantadas[identificador] = tarefasAdiantadas[identificador] == VERDADEIRO ? FALSO : VERDADEIRO;
}

static Boolean avaliarParticaoCandidata(Solucao *solucaoCandidata,const Boolean *tarefasAdiantadas,const IdentificadorDeTarefa *ordemAdiantada,const IdentificadorDeTarefa *ordemAtrasada,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custoCandidato) {
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;

    if(construirSequenciaDaParticao(solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO) {
        return FALSO;
    }

    return avaliarSolucao(solucaoCandidata,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,custoCandidato,&quantidadeDeTarefasAdiantadas);
}

Boolean controllerBuscaLocalTrocaKParticaoMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal) {
    Solucao solucaoCorrente;
    Solucao solucaoCanonicaInicial;
    Solucao solucaoCandidata;
    const Tarefa **tarefasPorIdentificador;
    IdentificadorDeTarefa *ordemAdiantada;
    IdentificadorDeTarefa *ordemAtrasada;
    IdentificadorDeTarefa *ordemInadequacaoAdiantada;
    IdentificadorDeTarefa *ordemInadequacaoAtrasada;
    IdentificadorDeTarefa *candidatosDuasTrocas;
    IdentificadorDeTarefa *candidatosTresTrocas;
    InteiroPositivoDe32Bits *temposPrefixados;
    Boolean *tarefasAdiantadas;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoOriginal;
    Custo custoCanonicoInicial;
    Custo custoCorrente;
    Custo melhorCusto;
    Custo custoCandidato;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;
    QuantidadeDeTarefas quantidadeDeCandidatosDuasTrocas;
    QuantidadeDeTarefas quantidadeDeCandidatosTresTrocas;
    QuantidadeDeTarefas indice1;
    QuantidadeDeTarefas indice2;
    QuantidadeDeTarefas indice3;
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificador;
    IdentificadorDeTarefa melhorIdentificador1;
    IdentificadorDeTarefa melhorIdentificador2;
    IdentificadorDeTarefa melhorIdentificador3;
    InteiroPositivoDe8Bits quantidadeDeTrocasDoMelhorMovimento;
    Boolean encontrouMelhoria;

    if(parametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO) {
        return FALSO;
    }

    (*resultadoBuscaLocal) = criarResultadoBuscaLocalVazio();
    solucaoCorrente = criarSolucaoVazia();
    solucaoCanonicaInicial = criarSolucaoVazia();
    solucaoCandidata = criarSolucaoVazia();
    tarefasPorIdentificador = NULL;
    ordemAdiantada = NULL;
    ordemAtrasada = NULL;
    ordemInadequacaoAdiantada = NULL;
    ordemInadequacaoAtrasada = NULL;
    candidatosDuasTrocas = NULL;
    candidatosTresTrocas = NULL;
    temposPrefixados = NULL;
    tarefasAdiantadas = NULL;

    if(copiarSolucao(solucaoInicial,&solucaoCorrente) == FALSO || copiarSolucao(solucaoInicial,&solucaoCanonicaInicial) == FALSO || copiarSolucao(solucaoInicial,&solucaoCandidata) == FALSO) {
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCanonicaInicial);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    tarefasPorIdentificador = (const Tarefa **) malloc(sizeof(Tarefa *) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));
    ordemAdiantada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemAtrasada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemInadequacaoAdiantada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemInadequacaoAtrasada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    candidatosDuasTrocas = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    candidatosTresTrocas = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    temposPrefixados = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * (*instancia).quantidadeDeTarefas);
    tarefasAdiantadas = (Boolean *) malloc(sizeof(Boolean) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));

    if(tarefasPorIdentificador == NULL || ordemAdiantada == NULL || ordemAtrasada == NULL || ordemInadequacaoAdiantada == NULL || ordemInadequacaoAtrasada == NULL || candidatosDuasTrocas == NULL || candidatosTresTrocas == NULL || temposPrefixados == NULL || tarefasAdiantadas == NULL) {
        free(tarefasAdiantadas);
        free(temposPrefixados);
        free(candidatosTresTrocas);
        free(candidatosDuasTrocas);
        free(ordemInadequacaoAtrasada);
        free(ordemInadequacaoAdiantada);
        free(ordemAtrasada);
        free(ordemAdiantada);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCanonicaInicial);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(montarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO || construirOrdens(instancia,tarefasPorIdentificador,ordemAdiantada,ordemAtrasada) == FALSO || construirOrdensDeInadequacao(instancia,tarefasPorIdentificador,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada) == FALSO) {
        free(tarefasAdiantadas);
        free(temposPrefixados);
        free(candidatosTresTrocas);
        free(candidatosDuasTrocas);
        free(ordemInadequacaoAtrasada);
        free(ordemInadequacaoAdiantada);
        free(ordemAtrasada);
        free(ordemAdiantada);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCanonicaInicial);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(avaliarSolucao(solucaoInicial,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoOriginal,&quantidadeDeTarefasAdiantadas) == FALSO) {
        return FALSO;
    }

    for(identificador = 0;identificador <= (*instancia).quantidadeDeTarefas;identificador++) {
        tarefasAdiantadas[identificador] = FALSO;
    }

    for(posicao = 0;posicao < quantidadeDeTarefasAdiantadas;posicao++) {
        tarefasAdiantadas[(*solucaoInicial).sequenciaDeTarefas[posicao]] = VERDADEIRO;
    }

    if(construirSequenciaDaParticao(&solucaoCanonicaInicial,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO || avaliarSolucao(&solucaoCanonicaInicial,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCanonicoInicial,&quantidadeDeTarefasAdiantadas) == FALSO) {
        return FALSO;
    }

    if(custoOriginal < custoCanonicoInicial) {
        custoCorrente = custoOriginal;

        if(copiarSequencia(solucaoInicial,&solucaoCorrente) == FALSO) {
            return FALSO;
        }
    }
    else {
        custoCorrente = custoCanonicoInicial;

        if(copiarSequencia(&solucaoCanonicaInicial,&solucaoCorrente) == FALSO) {
            return FALSO;
        }
    }

    (*resultadoBuscaLocal).custoInicial = custoOriginal;
    encontrouMelhoria = VERDADEIRO;

    while(encontrouMelhoria == VERDADEIRO) {
        (*resultadoBuscaLocal).quantidadeDeIteracoes++;
        encontrouMelhoria = FALSO;
        melhorCusto = custoCorrente;
        melhorIdentificador1 = 0;
        melhorIdentificador2 = 0;
        melhorIdentificador3 = 0;
        quantidadeDeTrocasDoMelhorMovimento = 0;

        for(identificador = 1;identificador <= (*instancia).quantidadeDeTarefas;identificador++) {
            alternarTarefa(tarefasAdiantadas,identificador);

            if(avaliarParticaoCandidata(&solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                return FALSO;
            }

            (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
            (*resultadoBuscaLocal).quantidadeDeVizinhosPorReinsercao++;

            if(custoCandidato < melhorCusto) {
                melhorCusto = custoCandidato;
                melhorIdentificador1 = identificador;
                melhorIdentificador2 = 0;
                melhorIdentificador3 = 0;
                quantidadeDeTrocasDoMelhorMovimento = 1;
                encontrouMelhoria = VERDADEIRO;
            }

            alternarTarefa(tarefasAdiantadas,identificador);
        }

        quantidadeDeCandidatosDuasTrocas = prepararCandidatosDaParticao(ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,tarefasAdiantadas,(*instancia).quantidadeDeTarefas,obterLimiteDeCandidatosParaDuasTrocas((*instancia).quantidadeDeTarefas),candidatosDuasTrocas);
        quantidadeDeCandidatosTresTrocas = prepararCandidatosDaParticao(ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,tarefasAdiantadas,(*instancia).quantidadeDeTarefas,obterLimiteDeCandidatosParaTresTrocas((*instancia).quantidadeDeTarefas),candidatosTresTrocas);

        for(indice1 = 0;indice1 < quantidadeDeCandidatosDuasTrocas;indice1++) {
            alternarTarefa(tarefasAdiantadas,candidatosDuasTrocas[indice1]);

            for(indice2 = indice1 + 1;indice2 < quantidadeDeCandidatosDuasTrocas;indice2++) {
                alternarTarefa(tarefasAdiantadas,candidatosDuasTrocas[indice2]);

                if(avaliarParticaoCandidata(&solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                    return FALSO;
                }

                (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
                (*resultadoBuscaLocal).quantidadeDeVizinhosPorTroca++;

                if(custoCandidato < melhorCusto) {
                    melhorCusto = custoCandidato;
                    melhorIdentificador1 = candidatosDuasTrocas[indice1];
                    melhorIdentificador2 = candidatosDuasTrocas[indice2];
                    melhorIdentificador3 = 0;
                    quantidadeDeTrocasDoMelhorMovimento = 2;
                    encontrouMelhoria = VERDADEIRO;
                }

                alternarTarefa(tarefasAdiantadas,candidatosDuasTrocas[indice2]);
            }

            alternarTarefa(tarefasAdiantadas,candidatosDuasTrocas[indice1]);
        }

        for(indice1 = 0;indice1 < quantidadeDeCandidatosTresTrocas;indice1++) {
            alternarTarefa(tarefasAdiantadas,candidatosTresTrocas[indice1]);

            for(indice2 = indice1 + 1;indice2 < quantidadeDeCandidatosTresTrocas;indice2++) {
                alternarTarefa(tarefasAdiantadas,candidatosTresTrocas[indice2]);

                for(indice3 = indice2 + 1;indice3 < quantidadeDeCandidatosTresTrocas;indice3++) {
                    alternarTarefa(tarefasAdiantadas,candidatosTresTrocas[indice3]);

                    if(avaliarParticaoCandidata(&solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                        return FALSO;
                    }

                    (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
                    (*resultadoBuscaLocal).quantidadeDeVizinhosPorTroca++;

                    if(custoCandidato < melhorCusto) {
                        melhorCusto = custoCandidato;
                        melhorIdentificador1 = candidatosTresTrocas[indice1];
                        melhorIdentificador2 = candidatosTresTrocas[indice2];
                        melhorIdentificador3 = candidatosTresTrocas[indice3];
                        quantidadeDeTrocasDoMelhorMovimento = 3;
                        encontrouMelhoria = VERDADEIRO;
                    }

                    alternarTarefa(tarefasAdiantadas,candidatosTresTrocas[indice3]);
                }

                alternarTarefa(tarefasAdiantadas,candidatosTresTrocas[indice2]);
            }

            alternarTarefa(tarefasAdiantadas,candidatosTresTrocas[indice1]);
        }

        if(encontrouMelhoria == VERDADEIRO) {
            alternarTarefa(tarefasAdiantadas,melhorIdentificador1);

            if(quantidadeDeTrocasDoMelhorMovimento >= 2) {
                alternarTarefa(tarefasAdiantadas,melhorIdentificador2);
            }

            if(quantidadeDeTrocasDoMelhorMovimento == 3) {
                alternarTarefa(tarefasAdiantadas,melhorIdentificador3);
            }

            if(construirSequenciaDaParticao(&solucaoCorrente,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO || avaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO || custoCorrente != melhorCusto) {
                return FALSO;
            }

            if(quantidadeDeTrocasDoMelhorMovimento == 1) {
                (*resultadoBuscaLocal).quantidadeDeMelhoriasPorReinsercao++;
            }
            else {
                (*resultadoBuscaLocal).quantidadeDeMelhoriasPorTroca++;
            }
        }
    }

    if(custoOriginal < custoCorrente) {
        (*resultadoBuscaLocal).custoFinal = custoOriginal;

        if(copiarSolucao(solucaoInicial,solucaoFinal) == FALSO) {
            return FALSO;
        }
    }
    else {
        (*resultadoBuscaLocal).custoFinal = custoCorrente;

        if(copiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
            return FALSO;
        }
    }

    free(tarefasAdiantadas);
    free(temposPrefixados);
    free(candidatosTresTrocas);
    free(candidatosDuasTrocas);
    free(ordemInadequacaoAtrasada);
    free(ordemInadequacaoAdiantada);
    free(ordemAtrasada);
    free(ordemAdiantada);
    free(tarefasPorIdentificador);
    liberarSolucao(&solucaoCandidata);
    liberarSolucao(&solucaoCanonicaInicial);
    liberarSolucao(&solucaoCorrente);

    return VERDADEIRO;
}