#include <stdio.h>

#include "../controller/controller_experimento/controller_experimento.h"
#include "../model/entidades/experimento/experimento.h"
#include "../services/gerenciador_de_arquivos/gerenciador_de_arquivos.h"

#define CAMINHO_TESTE_RESULTADOS_EXECUCOES "resultados/teste_controller_resultados_execucoes.csv"
#define CAMINHO_TESTE_MEDIAS_POR_N_H "resultados/teste_controller_medias_por_n_h.csv"

static int contarLinhasDoArquivo(const char *caminhoDoArquivo) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];
    int quantidadeDeLinhas;

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(caminhoDoArquivo);

    if(arquivo == NULL) {
        return -1;
    }

    quantidadeDeLinhas = 0;

    while(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA_CSV) == VERDADEIRO) {
        quantidadeDeLinhas++;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return quantidadeDeLinhas;
}

static int testarExecucaoCompletaDoExperimentoPadrao(void) {
    Experimento experimento;
    int quantidadeDeLinhasDoArquivoDeResultados;
    int quantidadeDeLinhasDoArquivoDeMedias;

    experimento = criarExperimentoPadrao();

    if(controllerExperimentoExecutarExperimento(
        &experimento,
        CAMINHO_TESTE_RESULTADOS_EXECUCOES,
        CAMINHO_TESTE_MEDIAS_POR_N_H
    ) == FALSO) {
        printf("[ERRO] Experimento padrao deveria ser executado.\n");

        return 1;
    }

    quantidadeDeLinhasDoArquivoDeResultados = contarLinhasDoArquivo(CAMINHO_TESTE_RESULTADOS_EXECUCOES);

    if(quantidadeDeLinhasDoArquivoDeResultados != (QUANTIDADE_TOTAL_DE_EXECUCOES + 1)) {
        printf("[ERRO] Arquivo de resultados deveria ter 281 linhas.\n");

        return 1;
    }

    quantidadeDeLinhasDoArquivoDeMedias = contarLinhasDoArquivo(CAMINHO_TESTE_MEDIAS_POR_N_H);

    if(quantidadeDeLinhasDoArquivoDeMedias != ((QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA * QUANTIDADE_DE_VALORES_DE_H) + 1)) {
        printf("[ERRO] Arquivo de medias deveria ter 29 linhas.\n");

        return 1;
    }

    printf("[OK] Execucao completa do experimento padrao.\n");

    return 0;
}

static int testarExperimentoNuloInvalido(void) {
    if(controllerExperimentoExecutarExperimento(
        NULL,
        CAMINHO_TESTE_RESULTADOS_EXECUCOES,
        CAMINHO_TESTE_MEDIAS_POR_N_H
    ) != FALSO) {
        printf("[ERRO] Experimento NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Experimento nulo invalido no controller de experimento.\n");

    return 0;
}

static int testarCaminhoDeResultadosNuloInvalido(void) {
    Experimento experimento;

    experimento = criarExperimentoPadrao();

    if(controllerExperimentoExecutarExperimento(
        &experimento,
        NULL,
        CAMINHO_TESTE_MEDIAS_POR_N_H
    ) != FALSO) {
        printf("[ERRO] Caminho de resultados NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Caminho de resultados nulo invalido no controller de experimento.\n");

    return 0;
}

static int testarCaminhoDeMediasNuloInvalido(void) {
    Experimento experimento;

    experimento = criarExperimentoPadrao();

    if(controllerExperimentoExecutarExperimento(
        &experimento,
        CAMINHO_TESTE_RESULTADOS_EXECUCOES,
        NULL
    ) != FALSO) {
        printf("[ERRO] Caminho de medias NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Caminho de medias nulo invalido no controller de experimento.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do ControllerExperimento...\n\n");

    quantidadeDeErros += testarExecucaoCompletaDoExperimentoPadrao();
    quantidadeDeErros += testarExperimentoNuloInvalido();
    quantidadeDeErros += testarCaminhoDeResultadosNuloInvalido();
    quantidadeDeErros += testarCaminhoDeMediasNuloInvalido();

    remove(CAMINHO_TESTE_RESULTADOS_EXECUCOES);
    remove(CAMINHO_TESTE_MEDIAS_POR_N_H);

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do ControllerExperimento passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes do ControllerExperimento: %d\n",quantidadeDeErros);

    return 1;
}
