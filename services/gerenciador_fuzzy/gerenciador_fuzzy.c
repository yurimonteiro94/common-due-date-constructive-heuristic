#include "gerenciador_fuzzy.h"

#include <stddef.h>

#define TEMPO_DE_PROCESSAMENTO_MINIMO_FUZZY 1.0
#define TEMPO_DE_PROCESSAMENTO_MAXIMO_FUZZY 20.0

#define PENALIDADE_MINIMA_FUZZY 1.0
#define PENALIDADE_MAXIMA_FUZZY 15.0

#define PESO_BETA_ALTA_ANTES_DA_DATA 0.40
#define PESO_TEMPO_CURTO_ANTES_DA_DATA 0.25
#define PESO_ALPHA_BAIXA_ANTES_DA_DATA 0.20
#define PESO_ENCAIXE_ANTES_DA_DATA 0.15

#define PESO_ALPHA_ALTA_DEPOIS_DA_DATA 0.45
#define PESO_BETA_BAIXA_DEPOIS_DA_DATA 0.30
#define PESO_TEMPO_LONGO_DEPOIS_DA_DATA 0.25

static double gerenciadorFuzzyLimitarEntreZeroEUm(double valor) {
    if(valor < 0.0) {
        return 0.0;
    }

    else if(valor > 1.0) {
        return 1.0;
    }

    return valor;
}

static double gerenciadorFuzzyCalcularEncaixeAntesDaDataDeEntrega(TempoDeProcessamento tempoDeProcessamento,DataDeEntregaComum dataDeEntregaComum,SomaDosTemposDeProcessamento tempoJaAlocadoAntesDaDataDeEntrega) {
    DataDeEntregaComum folga;

    if(dataDeEntregaComum <= tempoJaAlocadoAntesDaDataDeEntrega) {
        return 0.0;
    }

    folga = dataDeEntregaComum - tempoJaAlocadoAntesDaDataDeEntrega;

    if(tempoDeProcessamento <= folga) {
        return 1.0;
    }

    return gerenciadorFuzzyLimitarEntreZeroEUm((double) folga / (double) tempoDeProcessamento);
}

Boolean gerenciadorFuzzyCalcularPertinenciaCrescente(double valor,double limiteInferior,double limiteSuperior,double *pertinencia) {
    if(pertinencia == NULL) {
        return FALSO;
    }

    else if(limiteSuperior <= limiteInferior) {
        (*pertinencia) = 0.0;

        return FALSO;
    }

    if(valor <= limiteInferior) {
        (*pertinencia) = 0.0;

        return VERDADEIRO;
    }

    else if(valor >= limiteSuperior) {
        (*pertinencia) = 1.0;

        return VERDADEIRO;
    }

    (*pertinencia) = (valor - limiteInferior) / (limiteSuperior - limiteInferior);
    (*pertinencia) = gerenciadorFuzzyLimitarEntreZeroEUm((*pertinencia));

    return VERDADEIRO;
}

Boolean gerenciadorFuzzyCalcularPertinenciaDecrescente(double valor,double limiteInferior,double limiteSuperior,double *pertinencia) {
    double pertinenciaCrescente;

    if(pertinencia == NULL) {
        return FALSO;
    }

    if(gerenciadorFuzzyCalcularPertinenciaCrescente(valor,limiteInferior,limiteSuperior,&pertinenciaCrescente) == FALSO) {
        (*pertinencia) = 0.0;

        return FALSO;
    }

    (*pertinencia) = 1.0 - pertinenciaCrescente;
    (*pertinencia) = gerenciadorFuzzyLimitarEntreZeroEUm((*pertinencia));

    return VERDADEIRO;
}

Boolean gerenciadorFuzzyCalcularPertinenciaTriangular(double valor,double limiteInferior,double limiteCentral,double limiteSuperior,double *pertinencia) {
    if(pertinencia == NULL) {
        return FALSO;
    }

    else if(limiteCentral <= limiteInferior) {
        (*pertinencia) = 0.0;

        return FALSO;
    }

    else if(limiteSuperior <= limiteCentral) {
        (*pertinencia) = 0.0;

        return FALSO;
    }

    if(valor <= limiteInferior) {
        (*pertinencia) = 0.0;

        return VERDADEIRO;
    }

    else if(valor >= limiteSuperior) {
        (*pertinencia) = 0.0;

        return VERDADEIRO;
    }

    else if(valor == limiteCentral) {
        (*pertinencia) = 1.0;

        return VERDADEIRO;
    }

    else if(valor < limiteCentral) {
        (*pertinencia) = (valor - limiteInferior) / (limiteCentral - limiteInferior);
        (*pertinencia) = gerenciadorFuzzyLimitarEntreZeroEUm((*pertinencia));

        return VERDADEIRO;
    }

    (*pertinencia) = (limiteSuperior - valor) / (limiteSuperior - limiteCentral);
    (*pertinencia) = gerenciadorFuzzyLimitarEntreZeroEUm((*pertinencia));

    return VERDADEIRO;
}

