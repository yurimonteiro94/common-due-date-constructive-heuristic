#include <stdio.h>

#include "../model/entidades/tarefa/tarefa.h"

static int testarCriacaoDeTarefaValida(void) {
    Tarefa tarefa;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 1,
        (TempoDeProcessamento) 20,
        (Penalidade) 4,
        (Penalidade) 5
    );

    if(tarefaEhValida(&tarefa) == FALSO) {
        printf("[ERRO] A tarefa deveria ser valida.\n");

        return 1;
    }

    if(tarefa.identificador != 1) {
        printf("[ERRO] Identificador incorreto.\n");

        return 1;
    }

    if(tarefa.tempoProcessamento != 20) {
        printf("[ERRO] Tempo de processamento incorreto.\n");

        return 1;
    }

    if(tarefa.penalidadeAdiantamento != 4) {
        printf("[ERRO] Penalidade de adiantamento incorreta.\n");

        return 1;
    }

    if(tarefa.penalidadeAtraso != 5) {
        printf("[ERRO] Penalidade de atraso incorreta.\n");

        return 1;
    }

    printf("[OK] Criacao de tarefa valida.\n");

    return 0;
}

static int testarTarefaNulaInvalida(void) {
    if(tarefaEhValida(NULL) != FALSO) {
        printf("[ERRO] Ponteiro NULL deveria ser invalido.\n");

        return 1;
    }

    printf("[OK] Tarefa nula invalida.\n");

    return 0;
}

static int testarTarefaComIdentificadorZeroInvalida(void) {
    Tarefa tarefa;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 0,
        (TempoDeProcessamento) 20,
        (Penalidade) 4,
        (Penalidade) 5
    );

    if(tarefaEhValida(&tarefa) != FALSO) {
        printf("[ERRO] Tarefa com identificador zero deveria ser invalida.\n");

        return 1;
    }

    printf("[OK] Tarefa com identificador zero invalida.\n");

    return 0;
}

static int testarTarefaComTempoZeroInvalida(void) {
    Tarefa tarefa;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 1,
        (TempoDeProcessamento) 0,
        (Penalidade) 4,
        (Penalidade) 5
    );

    if(tarefaEhValida(&tarefa) != FALSO) {
        printf("[ERRO] Tarefa com tempo zero deveria ser invalida.\n");

        return 1;
    }

    printf("[OK] Tarefa com tempo zero invalida.\n");

    return 0;
}

static int testarTarefaComPenalidadeAdiantamentoZeroInvalida(void) {
    Tarefa tarefa;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 1,
        (TempoDeProcessamento) 20,
        (Penalidade) 0,
        (Penalidade) 5
    );

    if(tarefaEhValida(&tarefa) != FALSO) {
        printf("[ERRO] Tarefa com penalidade de adiantamento zero deveria ser invalida.\n");

        return 1;
    }

    printf("[OK] Tarefa com penalidade de adiantamento zero invalida.\n");

    return 0;
}

static int testarTarefaComPenalidadeAtrasoZeroInvalida(void) {
    Tarefa tarefa;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 1,
        (TempoDeProcessamento) 20,
        (Penalidade) 4,
        (Penalidade) 0
    );

    if(tarefaEhValida(&tarefa) != FALSO) {
        printf("[ERRO] Tarefa com penalidade de atraso zero deveria ser invalida.\n");

        return 1;
    }

    printf("[OK] Tarefa com penalidade de atraso zero invalida.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes da entidade Tarefa...\n\n");

    quantidadeDeErros += testarCriacaoDeTarefaValida();
    quantidadeDeErros += testarTarefaNulaInvalida();
    quantidadeDeErros += testarTarefaComIdentificadorZeroInvalida();
    quantidadeDeErros += testarTarefaComTempoZeroInvalida();
    quantidadeDeErros += testarTarefaComPenalidadeAdiantamentoZeroInvalida();
    quantidadeDeErros += testarTarefaComPenalidadeAtrasoZeroInvalida();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes da entidade Tarefa passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes da entidade Tarefa: %d\n",quantidadeDeErros);

    return 1;
}
