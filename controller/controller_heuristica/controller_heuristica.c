#include "controller_heuristica.h"

#include <stdlib.h>

#define LIMITE_AVALIACAO_COMPLETA 120
#define QUANTIDADE_DE_AMOSTRAS_GLOBAIS 60
#define RAIO_DA_JANELA_TEMPORAL 20
#define RAIO_DA_JANELA_V_SHAPED 12
#define QUANTIDADE_MAXIMA_DE_TAREFAS_CANDIDATAS 4

typedef struct TarefaParaInsercao {
    IdentificadorDeTarefa identificador;
    TempoDeProcessamento tempoProcessamento;
    Penalidade penalidadeAdiantamento;
    Penalidade penalidadeAtraso;
    double chavePrioridade;
} TarefaParaInsercao;

static Boolean controllerHeuristicaParametrosSaoValidos(const Instancia *instancia,const Heuristica *heuristica,FatorH fatorH,Solucao *solucao) {
    if(instancia == NULL) {
        return FALSO;
    }

    if(heuristica == NULL) {
        return FALSO;
    }

    if(heuristicaEhValida(heuristica) == FALSO) {
        return FALSO;
    }

    if(fatorH != FATOR_H_02 && fatorH != FATOR_H_04 && fatorH != FATOR_H_06 && fatorH != FATOR_H_08) {
        return FALSO;
    }

    if(solucao == NULL) {
        return FALSO;
    }

    if((*instancia).tarefas == NULL) {
        return FALSO;
    }

    if((*instancia).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    return VERDADEIRO;
}

static double controllerHeuristicaCalcularChavePrioridade(const Tarefa *tarefa) {
    double tempoProcessamento;
    double penalidadeAdiantamento;
    double penalidadeAtraso;

    if(tarefa == NULL) {
        return 0.0;
    }

    tempoProcessamento = (double) (*tarefa).tempoProcessamento;
    penalidadeAdiantamento = (double) (*tarefa).penalidadeAdiantamento;
    penalidadeAtraso = (double) (*tarefa).penalidadeAtraso;

    return tempoProcessamento * (penalidadeAdiantamento + penalidadeAtraso);
}

static int controllerHeuristicaCompararPrioridadeDecrescente(const void *primeiro,const void *segundo) {
    const TarefaParaInsercao *tarefaA;
    const TarefaParaInsercao *tarefaB;

    tarefaA = (const TarefaParaInsercao *) primeiro;
    tarefaB = (const TarefaParaInsercao *) segundo;

    if((*tarefaA).chavePrioridade > (*tarefaB).chavePrioridade) {
        return -1;
    }

    if((*tarefaA).chavePrioridade < (*tarefaB).chavePrioridade) {
        return 1;
    }

    if((*tarefaA).tempoProcessamento > (*tarefaB).tempoProcessamento) {
        return -1;
    }

    if((*tarefaA).tempoProcessamento < (*tarefaB).tempoProcessamento) {
        return 1;
    }

    if((*tarefaA).identificador < (*tarefaB).identificador) {
        return -1;
    }

    if((*tarefaA).identificador > (*tarefaB).identificador) {
        return 1;
    }

    return 0;
}

static Boolean controllerHeuristicaMontarTarefasPorIdentificador(const Instancia *instancia,const Tarefa **tarefasPorIdentificador) {
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

    for(indiceDaTarefa = 1;indiceDaTarefa <= (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        if(tarefasPorIdentificador[indiceDaTarefa] == NULL) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

static Boolean controllerHeuristicaCriarOrdemDePrioridade(const Instancia *instancia,TarefaParaInsercao *tarefasOrdenadas) {
    QuantidadeDeTarefas indiceDaTarefa;
    const Tarefa *tarefa;

    if(instancia == NULL) {
        return FALSO;
    }

    if(tarefasOrdenadas == NULL) {
        return FALSO;
    }

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        tarefa = &((*instancia).tarefas[indiceDaTarefa]);

        tarefasOrdenadas[indiceDaTarefa].identificador = (*tarefa).identificador;
        tarefasOrdenadas[indiceDaTarefa].tempoProcessamento = (*tarefa).tempoProcessamento;
        tarefasOrdenadas[indiceDaTarefa].penalidadeAdiantamento = (*tarefa).penalidadeAdiantamento;
        tarefasOrdenadas[indiceDaTarefa].penalidadeAtraso = (*tarefa).penalidadeAtraso;
        tarefasOrdenadas[indiceDaTarefa].chavePrioridade = controllerHeuristicaCalcularChavePrioridade(tarefa);
    }

    qsort(tarefasOrdenadas,(*instancia).quantidadeDeTarefas,sizeof(TarefaParaInsercao),controllerHeuristicaCompararPrioridadeDecrescente);

    return VERDADEIRO;
}

static Boolean controllerHeuristicaCalcularMelhorCustoDaSequencia(const Tarefa **tarefasPorIdentificador,const IdentificadorDeTarefa *sequencia,QuantidadeDeTarefas quantidadeDeTarefas,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *temposDeConclusaoCompactos,Custo *custo) {
    QuantidadeDeTarefas indiceDaSequencia;
    QuantidadeDeTarefas indiceReverso;
    IdentificadorDeTarefa identificadorDaTarefa;
    const Tarefa *tarefa;
    InteiroPositivoDe32Bits tempoAcumulado;
    InteiroPositivoDe32Bits proximoInstanteInicial;
    InteiroPositivoDe32Bits instanteInicialAtual;
    long long custoAtual;
    long long melhorCusto;
    long long inclinacao;
    long long delta;

    if(tarefasPorIdentificador == NULL) {
        return FALSO;
    }

    if(sequencia == NULL) {
        return FALSO;
    }

    if(temposDeConclusaoCompactos == NULL) {
        return FALSO;
    }

    if(custo == NULL) {
        return FALSO;
    }

    if(quantidadeDeTarefas == 0) {
        return FALSO;
    }

    tempoAcumulado = 0;
    custoAtual = 0;
    inclinacao = 0;

    for(indiceDaSequencia = 0;indiceDaSequencia < quantidadeDeTarefas;indiceDaSequencia++) {
        identificadorDaTarefa = sequencia[indiceDaSequencia];

        if(identificadorDaTarefa == 0) {
            return FALSO;
        }

        tarefa = tarefasPorIdentificador[identificadorDaTarefa];

        if(tarefa == NULL) {
            return FALSO;
        }

        tempoAcumulado += (*tarefa).tempoProcessamento;
        temposDeConclusaoCompactos[indiceDaSequencia] = tempoAcumulado;

        if(tempoAcumulado < dataDeEntregaComum) {
            custoAtual += ((long long) (*tarefa).penalidadeAdiantamento) * ((long long) (dataDeEntregaComum - tempoAcumulado));
            inclinacao -= (long long) (*tarefa).penalidadeAdiantamento;
        }
        else if(tempoAcumulado > dataDeEntregaComum) {
            custoAtual += ((long long) (*tarefa).penalidadeAtraso) * ((long long) (tempoAcumulado - dataDeEntregaComum));
            inclinacao += (long long) (*tarefa).penalidadeAtraso;
        }
        else {
            inclinacao += (long long) (*tarefa).penalidadeAtraso;
        }
    }

    melhorCusto = custoAtual;
    instanteInicialAtual = 0;

    indiceReverso = quantidadeDeTarefas;

    while(indiceReverso > 0) {
        indiceReverso--;

        if(temposDeConclusaoCompactos[indiceReverso] < dataDeEntregaComum) {
            proximoInstanteInicial = dataDeEntregaComum - temposDeConclusaoCompactos[indiceReverso];

            if(proximoInstanteInicial > instanteInicialAtual) {
                delta = (long long) (proximoInstanteInicial - instanteInicialAtual);
                custoAtual += inclinacao * delta;
                instanteInicialAtual = proximoInstanteInicial;

                if(custoAtual < melhorCusto) {
                    melhorCusto = custoAtual;
                }
            }

            identificadorDaTarefa = sequencia[indiceReverso];
            tarefa = tarefasPorIdentificador[identificadorDaTarefa];

            if(tarefa == NULL) {
                return FALSO;
            }

            inclinacao += ((long long) (*tarefa).penalidadeAdiantamento) + ((long long) (*tarefa).penalidadeAtraso);
        }
    }

    if(melhorCusto < 0) {
        melhorCusto = 0;
    }

    (*custo) = (Custo) melhorCusto;

    return VERDADEIRO;
}

static Boolean controllerHeuristicaMontarSequenciaTemporaria(const IdentificadorDeTarefa *sequenciaAtual,QuantidadeDeTarefas quantidadeAtual,IdentificadorDeTarefa identificadorDaTarefa,QuantidadeDeTarefas posicaoDeInsercao,IdentificadorDeTarefa *sequenciaTemporaria) {
    QuantidadeDeTarefas indiceDaSequencia;

    if(sequenciaAtual == NULL && quantidadeAtual > 0) {
        return FALSO;
    }

    if(sequenciaTemporaria == NULL) {
        return FALSO;
    }

    if(posicaoDeInsercao > quantidadeAtual) {
        return FALSO;
    }

    for(indiceDaSequencia = 0;indiceDaSequencia < posicaoDeInsercao;indiceDaSequencia++) {
        sequenciaTemporaria[indiceDaSequencia] = sequenciaAtual[indiceDaSequencia];
    }

    sequenciaTemporaria[posicaoDeInsercao] = identificadorDaTarefa;

    for(indiceDaSequencia = posicaoDeInsercao;indiceDaSequencia < quantidadeAtual;indiceDaSequencia++) {
        sequenciaTemporaria[indiceDaSequencia + 1] = sequenciaAtual[indiceDaSequencia];
    }

    return VERDADEIRO;
}

static Boolean controllerHeuristicaInserirTarefaNaSequencia(IdentificadorDeTarefa *sequencia,QuantidadeDeTarefas quantidadeAtual,IdentificadorDeTarefa identificadorDaTarefa,QuantidadeDeTarefas posicaoDeInsercao) {
    QuantidadeDeTarefas posicaoAtual;

    if(sequencia == NULL) {
        return FALSO;
    }

    if(posicaoDeInsercao > quantidadeAtual) {
        return FALSO;
    }

    posicaoAtual = quantidadeAtual;

    while(posicaoAtual > posicaoDeInsercao) {
        sequencia[posicaoAtual] = sequencia[posicaoAtual - 1];
        posicaoAtual--;
    }

    sequencia[posicaoDeInsercao] = identificadorDaTarefa;

    return VERDADEIRO;
}

static void controllerHeuristicaAdicionarPosicaoCandidata(QuantidadeDeTarefas *posicoesCandidatas,QuantidadeDeTarefas *quantidadeDePosicoes,QuantidadeDeTarefas quantidadeAtual,int posicao) {
    QuantidadeDeTarefas indiceDaPosicao;
    QuantidadeDeTarefas posicaoAjustada;

    if(posicoesCandidatas == NULL) {
        return;
    }

    if(quantidadeDePosicoes == NULL) {
        return;
    }

    if(posicao < 0) {
        posicaoAjustada = 0;
    }
    else if(posicao > (int) quantidadeAtual) {
        posicaoAjustada = quantidadeAtual;
    }
    else {
        posicaoAjustada = (QuantidadeDeTarefas) posicao;
    }

    for(indiceDaPosicao = 0;indiceDaPosicao < (*quantidadeDePosicoes);indiceDaPosicao++) {
        if(posicoesCandidatas[indiceDaPosicao] == posicaoAjustada) {
            return;
        }
    }

    posicoesCandidatas[*quantidadeDePosicoes] = posicaoAjustada;
    (*quantidadeDePosicoes)++;
}

static QuantidadeDeTarefas controllerHeuristicaEncontrarPosicaoPorAlvoTemporal(const Tarefa **tarefasPorIdentificador,const IdentificadorDeTarefa *sequenciaAtual,QuantidadeDeTarefas quantidadeAtual,InteiroPositivoDe32Bits alvoTemporal) {
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificadorDaTarefa;
    const Tarefa *tarefa;
    InteiroPositivoDe32Bits tempoAcumulado;

    if(tarefasPorIdentificador == NULL) {
        return 0;
    }

    if(sequenciaAtual == NULL && quantidadeAtual > 0) {
        return 0;
    }

    if(alvoTemporal == 0) {
        return 0;
    }

    tempoAcumulado = 0;

    for(posicao = 0;posicao < quantidadeAtual;posicao++) {
        identificadorDaTarefa = sequenciaAtual[posicao];
        tarefa = tarefasPorIdentificador[identificadorDaTarefa];

        if(tarefa == NULL) {
            return quantidadeAtual;
        }

        tempoAcumulado += (*tarefa).tempoProcessamento;

        if(tempoAcumulado >= alvoTemporal) {
            return posicao;
        }
    }

    return quantidadeAtual;
}


static Boolean controllerHeuristicaTarefaNovaDeveVirAntesPorRazaoAdiantamento(const TarefaParaInsercao *tarefaNova,const Tarefa *tarefaAtual) {
    unsigned long long ladoNovo;
    unsigned long long ladoAtual;
    double prioridadeAtual;

    if(tarefaNova == NULL) {
        return FALSO;
    }

    if(tarefaAtual == NULL) {
        return VERDADEIRO;
    }

    ladoNovo = ((unsigned long long) (*tarefaNova).tempoProcessamento) * ((unsigned long long) (*tarefaAtual).penalidadeAdiantamento);
    ladoAtual = ((unsigned long long) (*tarefaAtual).tempoProcessamento) * ((unsigned long long) (*tarefaNova).penalidadeAdiantamento);

    if(ladoNovo > ladoAtual) {
        return VERDADEIRO;
    }

    if(ladoNovo < ladoAtual) {
        return FALSO;
    }

    prioridadeAtual = controllerHeuristicaCalcularChavePrioridade(tarefaAtual);

    if((*tarefaNova).chavePrioridade > prioridadeAtual) {
        return VERDADEIRO;
    }

    if((*tarefaNova).chavePrioridade < prioridadeAtual) {
        return FALSO;
    }

    if((*tarefaNova).identificador < (*tarefaAtual).identificador) {
        return VERDADEIRO;
    }

    return FALSO;
}

static Boolean controllerHeuristicaTarefaNovaDeveVirAntesPorRazaoAtraso(const TarefaParaInsercao *tarefaNova,const Tarefa *tarefaAtual) {
    unsigned long long ladoNovo;
    unsigned long long ladoAtual;
    double prioridadeAtual;

    if(tarefaNova == NULL) {
        return FALSO;
    }

    if(tarefaAtual == NULL) {
        return VERDADEIRO;
    }

    ladoNovo = ((unsigned long long) (*tarefaNova).tempoProcessamento) * ((unsigned long long) (*tarefaAtual).penalidadeAtraso);
    ladoAtual = ((unsigned long long) (*tarefaAtual).tempoProcessamento) * ((unsigned long long) (*tarefaNova).penalidadeAtraso);

    if(ladoNovo < ladoAtual) {
        return VERDADEIRO;
    }

    if(ladoNovo > ladoAtual) {
        return FALSO;
    }

    prioridadeAtual = controllerHeuristicaCalcularChavePrioridade(tarefaAtual);

    if((*tarefaNova).chavePrioridade > prioridadeAtual) {
        return VERDADEIRO;
    }

    if((*tarefaNova).chavePrioridade < prioridadeAtual) {
        return FALSO;
    }

    if((*tarefaNova).identificador < (*tarefaAtual).identificador) {
        return VERDADEIRO;
    }

    return FALSO;
}

static QuantidadeDeTarefas controllerHeuristicaEncontrarPosicaoPorRazaoAdiantamento(const Tarefa **tarefasPorIdentificador,const IdentificadorDeTarefa *sequenciaAtual,QuantidadeDeTarefas quantidadeAtual,const TarefaParaInsercao *tarefaNova) {
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificadorDaTarefaAtual;
    const Tarefa *tarefaAtual;

    if(tarefasPorIdentificador == NULL) {
        return 0;
    }

    if(sequenciaAtual == NULL && quantidadeAtual > 0) {
        return 0;
    }

    if(tarefaNova == NULL) {
        return quantidadeAtual;
    }

    for(posicao = 0;posicao < quantidadeAtual;posicao++) {
        identificadorDaTarefaAtual = sequenciaAtual[posicao];

        if(identificadorDaTarefaAtual == 0) {
            return quantidadeAtual;
        }

        tarefaAtual = tarefasPorIdentificador[identificadorDaTarefaAtual];

        if(tarefaAtual == NULL) {
            return quantidadeAtual;
        }

        if(controllerHeuristicaTarefaNovaDeveVirAntesPorRazaoAdiantamento(tarefaNova,tarefaAtual) == VERDADEIRO) {
            return posicao;
        }
    }

    return quantidadeAtual;
}

static QuantidadeDeTarefas controllerHeuristicaEncontrarPosicaoPorRazaoAtraso(const Tarefa **tarefasPorIdentificador,const IdentificadorDeTarefa *sequenciaAtual,QuantidadeDeTarefas quantidadeAtual,const TarefaParaInsercao *tarefaNova) {
    QuantidadeDeTarefas posicao;
    IdentificadorDeTarefa identificadorDaTarefaAtual;
    const Tarefa *tarefaAtual;

    if(tarefasPorIdentificador == NULL) {
        return 0;
    }

    if(sequenciaAtual == NULL && quantidadeAtual > 0) {
        return 0;
    }

    if(tarefaNova == NULL) {
        return quantidadeAtual;
    }

    for(posicao = 0;posicao < quantidadeAtual;posicao++) {
        identificadorDaTarefaAtual = sequenciaAtual[posicao];

        if(identificadorDaTarefaAtual == 0) {
            return quantidadeAtual;
        }

        tarefaAtual = tarefasPorIdentificador[identificadorDaTarefaAtual];

        if(tarefaAtual == NULL) {
            return quantidadeAtual;
        }

        if(controllerHeuristicaTarefaNovaDeveVirAntesPorRazaoAtraso(tarefaNova,tarefaAtual) == VERDADEIRO) {
            return posicao;
        }
    }

    return quantidadeAtual;
}

static void controllerHeuristicaAdicionarJanelaVShaped(QuantidadeDeTarefas *posicoesCandidatas,QuantidadeDeTarefas *quantidadeDePosicoes,QuantidadeDeTarefas quantidadeAtual,QuantidadeDeTarefas posicaoCentral) {
    int deslocamento;

    if(posicoesCandidatas == NULL) {
        return;
    }

    if(quantidadeDePosicoes == NULL) {
        return;
    }

    for(deslocamento = -RAIO_DA_JANELA_V_SHAPED;deslocamento <= RAIO_DA_JANELA_V_SHAPED;deslocamento++) {
        controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,((int) posicaoCentral) + deslocamento);
    }
}

static Boolean controllerHeuristicaMontarPosicoesCandidatas(const Tarefa **tarefasPorIdentificador,const IdentificadorDeTarefa *sequenciaAtual,QuantidadeDeTarefas quantidadeAtual,const TarefaParaInsercao *tarefaNova,DataDeEntregaComum dataDeEntregaComum,QuantidadeDeTarefas *posicoesCandidatas,QuantidadeDeTarefas *quantidadeDePosicoes) {
    QuantidadeDeTarefas posicao;
    QuantidadeDeTarefas passoGlobal;
    QuantidadeDeTarefas posicaoTemporal;
    QuantidadeDeTarefas posicaoPorRazaoAdiantamento;
    QuantidadeDeTarefas posicaoPorRazaoAtraso;
    InteiroPositivoDe32Bits alvoAntes;
    InteiroPositivoDe32Bits alvoNaData;
    InteiroPositivoDe32Bits alvoDepois;
    int deslocamento;

    if(tarefasPorIdentificador == NULL) {
        return FALSO;
    }

    if(posicoesCandidatas == NULL) {
        return FALSO;
    }

    if(quantidadeDePosicoes == NULL) {
        return FALSO;
    }

    if(tarefaNova == NULL) {
        return FALSO;
    }

    (*quantidadeDePosicoes) = 0;

    if(quantidadeAtual <= LIMITE_AVALIACAO_COMPLETA) {
        for(posicao = 0;posicao <= quantidadeAtual;posicao++) {
            controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,(int) posicao);
        }

        return VERDADEIRO;
    }

    controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,0);
    controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,(int) quantidadeAtual);
    controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,(int) (quantidadeAtual / 2));

    passoGlobal = quantidadeAtual / QUANTIDADE_DE_AMOSTRAS_GLOBAIS;

    if(passoGlobal == 0) {
        passoGlobal = 1;
    }

    posicao = 0;

    while(posicao <= quantidadeAtual) {
        controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,(int) posicao);

        if((QuantidadeDeTarefas) (quantidadeAtual - posicao) < passoGlobal) {
            break;
        }

        posicao = (QuantidadeDeTarefas) (posicao + passoGlobal);
    }

    controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,(int) quantidadeAtual);

    posicaoPorRazaoAdiantamento = controllerHeuristicaEncontrarPosicaoPorRazaoAdiantamento(tarefasPorIdentificador,sequenciaAtual,quantidadeAtual,tarefaNova);
    controllerHeuristicaAdicionarJanelaVShaped(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,posicaoPorRazaoAdiantamento);

    posicaoPorRazaoAtraso = controllerHeuristicaEncontrarPosicaoPorRazaoAtraso(tarefasPorIdentificador,sequenciaAtual,quantidadeAtual,tarefaNova);
    controllerHeuristicaAdicionarJanelaVShaped(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,posicaoPorRazaoAtraso);

    if(dataDeEntregaComum > (*tarefaNova).tempoProcessamento) {
        alvoAntes = dataDeEntregaComum - (*tarefaNova).tempoProcessamento;
    }
    else {
        alvoAntes = 0;
    }

    alvoNaData = dataDeEntregaComum;
    alvoDepois = dataDeEntregaComum + (*tarefaNova).tempoProcessamento;

    posicaoTemporal = controllerHeuristicaEncontrarPosicaoPorAlvoTemporal(tarefasPorIdentificador,sequenciaAtual,quantidadeAtual,alvoAntes);

    for(deslocamento = -RAIO_DA_JANELA_TEMPORAL;deslocamento <= RAIO_DA_JANELA_TEMPORAL;deslocamento++) {
        controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,((int) posicaoTemporal) + deslocamento);
    }

    posicaoTemporal = controllerHeuristicaEncontrarPosicaoPorAlvoTemporal(tarefasPorIdentificador,sequenciaAtual,quantidadeAtual,alvoNaData);

    for(deslocamento = -RAIO_DA_JANELA_TEMPORAL;deslocamento <= RAIO_DA_JANELA_TEMPORAL;deslocamento++) {
        controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,((int) posicaoTemporal) + deslocamento);
    }

    posicaoTemporal = controllerHeuristicaEncontrarPosicaoPorAlvoTemporal(tarefasPorIdentificador,sequenciaAtual,quantidadeAtual,alvoDepois);

    for(deslocamento = -RAIO_DA_JANELA_TEMPORAL;deslocamento <= RAIO_DA_JANELA_TEMPORAL;deslocamento++) {
        controllerHeuristicaAdicionarPosicaoCandidata(posicoesCandidatas,quantidadeDePosicoes,quantidadeAtual,((int) posicaoTemporal) + deslocamento);
    }

    return VERDADEIRO;
}

