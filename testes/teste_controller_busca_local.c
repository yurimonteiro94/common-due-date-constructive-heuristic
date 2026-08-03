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

static Boolean testeBuscaLocalExecutarEValidar(const char *nomeDoTeste,const Instancia *instancia,const Solucao *solucaoInicial,FatorH fatorH,QuantidadeDeTarefas raio,Boolean usarRaioLimitado) {
    Solucao solucaoFinal;
    ResultadoBuscaLocal resultadoBuscaLocal;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;
    Boolean resultadoDaExecucao;

    solucaoFinal = criarSolucaoVazia();
    resultadoBuscaLocal = criarResultadoBuscaLocalVazio();
    custoVerificado = 0;

    if(usarRaioLimitado == VERDADEIRO) {
        resultadoDaExecucao = controllerBuscaLocalMelhorarSolucaoPorReinsercaoLimitada(instancia,fatorH,solucaoInicial,&solucaoFinal,&resultadoBuscaLocal,raio);
    }
    else {
        resultadoDaExecucao = controllerBuscaLocalMelhorarSolucaoPorReinsercao(instancia,fatorH,solucaoInicial,&solucaoFinal,&resultadoBuscaLocal);
    }

    if(resultadoDaExecucao == FALSO) {
        printf("Falha ao executar %s.\n",nomeDoTeste);
        liberarSolucao(&solucaoFinal);

        return FALSO;
    }

    if(solucaoEhValida(&solucaoFinal) == FALSO) {
        printf("Solucao final invalida em %s.\n",nomeDoTeste);
        liberarSolucao(&solucaoFinal);

        return FALSO;
    }

    if(resultadoBuscaLocal.custoFinal > resultadoBuscaLocal.custoInicial) {
        printf("Busca local piorou a solucao em %s.\n",nomeDoTeste);
        liberarSolucao(&solucaoFinal);

        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(instancia,&solucaoFinal,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Nao foi possivel verificar custo em %s.\n",nomeDoTeste);
        liberarSolucao(&solucaoFinal);

        return FALSO;
    }

    if(custoVerificado != resultadoBuscaLocal.custoFinal) {
        printf("Custo verificado diferente em %s.\n",nomeDoTeste);
        printf("Custo da busca local: %llu\n",(unsigned long long) resultadoBuscaLocal.custoFinal);
        printf("Custo verificado: %llu\n",(unsigned long long) custoVerificado);
        liberarSolucao(&solucaoFinal);

        return FALSO;
    }

    printf("%s aprovado.\n",nomeDoTeste);
    printf("Custo inicial: %llu\n",(unsigned long long) resultadoBuscaLocal.custoInicial);
    printf("Custo final: %llu\n",(unsigned long long) resultadoBuscaLocal.custoFinal);
    printf("Iteracoes: %u\n",(unsigned int) resultadoBuscaLocal.quantidadeDeIteracoes);
    printf("Vizinhos avaliados: %u\n",(unsigned int) resultadoBuscaLocal.quantidadeDeVizinhosAvaliados);

    liberarSolucao(&solucaoFinal);

    return VERDADEIRO;
}

int main(void) {
    Instancia instancia;
    Solucao solucaoInicial;

    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();

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

    if(testeBuscaLocalExecutarEValidar("Reinsercao completa",&instancia,&solucaoInicial,FATOR_H_06,0,FALSO) == FALSO) {
        liberarSolucao(&solucaoInicial);
        liberarInstancia(&instancia);

        return 1;
    }

    if(testeBuscaLocalExecutarEValidar("Reinsercao limitada",&instancia,&solucaoInicial,FATOR_H_06,3,VERDADEIRO) == FALSO) {
        liberarSolucao(&solucaoInicial);
        liberarInstancia(&instancia);

        return 1;
    }

    printf("Teste de busca local finalizado com sucesso.\n");

    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);

    return 0;
}