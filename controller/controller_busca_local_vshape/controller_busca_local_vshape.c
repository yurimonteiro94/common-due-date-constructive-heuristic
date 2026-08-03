#include "controller_busca_local_vshape.h"

#include <stddef.h>
#include <stdlib.h>

static Boolean controllerBuscaLocalVShapeParametrosSaoValidos(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalVShape *resultado) {
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

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalVShapeCopiarSolucao(const Solucao *origem,Solucao *destino) {
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

static Boolean controllerBuscaLocalVShapeReinserirTarefa(Solucao *solucao,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas posicaoDestino) {
    IdentificadorDeTarefa identificadorDaTarefa;
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

    identificadorDaTarefa = (*solucao).sequenciaDeTarefas[posicaoOrigem];

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

    (*solucao).sequenciaDeTarefas[posicaoDestino] = identificadorDaTarefa;

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalVShapeMontarMapaDeTarefas(const Instancia *instancia,const Tarefa **tarefasPorIdentificador) {
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

static Boolean controllerBuscaLocalVShapeAvaliarSolucao(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custo,QuantidadeDeTarefas *quantidadeDeTarefasAdiantadas) {
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

static Boolean controllerBuscaLocalVShapeTarefaDeveVirAntesNoLadoAdiantado(const Tarefa *primeiraTarefa,const Tarefa *segundaTarefa) {
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

static Boolean controllerBuscaLocalVShapeTarefaDeveVirAntesNoLadoAtrasado(const Tarefa *primeiraTarefa,const Tarefa *segundaTarefa) {
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

static QuantidadeDeTarefas controllerBuscaLocalVShapeCalcularDestinoNoLadoAdiantado(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas) {
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

        if(controllerBuscaLocalVShapeTarefaDeveVirAntesNoLadoAdiantado(tarefaComparada,tarefaMovida) == VERDADEIRO) {
            posicaoDestino++;
        }
    }

    return posicaoDestino;
}

static QuantidadeDeTarefas controllerBuscaLocalVShapeCalcularDestinoNoLadoAtrasado(const Solucao *solucao,const Tarefa **tarefasPorIdentificador,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas) {
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

        if(controllerBuscaLocalVShapeTarefaDeveVirAntesNoLadoAtrasado(tarefaComparada,tarefaMovida) == VERDADEIRO) {
            posicaoDestino++;
        }
    }

    if(posicaoDestino >= (*solucao).quantidadeDeTarefas) {
        posicaoDestino = (QuantidadeDeTarefas) ((*solucao).quantidadeDeTarefas - 1);
    }

    return posicaoDestino;
}

static Boolean controllerBuscaLocalVShapeTestarReinsercao(Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,QuantidadeDeTarefas posicaoOrigem,QuantidadeDeTarefas posicaoDestino,Custo *custoCorrente,InteiroPositivoDe32Bits *quantidadeDeVizinhos,InteiroPositivoDe32Bits *quantidadeDeMelhorias,Boolean *houveMelhoria) {
    Custo custoCandidato;

    if(posicaoOrigem == posicaoDestino) {
        return VERDADEIRO;
    }

    if(controllerBuscaLocalVShapeReinserirTarefa(solucao,posicaoOrigem,posicaoDestino) == FALSO) {
        return FALSO;
    }

    if(controllerBuscaLocalVShapeAvaliarSolucao(solucao,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato,NULL) == FALSO) {
        return FALSO;
    }

    (*quantidadeDeVizinhos)++;

    if(custoCandidato < (*custoCorrente)) {
        (*custoCorrente) = custoCandidato;
        (*quantidadeDeMelhorias)++;
        (*houveMelhoria) = VERDADEIRO;

        return VERDADEIRO;
    }

    if(controllerBuscaLocalVShapeReinserirTarefa(solucao,posicaoDestino,posicaoOrigem) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalVShapeTentarOrdenacao(Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas,Custo *custoCorrente,ResultadoBuscaLocalVShape *resultado,Boolean *houveMelhoria) {
    QuantidadeDeTarefas posicaoOrigem;
    QuantidadeDeTarefas posicaoDestino;

    (*houveMelhoria) = FALSO;

    for(posicaoOrigem = 0;posicaoOrigem < quantidadeDeTarefasAdiantadas && (*houveMelhoria) == FALSO;posicaoOrigem++) {
        posicaoDestino = controllerBuscaLocalVShapeCalcularDestinoNoLadoAdiantado(solucao,tarefasPorIdentificador,posicaoOrigem,quantidadeDeTarefasAdiantadas);

        if(controllerBuscaLocalVShapeTestarReinsercao(solucao,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,posicaoOrigem,posicaoDestino,custoCorrente,&((*resultado).quantidadeDeVizinhosDeOrdenacao),&((*resultado).quantidadeDeMelhoriasDeOrdenacao),houveMelhoria) == FALSO) {
            return FALSO;
        }
    }

    for(posicaoOrigem = quantidadeDeTarefasAdiantadas;posicaoOrigem < (*solucao).quantidadeDeTarefas && (*houveMelhoria) == FALSO;posicaoOrigem++) {
        posicaoDestino = controllerBuscaLocalVShapeCalcularDestinoNoLadoAtrasado(solucao,tarefasPorIdentificador,posicaoOrigem,quantidadeDeTarefasAdiantadas);

        if(controllerBuscaLocalVShapeTestarReinsercao(solucao,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,posicaoOrigem,posicaoDestino,custoCorrente,&((*resultado).quantidadeDeVizinhosDeOrdenacao),&((*resultado).quantidadeDeMelhoriasDeOrdenacao),houveMelhoria) == FALSO) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalVShapeTentarFronteira(Solucao *solucao,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,QuantidadeDeTarefas quantidadeDeTarefasAdiantadas,Custo *custoCorrente,ResultadoBuscaLocalVShape *resultado,Boolean *houveMelhoria) {
    QuantidadeDeTarefas posicaoOrigem;
    QuantidadeDeTarefas primeiraPosicaoDestino;
    QuantidadeDeTarefas segundaPosicaoDestino;

    (*houveMelhoria) = FALSO;

    if(quantidadeDeTarefasAdiantadas == 0) {
        primeiraPosicaoDestino = 0;
        segundaPosicaoDestino = 0;
    }
    else if(quantidadeDeTarefasAdiantadas >= (*solucao).quantidadeDeTarefas) {
        primeiraPosicaoDestino = (QuantidadeDeTarefas) ((*solucao).quantidadeDeTarefas - 1);
        segundaPosicaoDestino = primeiraPosicaoDestino;
    }
    else {
        primeiraPosicaoDestino = (QuantidadeDeTarefas) (quantidadeDeTarefasAdiantadas - 1);
        segundaPosicaoDestino = quantidadeDeTarefasAdiantadas;
    }

    for(posicaoOrigem = 0;posicaoOrigem < (*solucao).quantidadeDeTarefas && (*houveMelhoria) == FALSO;posicaoOrigem++) {
        if(controllerBuscaLocalVShapeTestarReinsercao(solucao,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,posicaoOrigem,primeiraPosicaoDestino,custoCorrente,&((*resultado).quantidadeDeVizinhosDeFronteira),&((*resultado).quantidadeDeMelhoriasDeFronteira),houveMelhoria) == FALSO) {
            return FALSO;
        }

        if((*houveMelhoria) == FALSO && segundaPosicaoDestino != primeiraPosicaoDestino) {
            if(controllerBuscaLocalVShapeTestarReinsercao(solucao,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,posicaoOrigem,segundaPosicaoDestino,custoCorrente,&((*resultado).quantidadeDeVizinhosDeFronteira),&((*resultado).quantidadeDeMelhoriasDeFronteira),houveMelhoria) == FALSO) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

static Boolean controllerBuscaLocalVShapeExecutarBuscaComposta(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalVShape *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca) {
    ResultadoBuscaLocal resultadoBuscaLocal;

    resultadoBuscaLocal = criarResultadoBuscaLocalVazio();

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(instancia,fatorH,solucaoInicial,solucaoFinal,&resultadoBuscaLocal,raioDeReinsercao,raioDeTroca) == FALSO) {
        return FALSO;
    }

    (*resultado).quantidadeDeChamadasDaBuscaComposta++;
    (*resultado).quantidadeDeVizinhosDaBuscaComposta += resultadoBuscaLocal.quantidadeDeVizinhosAvaliados;

    if((*resultado).quantidadeDeChamadasDaBuscaComposta == 1) {
        (*resultado).custoInicial = resultadoBuscaLocal.custoInicial;
        (*resultado).custoAposPrimeiraBuscaComposta = resultadoBuscaLocal.custoFinal;
    }

    return VERDADEIRO;
}

ResultadoBuscaLocalVShape criarResultadoBuscaLocalVShapeVazio(void) {
    ResultadoBuscaLocalVShape resultado;

    resultado.custoInicial = 0;
    resultado.custoAposPrimeiraBuscaComposta = 0;
    resultado.custoFinal = 0;
    resultado.quantidadeDeCiclos = 0;
    resultado.quantidadeDeChamadasDaBuscaComposta = 0;
    resultado.quantidadeDeVizinhosDaBuscaComposta = 0;
    resultado.quantidadeDeVizinhosDeOrdenacao = 0;
    resultado.quantidadeDeVizinhosDeFronteira = 0;
    resultado.quantidadeDeMelhoriasDeOrdenacao = 0;
    resultado.quantidadeDeMelhoriasDeFronteira = 0;

    return resultado;
}

Boolean controllerBuscaLocalVShapeMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalVShape *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca) {
    Solucao solucaoCorrente;
    Solucao solucaoOtimizada;
    const Tarefa **tarefasPorIdentificador;
    InteiroPositivoDe32Bits *temposPrefixados;
    DataDeEntregaComum dataDeEntregaComum;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;
    Custo custoCorrente;
    Boolean houveMelhoria;

    if(controllerBuscaLocalVShapeParametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultado) == FALSO) {
        return FALSO;
    }

    if(raioDeReinsercao == 0 || raioDeTroca == 0) {
        return FALSO;
    }

    (*resultado) = criarResultadoBuscaLocalVShapeVazio();
    solucaoCorrente = criarSolucaoVazia();
    solucaoOtimizada = criarSolucaoVazia();
    tarefasPorIdentificador = NULL;
    temposPrefixados = NULL;

    if(controllerBuscaLocalVShapeExecutarBuscaComposta(instancia,fatorH,solucaoInicial,&solucaoCorrente,resultado,raioDeReinsercao,raioDeTroca) == FALSO) {
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

    if(controllerBuscaLocalVShapeMontarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    houveMelhoria = VERDADEIRO;

    while(houveMelhoria == VERDADEIRO) {
        houveMelhoria = FALSO;
        (*resultado).quantidadeDeCiclos++;

        if(controllerBuscaLocalVShapeAvaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO) {
            free(temposPrefixados);
            free(tarefasPorIdentificador);
            liberarSolucao(&solucaoCorrente);

            return FALSO;
        }

        if(controllerBuscaLocalVShapeTentarOrdenacao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,quantidadeDeTarefasAdiantadas,&custoCorrente,resultado,&houveMelhoria) == FALSO) {
            free(temposPrefixados);
            free(tarefasPorIdentificador);
            liberarSolucao(&solucaoCorrente);

            return FALSO;
        }

        if(houveMelhoria == FALSO) {
            if(controllerBuscaLocalVShapeTentarFronteira(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,quantidadeDeTarefasAdiantadas,&custoCorrente,resultado,&houveMelhoria) == FALSO) {
                free(temposPrefixados);
                free(tarefasPorIdentificador);
                liberarSolucao(&solucaoCorrente);

                return FALSO;
            }
        }

        if(houveMelhoria == VERDADEIRO) {
            solucaoOtimizada = criarSolucaoVazia();

            if(controllerBuscaLocalVShapeExecutarBuscaComposta(instancia,fatorH,&solucaoCorrente,&solucaoOtimizada,resultado,raioDeReinsercao,raioDeTroca) == FALSO) {
                free(temposPrefixados);
                free(tarefasPorIdentificador);
                liberarSolucao(&solucaoCorrente);

                return FALSO;
            }

            liberarSolucao(&solucaoCorrente);
            solucaoCorrente = solucaoOtimizada;
        }
    }

    if(controllerBuscaLocalVShapeAvaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,NULL) == FALSO) {
        free(temposPrefixados);
        free(tarefasPorIdentificador);
        liberarSolucao(&solucaoCorrente);

        return FALSO;
    }

    (*resultado).custoFinal = custoCorrente;

    if(controllerBuscaLocalVShapeCopiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
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