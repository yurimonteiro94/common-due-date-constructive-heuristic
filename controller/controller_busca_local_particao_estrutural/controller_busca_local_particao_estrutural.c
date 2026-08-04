#include "controller_busca_local_particao_estrutural.h"

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

static QuantidadeDeTarefas obterLimiteDeCandidatos(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 100) {
        return LIMITE_CANDIDATOS_INTERCAMBIO_PARTICAO_100;
    }

    if(quantidadeDeTarefas <= 200) {
        return LIMITE_CANDIDATOS_INTERCAMBIO_PARTICAO_200;
    }

    if(quantidadeDeTarefas <= 500) {
        return LIMITE_CANDIDATOS_INTERCAMBIO_PARTICAO_500;
    }

    return LIMITE_CANDIDATOS_INTERCAMBIO_PARTICAO_1000;
}

static QuantidadeDeTarefas prepararCandidatos(const IdentificadorDeTarefa *ordem,const Boolean *tarefasAdiantadas,QuantidadeDeTarefas quantidadeDeTarefas,Boolean selecionarAdiantadas,IdentificadorDeTarefa *candidatos) {
    QuantidadeDeTarefas indice;
    QuantidadeDeTarefas quantidadeDeCandidatos;
    QuantidadeDeTarefas limite;
    IdentificadorDeTarefa identificador;

    quantidadeDeCandidatos = 0;
    limite = obterLimiteDeCandidatos(quantidadeDeTarefas);

    for(indice = 0;indice < quantidadeDeTarefas && quantidadeDeCandidatos < limite;indice++) {
        identificador = ordem[indice];

        if(tarefasAdiantadas[identificador] == selecionarAdiantadas) {
            candidatos[quantidadeDeCandidatos] = identificador;
            quantidadeDeCandidatos++;
        }
    }

    return quantidadeDeCandidatos;
}

