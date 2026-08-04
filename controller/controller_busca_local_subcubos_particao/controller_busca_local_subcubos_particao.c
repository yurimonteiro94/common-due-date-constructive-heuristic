#include "controller_busca_local_subcubos_particao.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static Boolean parametrosSaoValidos(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalSubcubosParticao *resultadoBuscaLocal) {
    if(instanciaEhValida(instancia) == FALSO) {
        return FALSO;
    }
    if(fatorH != FATOR_H_02 && fatorH != FATOR_H_04 && fatorH != FATOR_H_06 && fatorH != FATOR_H_08) {
        return FALSO;
    }
    if(solucaoEhValida(solucaoInicial) == FALSO || solucaoFinal == NULL || resultadoBuscaLocal == NULL) {
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

static void copiarParticao(Boolean *destino,const Boolean *origem,QuantidadeDeTarefas quantidadeDeTarefas) {
    IdentificadorDeTarefa identificador;
    for(identificador = 0;identificador <= quantidadeDeTarefas;identificador++) {
        destino[identificador] = origem[identificador];
    }
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

QuantidadeDeTarefas controllerBuscaLocalSubcubosParticaoObterTamanhoDoSubcubo(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 20) {
        return quantidadeDeTarefas;
    }
    if(quantidadeDeTarefas <= 50) {
        return TAMANHO_SUBCUBO_50;
    }
    if(quantidadeDeTarefas <= 100) {
        return TAMANHO_SUBCUBO_100;
    }
    if(quantidadeDeTarefas <= 200) {
        return TAMANHO_SUBCUBO_200;
    }
    if(quantidadeDeTarefas <= 500) {
        return TAMANHO_SUBCUBO_500;
    }
    return TAMANHO_SUBCUBO_1000;
}

InteiroPositivoDe8Bits controllerBuscaLocalSubcubosParticaoObterQuantidadeDePaineis(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 20) {
        return QUANTIDADE_PAINEIS_20;
    }
    if(quantidadeDeTarefas <= 50) {
        return QUANTIDADE_PAINEIS_50;
    }
    if(quantidadeDeTarefas <= 100) {
        return QUANTIDADE_PAINEIS_100;
    }
    if(quantidadeDeTarefas <= 200) {
        return QUANTIDADE_PAINEIS_200;
    }
    if(quantidadeDeTarefas <= 500) {
        return QUANTIDADE_PAINEIS_500;
    }
    return QUANTIDADE_PAINEIS_1000;
}

ResultadoBuscaLocalSubcubosParticao criarResultadoBuscaLocalSubcubosParticaoVazio(void) {
    ResultadoBuscaLocalSubcubosParticao resultado;
    resultado.resultadoBase = criarResultadoBuscaLocalVazio();
    resultado.quantidadeDePaineisAvaliados = 0;
    resultado.maiorCardinalidadeAceita = 0;
    resultado.quantidadeDeMovimentosComQuatroOuMaisAceitos = 0;
    return resultado;
}

static Boolean identificadorJaEstaNoPainel(const IdentificadorDeTarefa *painel,QuantidadeDeTarefas quantidade,IdentificadorDeTarefa identificador) {
    QuantidadeDeTarefas indice;
    for(indice = 0;indice < quantidade;indice++) {
        if(painel[indice] == identificador) {
            return VERDADEIRO;
        }
    }
    return FALSO;
}

static void adicionarAoPainelSePossivel(IdentificadorDeTarefa *painel,QuantidadeDeTarefas *quantidade,QuantidadeDeTarefas limite,IdentificadorDeTarefa identificador) {
    if((*quantidade) >= limite || identificador == 0) {
        return;
    }
    if(identificadorJaEstaNoPainel(painel,(*quantidade),identificador) == VERDADEIRO) {
        return;
    }
    painel[(*quantidade)] = identificador;
    (*quantidade)++;
}

static QuantidadeDeTarefas prepararPainelCompleto(const IdentificadorDeTarefa *ordemAdiantada,QuantidadeDeTarefas quantidadeDeTarefas,IdentificadorDeTarefa *painel) {
    QuantidadeDeTarefas indice;
    for(indice = 0;indice < quantidadeDeTarefas;indice++) {
        painel[indice] = ordemAdiantada[indice];
    }
    return quantidadeDeTarefas;
}

