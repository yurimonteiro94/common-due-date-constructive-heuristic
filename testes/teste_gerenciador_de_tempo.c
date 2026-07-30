#include <stdio.h>

#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

static int testarObtencaoDeTempoAtual(void) {
    TempoComputacionalEmSegundos tempoAtual;

    tempoAtual = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(tempoAtual < 0.0) {
        printf("[ERRO] Tempo atual nao deveria ser negativo.\n");

        return 1;
    }

    printf("[OK] Obtencao de tempo atual.\n");

    return 0;
}

static int testarCalculoDeDuracaoValida(void) {
    TempoComputacionalEmSegundos duracao;

    duracao = gerenciadorDeTempoCalcularDuracaoEmSegundos(2.5,5.0);

    if(duracao != 2.5) {
        printf("[ERRO] Duracao deveria ser 2.5 segundos.\n");

        return 1;
    }

    printf("[OK] Calculo de duracao valida.\n");

    return 0;
}

static int testarCalculoDeDuracaoComTempoFinalMenor(void) {
    TempoComputacionalEmSegundos duracao;

    duracao = gerenciadorDeTempoCalcularDuracaoEmSegundos(5.0,2.5);

    if(duracao != 0.0) {
        printf("[ERRO] Duracao com tempo final menor deveria retornar zero.\n");

        return 1;
    }

    printf("[OK] Calculo de duracao com tempo final menor.\n");

    return 0;
}

static int testarCalculoDeDuracaoComTempoInicialNegativo(void) {
    TempoComputacionalEmSegundos duracao;

    duracao = gerenciadorDeTempoCalcularDuracaoEmSegundos(-1.0,2.5);

    if(duracao != 0.0) {
        printf("[ERRO] Duracao com tempo inicial negativo deveria retornar zero.\n");

        return 1;
    }

    printf("[OK] Calculo de duracao com tempo inicial negativo.\n");

    return 0;
}

static int testarConversaoDeSegundosParaMilissegundos(void) {
    TempoComputacionalEmMilissegundos tempoEmMilissegundos;

    tempoEmMilissegundos = gerenciadorDeTempoConverterSegundosParaMilissegundos(1.25);

    if(tempoEmMilissegundos != 1250.0) {
        printf("[ERRO] Conversao deveria resultar em 1250.0 milissegundos.\n");

        return 1;
    }

    printf("[OK] Conversao de segundos para milissegundos.\n");

    return 0;
}

static int testarConversaoDeTempoNegativoParaMilissegundos(void) {
    TempoComputacionalEmMilissegundos tempoEmMilissegundos;

    tempoEmMilissegundos = gerenciadorDeTempoConverterSegundosParaMilissegundos(-1.25);

    if(tempoEmMilissegundos != 0.0) {
        printf("[ERRO] Conversao de tempo negativo deveria retornar zero.\n");

        return 1;
    }

    printf("[OK] Conversao de tempo negativo para milissegundos.\n");

    return 0;
}

static int testarValidacaoDeTempo(void) {
    if(gerenciadorDeTempoValorEhValido(0.0) == FALSO) {
        printf("[ERRO] Tempo zero deveria ser valido.\n");

        return 1;
    }

    if(gerenciadorDeTempoValorEhValido(1.0) == FALSO) {
        printf("[ERRO] Tempo positivo deveria ser valido.\n");

        return 1;
    }

    if(gerenciadorDeTempoValorEhValido(-1.0) != FALSO) {
        printf("[ERRO] Tempo negativo deveria ser invalido.\n");

        return 1;
    }

    printf("[OK] Validacao de tempo.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do GerenciadorDeTempo...\n\n");

    quantidadeDeErros += testarObtencaoDeTempoAtual();
    quantidadeDeErros += testarCalculoDeDuracaoValida();
    quantidadeDeErros += testarCalculoDeDuracaoComTempoFinalMenor();
    quantidadeDeErros += testarCalculoDeDuracaoComTempoInicialNegativo();
    quantidadeDeErros += testarConversaoDeSegundosParaMilissegundos();
    quantidadeDeErros += testarConversaoDeTempoNegativoParaMilissegundos();
    quantidadeDeErros += testarValidacaoDeTempo();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do GerenciadorDeTempo passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes do GerenciadorDeTempo: %d\n",quantidadeDeErros);

    return 1;
}
