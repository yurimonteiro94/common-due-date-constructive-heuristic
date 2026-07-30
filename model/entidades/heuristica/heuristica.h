#ifndef HEURISTICA_H
#define HEURISTICA_H

#include "../../../services/constantes/constantes.h"

typedef struct Heuristica {
    IdentificadorDeHeuristica identificadorDaHeuristica;
    char nomeDaHeuristica[TAMANHO_MAXIMO_DE_NOME_DE_HEURISTICA];
    char descricaoDaHeuristica[TAMANHO_MAXIMO_DE_DESCRICAO_DE_HEURISTICA];
} Heuristica;

Heuristica criarHeuristicaVazia(void);
Boolean inicializarHeuristica(Heuristica *heuristica,IdentificadorDeHeuristica identificadorDaHeuristica,const char *nomeDaHeuristica,const char *descricaoDaHeuristica);
Heuristica criarHeuristicaConstrutivaFuzzy(void);
Boolean heuristicaEhValida(const Heuristica *heuristica);
void limparHeuristica(Heuristica *heuristica);

#endif
