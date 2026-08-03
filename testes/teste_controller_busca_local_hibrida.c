#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_busca_local_hibrida/controller_busca_local_hibrida.h"
#include "../controller/controller_busca_local_melhor_melhoria/controller_busca_local_melhor_melhoria.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

#include <stdio.h>

static Boolean montarInstancia(Instancia *instancia) {
    if(inicializarInstancia(instancia,"teste_busca_local_hibrida",1,8) == FALSO) {
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
    Solucao solucaoPrimeira;
    Solucao solucaoMelhor;
    Solucao solucaoHibrida;
    ResultadoBuscaLocal resultadoPrimeira;
    ResultadoBuscaLocal resultadoMelhor;
    ResultadoBuscaLocalHibrida resultadoHibrida;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;

    instancia = criarInstanciaVazia();
    solucaoInicial = criarSolucaoVazia();
    solucaoPrimeira = criarSolucaoVazia();
    solucaoMelhor = criarSolucaoVazia();
    solucaoHibrida = criarSolucaoVazia();
    resultadoPrimeira = criarResultadoBuscaLocalVazio();
    resultadoMelhor = criarResultadoBuscaLocalVazio();
    resultadoHibrida = criarResultadoBuscaLocalHibridaVazio();

    if(montarInstancia(&instancia) == FALSO || montarSolucao(&solucaoInicial) == FALSO) {
        printf("Falha na preparacao do teste.\n");

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(&instancia,FATOR_H_04,&solucaoInicial,&solucaoPrimeira,&resultadoPrimeira,4,4) == FALSO) {
        printf("Falha na primeira melhoria.\n");

        return 1;
    }

    if(controllerBuscaLocalMelhorarSolucaoComMelhorMelhoria(&instancia,FATOR_H_04,&solucaoInicial,&solucaoMelhor,&resultadoMelhor,4,4) == FALSO) {
        printf("Falha na melhor melhoria.\n");

        return 1;
    }

    if(controllerBuscaLocalHibridaMelhorarSolucao(&instancia,FATOR_H_04,&solucaoInicial,&solucaoHibrida,&resultadoHibrida,4,4) == FALSO) {
        printf("Falha na busca local hibrida.\n");

        return 1;
    }

    if(solucaoEhValida(&solucaoHibrida) == FALSO) {
        printf("Solucao hibrida invalida.\n");

        return 1;
    }

    if(resultadoHibrida.custoFinal > resultadoPrimeira.custoFinal || resultadoHibrida.custoFinal > resultadoMelhor.custoFinal) {
        printf("Busca hibrida nao dominou as buscas isoladas.\n");

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,FATOR_H_04);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoHibrida,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Falha na verificacao do custo.\n");

        return 1;
    }

    if(custoVerificado != resultadoHibrida.custoFinal) {
        printf("Custo hibrido diferente do custo verificado.\n");

        return 1;
    }

    printf("Teste da busca local hibrida aprovado.\n");
    printf("Custo primeira melhoria: %llu\n",(unsigned long long) resultadoPrimeira.custoFinal);
    printf("Custo melhor melhoria: %llu\n",(unsigned long long) resultadoMelhor.custoFinal);
    printf("Custo trajetoria A: %llu\n",(unsigned long long) resultadoHibrida.custoTrajetoriaPrimeiraMelhorPrimeira);
    printf("Custo trajetoria B: %llu\n",(unsigned long long) resultadoHibrida.custoTrajetoriaMelhorPrimeiraMelhor);
    printf("Custo hibrido: %llu\n",(unsigned long long) resultadoHibrida.custoFinal);
    printf("Trajetoria selecionada: %u\n",(unsigned int) resultadoHibrida.trajetoriaSelecionada);
    printf("Chamadas de busca local: %u\n",(unsigned int) resultadoHibrida.quantidadeDeChamadasDeBuscaLocal);
    printf("Vizinhos avaliados: %llu\n",(unsigned long long) resultadoHibrida.quantidadeTotalDeVizinhosAvaliados);

    liberarSolucao(&solucaoHibrida);
    liberarSolucao(&solucaoMelhor);
    liberarSolucao(&solucaoPrimeira);
    liberarSolucao(&solucaoInicial);
    liberarInstancia(&instancia);

    return 0;
}