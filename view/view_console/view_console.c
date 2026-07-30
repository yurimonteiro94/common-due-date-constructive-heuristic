#include "view_console.h"

#include <stdio.h>

void viewConsoleExibirCabecalho(void) {
    printf("============================================================\n");
    printf("Prazo Nebuloso\n");
    printf("Heuristica Construtiva por Arbitragem Temporal Preditiva\n");
    printf("============================================================\n");
    printf("\n");
}

void viewConsoleExibirMensagem(const char *mensagem) {
    if(mensagem == NULL) {
        return;
    }

    printf("%s\n",mensagem);
}

void viewConsoleExibirErro(const char *mensagem) {
    if(mensagem == NULL) {
        return;
    }

    printf("[ERRO] %s\n",mensagem);
}

void viewConsoleExibirInicioDoExperimento(void) {
    printf("Iniciando experimento completo...\n");
    printf("Arquivos de instancia: %u\n",(unsigned int) QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA);
    printf("Problemas por arquivo: %u\n",(unsigned int) QUANTIDADE_DE_PROBLEMAS_POR_ARQUIVO);
    printf("Valores de h: %u\n",(unsigned int) QUANTIDADE_DE_VALORES_DE_H);
    printf("Total esperado de execucoes: %u\n",(unsigned int) QUANTIDADE_TOTAL_DE_EXECUCOES);
    printf("\n");
}

void viewConsoleExibirFimDoExperimento(const char *caminhoArquivoResultadosDeExecucoes,const char *caminhoArquivoMediasPorNH) {
    printf("\n");
    printf("Experimento finalizado com sucesso.\n");
    printf("\n");

    if(caminhoArquivoResultadosDeExecucoes != NULL) {
        printf("Arquivo de execucoes: %s\n",caminhoArquivoResultadosDeExecucoes);
    }

    if(caminhoArquivoMediasPorNH != NULL) {
        printf("Arquivo de medias:    %s\n",caminhoArquivoMediasPorNH);
    }

    printf("\n");
}

void viewConsoleExibirFalhaDoExperimento(void) {
    printf("\n");
    printf("[ERRO] O experimento nao foi finalizado.\n");
    printf("Verifique se os arquivos de instancia existem na pasta instancias.\n");
    printf("\n");
}
