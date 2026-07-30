#include "tarefa.h"

#include <stddef.h>

Tarefa criarTarefa(IdentificadorDeTarefa identificador,TempoDeProcessamento tempoProcessamento,Penalidade penalidadeAdiantamento,Penalidade penalidadeAtraso) {
    Tarefa tarefa;

    tarefa.identificador = identificador;
    tarefa.tempoProcessamento = tempoProcessamento;
    tarefa.penalidadeAdiantamento = penalidadeAdiantamento;
    tarefa.penalidadeAtraso = penalidadeAtraso;

    return tarefa;
}

Boolean tarefaEhValida(const Tarefa *tarefa) {
    if(tarefa == NULL) {
        return FALSO;
    }

    else if((*tarefa).identificador == 0) {
        return FALSO;
    }

    else if((*tarefa).tempoProcessamento == 0) {
        return FALSO;
    }

    else if((*tarefa).penalidadeAdiantamento == 0) {
        return FALSO;
    }

    else if((*tarefa).penalidadeAtraso == 0) {
        return FALSO;
    }

    return VERDADEIRO;
}
