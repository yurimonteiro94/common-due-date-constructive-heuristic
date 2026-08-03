#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_busca_local_vshape/controller_busca_local_vshape.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stdio.h>

static Boolean montarInstancia(Instancia *instancia) {
    if(inicializarInstancia(instancia,"teste_busca_local_vshape",1,10) == FALSO) {
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

    if(instanciaAdicionarTarefa(instancia,8,criarTarefa(9,14,6,11)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,9,criarTarefa(10,7,5,6)) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean montarSolucaoInicial(Solucao *solucao) {
    IdentificadorDeTarefa sequencia[10];
    QuantidadeDeTarefas posicao;

    sequencia[0] = 1;
    sequencia[1] = 7;
    sequencia[2] = 3;
    sequencia[3] = 9;
    sequencia[4] = 5;
    sequencia[5] = 2;
    sequencia[6] = 10;
    sequencia[7] = 6;
    sequencia[8] = 4;
    sequencia[9] = 8;

    if(inicializarSolucao(solucao,10) == FALSO) {
        return FALSO;
    }

    for(posicao = 0;posicao < 10;posicao++) {
        if(solucaoDefinirTarefaNaPosicao(solucao,posicao,sequencia[posicao]) == FALSO) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

int main(void) {
    Instancia instancia;
    Solucao solucaoInicial;
    Solucao solucaoComposta;
    Solucao solucaoVShape;
    ResultadoBuscaLocal resultadoComposta;
    ResultadoBuscaLocalVShape resultadoVShape;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;

    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();
    solucaoComposta = criarSolucaoVazia();
    solucaoVShape = criarSolucaoVazia();
    resultadoComposta = criarResultadoBuscaLocalVazio();
    resultadoVShape = criarResultadoBuscaLocalVShapeVazio();

    if(montarInstancia(&instancia) == FALSO || montarSolucaoInicial(&solucaoInicial) == FALSO) {
        printf("Falha ao preparar teste V-shaped.\n");

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(&instancia,FATOR_H_04,&solucaoInicial,&solucaoComposta,&resultadoComposta,6,6) == FALSO) {
        printf("Falha na busca composta.\n");

        return 1;
    }

    if(controllerBuscaLocalVShapeMelhorarSolucao(&instancia,FATOR_H_04,&solucaoInicial,&solucaoVShape,&resultadoVShape,6,6) == FALSO) {
        printf("Falha na busca V-shaped.\n");

        return 1;
    }

    if(solucaoEhValida(&solucaoVShape) == FALSO) {
        printf("Solucao V-shaped invalida.\n");

        return 1;
    }

    if(resultadoVShape.custoFinal > resultadoComposta.custoFinal) {
        printf("Busca V-shaped terminou pior que a busca composta.\n");

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,FATOR_H_04);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoVShape,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Falha ao verificar custo da busca V-shaped.\n");

        return 1;
    }

    if(custoVerificado != resultadoVShape.custoFinal) {
        printf("Custo V-shaped diferente do custo verificado.\n");
        printf("Custo V-shaped: %llu\n",(unsigned long long) resultadoVShape.custoFinal);
        printf("Custo verificado: %llu\n",(unsigned long long) custoVerificado);

        return 1;
    }

    printf("Teste da busca local V-shaped aprovado.\n");
    printf("Custo inicial: %llu\n",(unsigned long long) resultadoVShape.custoInicial);
    printf("Custo composta: %llu\n",(unsigned long long) resultadoComposta.custoFinal);
    printf("Custo V-shaped: %llu\n",(unsigned long long) resultadoVShape.custoFinal);
    printf("Chamadas composta: %u\n",(unsigned int) resultadoVShape.quantidadeDeChamadasDaBuscaComposta);
    printf("Vizinhos ordenacao: %u\n",(unsigned int) resultadoVShape.quantidadeDeVizinhosDeOrdenacao);
    printf("Vizinhos fronteira: %u\n",(unsigned int) resultadoVShape.quantidadeDeVizinhosDeFronteira);
    printf("Melhorias ordenacao: %u\n",(unsigned int) resultadoVShape.quantidadeDeMelhoriasDeOrdenacao);
    printf("Melhorias fronteira: %u\n",(unsigned int) resultadoVShape.quantidadeDeMelhoriasDeFronteira);

    liberarSolucao(&solucaoVShape);
    liberarSolucao(&solucaoComposta);
    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);

    return 0;
}