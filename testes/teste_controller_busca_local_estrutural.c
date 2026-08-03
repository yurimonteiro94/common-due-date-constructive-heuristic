#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_busca_local_estrutural/controller_busca_local_estrutural.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stdio.h>

static Boolean montarInstancia(Instancia *instancia) {
    if(inicializarInstancia(instancia,"teste_busca_local_estrutural",1,10) == FALSO) {
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
    QuantidadeDeTarefas posicao;

    if(inicializarSolucao(solucao,10) == FALSO) {
        return FALSO;
    }

    for(posicao = 0;posicao < 10;posicao++) {
        if(solucaoDefinirTarefaNaPosicao(solucao,posicao,(IdentificadorDeTarefa) (posicao + 1)) == FALSO) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

int main(void) {
    Instancia instancia;
    Solucao solucaoInicial;
    Solucao solucaoComposta;
    Solucao solucaoEstrutural;
    ResultadoBuscaLocal resultadoComposta;
    ResultadoBuscaLocalEstrutural resultadoEstrutural;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;

    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();
    solucaoComposta = criarSolucaoVazia();
    solucaoEstrutural = criarSolucaoVazia();
    resultadoComposta = criarResultadoBuscaLocalVazio();
    resultadoEstrutural = criarResultadoBuscaLocalEstruturalVazio();

    if(montarInstancia(&instancia) == FALSO || montarSolucaoInicial(&solucaoInicial) == FALSO) {
        printf("Falha ao preparar teste estrutural.\n");

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(&instancia,FATOR_H_04,&solucaoInicial,&solucaoComposta,&resultadoComposta,6,6) == FALSO) {
        printf("Falha na busca composta.\n");

        return 1;
    }

    if(controllerBuscaLocalEstruturalMelhorarSolucao(&instancia,FATOR_H_04,&solucaoInicial,&solucaoEstrutural,&resultadoEstrutural,6,6,6) == FALSO) {
        printf("Falha na busca estrutural.\n");

        return 1;
    }

    if(solucaoEhValida(&solucaoEstrutural) == FALSO) {
        printf("Solucao estrutural invalida.\n");

        return 1;
    }

    if(resultadoEstrutural.custoFinal > resultadoComposta.custoFinal) {
        printf("Busca estrutural terminou pior que a busca composta.\n");

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,FATOR_H_04);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoEstrutural,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Falha ao verificar custo estrutural.\n");

        return 1;
    }

    if(custoVerificado != resultadoEstrutural.custoFinal) {
        printf("Custo estrutural diferente do custo verificado.\n");
        printf("Custo estrutural: %llu\n",(unsigned long long) resultadoEstrutural.custoFinal);
        printf("Custo verificado: %llu\n",(unsigned long long) custoVerificado);

        return 1;
    }

    printf("Teste da busca local estrutural aprovado.\n");
    printf("Custo inicial: %llu\n",(unsigned long long) resultadoEstrutural.custoInicial);
    printf("Custo composta: %llu\n",(unsigned long long) resultadoComposta.custoFinal);
    printf("Custo estrutural: %llu\n",(unsigned long long) resultadoEstrutural.custoFinal);
    printf("Chamadas composta: %u\n",(unsigned int) resultadoEstrutural.quantidadeDeChamadasDaBuscaComposta);
    printf("Vizinhos bloco 2: %u\n",(unsigned int) resultadoEstrutural.quantidadeDeVizinhosDeBlocoDois);
    printf("Vizinhos bloco 3: %u\n",(unsigned int) resultadoEstrutural.quantidadeDeVizinhosDeBlocoTres);
    printf("Melhorias bloco 2: %u\n",(unsigned int) resultadoEstrutural.quantidadeDeMelhoriasDeBlocoDois);
    printf("Melhorias bloco 3: %u\n",(unsigned int) resultadoEstrutural.quantidadeDeMelhoriasDeBlocoTres);

    liberarSolucao(&solucaoEstrutural);
    liberarSolucao(&solucaoComposta);
    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);

    return 0;
}