static QuantidadeDeTarefas prepararPainelDeInadequacao(const IdentificadorDeTarefa *ordemInadequacaoAdiantada,const IdentificadorDeTarefa *ordemInadequacaoAtrasada,const Boolean *tarefasAdiantadas,QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas limite,QuantidadeDeTarefas deslocamento,IdentificadorDeTarefa *painel) {
    QuantidadeDeTarefas quantidade;
    QuantidadeDeTarefas indiceAdiantado;
    QuantidadeDeTarefas indiceAtrasado;
    IdentificadorDeTarefa identificador;
    quantidade = 0;
    indiceAdiantado = deslocamento;
    indiceAtrasado = deslocamento;
    while(quantidade < limite && (indiceAdiantado < quantidadeDeTarefas || indiceAtrasado < quantidadeDeTarefas)) {
        while(indiceAdiantado < quantidadeDeTarefas) {
            identificador = ordemInadequacaoAdiantada[indiceAdiantado];
            indiceAdiantado++;
            if(tarefasAdiantadas[identificador] == VERDADEIRO) {
                adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
                break;
            }
        }
        while(indiceAtrasado < quantidadeDeTarefas) {
            identificador = ordemInadequacaoAtrasada[indiceAtrasado];
            indiceAtrasado++;
            if(tarefasAdiantadas[identificador] == FALSO) {
                adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
                break;
            }
        }
    }
    return quantidade;
}

static QuantidadeDeTarefas prepararPainelDaFronteira(const IdentificadorDeTarefa *ordemAdiantada,const IdentificadorDeTarefa *ordemAtrasada,const Boolean *tarefasAdiantadas,QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas limite,IdentificadorDeTarefa *painel) {
    QuantidadeDeTarefas quantidade;
    QuantidadeDeTarefas indiceReverso;
    QuantidadeDeTarefas indice;
    IdentificadorDeTarefa identificador;
    quantidade = 0;
    indiceReverso = quantidadeDeTarefas;
    indice = 0;
    while(quantidade < limite && (indiceReverso > 0 || indice < quantidadeDeTarefas)) {
        while(indiceReverso > 0) {
            indiceReverso--;
            identificador = ordemAdiantada[indiceReverso];
            if(tarefasAdiantadas[identificador] == VERDADEIRO) {
                adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
                break;
            }
        }
        while(indice < quantidadeDeTarefas) {
            identificador = ordemAtrasada[indice];
            indice++;
            if(tarefasAdiantadas[identificador] == FALSO) {
                adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
                break;
            }
        }
    }
    return quantidade;
}

static QuantidadeDeTarefas prepararPainelDosExtremos(const IdentificadorDeTarefa *ordemInadequacaoAdiantada,const IdentificadorDeTarefa *ordemInadequacaoAtrasada,const Boolean *tarefasAdiantadas,QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas limite,IdentificadorDeTarefa *painel) {
    QuantidadeDeTarefas quantidade;
    QuantidadeDeTarefas indice;
    QuantidadeDeTarefas indiceReverso;
    IdentificadorDeTarefa identificador;
    quantidade = 0;
    indice = 0;
    indiceReverso = quantidadeDeTarefas;
    while(quantidade < limite && (indice < quantidadeDeTarefas || indiceReverso > 0)) {
        if(indice < quantidadeDeTarefas) {
            identificador = ordemInadequacaoAdiantada[indice];
            if(tarefasAdiantadas[identificador] == VERDADEIRO) {
                adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
            }
            identificador = ordemInadequacaoAtrasada[indice];
            if(tarefasAdiantadas[identificador] == FALSO) {
                adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
            }
            indice++;
        }
        if(indiceReverso > 0) {
            indiceReverso--;
            identificador = ordemInadequacaoAdiantada[indiceReverso];
            if(tarefasAdiantadas[identificador] == VERDADEIRO) {
                adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
            }
            identificador = ordemInadequacaoAtrasada[indiceReverso];
            if(tarefasAdiantadas[identificador] == FALSO) {
                adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
            }
        }
    }
    return quantidade;
}

