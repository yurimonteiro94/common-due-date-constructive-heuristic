#include "../controller/controller_busca_local_hibrida/controller_busca_local_hibrida.h"
#include "../controller/controller_busca_local_inversao/controller_busca_local_inversao.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stdio.h>

static Boolean montarInstancia(Instancia *instancia) {
    if(inicializarInstancia(instancia,"teste_busca_local_inversao",1,10) == FALSO) {
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

    if(instanciaAdicionarTarefa(instancia,8,criarTarefa(9,14,6,10)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,9,criarTarefa(10,7,10,3)) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean montarSolucaoInicial(Solucao *solucao) {
    IdentificadorDeTarefa sequencia[10];
    QuantidadeDeTarefas posicao;

    sequencia[0] = 1;
    sequencia[1] = 2;
    sequencia[2] = 3;
    sequencia[3] = 4;
    sequencia[4] = 5;
    sequencia[5] = 6;
    sequencia[6] = 7;
    sequencia[7] = 8;
    sequencia[8] = 9;
    sequencia[9] = 10;

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
    Solucao solucaoHibrida;
    Solucao solucaoInversao;
    ResultadoBuscaLocalHibrida resultadoHibrido;
    ResultadoBuscaLocalInversao resultadoInversao;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;

    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();
    solucaoHibrida = criarSolucaoVazia();
    solucaoInversao = criarSolucaoVazia();
    resultadoHibrido = criarResultadoBuscaLocalHibridaVazio();
    resultadoInversao = criarResultadoBuscaLocalInversaoVazio();
    custoVerificado = 0;

    if(montarInstancia(&instancia) == FALSO || montarSolucaoInicial(&solucaoInicial) == FALSO) {
        printf("Falha na preparacao do teste.\n");

        return 1;
    }

    if(controllerBuscaLocalHibridaMelhorarSolucao(&instancia,FATOR_H_04,&solucaoInicial,&solucaoHibrida,&resultadoHibrido,5,5) == FALSO) {
        printf("Falha na busca local hibrida.\n");

        return 1;
    }

    if(controllerBuscaLocalInversaoMelhorarSolucao(&instancia,FATOR_H_04,&solucaoInicial,&solucaoInversao,&resultadoInversao,5,5,5) == FALSO) {
        printf("Falha na busca local por inversao.\n");

        return 1;
    }

    if(solucaoEhValida(&solucaoInversao) == FALSO) {
        printf("Solucao da busca por inversao invalida.\n");

        return 1;
    }

    if(resultadoInversao.custoFinal > resultadoInversao.custoAposBuscaHibrida) {
        printf("Busca por inversao piorou a solucao hibrida.\n");

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,FATOR_H_04);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoInversao,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Falha ao verificar o custo final.\n");

        return 1;
    }

    if(custoVerificado != resultadoInversao.custoFinal) {
        printf("Custo registrado diferente do custo verificado.\n");
        printf("Custo registrado: %llu\n",(unsigned long long) resultadoInversao.custoFinal);
        printf("Custo verificado: %llu\n",(unsigned long long) custoVerificado);

        return 1;
    }

    printf("Teste da busca local por inversao aprovado.\n");
    printf("Custo inicial: %llu\n",(unsigned long long) resultadoInversao.custoInicial);
    printf("Custo hibrido: %llu\n",(unsigned long long) resultadoHibrido.custoFinal);
    printf("Custo apos hibrida interna: %llu\n",(unsigned long long) resultadoInversao.custoAposBuscaHibrida);
    printf("Custo final: %llu\n",(unsigned long long) resultadoInversao.custoFinal);
    printf("Ciclos: %u\n",(unsigned int) resultadoInversao.quantidadeDeCiclos);
    printf("Iteracoes de inversao: %u\n",(unsigned int) resultadoInversao.quantidadeDeIteracoesDeInversao);
    printf("Vizinhos de inversao: %llu\n",(unsigned long long) resultadoInversao.quantidadeDeVizinhosPorInversao);
    printf("Inversoes aceitas: %u\n",(unsigned int) resultadoInversao.quantidadeDeInversoesAceitas);
    printf("Chamadas da busca composta: %u\n",(unsigned int) resultadoInversao.quantidadeDeChamadasDaBuscaComposta);
    printf("Vizinhos da busca composta: %llu\n",(unsigned long long) resultadoInversao.quantidadeDeVizinhosDaBuscaComposta);

    liberarSolucao(&solucaoInversao);
    liberarSolucao(&solucaoHibrida);
    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);

    return 0;
}