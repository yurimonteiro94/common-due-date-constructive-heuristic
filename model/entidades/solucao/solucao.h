#ifndef SOLUCAO_H
#define SOLUCAO_H

#include "../../../services/constantes/constantes.h"

typedef struct Solucao {
    QuantidadeDeTarefas quantidadeDeTarefas;
    QuantidadeDeTarefas quantidadeDeTarefasAlocadas;
    IdentificadorDeTarefa *sequenciaDeTarefas;
} Solucao;

Solucao criarSolucaoVazia(void);
Boolean inicializarSolucao(Solucao *solucao,QuantidadeDeTarefas quantidadeDeTarefas);
Boolean solucaoDefinirTarefaNaPosicao(Solucao *solucao,QuantidadeDeTarefas posicao,IdentificadorDeTarefa identificadorDaTarefa);
Boolean solucaoEhCompleta(const Solucao *solucao);
Boolean solucaoEhValida(const Solucao *solucao);
void liberarSolucao(Solucao *solucao);

#endif
