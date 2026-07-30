#include <stdio.h>
#include <string.h>

#include "../services/ferramentas/ferramentas.h"
#include "../services/gerenciador_de_arquivos/gerenciador_de_arquivos.h"

#define CAMINHO_DO_ARQUIVO_DE_TESTE "resultados/teste_gerenciador_de_arquivos.txt"

static int testarCriacaoEExistenciaDeArquivo(void) {
    FILE *arquivo;

    arquivo = gerenciadorDeArquivosAbrirArquivoParaEscrita(CAMINHO_DO_ARQUIVO_DE_TESTE);

    if(arquivo == NULL) {
        printf("[ERRO] Arquivo de teste deveria ser aberto para escrita.\n");

        return 1;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    if(gerenciadorDeArquivosArquivoExiste(CAMINHO_DO_ARQUIVO_DE_TESTE) == FALSO) {
        printf("[ERRO] Arquivo de teste deveria existir.\n");

        return 1;
    }

    printf("[OK] Criacao e existencia de arquivo.\n");

    return 0;
}

static int testarGravacaoELeituraDeArquivo(void) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA];

    arquivo = gerenciadorDeArquivosAbrirArquivoParaEscrita(CAMINHO_DO_ARQUIVO_DE_TESTE);

    if(arquivo == NULL) {
        printf("[ERRO] Arquivo deveria abrir para escrita.\n");

        return 1;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,"linha 1\n") == FALSO) {
        printf("[ERRO] Linha 1 deveria ser gravada.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,"linha 2\n") == FALSO) {
        printf("[ERRO] Linha 2 deveria ser gravada.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(CAMINHO_DO_ARQUIVO_DE_TESTE);

    if(arquivo == NULL) {
        printf("[ERRO] Arquivo deveria abrir para leitura.\n");

        return 1;
    }

    if(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA) == FALSO) {
        printf("[ERRO] Primeira linha deveria ser lida.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    removerQuebraDeLinha(linha);

    if(strcmp(linha,"linha 1") != 0) {
        printf("[ERRO] Primeira linha lida incorretamente.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    if(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA) == FALSO) {
        printf("[ERRO] Segunda linha deveria ser lida.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    removerQuebraDeLinha(linha);

    if(strcmp(linha,"linha 2") != 0) {
        printf("[ERRO] Segunda linha lida incorretamente.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    printf("[OK] Gravacao e leitura de arquivo.\n");

    return 0;
}

static int testarLeituraAposFimDoArquivo(void) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA];

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(CAMINHO_DO_ARQUIVO_DE_TESTE);

    if(arquivo == NULL) {
        printf("[ERRO] Arquivo deveria abrir para leitura.\n");

        return 1;
    }

    gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA);
    gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA);

    if(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA) != FALSO) {
        printf("[ERRO] Leitura apos fim do arquivo deveria falhar.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    printf("[OK] Leitura apos fim do arquivo.\n");

    return 0;
}

static int testarArquivoInexistente(void) {
    if(gerenciadorDeArquivosArquivoExiste("resultados/arquivo_inexistente.txt") != FALSO) {
        printf("[ERRO] Arquivo inexistente nao deveria existir.\n");

        return 1;
    }

    if(gerenciadorDeArquivosAbrirArquivoParaLeitura("resultados/arquivo_inexistente.txt") != NULL) {
        printf("[ERRO] Arquivo inexistente nao deveria abrir para leitura.\n");

        return 1;
    }

    printf("[OK] Arquivo inexistente.\n");

    return 0;
}

static int testarEntradasInvalidas(void) {
    char linha[TAMANHO_MAXIMO_DE_LINHA];

    if(gerenciadorDeArquivosAbrirArquivoParaLeitura(NULL) != NULL) {
        printf("[ERRO] Caminho NULL nao deveria abrir para leitura.\n");

        return 1;
    }

    if(gerenciadorDeArquivosAbrirArquivoParaEscrita(NULL) != NULL) {
        printf("[ERRO] Caminho NULL nao deveria abrir para escrita.\n");

        return 1;
    }

    if(gerenciadorDeArquivosArquivoExiste(NULL) != FALSO) {
        printf("[ERRO] Arquivo NULL nao deveria existir.\n");

        return 1;
    }

    if(gerenciadorDeArquivosLerLinha(NULL,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA) != FALSO) {
        printf("[ERRO] Leitura com arquivo NULL deveria falhar.\n");

        return 1;
    }

    if(gerenciadorDeArquivosGravarLinha(NULL,"linha\n") != FALSO) {
        printf("[ERRO] Gravacao com arquivo NULL deveria falhar.\n");

        return 1;
    }

    gerenciadorDeArquivosFecharArquivo(NULL);

    printf("[OK] Entradas invalidas do gerenciador de arquivos.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do GerenciadorDeArquivos...\n\n");

    quantidadeDeErros += testarCriacaoEExistenciaDeArquivo();
    quantidadeDeErros += testarGravacaoELeituraDeArquivo();
    quantidadeDeErros += testarLeituraAposFimDoArquivo();
    quantidadeDeErros += testarArquivoInexistente();
    quantidadeDeErros += testarEntradasInvalidas();

    remove(CAMINHO_DO_ARQUIVO_DE_TESTE);

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do GerenciadorDeArquivos passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes do GerenciadorDeArquivos: %d\n",quantidadeDeErros);

    return 1;
}
