#include "../controller/controller_busca_local_subcubos_particao/controller_busca_local_subcubos_particao.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include <stdio.h>

static Boolean montarInstancia(Instancia *instancia) {
    if(inicializarInstancia(instancia,"teste_subcubos_particao",1,8) == FALSO) {
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

static Boolean montarSolucaoInicial(Solucao *solucao) {
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
    Solucao solucaoFinal;
    ResultadoBuscaLocalSubcubosParticao resultadoBuscaLocal;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;
    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();
    solucaoFinal = criarSolucaoVazia();
    resultadoBuscaLocal = criarResultadoBuscaLocalSubcubosParticaoVazio();
    custoVerificado = 0;
    if(montarInstancia(&instancia) == FALSO || montarSolucaoInicial(&solucaoInicial) == FALSO) {
        printf("Falha na preparacao do teste.\n");
        return 1;
    }
    if(controllerBuscaLocalSubcubosParticaoMelhorarSolucao(&instancia,FATOR_H_04,&solucaoInicial,&solucaoFinal,&resultadoBuscaLocal) == FALSO) {
        printf("Falha na busca local por subcubos da particao.\n");
        return 1;
    }
    if(solucaoEhValida(&solucaoFinal) == FALSO) {
        printf("Solucao final invalida.\n");
        return 1;
    }
    if(resultadoBuscaLocal.resultadoBase.custoFinal > resultadoBuscaLocal.resultadoBase.custoInicial) {
        printf("A busca local piorou a solucao inicial.\n");
        return 1;
    }
    if(resultadoBuscaLocal.resultadoBase.quantidadeDeVizinhosPorReinsercao == 0 || resultadoBuscaLocal.resultadoBase.quantidadeDeVizinhosPorTroca == 0 || resultadoBuscaLocal.quantidadeDePaineisAvaliados == 0) {
        printf("A vizinhanca de subcubos nao foi integralmente avaliada.\n");
        return 1;
    }
    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,FATOR_H_04);
    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoFinal,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Falha na verificacao independente do custo.\n");
        return 1;
    }
    if(custoVerificado != resultadoBuscaLocal.resultadoBase.custoFinal) {
        printf("Custo registrado diferente do custo verificado.\n");
        printf("Custo registrado: %llu\n",(unsigned long long) resultadoBuscaLocal.resultadoBase.custoFinal);
        printf("Custo verificado: %llu\n",(unsigned long long) custoVerificado);
        return 1;
    }
    printf("Teste da busca local por subcubos da particao aprovado.\n");
    printf("Custo inicial: %llu\n",(unsigned long long) resultadoBuscaLocal.resultadoBase.custoInicial);
    printf("Custo final: %llu\n",(unsigned long long) resultadoBuscaLocal.resultadoBase.custoFinal);
    printf("Iteracoes: %u\n",(unsigned int) resultadoBuscaLocal.resultadoBase.quantidadeDeIteracoes);
    printf("Vizinhos de uma troca: %u\n",(unsigned int) resultadoBuscaLocal.resultadoBase.quantidadeDeVizinhosPorReinsercao);
    printf("Vizinhos de subcubos: %u\n",(unsigned int) resultadoBuscaLocal.resultadoBase.quantidadeDeVizinhosPorTroca);
    printf("Paineis avaliados: %u\n",(unsigned int) resultadoBuscaLocal.quantidadeDePaineisAvaliados);
    printf("Maior cardinalidade aceita: %u\n",(unsigned int) resultadoBuscaLocal.maiorCardinalidadeAceita);
    printf("Movimentos com quatro ou mais trocas: %u\n",(unsigned int) resultadoBuscaLocal.quantidadeDeMovimentosComQuatroOuMaisAceitos);
    liberarSolucao(&solucaoFinal);
    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);
    return 0;
}