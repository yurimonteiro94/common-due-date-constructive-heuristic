#include "controller_heuristica.h"

#include "../../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stdlib.h>

#define QUANTIDADE_DE_REGRAS_DE_SELECAO 8
#define QUANTIDADE_DE_FATORES_DE_ALVO 7
#define QUANTIDADE_DE_ESTRATEGIAS_DIRETAS 6
#define QUANTIDADE_TOTAL_DE_ESTRATEGIAS ((QUANTIDADE_DE_REGRAS_DE_SELECAO * QUANTIDADE_DE_FATORES_DE_ALVO) + QUANTIDADE_DE_ESTRATEGIAS_DIRETAS)

typedef struct TarefaParaArbitragem {
    IdentificadorDeTarefa identificador;
    TempoDeProcessamento tempoProcessamento;
    Penalidade penalidadeAdiantamento;
    Penalidade penalidadeAtraso;
    double chaveSelecao;
    double chaveOrdenacao;
} TarefaParaArbitragem;

static Boolean controllerHeuristicaParametrosSaoValidos(const Instancia *instancia,Solucao *solucao) {
    if(instancia == NULL) {
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

static double controllerHeuristicaDividir(double numerador,double denominador) {
    if(denominador == 0.0) {
        return numerador;
    }

    return numerador / denominador;
}

static double controllerHeuristicaCalcularChaveDeSelecao(const TarefaParaArbitragem *tarefa,InteiroPositivoDe16Bits regra) {
    double p;
    double alpha;
    double beta;

    if(tarefa == NULL) {
        return 0.0;
    }

    p = (double) (*tarefa).tempoProcessamento;
    alpha = (double) (*tarefa).penalidadeAdiantamento;
    beta = (double) (*tarefa).penalidadeAtraso;

    if(regra == 0) {
        return controllerHeuristicaDividir(beta,p);
    }

    if(regra == 1) {
        return controllerHeuristicaDividir(beta - alpha,p);
    }

    if(regra == 2) {
        return beta;
    }

    if(regra == 3) {
        return controllerHeuristicaDividir(beta,alpha + 1.0);
    }

    if(regra == 4) {
        return controllerHeuristicaDividir((2.0 * beta) + alpha,p);
    }

    if(regra == 5) {
        return controllerHeuristicaDividir(beta + alpha,p);
    }

    if(regra == 6) {
        return -p;
    }

    return controllerHeuristicaDividir((3.0 * beta) - alpha,p);
}

static double controllerHeuristicaCalcularChaveDireta(const TarefaParaArbitragem *tarefa,InteiroPositivoDe16Bits estrategiaDireta) {
    double p;
    double alpha;
    double beta;

    if(tarefa == NULL) {
        return 0.0;
    }

    p = (double) (*tarefa).tempoProcessamento;
    alpha = (double) (*tarefa).penalidadeAdiantamento;
    beta = (double) (*tarefa).penalidadeAtraso;

    if(estrategiaDireta == 0) {
        return controllerHeuristicaDividir(beta,p);
    }

    if(estrategiaDireta == 1) {
        return controllerHeuristicaDividir(alpha,p);
    }

    if(estrategiaDireta == 2) {
        return -p;
    }

    if(estrategiaDireta == 3) {
        return p;
    }

    if(estrategiaDireta == 4) {
        return beta;
    }

    return alpha;
}

static int controllerHeuristicaCompararDecrescente(const void *primeiro,const void *segundo) {
    const TarefaParaArbitragem *tarefaA;
    const TarefaParaArbitragem *tarefaB;

    tarefaA = (const TarefaParaArbitragem *) primeiro;
    tarefaB = (const TarefaParaArbitragem *) segundo;

    if((*tarefaA).chaveOrdenacao > (*tarefaB).chaveOrdenacao) {
        return -1;
    }

    if((*tarefaA).chaveOrdenacao < (*tarefaB).chaveOrdenacao) {
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

static int controllerHeuristicaCompararCrescente(const void *primeiro,const void *segundo) {
    const TarefaParaArbitragem *tarefaA;
    const TarefaParaArbitragem *tarefaB;

    tarefaA = (const TarefaParaArbitragem *) primeiro;
    tarefaB = (const TarefaParaArbitragem *) segundo;

    if((*tarefaA).chaveOrdenacao < (*tarefaB).chaveOrdenacao) {
        return -1;
    }

    if((*tarefaA).chaveOrdenacao > (*tarefaB).chaveOrdenacao) {
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

static void controllerHeuristicaCopiarTarefasDaInstancia(const Instancia *instancia,TarefaParaArbitragem *tarefas) {
    QuantidadeDeTarefas indiceDaTarefa;

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        tarefas[indiceDaTarefa].identificador = (*instancia).tarefas[indiceDaTarefa].identificador;
        tarefas[indiceDaTarefa].tempoProcessamento = (*instancia).tarefas[indiceDaTarefa].tempoProcessamento;
        tarefas[indiceDaTarefa].penalidadeAdiantamento = (*instancia).tarefas[indiceDaTarefa].penalidadeAdiantamento;
        tarefas[indiceDaTarefa].penalidadeAtraso = (*instancia).tarefas[indiceDaTarefa].penalidadeAtraso;
        tarefas[indiceDaTarefa].chaveSelecao = 0.0;
        tarefas[indiceDaTarefa].chaveOrdenacao = 0.0;
    }
}

static InteiroPositivoDe32Bits controllerHeuristicaCalcularAlvoTemporal(DataDeEntregaComum dataDeEntregaComum,SomaDosTemposDeProcessamento somaDosTemposDeProcessamento,InteiroPositivoDe16Bits indiceDoFator) {
    static const InteiroPositivoDe16Bits fatoresPercentuais[QUANTIDADE_DE_FATORES_DE_ALVO] = {70,80,90,100,110,120,130};
    InteiroPositivoDe32Bits alvoTemporal;

    alvoTemporal = ((InteiroPositivoDe32Bits) dataDeEntregaComum * (InteiroPositivoDe32Bits) fatoresPercentuais[indiceDoFator]) / 100;

    if(alvoTemporal > somaDosTemposDeProcessamento) {
        alvoTemporal = somaDosTemposDeProcessamento;
    }

    return alvoTemporal;
}

static Boolean controllerHeuristicaConstruirSequenciaParticionada(const Instancia *instancia,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe16Bits regraDeSelecao,InteiroPositivoDe16Bits indiceDoFator,IdentificadorDeTarefa *sequencia) {
    TarefaParaArbitragem *tarefasOrdenadas;
    TarefaParaArbitragem *tarefasAntes;
    TarefaParaArbitragem *tarefasDepois;
    QuantidadeDeTarefas indiceDaTarefa;
    QuantidadeDeTarefas quantidadeAntes;
    QuantidadeDeTarefas quantidadeDepois;
    QuantidadeDeTarefas indiceDaSequencia;
    InteiroPositivoDe32Bits somaAntes;
    InteiroPositivoDe32Bits alvoTemporal;

    if(instancia == NULL) {
        return FALSO;
    }

    if(sequencia == NULL) {
        return FALSO;
    }

    tarefasOrdenadas = (TarefaParaArbitragem *) malloc(sizeof(TarefaParaArbitragem) * (*instancia).quantidadeDeTarefas);
    tarefasAntes = (TarefaParaArbitragem *) malloc(sizeof(TarefaParaArbitragem) * (*instancia).quantidadeDeTarefas);
    tarefasDepois = (TarefaParaArbitragem *) malloc(sizeof(TarefaParaArbitragem) * (*instancia).quantidadeDeTarefas);

    if(tarefasOrdenadas == NULL) {
        free(tarefasAntes);
        free(tarefasDepois);
        return FALSO;
    }

    if(tarefasAntes == NULL) {
        free(tarefasOrdenadas);
        free(tarefasDepois);
        return FALSO;
    }

    if(tarefasDepois == NULL) {
        free(tarefasOrdenadas);
        free(tarefasAntes);
        return FALSO;
    }

    controllerHeuristicaCopiarTarefasDaInstancia(instancia,tarefasOrdenadas);

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        tarefasOrdenadas[indiceDaTarefa].chaveSelecao = controllerHeuristicaCalcularChaveDeSelecao(&(tarefasOrdenadas[indiceDaTarefa]),regraDeSelecao);
        tarefasOrdenadas[indiceDaTarefa].chaveOrdenacao = tarefasOrdenadas[indiceDaTarefa].chaveSelecao;
    }

    qsort(tarefasOrdenadas,(*instancia).quantidadeDeTarefas,sizeof(TarefaParaArbitragem),controllerHeuristicaCompararDecrescente);

    quantidadeAntes = 0;
    quantidadeDepois = 0;
    somaAntes = 0;
    alvoTemporal = controllerHeuristicaCalcularAlvoTemporal(dataDeEntregaComum,(*instancia).somaDosTemposDeProcessamento,indiceDoFator);

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        if(somaAntes + tarefasOrdenadas[indiceDaTarefa].tempoProcessamento <= alvoTemporal) {
            tarefasAntes[quantidadeAntes] = tarefasOrdenadas[indiceDaTarefa];
            somaAntes += tarefasOrdenadas[indiceDaTarefa].tempoProcessamento;
            quantidadeAntes++;
        }
        else {
            tarefasDepois[quantidadeDepois] = tarefasOrdenadas[indiceDaTarefa];
            quantidadeDepois++;
        }
    }

    for(indiceDaTarefa = 0;indiceDaTarefa < quantidadeAntes;indiceDaTarefa++) {
        tarefasAntes[indiceDaTarefa].chaveOrdenacao = controllerHeuristicaDividir((double) tarefasAntes[indiceDaTarefa].penalidadeAdiantamento,(double) tarefasAntes[indiceDaTarefa].tempoProcessamento);
    }

    for(indiceDaTarefa = 0;indiceDaTarefa < quantidadeDepois;indiceDaTarefa++) {
        tarefasDepois[indiceDaTarefa].chaveOrdenacao = controllerHeuristicaDividir((double) tarefasDepois[indiceDaTarefa].penalidadeAtraso,(double) tarefasDepois[indiceDaTarefa].tempoProcessamento);
    }

    qsort(tarefasAntes,quantidadeAntes,sizeof(TarefaParaArbitragem),controllerHeuristicaCompararCrescente);
    qsort(tarefasDepois,quantidadeDepois,sizeof(TarefaParaArbitragem),controllerHeuristicaCompararDecrescente);

    indiceDaSequencia = 0;

    for(indiceDaTarefa = 0;indiceDaTarefa < quantidadeAntes;indiceDaTarefa++) {
        sequencia[indiceDaSequencia] = tarefasAntes[indiceDaTarefa].identificador;
        indiceDaSequencia++;
    }

    for(indiceDaTarefa = 0;indiceDaTarefa < quantidadeDepois;indiceDaTarefa++) {
        sequencia[indiceDaSequencia] = tarefasDepois[indiceDaTarefa].identificador;
        indiceDaSequencia++;
    }

    free(tarefasOrdenadas);
    free(tarefasAntes);
    free(tarefasDepois);

    return VERDADEIRO;
}

static Boolean controllerHeuristicaConstruirSequenciaDireta(const Instancia *instancia,InteiroPositivoDe16Bits estrategiaDireta,IdentificadorDeTarefa *sequencia) {
    TarefaParaArbitragem *tarefasOrdenadas;
    QuantidadeDeTarefas indiceDaTarefa;

    if(instancia == NULL) {
        return FALSO;
    }

    if(sequencia == NULL) {
        return FALSO;
    }

    tarefasOrdenadas = (TarefaParaArbitragem *) malloc(sizeof(TarefaParaArbitragem) * (*instancia).quantidadeDeTarefas);

    if(tarefasOrdenadas == NULL) {
        return FALSO;
    }

    controllerHeuristicaCopiarTarefasDaInstancia(instancia,tarefasOrdenadas);

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        tarefasOrdenadas[indiceDaTarefa].chaveOrdenacao = controllerHeuristicaCalcularChaveDireta(&(tarefasOrdenadas[indiceDaTarefa]),estrategiaDireta);
    }

    qsort(tarefasOrdenadas,(*instancia).quantidadeDeTarefas,sizeof(TarefaParaArbitragem),controllerHeuristicaCompararDecrescente);

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        sequencia[indiceDaTarefa] = tarefasOrdenadas[indiceDaTarefa].identificador;
    }

    free(tarefasOrdenadas);

    return VERDADEIRO;
}

static Boolean controllerHeuristicaAvaliarSequencia(const Instancia *instancia,DataDeEntregaComum dataDeEntregaComum,IdentificadorDeTarefa *sequencia,Custo *custo) {
    Solucao solucaoCandidata;

    if(instancia == NULL) {
        return FALSO;
    }

    if(sequencia == NULL) {
        return FALSO;
    }

    if(custo == NULL) {
        return FALSO;
    }

    solucaoCandidata.quantidadeDeTarefas = (*instancia).quantidadeDeTarefas;
    solucaoCandidata.quantidadeDeTarefasAlocadas = (*instancia).quantidadeDeTarefas;
    solucaoCandidata.sequenciaDeTarefas = sequencia;

    return gerenciadorDeCustosCalcularCustoDaSolucao(instancia,&solucaoCandidata,dataDeEntregaComum,custo);
}

static Boolean controllerHeuristicaCopiarSequencia(IdentificadorDeTarefa *destino,const IdentificadorDeTarefa *origem,QuantidadeDeTarefas quantidadeDeTarefas) {
    QuantidadeDeTarefas indiceDaTarefa;

    if(destino == NULL) {
        return FALSO;
    }

    if(origem == NULL) {
        return FALSO;
    }

    for(indiceDaTarefa = 0;indiceDaTarefa < quantidadeDeTarefas;indiceDaTarefa++) {
        destino[indiceDaTarefa] = origem[indiceDaTarefa];
    }

    return VERDADEIRO;
}

Boolean controllerHeuristicaConstruirSolucao(const Instancia *instancia,const Heuristica *heuristica,FatorH fatorH,Solucao *solucao) {
    IdentificadorDeTarefa *sequenciaCandidata;
    IdentificadorDeTarefa *melhorSequencia;
    InteiroPositivoDe16Bits indiceDaEstrategia;
    InteiroPositivoDe16Bits regraDeSelecao;
    InteiroPositivoDe16Bits indiceDoFator;
    InteiroPositivoDe16Bits estrategiaDireta;
    Custo custoCandidato;
    Custo melhorCusto;
    Boolean encontrouMelhorSolucao;
    DataDeEntregaComum dataDeEntregaComum;
    Boolean construiuSequencia;

    (void) heuristica;
    dataDeEntregaComum = ((*instancia).somaDosTemposDeProcessamento * fatorH) / FATOR_DE_ESCALA_H;

    if(controllerHeuristicaParametrosSaoValidos(instancia,solucao) == FALSO) {
        return FALSO;
    }

    sequenciaCandidata = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);
    melhorSequencia = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * (*instancia).quantidadeDeTarefas);

    if(sequenciaCandidata == NULL) {
        free(melhorSequencia);
        return FALSO;
    }

    if(melhorSequencia == NULL) {
        free(sequenciaCandidata);
        return FALSO;
    }

    encontrouMelhorSolucao = FALSO;
    melhorCusto = 0;

    for(indiceDaEstrategia = 0;indiceDaEstrategia < QUANTIDADE_TOTAL_DE_ESTRATEGIAS;indiceDaEstrategia++) {
        construiuSequencia = FALSO;

        if(indiceDaEstrategia < QUANTIDADE_DE_REGRAS_DE_SELECAO * QUANTIDADE_DE_FATORES_DE_ALVO) {
            regraDeSelecao = indiceDaEstrategia % QUANTIDADE_DE_REGRAS_DE_SELECAO;
            indiceDoFator = indiceDaEstrategia / QUANTIDADE_DE_REGRAS_DE_SELECAO;
            construiuSequencia = controllerHeuristicaConstruirSequenciaParticionada(instancia,dataDeEntregaComum,regraDeSelecao,indiceDoFator,sequenciaCandidata);
        }
        else {
            estrategiaDireta = indiceDaEstrategia - (QUANTIDADE_DE_REGRAS_DE_SELECAO * QUANTIDADE_DE_FATORES_DE_ALVO);
            construiuSequencia = controllerHeuristicaConstruirSequenciaDireta(instancia,estrategiaDireta,sequenciaCandidata);
        }

        if(construiuSequencia == FALSO) {
            free(sequenciaCandidata);
            free(melhorSequencia);
            return FALSO;
        }

        if(controllerHeuristicaAvaliarSequencia(instancia,dataDeEntregaComum,sequenciaCandidata,&custoCandidato) == FALSO) {
            free(sequenciaCandidata);
            free(melhorSequencia);
            return FALSO;
        }

        if(encontrouMelhorSolucao == FALSO) {
            melhorCusto = custoCandidato;
            encontrouMelhorSolucao = VERDADEIRO;

            if(controllerHeuristicaCopiarSequencia(melhorSequencia,sequenciaCandidata,(*instancia).quantidadeDeTarefas) == FALSO) {
                free(sequenciaCandidata);
                free(melhorSequencia);
                return FALSO;
            }
        }
        else if(custoCandidato < melhorCusto) {
            melhorCusto = custoCandidato;

            if(controllerHeuristicaCopiarSequencia(melhorSequencia,sequenciaCandidata,(*instancia).quantidadeDeTarefas) == FALSO) {
                free(sequenciaCandidata);
                free(melhorSequencia);
                return FALSO;
            }
        }
    }

    if(encontrouMelhorSolucao == FALSO) {
        free(sequenciaCandidata);
        free(melhorSequencia);
        return FALSO;
    }

    (*solucao).quantidadeDeTarefas = (*instancia).quantidadeDeTarefas;
    (*solucao).quantidadeDeTarefasAlocadas = (*instancia).quantidadeDeTarefas;
    (*solucao).sequenciaDeTarefas = melhorSequencia;

    free(sequenciaCandidata);

    return VERDADEIRO;
}