static Boolean controllerHeuristicaMovimentoEhMelhor(Custo custoCandidato,QuantidadeDeTarefas posicaoCandidata,const TarefaParaInsercao *tarefaCandidata,Boolean encontrouMelhorMovimento,Custo melhorCusto,QuantidadeDeTarefas melhorPosicao,const TarefaParaInsercao *melhorTarefa) {
    if(tarefaCandidata == NULL) {
        return FALSO;
    }

    if(encontrouMelhorMovimento == FALSO) {
        return VERDADEIRO;
    }

    if(custoCandidato < melhorCusto) {
        return VERDADEIRO;
    }

    if(custoCandidato > melhorCusto) {
        return FALSO;
    }

    if(melhorTarefa == NULL) {
        return VERDADEIRO;
    }

    if((*tarefaCandidata).chavePrioridade > (*melhorTarefa).chavePrioridade) {
        return VERDADEIRO;
    }

    if((*tarefaCandidata).chavePrioridade < (*melhorTarefa).chavePrioridade) {
        return FALSO;
    }

    if(posicaoCandidata < melhorPosicao) {
        return VERDADEIRO;
    }

    if(posicaoCandidata > melhorPosicao) {
        return FALSO;
    }

    if((*tarefaCandidata).identificador < (*melhorTarefa).identificador) {
        return VERDADEIRO;
    }

    return FALSO;
}

