#include "ferramentas.h"

#include <stddef.h>
#include <stdio.h>

Boolean textoEhNuloOuVazio(const char *texto) {
    if(texto == NULL) {
        return VERDADEIRO;
    }

    else if(texto[0] == '\0') {
        return VERDADEIRO;
    }

    return FALSO;
}

void removerQuebraDeLinha(char *texto) {
    InteiroPositivoDe32Bits indice;

    if(texto == NULL) {
        return;
    }

    indice = 0;

    while(texto[indice] != '\0') {
        if(texto[indice] == '\n') {
            texto[indice] = '\0';

            return;
        }

        else if(texto[indice] == '\r') {
            texto[indice] = '\0';

            return;
        }

        indice++;
    }
}

Boolean copiarTexto(char *destino,InteiroPositivoDe32Bits tamanhoDoDestino,const char *origem) {
    int quantidadeDeCaracteresEscritos;

    if(destino == NULL) {
        return FALSO;
    }

    else if(tamanhoDoDestino == 0) {
        return FALSO;
    }

    else if(origem == NULL) {
        destino[0] = '\0';

        return FALSO;
    }

    destino[0] = '\0';

    quantidadeDeCaracteresEscritos = snprintf(
        destino,
        (size_t) tamanhoDoDestino,
        "%s",
        origem
    );

    if(quantidadeDeCaracteresEscritos < 0) {
        destino[0] = '\0';

        return FALSO;
    }

    else if((InteiroPositivoDe32Bits) quantidadeDeCaracteresEscritos >= tamanhoDoDestino) {
        return FALSO;
    }

    return VERDADEIRO;
}

Boolean montarCaminhoComArquivo(char *destino,InteiroPositivoDe32Bits tamanhoDoDestino,const char *caminhoDaPasta,const char *nomeDoArquivo) {
    int quantidadeDeCaracteresEscritos;

    if(destino == NULL) {
        return FALSO;
    }

    else if(tamanhoDoDestino == 0) {
        return FALSO;
    }

    else if(textoEhNuloOuVazio(caminhoDaPasta) == VERDADEIRO) {
        destino[0] = '\0';

        return FALSO;
    }

    else if(textoEhNuloOuVazio(nomeDoArquivo) == VERDADEIRO) {
        destino[0] = '\0';

        return FALSO;
    }

    destino[0] = '\0';

    quantidadeDeCaracteresEscritos = snprintf(
        destino,
        (size_t) tamanhoDoDestino,
        "%s/%s",
        caminhoDaPasta,
        nomeDoArquivo
    );

    if(quantidadeDeCaracteresEscritos < 0) {
        destino[0] = '\0';

        return FALSO;
    }

    else if((InteiroPositivoDe32Bits) quantidadeDeCaracteresEscritos >= tamanhoDoDestino) {
        return FALSO;
    }

    return VERDADEIRO;
}

double fatorHParaNumeroDecimal(FatorH fatorH) {
    if(fatorH == FATOR_H_02) {
        return 0.2;
    }

    else if(fatorH == FATOR_H_04) {
        return 0.4;
    }

    else if(fatorH == FATOR_H_06) {
        return 0.6;
    }

    else if(fatorH == FATOR_H_08) {
        return 0.8;
    }

    return 0.0;
}
