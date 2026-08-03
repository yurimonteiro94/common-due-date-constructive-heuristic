#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stdio.h>

static Boolean montarInstancia(Instancia *instancia) {
    if(inicializarInstancia(instancia,"teste_busca_local_composta",1,8) == FALSO) {
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

    if(instanciaAdicionarTarefa(instancia,6,criarTarefa(7,15,2,12)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,7,criarTarefa(8,3,8,5)) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean montarSolucao(Solucao *solucao) {
    QuantidadeDeTarefas posicao;

    if(inicializarSolucao(solucao,8) == FALSO) {
        return FALSO;
    }

    for(posicao = 0;posicao < 8;posicao++) {
        if(solucaoDefinirTarefaNaPosicao(solucao,posicao,(IdentificadorDeTarefa) (posicao + 1)) == FALSO) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

int main(void) {
    Instancia instancia;
    Solucao solucaoInicial;
    Solucao solucaoReinsercao;
    Solucao solucaoComposta;
    ResultadoBuscaLocal resultadoReinsercao;
    ResultadoBuscaLocal resultadoComposta;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;

    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();
    solucaoReinsercao = criarSolucaoVazia();
    solucaoComposta = criarSolucaoVazia();
    resultadoReinsercao = criarResultadoBuscaLocalVazio();
    resultadoComposta = criarResultadoBuscaLocalVazio();

    if(montarInstancia(&instancia) == FALSO || montarSolucao(&solucaoInicial) == FALSO) {
        printf("Falha na preparacao do teste.\n");

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoPorReinsercaoLimitada(&instancia,FATOR_H_04,&solucaoInicial,&solucaoReinsercao,&resultadoReinsercao,4) == FALSO) {
        printf("Falha na busca por reinsercao.\n");

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(&instancia,FATOR_H_04,&solucaoInicial,&solucaoComposta,&resultadoComposta,4,4) == FALSO) {
        printf("Falha na busca composta.\n");

        return 1;
    }

    if(solucaoEhValida(&solucaoComposta) == FALSO) {
        printf("Solucao composta invalida.\n");

        return 1;
    }

    if(resultadoComposta.custoFinal > resultadoComposta.custoInicial) {
        printf("Busca composta piorou a solucao.\n");

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,FATOR_H_04);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoComposta,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Falha na verificacao do custo.\n");

        return 1;
    }

    if(custoVerificado != resultadoComposta.custoFinal) {
        printf("Custo composto diferente do custo verificado.\n");

        return 1;
    }

    printf("Teste da busca local composta aprovado.\n");
    printf("Custo inicial: %llu\n",(unsigned long long) resultadoComposta.custoInicial);
    printf("Custo reinsercao: %llu\n",(unsigned long long) resultadoReinsercao.custoFinal);
    printf("Custo composta: %llu\n",(unsigned long long) resultadoComposta.custoFinal);
    printf("Iteracoes composta: %u\n",(unsigned int) resultadoComposta.quantidadeDeIteracoes);
    printf("Vizinhos reinsercao: %u\n",(unsigned int) resultadoComposta.quantidadeDeVizinhosPorReinsercao);
    printf("Vizinhos troca: %u\n",(unsigned int) resultadoComposta.quantidadeDeVizinhosPorTroca);
    printf("Melhorias reinsercao: %u\n",(unsigned int) resultadoComposta.quantidadeDeMelhoriasPorReinsercao);
    printf("Melhorias troca: %u\n",(unsigned int) resultadoComposta.quantidadeDeMelhoriasPorTroca);

    liberarSolucao(&solucaoComposta);
    liberarSolucao(&solucaoReinsercao);
    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);

    return 0;
}