static Boolean controllerHeuristicaConstruirSequenciaPorInsercaoAdaptativa(const Instancia *instancia,const Tarefa **tarefasPorIdentificador,const TarefaParaInsercao *tarefasOrdenadas,DataDeEntregaComum dataDeEntregaComum,IdentificadorDeTarefa *sequenciaConstruida) {
    IdentificadorDeTarefa *sequenciaTemporaria;
    QuantidadeDeTarefas *posicoesCandidatas;
    InteiroPositivoDe32Bits *temposDeConclusaoCompactos;
    Boolean *tarefaJaInserida;
    QuantidadeDeTarefas quantidadeAtual;
    QuantidadeDeTarefas indiceDaOrdem;
    QuantidadeDeTarefas quantidadeDeTarefasCandidatas;
    QuantidadeDeTarefas quantidadeDePosicoes;
    QuantidadeDeTarefas indiceDaPosicaoCandidata;
    QuantidadeDeTarefas posicaoDeInsercao;
    QuantidadeDeTarefas melhorPosicao;
    IdentificadorDeTarefa identificadorDaTarefa;
    IdentificadorDeTarefa melhorIdentificador;
    const TarefaParaInsercao *tarefaCandidata;
    const TarefaParaInsercao *melhorTarefa;
    Custo custoCandidato;
    Custo melhorCusto;
    Boolean encontrouMelhorMovimento;

    if(instancia == NULL) {
        return FALSO;
    }

    if(tarefasPorIdentificador == NULL) {
        return FALSO;
    }

    if(tarefasOrdenadas == NULL) {
        return FALSO;
    }

    if(sequenciaConstruida == NULL) {
        return FALSO;
    }

    sequenciaTemporaria = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    posicoesCandidatas = (QuantidadeDeTarefas *) malloc(sizeof(QuantidadeDeTarefas) * ((QuantidadeDeTarefas) ((*instancia).quantidadeDeTarefas + 1)));
    temposDeConclusaoCompactos = (InteiroPositivoDe32Bits *) malloc(sizeof(InteiroPositivoDe32Bits) * (*instancia).quantidadeDeTarefas);
    tarefaJaInserida = (Boolean *) malloc(sizeof(Boolean) * ((QuantidadeDeTarefas) ((*instancia).quantidadeDeTarefas + 1)));

    if(sequenciaTemporaria == NULL) {
        free(posicoesCandidatas);
        free(temposDeConclusaoCompactos);
        free(tarefaJaInserida);
        return FALSO;
    }

    if(posicoesCandidatas == NULL) {
        free(sequenciaTemporaria);
        free(temposDeConclusaoCompactos);
        free(tarefaJaInserida);
        return FALSO;
    }

    if(temposDeConclusaoCompactos == NULL) {
        free(sequenciaTemporaria);
        free(posicoesCandidatas);
        free(tarefaJaInserida);
        return FALSO;
    }

    if(tarefaJaInserida == NULL) {
        free(sequenciaTemporaria);
        free(posicoesCandidatas);
        free(temposDeConclusaoCompactos);
        return FALSO;
    }

    for(indiceDaOrdem = 0;indiceDaOrdem <= (*instancia).quantidadeDeTarefas;indiceDaOrdem++) {
        tarefaJaInserida[indiceDaOrdem] = FALSO;
    }

    quantidadeAtual = 0;

    while(quantidadeAtual < (*instancia).quantidadeDeTarefas) {
        quantidadeDeTarefasCandidatas = 0;
        encontrouMelhorMovimento = FALSO;
        melhorCusto = 0;
        melhorPosicao = 0;
        melhorIdentificador = 0;
        melhorTarefa = NULL;

        for(indiceDaOrdem = 0;indiceDaOrdem < (*instancia).quantidadeDeTarefas;indiceDaOrdem++) {
            tarefaCandidata = &(tarefasOrdenadas[indiceDaOrdem]);
            identificadorDaTarefa = (*tarefaCandidata).identificador;

            if(identificadorDaTarefa == 0) {
                free(sequenciaTemporaria);
                free(posicoesCandidatas);
                free(temposDeConclusaoCompactos);
                free(tarefaJaInserida);
                return FALSO;
            }

            if(identificadorDaTarefa > (*instancia).quantidadeDeTarefas) {
                free(sequenciaTemporaria);
                free(posicoesCandidatas);
                free(temposDeConclusaoCompactos);
                free(tarefaJaInserida);
                return FALSO;
            }

            if(tarefaJaInserida[identificadorDaTarefa] == VERDADEIRO) {
                continue;
            }

            quantidadeDeTarefasCandidatas++;

            if(controllerHeuristicaMontarPosicoesCandidatas(tarefasPorIdentificador,sequenciaConstruida,quantidadeAtual,tarefaCandidata,dataDeEntregaComum,posicoesCandidatas,&quantidadeDePosicoes) == FALSO) {
                free(sequenciaTemporaria);
                free(posicoesCandidatas);
                free(temposDeConclusaoCompactos);
                free(tarefaJaInserida);
                return FALSO;
            }

            for(indiceDaPosicaoCandidata = 0;indiceDaPosicaoCandidata < quantidadeDePosicoes;indiceDaPosicaoCandidata++) {
                posicaoDeInsercao = posicoesCandidatas[indiceDaPosicaoCandidata];

                if(controllerHeuristicaMontarSequenciaTemporaria(sequenciaConstruida,quantidadeAtual,identificadorDaTarefa,posicaoDeInsercao,sequenciaTemporaria) == FALSO) {
                    free(sequenciaTemporaria);
                    free(posicoesCandidatas);
                    free(temposDeConclusaoCompactos);
                    free(tarefaJaInserida);
                    return FALSO;
                }

                if(controllerHeuristicaCalcularMelhorCustoDaSequencia(tarefasPorIdentificador,sequenciaTemporaria,(QuantidadeDeTarefas) (quantidadeAtual + 1),dataDeEntregaComum,temposDeConclusaoCompactos,&custoCandidato) == FALSO) {
                    free(sequenciaTemporaria);
                    free(posicoesCandidatas);
                    free(temposDeConclusaoCompactos);
                    free(tarefaJaInserida);
                    return FALSO;
                }

                if(controllerHeuristicaMovimentoEhMelhor(custoCandidato,posicaoDeInsercao,tarefaCandidata,encontrouMelhorMovimento,melhorCusto,melhorPosicao,melhorTarefa) == VERDADEIRO) {
                    melhorCusto = custoCandidato;
                    melhorPosicao = posicaoDeInsercao;
                    melhorIdentificador = identificadorDaTarefa;
                    melhorTarefa = tarefaCandidata;
                    encontrouMelhorMovimento = VERDADEIRO;
                }
            }

            if(quantidadeDeTarefasCandidatas >= QUANTIDADE_MAXIMA_DE_TAREFAS_CANDIDATAS) {
                break;
            }
        }

        if(encontrouMelhorMovimento == FALSO) {
            free(sequenciaTemporaria);
            free(posicoesCandidatas);
            free(temposDeConclusaoCompactos);
            free(tarefaJaInserida);
            return FALSO;
        }

        if(controllerHeuristicaInserirTarefaNaSequencia(sequenciaConstruida,quantidadeAtual,melhorIdentificador,melhorPosicao) == FALSO) {
            free(sequenciaTemporaria);
            free(posicoesCandidatas);
            free(temposDeConclusaoCompactos);
            free(tarefaJaInserida);
            return FALSO;
        }

        tarefaJaInserida[melhorIdentificador] = VERDADEIRO;
        quantidadeAtual++;
    }

    free(sequenciaTemporaria);
    free(posicoesCandidatas);
    free(temposDeConclusaoCompactos);
    free(tarefaJaInserida);

    return VERDADEIRO;
}

