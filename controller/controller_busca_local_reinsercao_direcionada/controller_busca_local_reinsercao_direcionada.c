#include "controller_busca_local_reinsercao_direcionada.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

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

static QuantidadeDeTarefas calcularDestinoNoLadoAdiantado(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas) {
    QuantidadeDeTarefas posicao;
    QuantidadeDeTarefas posicaoDestino;
    IdentificadorDeTarefa identificadorMovido;
    IdentificadorDeTarefa identificadorComparado;
    const Tarefa *tarefaMovida;
    const Tarefa *tarefaComparada;

    identificadorMovido = (*solucao).sequenciaDeTarefas[posicaoOrigem];
    tarefaMovida = tarefasPorIdentificador[identificadorMovido];
    posicaoDestino = 0;

    for(posicao = 0;posicao < quantidadeDeTarefasAdiantadas;posicao++) {
        if(posicao == posicaoOrigem) {
            continue;
        }

        identificadorComparado = (*solucao).sequenciaDeTarefas[posicao];
        tarefaComparada = tarefasPorIdentificador[identificadorComparado];

        if(tarefaDeveVirAntesNoLadoAdiantado(tarefaComparada,tarefaMovida) == VERDADEIRO) {
            posicaoDestino++;
        }
    }

    if(posicaoDestino >= (*solucao).quantidadeDeTarefas) {
        posicaoDestino = (QuantidadeDeTarefas) ((*solucao).quantidadeDeTarefas - 1);
    }

    return posicaoDestino;
}

static QuantidadeDeTarefas calcularDestinoNoLadoAtrasado(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas) {
    QuantidadeDeTarefas posicao;
    QuantidadeDeTarefas posicaoDestino;
    IdentificadorDeTarefa identificadorMovido;
    IdentificadorDeTarefa identificadorComparado;
    const Tarefa *tarefaMovida;
    const Tarefa *tarefaComparada;

    identificadorMovido = (*solucao).sequenciaDeTarefas[posicaoOrigem];
    tarefaMovida = tarefasPorIdentificador[identificadorMovido];
    posicaoDestino = quantidadeDeTarefasAdiantadas;

    for(posicao = quantidadeDeTarefasAdiantadas;posicao < (*solucao).quantidadeDeTarefas;posicao++) {
        if(posicao == posicaoOrigem) {
            continue;
        }

        identificadorComparado = (*solucao).sequenciaDeTarefas[posicao];
        tarefaComparada = tarefasPorIdentificador[identificadorComparado];

        if(tarefaDeveVirAntesNoLadoAtrasado(tarefaComparada,tarefaMovida) == VERDADEIRO) {
            posicaoDestino++;
        }
    }

    if(posicaoDestino >= (*solucao).quantidadeDeTarefas) {
        posicaoDestino = (QuantidadeDeTarefas) ((*solucao).quantidadeDeTarefas - 1);
    }

    return posicaoDestino;
}

static void marcarPosicao(Boolean *posicoesCandidatas,QuantidadeDeTarefas quantidadeDeTarefas,InteiroComSinalDe32Bits posicao) {
    if(posicao >= 0 && posicao < (InteiroComSinalDe32Bits) quantidadeDeTarefas) {
        posicoesCandidatas[posicao] = VERDADEIRO;
    }
}

