#include <stdio.h>
#include <string.h>

#include "../model/entidades/heuristica/heuristica.h"

static int testarCriacaoDeHeuristicaVazia(void) {
    Heuristica heuristica;

    heuristica = criarHeuristicaVazia();

    if(heuristica.identificadorDaHeuristica != 0) {
        printf("[ERRO] Identificador da heuristica vazia deveria ser zero.\n");

        return 1;
    }

    if(heuristica.nomeDaHeuristica[0] != '\0') {
        printf("[ERRO] Nome da heuristica vazia deveria iniciar vazio.\n");

        return 1;
    }

    if(heuristica.descricaoDaHeuristica[0] != '\0') {
        printf("[ERRO] Descricao da heuristica vazia deveria iniciar vazia.\n");

        return 1;
    }

    printf("[OK] Criacao de heuristica vazia.\n");

    return 0;
}

static int testarInicializacaoDeHeuristica(void) {
    Heuristica heuristica;
    Boolean resultado;

    heuristica = criarHeuristicaVazia();

    resultado = inicializarHeuristica(
        &heuristica,
        (IdentificadorDeHeuristica) 10,
        "Heuristica de Teste",
        "Descricao da heuristica de teste."
    );

    if(resultado == FALSO) {
        printf("[ERRO] Heuristica deveria ser inicializada.\n");

        return 1;
    }

    if(heuristica.identificadorDaHeuristica != 10) {
        printf("[ERRO] Identificador da heuristica incorreto.\n");

        return 1;
    }

    if(strcmp(heuristica.nomeDaHeuristica,"Heuristica de Teste") != 0) {
        printf("[ERRO] Nome da heuristica incorreto.\n");

        return 1;
    }

    if(strcmp(heuristica.descricaoDaHeuristica,"Descricao da heuristica de teste.") != 0) {
        printf("[ERRO] Descricao da heuristica incorreta.\n");

        return 1;
    }

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("[ERRO] Heuristica deveria ser valida.\n");

        return 1;
    }

    printf("[OK] Inicializacao de heuristica.\n");

    return 0;
}

static int testarCriacaoDaHeuristicaConstrutivaFuzzy(void) {
    Heuristica heuristica;

    heuristica = criarHeuristicaConstrutivaFuzzy();

    if(heuristica.identificadorDaHeuristica != IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY) {
        printf("[ERRO] Identificador da heuristica construtiva por arbitragem temporal preditiva incorreto.\n");

        return 1;
    }

    if(strcmp(heuristica.nomeDaHeuristica,NOME_HEURISTICA_CONSTRUTIVA_FUZZY) != 0) {
        printf("[ERRO] Nome da heuristica construtiva por arbitragem temporal preditiva incorreto.\n");

        return 1;
    }

    if(strcmp(heuristica.descricaoDaHeuristica,DESCRICAO_HEURISTICA_CONSTRUTIVA_FUZZY) != 0) {
        printf("[ERRO] Descricao da heuristica construtiva por arbitragem temporal preditiva incorreta.\n");

        return 1;
    }

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("[ERRO] Heuristica construtiva fuzzy deveria ser valida.\n");

        return 1;
    }

    printf("[OK] Criacao da heuristica construtiva por arbitragem temporal preditiva.\n");

    return 0;
}

static int testarHeuristicaNulaInvalida(void) {
    if(heuristicaEhValida(NULL) != FALSO) {
        printf("[ERRO] Heuristica NULL deveria ser invalida.\n");

        return 1;
    }

    printf("[OK] Heuristica nula invalida.\n");

    return 0;
}

static int testarInicializacaoComIdentificadorZeroInvalida(void) {
    Heuristica heuristica;
    Boolean resultado;

    heuristica = criarHeuristicaVazia();

    resultado = inicializarHeuristica(
        &heuristica,
        (IdentificadorDeHeuristica) 0,
        "Heuristica de Teste",
        "Descricao da heuristica de teste."
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com identificador zero deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com identificador zero invalida.\n");

    return 0;
}

static int testarInicializacaoComNomeNuloInvalida(void) {
    Heuristica heuristica;
    Boolean resultado;

    heuristica = criarHeuristicaVazia();

    resultado = inicializarHeuristica(
        &heuristica,
        (IdentificadorDeHeuristica) 1,
        NULL,
        "Descricao da heuristica de teste."
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com nome NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com nome nulo invalida.\n");

    return 0;
}

static int testarInicializacaoComNomeVazioInvalida(void) {
    Heuristica heuristica;
    Boolean resultado;

    heuristica = criarHeuristicaVazia();

    resultado = inicializarHeuristica(
        &heuristica,
        (IdentificadorDeHeuristica) 1,
        "",
        "Descricao da heuristica de teste."
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com nome vazio deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com nome vazio invalida.\n");

    return 0;
}

static int testarInicializacaoComDescricaoNulaInvalida(void) {
    Heuristica heuristica;
    Boolean resultado;

    heuristica = criarHeuristicaVazia();

    resultado = inicializarHeuristica(
        &heuristica,
        (IdentificadorDeHeuristica) 1,
        "Heuristica de Teste",
        NULL
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com descricao NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com descricao nula invalida.\n");

    return 0;
}

static int testarInicializacaoComDescricaoVaziaInvalida(void) {
    Heuristica heuristica;
    Boolean resultado;

    heuristica = criarHeuristicaVazia();

    resultado = inicializarHeuristica(
        &heuristica,
        (IdentificadorDeHeuristica) 1,
        "Heuristica de Teste",
        ""
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com descricao vazia deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com descricao vazia invalida.\n");

    return 0;
}

static int testarLimpezaDeHeuristica(void) {
    Heuristica heuristica;

    heuristica = criarHeuristicaConstrutivaFuzzy();

    limparHeuristica(&heuristica);

    if(heuristica.identificadorDaHeuristica != 0) {
        printf("[ERRO] Identificador deveria ser zero apos limpar.\n");

        return 1;
    }

    if(heuristica.nomeDaHeuristica[0] != '\0') {
        printf("[ERRO] Nome deveria ficar vazio apos limpar.\n");

        return 1;
    }

    if(heuristica.descricaoDaHeuristica[0] != '\0') {
        printf("[ERRO] Descricao deveria ficar vazia apos limpar.\n");

        return 1;
    }

    printf("[OK] Limpeza de heuristica.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes da entidade Heuristica...\n\n");

    quantidadeDeErros += testarCriacaoDeHeuristicaVazia();
    quantidadeDeErros += testarInicializacaoDeHeuristica();
    quantidadeDeErros += testarCriacaoDaHeuristicaConstrutivaFuzzy();
    quantidadeDeErros += testarHeuristicaNulaInvalida();
    quantidadeDeErros += testarInicializacaoComIdentificadorZeroInvalida();
    quantidadeDeErros += testarInicializacaoComNomeNuloInvalida();
    quantidadeDeErros += testarInicializacaoComNomeVazioInvalida();
    quantidadeDeErros += testarInicializacaoComDescricaoNulaInvalida();
    quantidadeDeErros += testarInicializacaoComDescricaoVaziaInvalida();
    quantidadeDeErros += testarLimpezaDeHeuristica();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes da entidade Heuristica passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes da entidade Heuristica: %d\n",quantidadeDeErros);

    return 1;
}
