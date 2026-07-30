#include <stdio.h>
#include <string.h>

#include "../services/ferramentas/ferramentas.h"

static int testarTextoNuloOuVazio(void) {
    if(textoEhNuloOuVazio(NULL) != VERDADEIRO) {
        printf("[ERRO] Texto NULL deveria ser considerado vazio.\n");

        return 1;
    }

    if(textoEhNuloOuVazio("") != VERDADEIRO) {
        printf("[ERRO] Texto vazio deveria ser considerado vazio.\n");

        return 1;
    }

    if(textoEhNuloOuVazio("abc") != FALSO) {
        printf("[ERRO] Texto preenchido nao deveria ser considerado vazio.\n");

        return 1;
    }

    printf("[OK] Verificacao de texto nulo ou vazio.\n");

    return 0;
}

static int testarRemocaoDeQuebraDeLinhaComBarraN(void) {
    char texto[32];

    copiarTexto(texto,(InteiroPositivoDe32Bits) 32,"linha teste\n");

    removerQuebraDeLinha(texto);

    if(strcmp(texto,"linha teste") != 0) {
        printf("[ERRO] Quebra de linha com barra n nao foi removida corretamente.\n");

        return 1;
    }

    printf("[OK] Remocao de quebra de linha com barra n.\n");

    return 0;
}

static int testarRemocaoDeQuebraDeLinhaComBarraRBarraN(void) {
    char texto[32];

    copiarTexto(texto,(InteiroPositivoDe32Bits) 32,"linha teste\r\n");

    removerQuebraDeLinha(texto);

    if(strcmp(texto,"linha teste") != 0) {
        printf("[ERRO] Quebra de linha com barra r barra n nao foi removida corretamente.\n");

        return 1;
    }

    printf("[OK] Remocao de quebra de linha com barra r barra n.\n");

    return 0;
}

static int testarCopiaDeTexto(void) {
    char destino[32];
    Boolean resultado;

    resultado = copiarTexto(destino,(InteiroPositivoDe32Bits) 32,"texto copiado");

    if(resultado == FALSO) {
        printf("[ERRO] Copia de texto deveria funcionar.\n");

        return 1;
    }

    if(strcmp(destino,"texto copiado") != 0) {
        printf("[ERRO] Texto copiado incorretamente.\n");

        return 1;
    }

    printf("[OK] Copia de texto.\n");

    return 0;
}

static int testarCopiaComBufferPequeno(void) {
    char destino[4];
    Boolean resultado;

    resultado = copiarTexto(destino,(InteiroPositivoDe32Bits) 4,"texto grande");

    if(resultado != FALSO) {
        printf("[ERRO] Copia com buffer pequeno deveria falhar.\n");

        return 1;
    }

    printf("[OK] Copia com buffer pequeno.\n");

    return 0;
}

static int testarMontagemDeCaminhoComArquivo(void) {
    char caminho[TAMANHO_MAXIMO_DE_CAMINHO];
    Boolean resultado;

    resultado = montarCaminhoComArquivo(
        caminho,
        (InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_CAMINHO,
        CAMINHO_DA_PASTA_DE_INSTANCIAS,
        NOME_ARQUIVO_SCH10
    );

    if(resultado == FALSO) {
        printf("[ERRO] Montagem de caminho deveria funcionar.\n");

        return 1;
    }

    if(strcmp(caminho,"instancias/sch10.txt") != 0) {
        printf("[ERRO] Caminho montado incorretamente.\n");

        return 1;
    }

    printf("[OK] Montagem de caminho com arquivo.\n");

    return 0;
}

static int testarConversaoDeFatorHParaNumeroDecimal(void) {
    if(fatorHParaNumeroDecimal((FatorH) FATOR_H_02) != 0.2) {
        printf("[ERRO] Fator h 0.2 incorreto.\n");

        return 1;
    }

    if(fatorHParaNumeroDecimal((FatorH) FATOR_H_04) != 0.4) {
        printf("[ERRO] Fator h 0.4 incorreto.\n");

        return 1;
    }

    if(fatorHParaNumeroDecimal((FatorH) FATOR_H_06) != 0.6) {
        printf("[ERRO] Fator h 0.6 incorreto.\n");

        return 1;
    }

    if(fatorHParaNumeroDecimal((FatorH) FATOR_H_08) != 0.8) {
        printf("[ERRO] Fator h 0.8 incorreto.\n");

        return 1;
    }

    if(fatorHParaNumeroDecimal((FatorH) 5) != 0.0) {
        printf("[ERRO] Fator h invalido deveria retornar 0.0.\n");

        return 1;
    }

    printf("[OK] Conversao de fator h para numero decimal.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes de Ferramentas...\n\n");

    quantidadeDeErros += testarTextoNuloOuVazio();
    quantidadeDeErros += testarRemocaoDeQuebraDeLinhaComBarraN();
    quantidadeDeErros += testarRemocaoDeQuebraDeLinhaComBarraRBarraN();
    quantidadeDeErros += testarCopiaDeTexto();
    quantidadeDeErros += testarCopiaComBufferPequeno();
    quantidadeDeErros += testarMontagemDeCaminhoComArquivo();
    quantidadeDeErros += testarConversaoDeFatorHParaNumeroDecimal();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes de Ferramentas passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes de Ferramentas: %d\n",quantidadeDeErros);

    return 1;
}
