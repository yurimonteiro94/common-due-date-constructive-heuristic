#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stdio.h>

static Boolean testeBuscaLocalMontarInstancia(Instancia *instancia) {
    if(inicializarInstancia(instancia,"teste_busca_local",1,6) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,0,criarTarefa(1,12,8,3)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,1,criarTarefa(2,5,2,9)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,2,criarTarefa(3,9,7,4)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,3,criarTarefa(4,4,3,8)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,4,criarTarefa(5,11,9,2)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,5,criarTarefa(6,6,4,7)) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean testeBuscaLocalMontarSolucaoInicial(Solucao *solucao) {
    if(inicializarSolucao(solucao,6) == FALSO) {
        return FALSO;
    }

    if(solucaoDefinirTarefaNaPosicao(solucao,0,1) == FALSO) {
        return FALSO;
    }

    if(solucaoDefinirTarefaNaPosicao(solucao,1,2) == FALSO) {
        return FALSO;
    }

    if(solucaoDefinirTarefaNaPosicao(solucao,2,3) == FALSO) {
        return FALSO;
    }

    if(solucaoDefinirTarefaNaPosicao(solucao,3,4) == FALSO) {
        return FALSO;
    }

    if(solucaoDefinirTarefaNaPosicao(solucao,4,5) == FALSO) {
        return FALSO;
    }

    if(solucaoDefinirTarefaNaPosicao(solucao,5,6) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

int main(void) {
    Instancia instancia;
    Solucao solucaoInicial;
    Solucao solucaoFinal;
    ResultadoBuscaLocal resultadoBuscaLocal;

    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();
    solucaoFinal = criarSolucaoVazia();

    if(testeBuscaLocalMontarInstancia(&instancia) == FALSO) {
        printf("Falha ao montar instancia.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(testeBuscaLocalMontarSolucaoInicial(&solucaoInicial) == FALSO) {
        printf("Falha ao montar solucao inicial.\n");
        liberarSolucao(&solucaoInicial);
        liberarInstancia(&instancia);

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoPorReinsercao(&instancia,FATOR_H_06,&solucaoInicial,&solucaoFinal,&resultadoBuscaLocal) == FALSO) {
        printf("Falha ao executar busca local.\n");
        liberarSolucao(&solucaoFinal);
        liberarSolucao(&solucaoInicial);
        liberarInstancia(&instancia);

        return 1;
    }

    if(solucaoEhValida(&solucaoFinal) == FALSO) {
        printf("Solucao final invalida.\n");
        liberarSolucao(&solucaoFinal);
        liberarSolucao(&solucaoInicial);
        liberarInstancia(&instancia);

        return 1;
    }

    if(resultadoBuscaLocal.custoFinal > resultadoBuscaLocal.custoInicial) {
        printf("Busca local piorou a solucao.\n");
        liberarSolucao(&solucaoFinal);
        liberarSolucao(&solucaoInicial);
        liberarInstancia(&instancia);

        return 1;
    }

    printf("Teste de busca local aprovado.\n");
    printf("Custo inicial: %llu\n",(unsigned long long) resultadoBuscaLocal.custoInicial);
    printf("Custo final: %llu\n",(unsigned long long) resultadoBuscaLocal.custoFinal);
    printf("Iteracoes: %u\n",(unsigned int) resultadoBuscaLocal.quantidadeDeIteracoes);
    printf("Vizinhos avaliados: %u\n",(unsigned int) resultadoBuscaLocal.quantidadeDeVizinhosAvaliados);

    liberarSolucao(&solucaoFinal);
    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);

    return 0;
}