static QuantidadeDeTarefas completarPainel(IdentificadorDeTarefa *painel,QuantidadeDeTarefas quantidade,QuantidadeDeTarefas limite,QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas deslocamento) {
    QuantidadeDeTarefas passo;
    IdentificadorDeTarefa identificador;
    for(passo = 0;passo < quantidadeDeTarefas && quantidade < limite;passo++) {
        identificador = (IdentificadorDeTarefa) (((deslocamento + passo) % quantidadeDeTarefas) + 1);
        adicionarAoPainelSePossivel(painel,&quantidade,limite,identificador);
    }
    return quantidade;
}

static QuantidadeDeTarefas prepararPainel(InteiroPositivoDe8Bits indiceDoPainel,const IdentificadorDeTarefa *ordemAdiantada,const IdentificadorDeTarefa *ordemAtrasada,const IdentificadorDeTarefa *ordemInadequacaoAdiantada,const IdentificadorDeTarefa *ordemInadequacaoAtrasada,const Boolean *tarefasAdiantadas,QuantidadeDeTarefas quantidadeDeTarefas,QuantidadeDeTarefas limite,IdentificadorDeTarefa *painel) {
    QuantidadeDeTarefas quantidade;
    QuantidadeDeTarefas deslocamento;
    if(quantidadeDeTarefas <= 20) {
        return prepararPainelCompleto(ordemAdiantada,quantidadeDeTarefas,painel);
    }
    quantidade = 0;
    deslocamento = ((QuantidadeDeTarefas) indiceDoPainel) * ((limite / 3) + 1);
    if(indiceDoPainel == 0) {
        quantidade = prepararPainelDeInadequacao(ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,tarefasAdiantadas,quantidadeDeTarefas,limite,0,painel);
    }
    else if(indiceDoPainel == 1) {
        quantidade = prepararPainelDaFronteira(ordemAdiantada,ordemAtrasada,tarefasAdiantadas,quantidadeDeTarefas,limite,painel);
    }
    else if(indiceDoPainel == 2) {
        quantidade = prepararPainelDeInadequacao(ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,tarefasAdiantadas,quantidadeDeTarefas,limite,deslocamento,painel);
    }
    else {
        quantidade = prepararPainelDosExtremos(ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,tarefasAdiantadas,quantidadeDeTarefas,limite,painel);
    }
    return completarPainel(painel,quantidade,limite,quantidadeDeTarefas,deslocamento);
}

static QuantidadeDeTarefas contarBits(uint32_t mascara) {
    QuantidadeDeTarefas quantidade;
    quantidade = 0;
    while(mascara != 0u) {
        quantidade = (QuantidadeDeTarefas) (quantidade + (QuantidadeDeTarefas) (mascara & 1u));
        mascara >>= 1u;
    }
    return quantidade;
}

static void alternarTarefa(Boolean *tarefasAdiantadas,IdentificadorDeTarefa identificador) {
    tarefasAdiantadas[identificador] = tarefasAdiantadas[identificador] == VERDADEIRO ? FALSO : VERDADEIRO;
}

static void alternarMascara(Boolean *tarefasAdiantadas,const IdentificadorDeTarefa *painel,QuantidadeDeTarefas quantidadeNoPainel,uint32_t mascara) {
    QuantidadeDeTarefas indice;
    for(indice = 0;indice < quantidadeNoPainel;indice++) {
        if((mascara & (1u << indice)) != 0u) {
            alternarTarefa(tarefasAdiantadas,painel[indice]);
        }
    }
}

