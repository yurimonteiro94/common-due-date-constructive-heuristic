#include "gerenciador_de_arquivos.h"

#include <stddef.h>

FILE *gerenciadorDeArquivosAbrirArquivoParaLeitura(const char *caminhoDoArquivo) {
    FILE *arquivo;

    if(caminhoDoArquivo == NULL) {
        return NULL;
    }

    else if(caminhoDoArquivo[0] == '\0') {
        return NULL;
    }

    arquivo = fopen(caminhoDoArquivo,"r");

    return arquivo;
}

FILE *gerenciadorDeArquivosAbrirArquivoParaEscrita(const char *caminhoDoArquivo) {
    FILE *arquivo;

    if(caminhoDoArquivo == NULL) {
        return NULL;
    }

    else if(caminhoDoArquivo[0] == '\0') {
        return NULL;
    }

    arquivo = fopen(caminhoDoArquivo,"w");

    return arquivo;
}

FILE *gerenciadorDeArquivosAbrirArquivoParaAcrescimo(const char *caminhoDoArquivo) {
    FILE *arquivo;

    if(caminhoDoArquivo == NULL) {
        return NULL;
    }

    else if(caminhoDoArquivo[0] == '\0') {
        return NULL;
    }

    arquivo = fopen(caminhoDoArquivo,"a");

    return arquivo;
}

Boolean gerenciadorDeArquivosLerLinha(FILE *arquivo,char *linha,InteiroPositivoDe32Bits tamanhoDaLinha) {
    if(arquivo == NULL) {
        return FALSO;
    }

    else if(linha == NULL) {
        return FALSO;
    }

    else if(tamanhoDaLinha == 0) {
        return FALSO;
    }

    if(fgets(linha,(int) tamanhoDaLinha,arquivo) == NULL) {
        return FALSO;
    }

    return VERDADEIRO;
}

Boolean gerenciadorDeArquivosGravarLinha(FILE *arquivo,const char *linha) {
    if(arquivo == NULL) {
        return FALSO;
    }

    else if(linha == NULL) {
        return FALSO;
    }

    if(fputs(linha,arquivo) == EOF) {
        return FALSO;
    }

    return VERDADEIRO;
}

Boolean gerenciadorDeArquivosArquivoExiste(const char *caminhoDoArquivo) {
    FILE *arquivo;

    if(caminhoDoArquivo == NULL) {
        return FALSO;
    }

    else if(caminhoDoArquivo[0] == '\0') {
        return FALSO;
    }

    arquivo = fopen(caminhoDoArquivo,"r");

    if(arquivo == NULL) {
        return FALSO;
    }

    fclose(arquivo);

    return VERDADEIRO;
}

void gerenciadorDeArquivosFecharArquivo(FILE *arquivo) {
    if(arquivo == NULL) {
        return;
    }

    fclose(arquivo);
}
