#include "instancia.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Instancia criarInstanciaVazia(void) {
    Instancia instancia;

    instancia.nomeDoArquivoDeOrigem[0] = '\0';
    instancia.identificadorDaInstancia = 0;
    instancia.quantidadeDeTarefas = 0;
    instancia.tarefas = NULL;
    instancia.somaDosTemposDeProcessamento = 0;

    return instancia;
}

Boolean inicializarInstancia(Instancia *instancia,const char *nomeDoArquivoDeOrigem,InteiroPositivoDe16Bits identificadorDaInstancia,QuantidadeDeTarefas quantidadeDeTarefas) {
    QuantidadeDeTarefas indiceDaTarefa;

    if(instancia == NULL) {
        return FALSO;
    }

    else if(identificadorDaInstancia == 0) {
        return FALSO;
    }

    else if(quantidadeDeTarefas == 0) {
        return FALSO;
    }

    (*instancia) = criarInstanciaVazia();

    if(nomeDoArquivoDeOrigem != NULL) {
        snprintf((*instancia).nomeDoArquivoDeOrigem,TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO,"%s",nomeDoArquivoDeOrigem);
    }

    (*instancia).identificadorDaInstancia = identificadorDaInstancia;
    (*instancia).quantidadeDeTarefas = quantidadeDeTarefas;

    (*instancia).tarefas = (Tarefa *) malloc(sizeof(Tarefa) * quantidadeDeTarefas);

    if((*instancia).tarefas == NULL) {
        (*instancia) = criarInstanciaVazia();

        return FALSO;
    }

    for(indiceDaTarefa = 0; indiceDaTarefa < quantidadeDeTarefas; indiceDaTarefa++) {
        (*instancia).tarefas[indiceDaTarefa] = criarTarefa(
            (IdentificadorDeTarefa) 0,
            (TempoDeProcessamento) 0,
            (Penalidade) 0,
            (Penalidade) 0
        );
    }

    return VERDADEIRO;
}

Boolean instanciaAdicionarTarefa(Instancia *instancia,QuantidadeDeTarefas indiceDaTarefa,Tarefa tarefa) {
    Tarefa tarefaAnterior;

    if(instancia == NULL) {
        return FALSO;
    }

    else if((*instancia).tarefas == NULL) {
        return FALSO;
    }

    else if(indiceDaTarefa >= (*instancia).quantidadeDeTarefas) {
        return FALSO;
    }

    else if(tarefaEhValida(&tarefa) == FALSO) {
        return FALSO;
    }

    tarefaAnterior = (*instancia).tarefas[indiceDaTarefa];

    if(tarefaEhValida(&tarefaAnterior) == VERDADEIRO) {
        (*instancia).somaDosTemposDeProcessamento -= tarefaAnterior.tempoProcessamento;
    }

    (*instancia).tarefas[indiceDaTarefa] = tarefa;
    (*instancia).somaDosTemposDeProcessamento += tarefa.tempoProcessamento;

    return VERDADEIRO;
}

Boolean instanciaEhValida(const Instancia *instancia) {
    QuantidadeDeTarefas indiceDaTarefa;

    if(instancia == NULL) {
        return FALSO;
    }

    else if((*instancia).identificadorDaInstancia == 0) {
        return FALSO;
    }

    else if((*instancia).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    else if((*instancia).tarefas == NULL) {
        return FALSO;
    }

    else if((*instancia).somaDosTemposDeProcessamento == 0) {
        return FALSO;
    }

    for(indiceDaTarefa = 0; indiceDaTarefa < (*instancia).quantidadeDeTarefas; indiceDaTarefa++) {
        if(tarefaEhValida(&(*instancia).tarefas[indiceDaTarefa]) == FALSO) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

DataDeEntregaComum instanciaCalcularDataDeEntregaComum(const Instancia *instancia,FatorH fatorH) {
    DataDeEntregaComum dataDeEntregaComum;

    if(instancia == NULL) {
        return 0;
    }

    else if(fatorH == 0) {
        return 0;
    }

    dataDeEntregaComum = (DataDeEntregaComum) (((*instancia).somaDosTemposDeProcessamento * fatorH) / FATOR_DE_ESCALA_H);

    return dataDeEntregaComum;
}

void liberarInstancia(Instancia *instancia) {
    if(instancia == NULL) {
        return;
    }

    if((*instancia).tarefas != NULL) {
        free((*instancia).tarefas);
    }

    (*instancia) = criarInstanciaVazia();
}