Boolean controllerBuscaLocalParticaoEstruturalMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal) {
    Solucao solucaoCorrente;
    Solucao solucaoCandidata;
    const Tarefa **tarefasPorIdentificador;
    IdentificadorDeTarefa *ordemAdiantada;
    IdentificadorDeTarefa *ordemAtrasada;
    IdentificadorDeTarefa *ordemInadequacaoAdiantada;
    IdentificadorDeTarefa *ordemInadequacaoAtrasada;
    IdentificadorDeTarefa *candidatosAdiantados;
    IdentificadorDeTarefa *candidatosAtrasados;
    InteiroPositivoDe32Bits *temposPrefixados;
    Boolean *tarefasAdiantadas;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoOriginal;
    Custo custoCorrente;
    Custo melhorCusto;
    Custo custoCandidato;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadasCandidata;
    QuantidadeDeTarefas quantidadeDeCandidatosAdiantados;
    QuantidadeDeTarefas quantidadeDeCandidatosAtrasados;
    QuantidadeDeTarefas indiceAdiantado;
    QuantidadeDeTarefas indiceAtrasado;
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificador;
    IdentificadorDeTarefa identificadorAdiantado;
    IdentificadorDeTarefa identificadorAtrasado;
    IdentificadorDeTarefa melhorIdentificador1;
    IdentificadorDeTarefa melhorIdentificador2;
    InteiroPositivoDe8Bits tipoDoMelhorMovimento;
    Boolean encontrouMelhoria;

    if(parametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO) {
        return FALSO;
    }

    (*resultadoBuscaLocal) = criarResultadoBuscaLocalVazio();
    solucaoCorrente = criarSolucaoVazia();
    solucaoCandidata = criarSolucaoVazia();
    tarefasPorIdentificador = NULL;
    ordemAdiantada = NULL;
    ordemAtrasada = NULL;
    ordemInadequacaoAdiantada = NULL;
    ordemInadequacaoAtrasada = NULL;
    candidatosAdiantados = NULL;
    candidatosAtrasados = NULL;
    temposPrefixados = NULL;
    tarefasAdiantadas = NULL;

    if(copiarSolucao(solucaoInicial,&solucaoCorrente) == FALSO || copiarSolucao(solucaoInicial,&solucaoCandidata) == FALSO) {
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    tarefasPorIdentificador = (const Tarefa **) malloc(sizeof(Tarefa *) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));
    ordemAdiantada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemAtrasada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemInadequacaoAdiantada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemInadequacaoAtrasada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    candidatosAdiantados = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    candidatosAtrasados = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    temposPrefixados = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * (*instancia).quantidadeDeTarefas);
    tarefasAdiantadas = (Boolean *) malloc(sizeof(Boolean) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));

    if(tarefasPorIdentificador == NULL || ordemAdiantada == NULL || ordemAtrasada == NULL || ordemInadequacaoAdiantada == NULL || ordemInadequacaoAtrasada == NULL || candidatosAdiantados == NULL || candidatosAtrasados == NULL || temposPrefixados == NULL || tarefasAdiantadas == NULL) {
        free(tarefasAdiantadas);
        free(temposPrefixados);
        free(candidatosAtrasados);
        free(candidatosAdiantados);
        free(ordemInadequacaoAtrasada);
        free(ordemInadequacaoAdiantada);
        free(ordemAtrasada);
        free(ordemAdiantada);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(montarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO || construirOrdens(instancia,tarefasPorIdentificador,ordemAdiantada,ordemAtrasada) == FALSO || construirOrdensDeInadequacao(instancia,tarefasPorIdentificador,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada) == FALSO) {
        free(tarefasAdiantadas);
        free(temposPrefixados);
        free(candidatosAtrasados);
        free(candidatosAdiantados);
        free(ordemInadequacaoAtrasada);
        free(ordemInadequacaoAdiantada);
        free(ordemAtrasada);
        free(ordemAdiantada);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCandidata);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(avaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoOriginal,&quantidadeDeTarefasAdiantadas) == FALSO) {
        return FALSO;
    }

    for(identificador = 0;identificador <= (*instancia).quantidadeDeTarefas;identificador++) {
        tarefasAdiantadas[identificador] = FALSO;
    }

    for(posicao = 0;posicao < quantidadeDeTarefasAdiantadas;posicao++) {
        tarefasAdiantadas[(*solucaoInicial).sequenciaDeTarefas[posicao]] = VERDADEIRO;
    }

    if(construirSequenciaDaParticao(&solucaoCorrente,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO || avaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO) {
        return FALSO;
    }

    if(custoCorrente > custoOriginal) {
        for(posicao = 0;posicao < (*solucaoInicial).quantidadeDeTarefas;posicao++) {
            solucaoCorrente.sequenciaDeTarefas[posicao] = (*solucaoInicial).sequenciaDeTarefas[posicao];
        }

        custoCorrente = custoOriginal;
    }

    (*resultadoBuscaLocal).custoInicial = custoOriginal;
    encontrouMelhoria = VERDADEIRO;

    while(encontrouMelhoria == VERDADEIRO) {
        (*resultadoBuscaLocal).quantidadeDeIteracoes++;
        encontrouMelhoria = FALSO;
        melhorCusto = custoCorrente;
        melhorIdentificador1 = 0;
        melhorIdentificador2 = 0;
        tipoDoMelhorMovimento = 0;

        for(identificador = 1;identificador <= (*instancia).quantidadeDeTarefas;identificador++) {
            tarefasAdiantadas[identificador] = tarefasAdiantadas[identificador] == VERDADEIRO ? FALSO : VERDADEIRO;

            if(construirSequenciaDaParticao(&solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO || avaliarSolucao(&solucaoCandidata,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato,&quantidadeDeTarefasAdiantadasCandidata) == FALSO) {
                return FALSO;
            }

            (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
            (*resultadoBuscaLocal).quantidadeDeVizinhosPorReinsercao++;

            if(custoCandidato < melhorCusto) {
                melhorCusto = custoCandidato;
                melhorIdentificador1 = identificador;
                melhorIdentificador2 = 0;
                tipoDoMelhorMovimento = 1;
                encontrouMelhoria = VERDADEIRO;
            }

            tarefasAdiantadas[identificador] = tarefasAdiantadas[identificador] == VERDADEIRO ? FALSO : VERDADEIRO;
        }

        quantidadeDeCandidatosAdiantados = prepararCandidatos(ordemInadequacaoAdiantada,tarefasAdiantadas,(*instancia).quantidadeDeTarefas,VERDADEIRO,candidatosAdiantados);
        quantidadeDeCandidatosAtrasados = prepararCandidatos(ordemInadequacaoAtrasada,tarefasAdiantadas,(*instancia).quantidadeDeTarefas,FALSO,candidatosAtrasados);

        for(indiceAdiantado = 0;indiceAdiantado < quantidadeDeCandidatosAdiantados;indiceAdiantado++) {
            identificadorAdiantado = candidatosAdiantados[indiceAdiantado];

            for(indiceAtrasado = 0;indiceAtrasado < quantidadeDeCandidatosAtrasados;indiceAtrasado++) {
                identificadorAtrasado = candidatosAtrasados[indiceAtrasado];
                tarefasAdiantadas[identificadorAdiantado] = FALSO;
                tarefasAdiantadas[identificadorAtrasado] = VERDADEIRO;

                if(construirSequenciaDaParticao(&solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO || avaliarSolucao(&solucaoCandidata,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato,&quantidadeDeTarefasAdiantadasCandidata) == FALSO) {
                    return FALSO;
                }

                (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
                (*resultadoBuscaLocal).quantidadeDeVizinhosPorTroca++;

                if(custoCandidato < melhorCusto) {
                    melhorCusto = custoCandidato;
                    melhorIdentificador1 = identificadorAdiantado;
                    melhorIdentificador2 = identificadorAtrasado;
                    tipoDoMelhorMovimento = 2;
                    encontrouMelhoria = VERDADEIRO;
                }

                tarefasAdiantadas[identificadorAdiantado] = VERDADEIRO;
                tarefasAdiantadas[identificadorAtrasado] = FALSO;
            }
        }

        if(encontrouMelhoria == VERDADEIRO) {
            if(tipoDoMelhorMovimento == 1) {
                tarefasAdiantadas[melhorIdentificador1] = tarefasAdiantadas[melhorIdentificador1] == VERDADEIRO ? FALSO : VERDADEIRO;
                (*resultadoBuscaLocal).quantidadeDeMelhoriasPorReinsercao++;
            }
            else {
                tarefasAdiantadas[melhorIdentificador1] = FALSO;
                tarefasAdiantadas[melhorIdentificador2] = VERDADEIRO;
                (*resultadoBuscaLocal).quantidadeDeMelhoriasPorTroca++;
            }

            if(construirSequenciaDaParticao(&solucaoCorrente,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO || avaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO || custoCorrente != melhorCusto) {
                return FALSO;
            }
        }
    }

    (*resultadoBuscaLocal).custoFinal = custoCorrente;

    if(copiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
        return FALSO;
    }

    free(tarefasAdiantadas);
    free(temposPrefixados);
    free(candidatosAtrasados);
    free(candidatosAdiantados);
    free(ordemInadequacaoAtrasada);
    free(ordemInadequacaoAdiantada);
    free(ordemAtrasada);
    free(ordemAdiantada);
    free(tarefasPorIdentificador);
    liberarSolucao(&solucaoCandidata);
    liberarSolucao(&solucaoCorrente);

    return VERDADEIRO;
}
