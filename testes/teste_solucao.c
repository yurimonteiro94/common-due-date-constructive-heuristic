#include <stdio.h>

#include "../model/entidades/solucao/solucao.h"

static int testarCriacaoDeSolucaoVazia(void) {
    Solucao solucao;

    solucao = criarSolucaoVazia();

    if(solucao.quantidadeDeTarefas != 0) {
        printf("[ERRO] Quantidade de tarefas deveria iniciar em zero.\n");

        return 1;
    }

    if(solucao.quantidadeDeTarefasAlocadas != 0) {
        printf("[ERRO] Quantidade de tarefas alocadas deveria iniciar em zero.\n");

        return 1;
    }

    if(solucao.sequenciaDeTarefas != NULL) {
        printf("[ERRO] Sequencia de tarefas deveria iniciar como NULL.\n");

        return 1;
    }

    printf("[OK] Criacao de solucao vazia.\n");

    return 0;
}

static int testarInicializacaoDeSolucao(void) {
    Solucao solucao;
    Boolean resultado;

    solucao = criarSolucaoVazia();

    resultado = inicializarSolucao(&solucao,(QuantidadeDeTarefas) 3);

    if(resultado == FALSO) {
        printf("[ERRO] Solucao deveria ser inicializada.\n");

        return 1;
    }

    if(solucao.quantidadeDeTarefas != 3) {
        printf("[ERRO] Quantidade de tarefas incorreta.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    if(solucao.quantidadeDeTarefasAlocadas != 0) {
        printf("[ERRO] Quantidade de tarefas alocadas deveria iniciar em zero.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    if(solucao.sequenciaDeTarefas == NULL) {
        printf("[ERRO] Sequencia de tarefas nao deveria ser NULL.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    if(solucao.sequenciaDeTarefas[0] != 0 || solucao.sequenciaDeTarefas[1] != 0 || solucao.sequenciaDeTarefas[2] != 0) {
        printf("[ERRO] Sequencia deveria iniciar preenchida com zeros.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    liberarSolucao(&solucao);

    printf("[OK] Inicializacao de solucao.\n");

    return 0;
}

static int testarDefinicaoDeTarefasNaSolucao(void) {
    Solucao solucao;

    solucao = criarSolucaoVazia();

    inicializarSolucao(&solucao,(QuantidadeDeTarefas) 3);

    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 0,(IdentificadorDeTarefa) 1);
    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 1,(IdentificadorDeTarefa) 2);
    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 2,(IdentificadorDeTarefa) 3);

    if(solucao.quantidadeDeTarefasAlocadas != 3) {
        printf("[ERRO] Quantidade de tarefas alocadas deveria ser 3.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    if(solucaoEhCompleta(&solucao) == FALSO) {
        printf("[ERRO] Solucao deveria estar completa.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    if(solucaoEhValida(&solucao) == FALSO) {
        printf("[ERRO] Solucao deveria ser valida.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    liberarSolucao(&solucao);

    printf("[OK] Definicao de tarefas na solucao.\n");

    return 0;
}

static int testarSubstituicaoDeTarefaNaSolucao(void) {
    Solucao solucao;

    solucao = criarSolucaoVazia();

    inicializarSolucao(&solucao,(QuantidadeDeTarefas) 3);

    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 0,(IdentificadorDeTarefa) 1);
    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 0,(IdentificadorDeTarefa) 2);

    if(solucao.quantidadeDeTarefasAlocadas != 1) {
        printf("[ERRO] Substituicao nao deveria aumentar tarefas alocadas.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    if(solucao.sequenciaDeTarefas[0] != 2) {
        printf("[ERRO] Tarefa na posicao zero deveria ter sido substituida por 2.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    liberarSolucao(&solucao);

    printf("[OK] Substituicao de tarefa na solucao.\n");

    return 0;
}

static int testarSolucaoIncompletaInvalida(void) {
    Solucao solucao;

    solucao = criarSolucaoVazia();

    inicializarSolucao(&solucao,(QuantidadeDeTarefas) 3);

    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 0,(IdentificadorDeTarefa) 1);
    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 1,(IdentificadorDeTarefa) 2);

    if(solucaoEhCompleta(&solucao) != FALSO) {
        printf("[ERRO] Solucao incompleta deveria retornar FALSO em solucaoEhCompleta.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    if(solucaoEhValida(&solucao) != FALSO) {
        printf("[ERRO] Solucao incompleta deveria ser invalida.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    liberarSolucao(&solucao);

    printf("[OK] Solucao incompleta invalida.\n");

    return 0;
}

static int testarSolucaoComTarefaRepetidaInvalida(void) {
    Solucao solucao;

    solucao = criarSolucaoVazia();

    inicializarSolucao(&solucao,(QuantidadeDeTarefas) 3);

    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 0,(IdentificadorDeTarefa) 1);
    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 1,(IdentificadorDeTarefa) 2);
    solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 2,(IdentificadorDeTarefa) 2);

    if(solucaoEhCompleta(&solucao) == FALSO) {
        printf("[ERRO] Solucao com repeticao ainda deveria estar completa em quantidade.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    if(solucaoEhValida(&solucao) != FALSO) {
        printf("[ERRO] Solucao com tarefa repetida deveria ser invalida.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    liberarSolucao(&solucao);

    printf("[OK] Solucao com tarefa repetida invalida.\n");

    return 0;
}

static int testarPosicaoInvalidaNaSolucao(void) {
    Solucao solucao;
    Boolean resultado;

    solucao = criarSolucaoVazia();

    inicializarSolucao(&solucao,(QuantidadeDeTarefas) 2);

    resultado = solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 2,(IdentificadorDeTarefa) 1);

    if(resultado != FALSO) {
        printf("[ERRO] Posicao invalida deveria falhar.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    liberarSolucao(&solucao);

    printf("[OK] Posicao invalida na solucao.\n");

    return 0;
}

static int testarIdentificadorZeroNaSolucao(void) {
    Solucao solucao;
    Boolean resultado;

    solucao = criarSolucaoVazia();

    inicializarSolucao(&solucao,(QuantidadeDeTarefas) 2);

    resultado = solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 0,(IdentificadorDeTarefa) 0);

    if(resultado != FALSO) {
        printf("[ERRO] Identificador zero deveria falhar.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    liberarSolucao(&solucao);

    printf("[OK] Identificador zero na solucao.\n");

    return 0;
}

static int testarIdentificadorMaiorQueQuantidadeNaSolucao(void) {
    Solucao solucao;
    Boolean resultado;

    solucao = criarSolucaoVazia();

    inicializarSolucao(&solucao,(QuantidadeDeTarefas) 2);

    resultado = solucaoDefinirTarefaNaPosicao(&solucao,(QuantidadeDeTarefas) 0,(IdentificadorDeTarefa) 3);

    if(resultado != FALSO) {
        printf("[ERRO] Identificador maior que a quantidade de tarefas deveria falhar.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    liberarSolucao(&solucao);

    printf("[OK] Identificador maior que quantidade na solucao.\n");

    return 0;
}

static int testarInicializacaoComQuantidadeZeroInvalida(void) {
    Solucao solucao;
    Boolean resultado;

    solucao = criarSolucaoVazia();

    resultado = inicializarSolucao(&solucao,(QuantidadeDeTarefas) 0);

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com quantidade zero deveria falhar.\n");
        liberarSolucao(&solucao);

        return 1;
    }

    printf("[OK] Inicializacao de solucao com quantidade zero invalida.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes da entidade Solucao...\n\n");

    quantidadeDeErros += testarCriacaoDeSolucaoVazia();
    quantidadeDeErros += testarInicializacaoDeSolucao();
    quantidadeDeErros += testarDefinicaoDeTarefasNaSolucao();
    quantidadeDeErros += testarSubstituicaoDeTarefaNaSolucao();
    quantidadeDeErros += testarSolucaoIncompletaInvalida();
    quantidadeDeErros += testarSolucaoComTarefaRepetidaInvalida();
    quantidadeDeErros += testarPosicaoInvalidaNaSolucao();
    quantidadeDeErros += testarIdentificadorZeroNaSolucao();
    quantidadeDeErros += testarIdentificadorMaiorQueQuantidadeNaSolucao();
    quantidadeDeErros += testarInicializacaoComQuantidadeZeroInvalida();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes da entidade Solucao passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes da entidade Solucao: %d\n",quantidadeDeErros);

    return 1;
}