static Boolean avaliarParticaoCandidata(Solucao *solucaoCandidata,const Boolean *tarefasAdiantadas,const IdentificadorDeTarefa *ordemAdiantada,const IdentificadorDeTarefa *ordemAtrasada,const Tarefa **tarefasPorIdentificador,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposPrefixados,Custo *custoCandidato) {
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;
    if(construirSequenciaDaParticao(solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO) {
        return FALSO;
    }
    return avaliarSolucao(solucaoCandidata,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,custoCandidato,&quantidadeDeTarefasAdiantadas);
}

static void liberarRecursos(const Tarefa **tarefasPorIdentificador,IdentificadorDeTarefa *ordemAdiantada,IdentificadorDeTarefa *ordemAtrasada,IdentificadorDeTarefa *ordemInadequacaoAdiantada,IdentificadorDeTarefa *ordemInadequacaoAtrasada,IdentificadorDeTarefa *painel,InteiroPositivoDe32Bits *temposPrefixados,Boolean *tarefasAdiantadas,Boolean *melhorParticao,Solucao *solucaoCandidata,Solucao *solucaoCanonicaInicial,Solucao *solucaoCorrente) {
    free(melhorParticao);
    free(tarefasAdiantadas);
    free(temposPrefixados);
    free(painel);
    free(ordemInadequacaoAtrasada);
    free(ordemInadequacaoAdiantada);
    free(ordemAtrasada);
    free(ordemAdiantada);
    free((void *) tarefasPorIdentificador);
    liberarSolucao(solucaoCandidata);
    liberarSolucao(solucaoCanonicaInicial);
    liberarSolucao(solucaoCorrente);
}

Boolean controllerBuscaLocalSubcubosParticaoMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalSubcubosParticao *resultadoBuscaLocal) {
    Solucao solucaoCorrente;
    Solucao solucaoCanonicaInicial;
    Solucao solucaoCandidata;
    const Tarefa **tarefasPorIdentificador;
    IdentificadorDeTarefa *ordemAdiantada;
    IdentificadorDeTarefa *ordemAtrasada;
    IdentificadorDeTarefa *ordemInadequacaoAdiantada;
    IdentificadorDeTarefa *ordemInadequacaoAtrasada;
    IdentificadorDeTarefa *painel;
    InteiroPositivoDe32Bits *temposPrefixados;
    Boolean *tarefasAdiantadas;
    Boolean *melhorParticao;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoOriginal;
    Custo custoCanonicoInicial;
    Custo custoCorrente;
    Custo melhorCusto;
    Custo custoCandidato;
    QuantidadeDeTarefas quantidadeDeTarefasAdiantadas;
    QuantidadeDeTarefas tamanhoDoSubcubo;
    QuantidadeDeTarefas quantidadeNoPainel;
    QuantidadeDeTarefas cardinalidade;
    QuantidadeDeTarefas cardinalidadeDoMelhorMovimento;
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificador;
    InteiroPositivoDe8Bits quantidadeDePaineis;
    InteiroPositivoDe8Bits indiceDoPainel;
    uint32_t mascara;
    uint32_t limiteDeMascaras;
    Boolean encontrouMelhoria;
    Boolean sucesso;
    if(parametrosSaoValidos(instancia,fatorH,solucaoInicial,solucaoFinal,resultadoBuscaLocal) == FALSO) {
        return FALSO;
    }
    (*resultadoBuscaLocal) = criarResultadoBuscaLocalSubcubosParticaoVazio();
    solucaoCorrente = criarSolucaoVazia();
    solucaoCanonicaInicial = criarSolucaoVazia();
    solucaoCandidata = criarSolucaoVazia();
    tarefasPorIdentificador = NULL;
    ordemAdiantada = NULL;
    ordemAtrasada = NULL;
    ordemInadequacaoAdiantada = NULL;
    ordemInadequacaoAtrasada = NULL;
    painel = NULL;
    temposPrefixados = NULL;
    tarefasAdiantadas = NULL;
    melhorParticao = NULL;
    sucesso = FALSO;
    if(copiarSolucao(solucaoInicial,&solucaoCorrente) == FALSO || copiarSolucao(solucaoInicial,&solucaoCanonicaInicial) == FALSO || copiarSolucao(solucaoInicial,&solucaoCandidata) == FALSO) {
        liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
        return FALSO;
    }
    tarefasPorIdentificador = (const Tarefa **) malloc(sizeof(Tarefa *) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));
    ordemAdiantada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemAtrasada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemInadequacaoAdiantada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    ordemInadequacaoAtrasada = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    painel = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    temposPrefixados = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * (*instancia).quantidadeDeTarefas);
    tarefasAdiantadas = (Boolean *) malloc(sizeof(Boolean) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));
    melhorParticao = (Boolean *) malloc(sizeof(Boolean) * ((size_t) (*instancia).quantidadeDeTarefas + 1u));
    if(tarefasPorIdentificador == NULL || ordemAdiantada == NULL || ordemAtrasada == NULL || ordemInadequacaoAdiantada == NULL || ordemInadequacaoAtrasada == NULL || painel == NULL || temposPrefixados == NULL || tarefasAdiantadas == NULL || melhorParticao == NULL) {
        liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
        return FALSO;
    }
    if(montarMapaDeTarefas(instancia,tarefasPorIdentificador) == FALSO || construirOrdens(instancia,tarefasPorIdentificador,ordemAdiantada,ordemAtrasada) == FALSO || construirOrdensDeInadequacao(instancia,tarefasPorIdentificador,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada) == FALSO) {
        liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
        return FALSO;
    }
    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);
    if(avaliarSolucao(solucaoInicial,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoOriginal,&quantidadeDeTarefasAdiantadas) == FALSO) {
        liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
        return FALSO;
    }
    for(identificador = 0;identificador <= (*instancia).quantidadeDeTarefas;identificador++) {
        tarefasAdiantadas[identificador] = FALSO;
    }
    for(posicao = 0;posicao < quantidadeDeTarefasAdiantadas;posicao++) {
        tarefasAdiantadas[(*solucaoInicial).sequenciaDeTarefas[posicao]] = VERDADEIRO;
    }
    if(construirSequenciaDaParticao(&solucaoCanonicaInicial,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO || avaliarSolucao(&solucaoCanonicaInicial,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCanonicoInicial,&quantidadeDeTarefasAdiantadas) == FALSO) {
        liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
        return FALSO;
    }
    if(custoOriginal < custoCanonicoInicial) {
        custoCorrente = custoOriginal;
        if(copiarSequencia(solucaoInicial,&solucaoCorrente) == FALSO) {
            liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
            return FALSO;
        }
    }
    else {
        custoCorrente = custoCanonicoInicial;
        if(copiarSequencia(&solucaoCanonicaInicial,&solucaoCorrente) == FALSO) {
            liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
            return FALSO;
        }
    }
    (*resultadoBuscaLocal).resultadoBase.custoInicial = custoOriginal;
    tamanhoDoSubcubo = controllerBuscaLocalSubcubosParticaoObterTamanhoDoSubcubo((*instancia).quantidadeDeTarefas);
    quantidadeDePaineis = controllerBuscaLocalSubcubosParticaoObterQuantidadeDePaineis((*instancia).quantidadeDeTarefas);
    encontrouMelhoria = VERDADEIRO;
    while(encontrouMelhoria == VERDADEIRO) {
        (*resultadoBuscaLocal).resultadoBase.quantidadeDeIteracoes++;
        encontrouMelhoria = FALSO;
        melhorCusto = custoCorrente;
        cardinalidadeDoMelhorMovimento = 0;
        copiarParticao(melhorParticao,tarefasAdiantadas,(*instancia).quantidadeDeTarefas);
        for(identificador = 1;identificador <= (*instancia).quantidadeDeTarefas;identificador++) {
            alternarTarefa(tarefasAdiantadas,identificador);
            if(avaliarParticaoCandidata(&solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
                return FALSO;
            }
            (*resultadoBuscaLocal).resultadoBase.quantidadeDeVizinhosAvaliados++;
            (*resultadoBuscaLocal).resultadoBase.quantidadeDeVizinhosPorReinsercao++;
            if(custoCandidato < melhorCusto) {
                melhorCusto = custoCandidato;
                cardinalidadeDoMelhorMovimento = 1;
                copiarParticao(melhorParticao,tarefasAdiantadas,(*instancia).quantidadeDeTarefas);
                encontrouMelhoria = VERDADEIRO;
            }
            alternarTarefa(tarefasAdiantadas,identificador);
        }
        for(indiceDoPainel = 0;indiceDoPainel < quantidadeDePaineis;indiceDoPainel++) {
            quantidadeNoPainel = prepararPainel(indiceDoPainel,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,tarefasAdiantadas,(*instancia).quantidadeDeTarefas,tamanhoDoSubcubo,painel);
            if(quantidadeNoPainel == 0 || quantidadeNoPainel > 20) {
                continue;
            }
            (*resultadoBuscaLocal).quantidadeDePaineisAvaliados++;
            limiteDeMascaras = 1u << quantidadeNoPainel;
            for(mascara = 1u;mascara < limiteDeMascaras;mascara++) {
                cardinalidade = contarBits(mascara);
                if(cardinalidade < 2) {
                    continue;
                }
                alternarMascara(tarefasAdiantadas,painel,quantidadeNoPainel,mascara);
                if(avaliarParticaoCandidata(&solucaoCandidata,tarefasAdiantadas,ordemAdiantada,ordemAtrasada,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCandidato) == FALSO) {
                    liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
                    return FALSO;
                }
                (*resultadoBuscaLocal).resultadoBase.quantidadeDeVizinhosAvaliados++;
                (*resultadoBuscaLocal).resultadoBase.quantidadeDeVizinhosPorTroca++;
                if(custoCandidato < melhorCusto) {
                    melhorCusto = custoCandidato;
                    cardinalidadeDoMelhorMovimento = cardinalidade;
                    copiarParticao(melhorParticao,tarefasAdiantadas,(*instancia).quantidadeDeTarefas);
                    encontrouMelhoria = VERDADEIRO;
                }
                alternarMascara(tarefasAdiantadas,painel,quantidadeNoPainel,mascara);
            }
        }
        if(encontrouMelhoria == VERDADEIRO) {
            copiarParticao(tarefasAdiantadas,melhorParticao,(*instancia).quantidadeDeTarefas);
            if(construirSequenciaDaParticao(&solucaoCorrente,tarefasAdiantadas,ordemAdiantada,ordemAtrasada) == FALSO || avaliarSolucao(&solucaoCorrente,tarefasPorIdentificador,dataDeEntregaComum,temposPrefixados,&custoCorrente,&quantidadeDeTarefasAdiantadas) == FALSO || custoCorrente != melhorCusto) {
                liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
                return FALSO;
            }
            if(cardinalidadeDoMelhorMovimento == 1) {
                (*resultadoBuscaLocal).resultadoBase.quantidadeDeMelhoriasPorReinsercao++;
            }
            else {
                (*resultadoBuscaLocal).resultadoBase.quantidadeDeMelhoriasPorTroca++;
            }
            if(cardinalidadeDoMelhorMovimento > (*resultadoBuscaLocal).maiorCardinalidadeAceita) {
                (*resultadoBuscaLocal).maiorCardinalidadeAceita = cardinalidadeDoMelhorMovimento;
            }
            if(cardinalidadeDoMelhorMovimento >= 4) {
                (*resultadoBuscaLocal).quantidadeDeMovimentosComQuatroOuMaisAceitos++;
            }
            if((*instancia).quantidadeDeTarefas <= 20) {
                encontrouMelhoria = FALSO;
            }
        }
    }
    if(custoOriginal < custoCorrente) {
        (*resultadoBuscaLocal).resultadoBase.custoFinal = custoOriginal;
        if(copiarSolucao(solucaoInicial,solucaoFinal) == FALSO) {
            liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
            return FALSO;
        }
    }
    else {
        (*resultadoBuscaLocal).resultadoBase.custoFinal = custoCorrente;
        if(copiarSolucao(&solucaoCorrente,solucaoFinal) == FALSO) {
            liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
            return FALSO;
        }
    }
    sucesso = VERDADEIRO;
    liberarRecursos(tarefasPorIdentificador,ordemAdiantada,ordemAtrasada,ordemInadequacaoAdiantada,ordemInadequacaoAtrasada,painel,temposPrefixados,tarefasAdiantadas,melhorParticao,&solucaoCandidata,&solucaoCanonicaInicial,&solucaoCorrente);
    return sucesso;
}