Boolean gerenciadorFuzzyCalcularPrioridadeAntesDaDataDeEntrega(const Tarefa *tarefa,DataDeEntregaComum dataDeEntregaComum,SomaDosTemposDeProcessamento tempoJaAlocadoAntesDaDataDeEntrega,double *prioridade) {
    double pertinenciaBetaAlta;
    double pertinenciaTempoCurto;
    double pertinenciaAlphaBaixa;
    double pertinenciaEncaixe;

    if(tarefaEhValida(tarefa) == FALSO) {
        return FALSO;
    }

    else if(dataDeEntregaComum == 0) {
        return FALSO;
    }

    else if(prioridade == NULL) {
        return FALSO;
    }

    if(gerenciadorFuzzyCalcularPertinenciaCrescente((double) (*tarefa).penalidadeAtraso,PENALIDADE_MINIMA_FUZZY,PENALIDADE_MAXIMA_FUZZY,&pertinenciaBetaAlta) == FALSO) {
        return FALSO;
    }

    if(gerenciadorFuzzyCalcularPertinenciaDecrescente((double) (*tarefa).tempoProcessamento,TEMPO_DE_PROCESSAMENTO_MINIMO_FUZZY,TEMPO_DE_PROCESSAMENTO_MAXIMO_FUZZY,&pertinenciaTempoCurto) == FALSO) {
        return FALSO;
    }

    if(gerenciadorFuzzyCalcularPertinenciaDecrescente((double) (*tarefa).penalidadeAdiantamento,PENALIDADE_MINIMA_FUZZY,PENALIDADE_MAXIMA_FUZZY,&pertinenciaAlphaBaixa) == FALSO) {
        return FALSO;
    }

    pertinenciaEncaixe = gerenciadorFuzzyCalcularEncaixeAntesDaDataDeEntrega(
        (*tarefa).tempoProcessamento,
        dataDeEntregaComum,
        tempoJaAlocadoAntesDaDataDeEntrega
    );

    (*prioridade) = 0.0;
    (*prioridade) += PESO_BETA_ALTA_ANTES_DA_DATA * pertinenciaBetaAlta;
    (*prioridade) += PESO_TEMPO_CURTO_ANTES_DA_DATA * pertinenciaTempoCurto;
    (*prioridade) += PESO_ALPHA_BAIXA_ANTES_DA_DATA * pertinenciaAlphaBaixa;
    (*prioridade) += PESO_ENCAIXE_ANTES_DA_DATA * pertinenciaEncaixe;

    (*prioridade) = gerenciadorFuzzyLimitarEntreZeroEUm((*prioridade));

    return VERDADEIRO;
}

Boolean gerenciadorFuzzyCalcularPrioridadeDepoisDaDataDeEntrega(const Tarefa *tarefa,double *prioridade) {
    double pertinenciaAlphaAlta;
    double pertinenciaBetaBaixa;
    double pertinenciaTempoLongo;

    if(tarefaEhValida(tarefa) == FALSO) {
        return FALSO;
    }

    else if(prioridade == NULL) {
        return FALSO;
    }

    if(gerenciadorFuzzyCalcularPertinenciaCrescente((double) (*tarefa).penalidadeAdiantamento,PENALIDADE_MINIMA_FUZZY,PENALIDADE_MAXIMA_FUZZY,&pertinenciaAlphaAlta) == FALSO) {
        return FALSO;
    }

    if(gerenciadorFuzzyCalcularPertinenciaDecrescente((double) (*tarefa).penalidadeAtraso,PENALIDADE_MINIMA_FUZZY,PENALIDADE_MAXIMA_FUZZY,&pertinenciaBetaBaixa) == FALSO) {
        return FALSO;
    }

    if(gerenciadorFuzzyCalcularPertinenciaCrescente((double) (*tarefa).tempoProcessamento,TEMPO_DE_PROCESSAMENTO_MINIMO_FUZZY,TEMPO_DE_PROCESSAMENTO_MAXIMO_FUZZY,&pertinenciaTempoLongo) == FALSO) {
        return FALSO;
    }

    (*prioridade) = 0.0;
    (*prioridade) += PESO_ALPHA_ALTA_DEPOIS_DA_DATA * pertinenciaAlphaAlta;
    (*prioridade) += PESO_BETA_BAIXA_DEPOIS_DA_DATA * pertinenciaBetaBaixa;
    (*prioridade) += PESO_TEMPO_LONGO_DEPOIS_DA_DATA * pertinenciaTempoLongo;

    (*prioridade) = gerenciadorFuzzyLimitarEntreZeroEUm((*prioridade));

    return VERDADEIRO;
}
