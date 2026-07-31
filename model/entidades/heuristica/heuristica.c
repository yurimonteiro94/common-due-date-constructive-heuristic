#include "heuristica.h"

#include <string.h>

static Boolean heuristicaTextoEhNuloOuVazio(const char *texto) {
    if(texto == 0) {
        return VERDADEIRO;
    }

    if(texto[0] == '\0') {
        return VERDADEIRO;
    }

    return FALSO;
}

Heuristica criarHeuristicaVazia(void) {
    Heuristica heuristica;

    heuristica.identificadorDaHeuristica = 0;
    heuristica.nomeDaHeuristica[0] = '\0';
    heuristica.descricaoDaHeuristica[0] = '\0';

    return heuristica;
}

Boolean inicializarHeuristica(Heuristica *heuristica,IdentificadorDeHeuristica identificadorDaHeuristica,const char *nomeDaHeuristica,const char *descricaoDaHeuristica) {
    if(heuristica == 0) {
        return FALSO;
    }

    if(identificadorDaHeuristica == 0) {
        return FALSO;
    }

    if(heuristicaTextoEhNuloOuVazio(nomeDaHeuristica) == VERDADEIRO) {
        return FALSO;
    }

    if(heuristicaTextoEhNuloOuVazio(descricaoDaHeuristica) == VERDADEIRO) {
        return FALSO;
    }

    (*heuristica).identificadorDaHeuristica = identificadorDaHeuristica;

    strncpy((*heuristica).nomeDaHeuristica,nomeDaHeuristica,TAMANHO_MAXIMO_DE_NOME_DE_HEURISTICA - 1);
    (*heuristica).nomeDaHeuristica[TAMANHO_MAXIMO_DE_NOME_DE_HEURISTICA - 1] = '\0';

    strncpy((*heuristica).descricaoDaHeuristica,descricaoDaHeuristica,TAMANHO_MAXIMO_DE_DESCRICAO_DE_HEURISTICA - 1);
    (*heuristica).descricaoDaHeuristica[TAMANHO_MAXIMO_DE_DESCRICAO_DE_HEURISTICA - 1] = '\0';

    return VERDADEIRO;
}

Heuristica criarHeuristicaPorInsercaoTemporal(void) {
    Heuristica heuristica;

    heuristica = criarHeuristicaVazia();

    inicializarHeuristica(
        &heuristica,
        IDENTIFICADOR_HEURISTICA_INSERCAO_TEMPORAL,
        NOME_HEURISTICA_INSERCAO_TEMPORAL,
        DESCRICAO_HEURISTICA_INSERCAO_TEMPORAL
    );

    return heuristica;
}

Boolean heuristicaEhValida(const Heuristica *heuristica) {
    if(heuristica == 0) {
        return FALSO;
    }

    if((*heuristica).identificadorDaHeuristica == 0) {
        return FALSO;
    }

    if(heuristicaTextoEhNuloOuVazio((*heuristica).nomeDaHeuristica) == VERDADEIRO) {
        return FALSO;
    }

    if(heuristicaTextoEhNuloOuVazio((*heuristica).descricaoDaHeuristica) == VERDADEIRO) {
        return FALSO;
    }

    return VERDADEIRO;
}

void limparHeuristica(Heuristica *heuristica) {
    if(heuristica == 0) {
        return;
    }

    (*heuristica).identificadorDaHeuristica = 0;
    (*heuristica).nomeDaHeuristica[0] = '\0';
    (*heuristica).descricaoDaHeuristica[0] = '\0';
}