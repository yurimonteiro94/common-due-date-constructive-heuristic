#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_busca_local_melhor_melhoria/controller_busca_local_melhor_melhoria.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stdio.h>

static Boolean montarInstancia(Instancia *instancia) {
    if(inicializarInstancia(instancia,"teste_melhor_melhoria",1,8) == FALSO) {
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
    Solucao solucaoPrimeiraMelhoria;
    Solucao solucaoMelhorMelhoria;
    ResultadoBuscaLocal resultadoPrimeiraMelhoria;
    ResultadoBuscaLocal resultadoMelhorMelhoria;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;

    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();
    solucaoPrimeiraMelhoria = criarSolucaoVazia();
    solucaoMelhorMelhoria = criarSolucaoVazia();
    resultadoPrimeiraMelhoria = criarResultadoBuscaLocalVazio();
    resultadoMelhorMelhoria = criarResultadoBuscaLocalVazio();

    if(montarInstancia(&instancia) == FALSO || montarSolucao(&solucaoInicial) == FALSO) {
        printf("Falha na preparacao do teste.\n");

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(&instancia,FATOR_H_04,&solucaoInicial,&solucaoPrimeiraMelhoria,&resultadoPrimeiraMelhoria,4,4) == FALSO) {
        printf("Falha na busca por primeira melhoria.\n");

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoComMelhorMelhoria(&instancia,FATOR_H_04,&solucaoInicial,&solucaoMelhorMelhoria,&resultadoMelhorMelhoria,4,4) == FALSO) {
        printf("Falha na busca por melhor melhoria.\n");

        return 1;
    }

    if(solucaoEhValida(&solucaoMelhorMelhoria) == FALSO) {
        printf("Solucao da melhor melhoria invalida.\n");

        return 1;
    }

    if(resultadoMelhorMelhoria.custoFinal > resultadoMelhorMelhoria.custoInicial) {
        printf("Melhor melhoria piorou a solucao inicial.\n");

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,FATOR_H_04);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoMelhorMelhoria,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Falha na verificacao do custo.\n");

        return 1;
    }

    if(custoVerificado != resultadoMelhorMelhoria.custoFinal) {
        printf("Custo da melhor melhoria diferente do custo verificado.\n");
        printf("Custo informado: %llu\n",(unsigned long long) resultadoMelhorMelhoria.custoFinal);
        printf("Custo verificado: %llu\n",(unsigned long long) custoVerificado);

        return 1;
    }

    printf("Teste da busca local por melhor melhoria aprovado.\n");
    printf("Custo inicial: %llu\n",(unsigned long long) resultadoMelhorMelhoria.custoInicial);
    printf("Custo primeira melhoria: %llu\n",(unsigned long long) resultadoPrimeiraMelhoria.custoFinal);
    printf("Custo melhor melhoria: %llu\n",(unsigned long long) resultadoMelhorMelhoria.custoFinal);
    printf("Iteracoes melhor melhoria: %u\n",(unsigned int) resultadoMelhorMelhoria.quantidadeDeIteracoes);
    printf("Vizinhos reinsercao: %u\n",(unsigned int) resultadoMelhorMelhoria.quantidadeDeVizinhosPorReinsercao);
    printf("Vizinhos troca: %u\n",(unsigned int) resultadoMelhorMelhoria.quantidadeDeVizinhosPorTroca);
    printf("Melhorias reinsercao: %u\n",(unsigned int) resultadoMelhorMelhoria.quantidadeDeMelhoriasPorReinsercao);
    printf("Melhorias troca: %u\n",(unsigned int) resultadoMelhorMelhoria.quantidadeDeMelhoriasPorTroca);

    liberarSolucao(&solucaoMelhorMelhoria);
    liberarSolucao(&solucaoPrimeiraMelhoria);
    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);

    return 0;
}