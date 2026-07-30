#include <stdio.h>

#include "../model/entidades/tarefa/tarefa.h"
#include "../services/gerenciador_fuzzy/gerenciador_fuzzy.h"

static int valorEstaNoIntervalo(double valor,double limiteInferior,double limiteSuperior) {
    if(valor < limiteInferior) {
        return 0;
    }

    else if(valor > limiteSuperior) {
        return 0;
    }

    return 1;
}

static int testarPertinenciaCrescente(void) {
    double pertinencia;

    pertinencia = 0.0;

    if(gerenciadorFuzzyCalcularPertinenciaCrescente(5.0,0.0,10.0,&pertinencia) == FALSO) {
        printf("[ERRO] Pertinencia crescente deveria ser calculada.\n");

        return 1;
    }

    if(pertinencia != 0.5) {
        printf("[ERRO] Pertinencia crescente deveria ser 0.5.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaCrescente(-1.0,0.0,10.0,&pertinencia) == FALSO || pertinencia != 0.0) {
        printf("[ERRO] Pertinencia crescente abaixo do limite deveria ser 0.0.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaCrescente(15.0,0.0,10.0,&pertinencia) == FALSO || pertinencia != 1.0) {
        printf("[ERRO] Pertinencia crescente acima do limite deveria ser 1.0.\n");

        return 1;
    }

    printf("[OK] Pertinencia crescente.\n");

    return 0;
}

static int testarPertinenciaDecrescente(void) {
    double pertinencia;

    pertinencia = 0.0;

    if(gerenciadorFuzzyCalcularPertinenciaDecrescente(5.0,0.0,10.0,&pertinencia) == FALSO) {
        printf("[ERRO] Pertinencia decrescente deveria ser calculada.\n");

        return 1;
    }

    if(pertinencia != 0.5) {
        printf("[ERRO] Pertinencia decrescente deveria ser 0.5.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaDecrescente(-1.0,0.0,10.0,&pertinencia) == FALSO || pertinencia != 1.0) {
        printf("[ERRO] Pertinencia decrescente abaixo do limite deveria ser 1.0.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaDecrescente(15.0,0.0,10.0,&pertinencia) == FALSO || pertinencia != 0.0) {
        printf("[ERRO] Pertinencia decrescente acima do limite deveria ser 0.0.\n");

        return 1;
    }

    printf("[OK] Pertinencia decrescente.\n");

    return 0;
}

static int testarPertinenciaTriangular(void) {
    double pertinencia;

    pertinencia = 0.0;

    if(gerenciadorFuzzyCalcularPertinenciaTriangular(5.0,0.0,5.0,10.0,&pertinencia) == FALSO || pertinencia != 1.0) {
        printf("[ERRO] Pertinencia triangular no centro deveria ser 1.0.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaTriangular(2.5,0.0,5.0,10.0,&pertinencia) == FALSO || pertinencia != 0.5) {
        printf("[ERRO] Pertinencia triangular antes do centro deveria ser 0.5.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaTriangular(7.5,0.0,5.0,10.0,&pertinencia) == FALSO || pertinencia != 0.5) {
        printf("[ERRO] Pertinencia triangular depois do centro deveria ser 0.5.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaTriangular(0.0,0.0,5.0,10.0,&pertinencia) == FALSO || pertinencia != 0.0) {
        printf("[ERRO] Pertinencia triangular no limite inferior deveria ser 0.0.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaTriangular(10.0,0.0,5.0,10.0,&pertinencia) == FALSO || pertinencia != 0.0) {
        printf("[ERRO] Pertinencia triangular no limite superior deveria ser 0.0.\n");

        return 1;
    }

    printf("[OK] Pertinencia triangular.\n");

    return 0;
}

static int testarPrioridadeAntesDaDataDeEntrega(void) {
    Tarefa tarefa;
    double prioridade;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 1,
        (TempoDeProcessamento) 10,
        (Penalidade) 4,
        (Penalidade) 8
    );

    prioridade = 0.0;

    if(gerenciadorFuzzyCalcularPrioridadeAntesDaDataDeEntrega(&tarefa,(DataDeEntregaComum) 36,(SomaDosTemposDeProcessamento) 20,&prioridade) == FALSO) {
        printf("[ERRO] Prioridade antes da data deveria ser calculada.\n");

        return 1;
    }

    if(valorEstaNoIntervalo(prioridade,0.63,0.65) == 0) {
        printf("[ERRO] Prioridade antes da data ficou fora do intervalo esperado.\n");

        return 1;
    }

    printf("[OK] Prioridade antes da data de entrega.\n");

    return 0;
}

static int testarPrioridadeAntesDaDataDeEntregaSemFolga(void) {
    Tarefa tarefa;
    double prioridade;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 1,
        (TempoDeProcessamento) 10,
        (Penalidade) 4,
        (Penalidade) 8
    );

    prioridade = 0.0;

    if(gerenciadorFuzzyCalcularPrioridadeAntesDaDataDeEntrega(&tarefa,(DataDeEntregaComum) 36,(SomaDosTemposDeProcessamento) 36,&prioridade) == FALSO) {
        printf("[ERRO] Prioridade antes da data sem folga deveria ser calculada.\n");

        return 1;
    }

    if(valorEstaNoIntervalo(prioridade,0.48,0.50) == 0) {
        printf("[ERRO] Prioridade antes da data sem folga ficou fora do intervalo esperado.\n");

        return 1;
    }

    printf("[OK] Prioridade antes da data de entrega sem folga.\n");

    return 0;
}

static int testarPrioridadeDepoisDaDataDeEntrega(void) {
    Tarefa tarefa;
    double prioridade;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 1,
        (TempoDeProcessamento) 10,
        (Penalidade) 12,
        (Penalidade) 3
    );

    prioridade = 0.0;

    if(gerenciadorFuzzyCalcularPrioridadeDepoisDaDataDeEntrega(&tarefa,&prioridade) == FALSO) {
        printf("[ERRO] Prioridade depois da data deveria ser calculada.\n");

        return 1;
    }

    if(valorEstaNoIntervalo(prioridade,0.72,0.74) == 0) {
        printf("[ERRO] Prioridade depois da data ficou fora do intervalo esperado.\n");

        return 1;
    }

    printf("[OK] Prioridade depois da data de entrega.\n");

    return 0;
}

static int testarParametrosInvalidos(void) {
    Tarefa tarefa;
    double pertinencia;
    double prioridade;

    tarefa = criarTarefa(
        (IdentificadorDeTarefa) 1,
        (TempoDeProcessamento) 10,
        (Penalidade) 4,
        (Penalidade) 8
    );

    pertinencia = 0.0;
    prioridade = 0.0;

    if(gerenciadorFuzzyCalcularPertinenciaCrescente(5.0,10.0,0.0,&pertinencia) != FALSO) {
        printf("[ERRO] Pertinencia crescente com limites invalidos deveria falhar.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaDecrescente(5.0,10.0,0.0,&pertinencia) != FALSO) {
        printf("[ERRO] Pertinencia decrescente com limites invalidos deveria falhar.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPertinenciaTriangular(5.0,0.0,10.0,5.0,&pertinencia) != FALSO) {
        printf("[ERRO] Pertinencia triangular com limites invalidos deveria falhar.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPrioridadeAntesDaDataDeEntrega(NULL,(DataDeEntregaComum) 36,(SomaDosTemposDeProcessamento) 20,&prioridade) != FALSO) {
        printf("[ERRO] Prioridade antes com tarefa NULL deveria falhar.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPrioridadeAntesDaDataDeEntrega(&tarefa,(DataDeEntregaComum) 0,(SomaDosTemposDeProcessamento) 20,&prioridade) != FALSO) {
        printf("[ERRO] Prioridade antes com data zero deveria falhar.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPrioridadeAntesDaDataDeEntrega(&tarefa,(DataDeEntregaComum) 36,(SomaDosTemposDeProcessamento) 20,NULL) != FALSO) {
        printf("[ERRO] Prioridade antes com ponteiro NULL deveria falhar.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPrioridadeDepoisDaDataDeEntrega(NULL,&prioridade) != FALSO) {
        printf("[ERRO] Prioridade depois com tarefa NULL deveria falhar.\n");

        return 1;
    }

    if(gerenciadorFuzzyCalcularPrioridadeDepoisDaDataDeEntrega(&tarefa,NULL) != FALSO) {
        printf("[ERRO] Prioridade depois com ponteiro NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Parametros invalidos do gerenciador fuzzy.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do GerenciadorFuzzy...\n\n");

    quantidadeDeErros += testarPertinenciaCrescente();
    quantidadeDeErros += testarPertinenciaDecrescente();
    quantidadeDeErros += testarPertinenciaTriangular();
    quantidadeDeErros += testarPrioridadeAntesDaDataDeEntrega();
    quantidadeDeErros += testarPrioridadeAntesDaDataDeEntregaSemFolga();
    quantidadeDeErros += testarPrioridadeDepoisDaDataDeEntrega();
    quantidadeDeErros += testarParametrosInvalidos();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do GerenciadorFuzzy passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes do GerenciadorFuzzy: %d\n",quantidadeDeErros);

    return 1;
}
