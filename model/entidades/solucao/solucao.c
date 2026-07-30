#include "solucao.h"

#include <stddef.h>
#include <stdlib.h>

Solucao criarSolucaoVazia(void) {
    Solucao solucao;

    solucao.quantidadeDeTarefas = 0;
    solucao.quantidadeDeTarefasAlocadas = 0;
    solucao.sequenciaDeTarefas = NULL;

    return solucao;
}

Boolean inicializarSolucao(Solucao *solucao,QuantidadeDeTarefas quantidadeDeTarefas) {
    QuantidadeDeTarefas posicao;

    if(solucao == NULL) {
        return FALSO;
    }

    else if(quantidadeDeTarefas == 0) {
        return FALSO;
    }

    (*solucao) = criarSolucaoVazia();

    (*solucao).sequenciaDeTarefas = (IdentificadorDeTarefa *) malloc(sizeof(IdentificadorDeTarefa) * quantidadeDeTarefas);

    if((*solucao).sequenciaDeTarefas == NULL) {
        (*solucao) = criarSolucaoVazia();

        return FALSO;
    }

    (*solucao).quantidadeDeTarefas = quantidadeDeTarefas;
    (*solucao).quantidadeDeTarefasAlocadas = 0;

    for(posicao = 0; posicao < quantidadeDeTarefas; posicao++) {
        (*solucao).sequenciaDeTarefas[posicao] = 0;
    }

    return VERDADEIRO;
}

Boolean solucaoDefinirTarefaNaPosicao(Solucao *solucao,QuantidadeDeTarefas posicao,IdentificadorDeTarefa identificadorDaTarefa) {
    if(solucao == NULL) {
        return FALSO;
    }

    else if((*solucao).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    else if((*solucao).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    else if(posicao >= (*solucao).quantidadeDeTarefas) {
        return FALSO;
    }

    else if(identificadorDaTarefa == 0) {
        return FALSO;
    }

    else if(identificadorDaTarefa > (*solucao).quantidadeDeTarefas) {
        return FALSO;
    }

    if((*solucao).sequenciaDeTarefas[posicao] == 0) {
        (*solucao).quantidadeDeTarefasAlocadas++;
    }

    (*solucao).sequenciaDeTarefas[posicao] = identificadorDaTarefa;

    return VERDADEIRO;
}

Boolean solucaoEhCompleta(const Solucao *solucao) {
    if(solucao == NULL) {
        return FALSO;
    }

    else if((*solucao).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    else if((*solucao).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    else if((*solucao).quantidadeDeTarefasAlocadas != (*solucao).quantidadeDeTarefas) {
        return FALSO;
    }

    return VERDADEIRO;
}

Boolean solucaoEhValida(const Solucao *solucao) {
    QuantidadeDeTarefas posicaoAtual;
    QuantidadeDeTarefas proximaPosicao;
    IdentificadorDeTarefa identificadorAtual;
    IdentificadorDeTarefa proximoIdentificador;

    if(solucaoEhCompleta(solucao) == FALSO) {
        return FALSO;
    }

    for(posicaoAtual = 0; posicaoAtual < (*solucao).quantidadeDeTarefas; posicaoAtual++) {
        identificadorAtual = (*solucao).sequenciaDeTarefas[posicaoAtual];

        if(identificadorAtual == 0) {
            return FALSO;
        }

        else if(identificadorAtual > (*solucao).quantidadeDeTarefas) {
            return FALSO;
        }

        for(proximaPosicao = (QuantidadeDeTarefas) (posicaoAtual + 1); proximaPosicao < (*solucao).quantidadeDeTarefas; proximaPosicao++) {
            proximoIdentificador = (*solucao).sequenciaDeTarefas[proximaPosicao];

            if(identificadorAtual == proximoIdentificador) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

void liberarSolucao(Solucao *solucao) {
    if(solucao == NULL) {
        return;
    }

    if((*solucao).sequenciaDeTarefas != NULL) {
        free((*solucao).sequenciaDeTarefas);
    }

    (*solucao) = criarSolucaoVazia();
}