static void prepararPosicoesCandidatas(Boolean *posicoesCandidatas,const Solucao *solucao,const Tarefa **tarefasPorIdentificador,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas) {
    QuantidadeDeTarefas quantidadeDeTarefas;
    QuantidadeDeTarefas posicao;
    QuantidadeDeTarefas indiceDaAncora;
    QuantidadeDeTarefas posicaoDaAncora;
    InteiroComSinalDe32Bits deslocamento;

    quantidadeDeTarefas = (*solucao).quantidadeDeTarefas;
    memset(posicoesCandidatas,FALSO,sizeof(Boolean) * quantidadeDeTarefas);

    if(quantidadeDeTarefas <= LIMITE_REINSERCAO_DIRECIONADA_GLOBAL) {
        for(posicao = 0;posicao < quantidadeDeTarefas;posicao++) {
            posicoesCandidatas[posicao] = VERDADEIRO;
        }

        return;
    }

    if(quantidadeDeTarefas <= LIMITE_REINSERCAO_DIRECIONADA_REPRESENTATIVA) {
        for(indiceDaAncora = 0;indiceDaAncora < QUANTIDADE_ANCORAS_REINSERCAO_DIRECIONADA_REPRESENTATIVA;indiceDaAncora++) {
            posicaoDaAncora = (QuantidadeDeTarefas) ((((InteiroPositivoDe32Bits) indiceDaAncora) * ((InteiroPositivoDe32Bits) quantidadeDeTarefas - 1u)) / ((InteiroPositivoDe32Bits) QUANTIDADE_ANCORAS_REINSERCAO_DIRECIONADA_REPRESENTATIVA - 1u));
            posicoesCandidatas[posicaoDaAncora] = VERDADEIRO;
        }

        for(deslocamento = -(InteiroComSinalDe32Bits) RAIO_LOCAL_REINSERCAO_DIRECIONADA_REPRESENTATIVA;deslocamento <= (InteiroComSinalDe32Bits) RAIO_LOCAL_REINSERCAO_DIRECIONADA_REPRESENTATIVA;deslocamento++) {
            marcarPosicao(posicoesCandidatas,quantidadeDeTarefas,(InteiroComSinalDe32Bits) posicaoOrigem + deslocamento);
        }

        for(deslocamento = -(InteiroComSinalDe32Bits) RAIO_FRONTEIRA_REINSERCAO_DIRECIONADA_REPRESENTATIVA;deslocamento <= (InteiroComSinalDe32Bits) RAIO_FRONTEIRA_REINSERCAO_DIRECIONADA_REPRESENTATIVA;deslocamento++) {
            marcarPosicao(posicoesCandidatas,quantidadeDeTarefas,(InteiroComSinalDe32Bits) quantidadeDeTarefasAdiantadas + deslocamento);
        }

        return;
    }

    for(deslocamento = -(InteiroComSinalDe32Bits) RAIO_LOCAL_REINSERCAO_DIRECIONADA_GRANDE;deslocamento <= (InteiroComSinalDe32Bits) RAIO_LOCAL_REINSERCAO_DIRECIONADA_GRANDE;deslocamento++) {
        marcarPosicao(posicoesCandidatas,quantidadeDeTarefas,(InteiroComSinalDe32Bits) posicaoOrigem + deslocamento);
    }

    marcarPosicao(posicoesCandidatas,quantidadeDeTarefas,(InteiroComSinalDe32Bits) calcularDestinoNoLadoAdiantado(solucao,tarefasPorIdentificador,posicaoOrigem,quantidadeDeTarefasAdiantadas));
    marcarPosicao(posicoesCandidatas,quantidadeDeTarefas,(InteiroComSinalDe32Bits) calcularDestinoNoLadoAtrasado(solucao,tarefasPorIdentificador,posicaoOrigem,quantidadeDeTarefasAdiantadas));

    for(deslocamento = -(InteiroComSinalDe32Bits) RAIO_FRONTEIRA_REINSERCAO_DIRECIONADA_GRANDE;deslocamento <= (InteiroComSinalDe32Bits) RAIO_FRONTEIRA_REINSERCAO_DIRECIONADA_GRANDE;deslocamento++) {
        marcarPosicao(posicoesCandidatas,quantidadeDeTarefas,(InteiroComSinalDe32Bits) quantidadeDeTarefasAdiantadas + deslocamento);
    }
}

