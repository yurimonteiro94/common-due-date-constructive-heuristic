#include "heuristica.h"

#include <stddef.h>
#include <stdio.h>

Heuristica criarHeuristicaVazia(void) {
    Heuristica heuristica;

    heuristica.identificadorDaHeuristica = 0;
    heuristica.nomeDaHeuristica[0] = '\0';
    heuristica.descricaoDaHeuristica[0] = '\0';

    return heuristica;
}

Boolean inicializarHeuristica(Heuristica *heuristica,IdentificadorDeHeuristica identificadorDaHeuristica,const char *nomeDaHeuristica,const char *descricaoDaHeuristica) {
    if(heuristica == NULL) {
        return FALSO;
    }

    else if(identificadorDaHeuristica == 0) {
        return FALSO;
    }

    else if(nomeDaHeuristica == NULL) {
        return FALSO;
    }

    else if(nomeDaHeuristica[0] == '\0') {
        return FALSO;
    }

    else if(descricaoDaHeuristica == NULL) {
        return FALSO;
    }

    else if(descricaoDaHeuristica[0] == '\0') {
        return FALSO;
    }

    (*heuristica) = criarHeuristicaVazia();

    (*heuristica).identificadorDaHeuristica = identificadorDaHeuristica;

    snprintf(
        (*heuristica).nomeDaHeuristica,
        TAMANHO_MAXIMO_DE_NOME_DE_HEURISTICA,
        "%s",
        nomeDaHeuristica
    );

    snprintf(
        (*heuristica).descricaoDaHeuristica,
        TAMANHO_MAXIMO_DE_DESCRICAO_DE_HEURISTICA,
        "%s",
        descricaoDaHeuristica
    );

    return VERDADEIRO;
}

Heuristica criarHeuristicaConstrutivaFuzzy(void) {
    Heuristica heuristica;

    heuristica = criarHeuristicaVazia();

    inicializarHeuristica(
        &heuristica,
        (IdentificadorDeHeuristica) IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY,
        NOME_HEURISTICA_CONSTRUTIVA_FUZZY,
        DESCRICAO_HEURISTICA_CONSTRUTIVA_FUZZY
    );

    return heuristica;
}

Boolean heuristicaEhValida(const Heuristica *heuristica) {
    if(heuristica == NULL) {
        return FALSO;
    }

    else if((*heuristica).identificadorDaHeuristica == 0) {
        return FALSO;
    }

    else if((*heuristica).nomeDaHeuristica[0] == '\0') {
        return FALSO;
    }

    else if((*heuristica).descricaoDaHeuristica[0] == '\0') {
        return FALSO;
    }

    return VERDADEIRO;
}

void limparHeuristica(Heuristica *heuristica) {
    if(heuristica == NULL) {
        return;
    }

    (*heuristica) = criarHeuristicaVazia();
}