Boolean controllerHeuristicaConstruirSolucao(const Instancia *instancia,const Heuristica *heuristica,FatorH fatorH,Solucao *solucao) {
    TarefaParaInsercao *tarefasOrdenadas;
    const Tarefa **tarefasPorIdentificador;
    IdentificadorDeTarefa *sequenciaConstruida;
    DataDeEntregaComum dataDeEntregaComum;

    if(controllerHeuristicaParametrosSaoValidos(instancia,heuristica,fatorH,solucao) == FALSO) {
        return FALSO;
    }

    dataDeEntregaComum = ((*instancia).somaDosTemposDeProcessamento * fatorH) / FATOR_DE_ESCALA_H;

    tarefasOrdenadas = (TarefaParaInsercao *) malloc(sizeof(TarefaParaInsercao) * (*instancia).quantidadeDeTarefas);
    tarefasPorIdentificador = (const Tarefa **) malloc(sizeof(Tarefa *) * ((QuantidadeDeTarefas) ((*instancia).quantidadeDeTarefas + 1)));
    sequenciaConstruida = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);

    if(tarefasOrdenadas == NULL) {
        free(tarefasPorIdentificador);
        free(sequenciaConstruida);
        return FALSO;
    }

    if(tarefasPorIdentificador == NULL) {
        free(tarefasOrdenadas);
        free(sequenciaConstruida);
        return FALSO;
    }

    if(sequenciaConstruida == NULL) {
        free(tarefasOrdenadas);
        free(tarefasPorIdentificador);
        return FALSO;
    }

    if(controllerHeuristicaMontarTarefasPorIdentificador(instancia,tarefasPorIdentificador) == FALSO) {
        free(tarefasOrdenadas);
        free(tarefasPorIdentificador);
        free(sequenciaConstruida);
        return FALSO;
    }

    if(controllerHeuristicaCriarOrdemDePrioridade(instancia,tarefasOrdenadas) == FALSO) {
        free(tarefasOrdenadas);
        free(tarefasPorIdentificador);
        free(sequenciaConstruida);
        return FALSO;
    }

    if(controllerHeuristicaConstruirSequenciaPorInsercaoAdaptativa(instancia,tarefasPorIdentificador,tarefasOrdenadas,dataDeEntregaComum,sequenciaConstruida) == FALSO) {
        free(tarefasOrdenadas);
        free(tarefasPorIdentificador);
        free(sequenciaConstruida);
        return FALSO;
    }

    (*solucao).quantidadeDeTarefas = (*instancia).quantidadeDeTarefas;
    (*solucao).quantidadeDeTarefasAlocadas = (*instancia).quantidadeDeTarefas;
    (*solucao).sequenciaDeTarefas = sequenciaConstruida;

    free(tarefasOrdenadas);
    free(tarefasPorIdentificador);

    return VERDADEIRO;
}