static Boolean procurarMelhorReinsercao(Solucao *solucaoCorrente,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Boolean *posicoesCandidatas,Custo custoCorrente,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas *melhorPosicaoOrigem,QuantidadeDeTarefas *melhorPosicaoDestino,Custo *melhorCusto,Boolean *encontrouMelhoria) {
    QuantidadeDeTarefas posicaoOrigem;
    QuantidadeDeTarefas posicaoDestino;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadasCandidata;
    Custo custoCandidato;

    (*melhorPosicaoOrigem) = 0;
    (*melhorPosicaoDestino) = 0;
    (*melhorCusto) = custoCorrente;
    (*encontrouMelhoria) = FALSO;

    for(posicaoOrigem = 0;posicaoOrigem < (*solucaoCorrente).quantidadeDeTarefas;posicaoOrigem++) {
        prepararPosicoesCandidatas(posicoesCandidatas,solucaoCorrente,tarefasPorIdentificador,posicaoOrigem,quantidadeDeTarefasAdiantadas);

        for(posicaoDestino = 0;posicaoDestino < (*solucaoCorrente).quantidadeDeTarefas;posicaoDestino++) {
            if(posicoesCandidatas[posicaoDestino] == FALSO || posicaoDestino == posicaoOrigem) {
                continue;
            }

            if(reinserirTarefa(solucaoCorrente,posicaoOrigem,posicaoDestino) == FALSO) {
                return FALSO;
            }

            if(avaliarSolucao(solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato,&quantidadeDeTarefasAdiantadasCandidata) == FALSO) {
                return FALSO;
            }

            (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados++;
            (*resultadoBuscaLocal).quantidadeDeVizinhosPorReinsercao++;

            if(custoCandidato < (*melhorCusto)) {
                (*melhorPosicaoOrigem) = posicaoOrigem;
                (*melhorPosicaoDestino) = posicaoDestino;
                (*melhorCusto) = custoCandidato;
                (*encontrouMelhoria) = VERDADEIRO;
            }

            if(reinserirTarefa(solucaoCorrente,posicaoDestino,posicaoOrigem) == FALSO) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

Boolean controllerBuscaLocalReinsercaoDirecionadaMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal) {
    Solucao solucaoCorrente;
    const Tarefa **tarefasPorIdentificador;
    InteiroPositivoDe32Bits *temposPrefixados;
    Boolean *posicoesCandidatas;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoCorrente;
    Custo melhorCusto;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;
    QuantidadeDeTarefas melhorPosicaoOrigem;
    QuantidadeDeTarefas melhorPosicaoDestino;
    Boolean encontrouMelhoria;

    if(parametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO) {
        return FALSO;
    }

    (*resultadoBuscaLocal) = criarResultadoBuscaLocalVazio();
    solucaoCorrente = criarSolucaoVazia();
    tarefasPorIdentificador = NULL;
    temposPrefixados = NULL;
    posicoesCandidatas = NULL;

    if(copiarSolucao(solucaoInicial,&solucaoCorrente) == FALSO) {
        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    tarefasPorIdentificador = (const Tarefa **) malloc(sizeof(Tarefa *) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));
    temposPrefixados = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * (*instancia).quantidadeDeTarefas);
    posicoesCandidatas = (Boolean *) malloc(sizeof(Boolean) * (*instancia).quantidadeDeTarefas);

    if(tarefasPorIdentificador == NULL || temposPrefixados == NULL || posicoesCandidatas == NULL) {
        free(posicoesCandidatas);
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(montarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO) {
        free(posicoesCandidatas);
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    if(avaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO) {
        free(posicoesCandidatas);
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    (*resultadoBuscaLocal).custoInicial = custoCorrente;
    encontrouMelhoria = VERDADEIRO;

    while(encontrouMelhoria == VERDADEIRO) {
        (*resultadoBuscaLocal).quantidadeDeIteracoes++;

        if(procurarMelhorReinsercao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,posicoesCandidatas,custoCorrente,quantidadeDeTarefasAdiantadas,resultadoBuscaLocal,&melhorPosicaoOrigem,&melhorPosicaoDestino,&melhorCusto,&encontrouMelhoria) == FALSO) {
            free(posicoesCandidatas);
            free(temposPrefixados);
            free(tarefasPorIdentificador);
            liberarSolucao(&solucaoCorrente);

            return FALSO;
        }

        if(encontrouMelhoria == VERDADEIRO) {
            if(reinserirTarefa(&solucaoCorrente,melhorPosicaoOrigem,melhorPosicaoDestino) == FALSO) {
                free(posicoesCandidatas);
                free(temposPrefixados);
                free(tarefasPorIdentificador);
                liberarSolucao(&solucaoCorrente);

                return FALSO;
            }

            if(avaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO || custoCorrente != melhorCusto) {
                free(posicoesCandidatas);
                free(temposPrefixados);
                free(tarefasPorIdentificador);
                liberarSolucao(&solucaoCorrente);

                return FALSO;
            }

            (*resultadoBuscaLocal).quantidadeDeMelhoriasPorReinsercao++;
        }
    }

    (*resultadoBuscaLocal).custoFinal = custoCorrente;

    if(copiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
        free(posicoesCandidatas);
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    free(posicoesCandidatas);
    free(temposPrefixados);
    free(tarefasPorIdentificador);
    liberarSolucao(&solucaoCorrente);

    return VERDADEIRO;
}