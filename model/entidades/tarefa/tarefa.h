#ifndef TAREFA_H
#define TAREFA_H

#include "../../../services/constantes/constantes.h"

typedef struct Tarefa {
    IdentificadorDeTarefa identificador;
    TempoDeProcessamento tempoProcessamento;
    Penalidade penalidadeAdiantamento;
    Penalidade penalidadeAtraso;
} Tarefa;

Tarefa criarTarefa(IdentificadorDeTarefa identificador,TempoDeProcessamento tempoProcessamento,Penalidade penalidadeAdiantamento,Penalidade penalidadeAtraso);
Boolean tarefaEhValida(const Tarefa *tarefa);

#endif
