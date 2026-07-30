#include <stdio.h>

#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../model/entidades/instancia/instancia.h"
#include "../model/entidades/solucao/solucao.h"
#include "../model/entidades/tarefa/tarefa.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

static Boolean prepararInstanciaPequena(Instancia *instancia) {
    if(inicializarInstancia(instancia,NOME_ARQUIVO_SCH10,(InteiroPositivoDe16Bits) 1,(QuantidadeDeTarefas) 3) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,(QuantidadeDeTarefas) 0,criarTarefa((IdentificadorDeTarefa) 1,(TempoDeProcessamento) 10,(Penalidade) 4,(Penalidade) 8)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,(QuantidadeDeTarefas) 1,criarTarefa((IdentificadorDeTarefa) 2,(TempoDeProcessamento) 20,(Penalidade) 5,(Penalidade) 6)) == FALSO) {
        return FALSO;
    }

    if(instanciaAdicionarTarefa(instancia,(QuantidadeDeTarefas) 2,criarTarefa((IdentificadorDeTarefa) 3,(TempoDeProcessamento) 30,(Penalidade) 7,(Penalidade) 9)) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static int testarConstrucaoDeSolucaoPequena(void) {
    Instancia instancia;
    Heuristica heuristica;
    Solucao solucao;
    Custo custo;
    DataDeEntregaComum dataDeEntregaComum;

    instancia = criarInstanciaVazia();
    heuristica = criarHeuristicaConstrutivaFuzzy();
    solucao = criarSolucaoVazia();
    custo = 0;

    if(prepararInstanciaPequena(&instancia) == FALSO) {
        printf("[ERRO] Instancia pequena nao foi preparada.\n");

        return 1;
    }

    if(controllerHeuristicaConstruirSolucao(&instancia,&heuristica,(FatorH) FATOR_H_06,&solucao) == FALSO) {
        printf("[ERRO] Controller deveria construir solucao pequena.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(solucaoEhValida(&solucao) == FALSO) {
        printf("[ERRO] Solucao pequena deveria ser valida.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,(FatorH) FATOR_H_06);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucao,dataDeEntregaComum,&custo) == FALSO) {
        printf("[ERRO] Custo da solucao pequena deveria ser calculado.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    if(custo == 0) {
        printf("[ERRO] Custo da solucao pequena nao deveria ser zero.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    liberarSolucao(&solucao);
    liberarInstancia(&instancia);

    printf("[OK] Construcao de solucao pequena.\n");

    return 0;
}

static int testarConstrucaoDeSolucaoDaPrimeiraInstanciaSch10(void) {
    Instancia instancia;
    Heuristica heuristica;
    Solucao solucao;
    Custo custo;
    DataDeEntregaComum dataDeEntregaComum;

    instancia = criarInstanciaVazia();
    heuristica = criarHeuristicaConstrutivaFuzzy();
    solucao = criarSolucaoVazia();
    custo = 0;

    if(instanciaDaoLerInstanciaPorIdentificador(NOME_ARQUIVO_SCH10,(InteiroPositivoDe16Bits) 1,&instancia) == FALSO) {
        printf("[ERRO] Primeira instancia do sch10 deveria ser lida.\n");

        return 1;
    }

    if(controllerHeuristicaConstruirSolucao(&instancia,&heuristica,(FatorH) FATOR_H_06,&solucao) == FALSO) {
        printf("[ERRO] Controller deveria construir solucao para sch10.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(solucaoEhValida(&solucao) == FALSO) {
        printf("[ERRO] Solucao do sch10 deveria ser valida.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,(FatorH) FATOR_H_06);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucao,dataDeEntregaComum,&custo) == FALSO) {
        printf("[ERRO] Custo da solucao do sch10 deveria ser calculado.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    if(custo == 0) {
        printf("[ERRO] Custo da solucao do sch10 nao deveria ser zero.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    liberarSolucao(&solucao);
    liberarInstancia(&instancia);

    printf("[OK] Construcao de solucao da primeira instancia do sch10.\n");

    return 0;
}

static int testarConstrucaoDeSolucaoDaPrimeiraInstanciaSch1000(void) {
    Instancia instancia;
    Heuristica heuristica;
    Solucao solucao;
    Custo custo;
    DataDeEntregaComum dataDeEntregaComum;

    instancia = criarInstanciaVazia();
    heuristica = criarHeuristicaConstrutivaFuzzy();
    solucao = criarSolucaoVazia();
    custo = 0;

    if(instanciaDaoLerInstanciaPorIdentificador(NOME_ARQUIVO_SCH1000,(InteiroPositivoDe16Bits) 1,&instancia) == FALSO) {
        printf("[ERRO] Primeira instancia do sch1000 deveria ser lida.\n");

        return 1;
    }

    if(controllerHeuristicaConstruirSolucao(&instancia,&heuristica,(FatorH) FATOR_H_06,&solucao) == FALSO) {
        printf("[ERRO] Controller deveria construir solucao para sch1000.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(solucaoEhValida(&solucao) == FALSO) {
        printf("[ERRO] Solucao do sch1000 deveria ser valida.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,(FatorH) FATOR_H_06);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucao,dataDeEntregaComum,&custo) == FALSO) {
        printf("[ERRO] Custo da solucao do sch1000 deveria ser calculado.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    liberarSolucao(&solucao);
    liberarInstancia(&instancia);

    printf("[OK] Construcao de solucao da primeira instancia do sch1000.\n");

    return 0;
}

static int testarInstanciaNulaInvalida(void) {
    Heuristica heuristica;
    Solucao solucao;

    heuristica = criarHeuristicaConstrutivaFuzzy();
    solucao = criarSolucaoVazia();

    if(controllerHeuristicaConstruirSolucao(NULL,&heuristica,(FatorH) FATOR_H_06,&solucao) != FALSO) {
        printf("[ERRO] Instancia NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Instancia nula invalida no controller de heuristica.\n");

    return 0;
}

static int testarHeuristicaNulaInvalida(void) {
    Instancia instancia;
    Solucao solucao;

    instancia = criarInstanciaVazia();
    solucao = criarSolucaoVazia();

    prepararInstanciaPequena(&instancia);

    if(controllerHeuristicaConstruirSolucao(&instancia,NULL,(FatorH) FATOR_H_06,&solucao) != FALSO) {
        printf("[ERRO] Heuristica NULL deveria falhar.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Heuristica nula invalida no controller de heuristica.\n");

    return 0;
}

static int testarPonteiroDeSolucaoNuloInvalido(void) {
    Instancia instancia;
    Heuristica heuristica;

    instancia = criarInstanciaVazia();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    prepararInstanciaPequena(&instancia);

    if(controllerHeuristicaConstruirSolucao(&instancia,&heuristica,(FatorH) FATOR_H_06,NULL) != FALSO) {
        printf("[ERRO] Ponteiro de solucao NULL deveria falhar.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Ponteiro de solucao nulo invalido no controller de heuristica.\n");

    return 0;
}

static int testarFatorHInvalido(void) {
    Instancia instancia;
    Heuristica heuristica;
    Solucao solucao;

    instancia = criarInstanciaVazia();
    heuristica = criarHeuristicaConstrutivaFuzzy();
    solucao = criarSolucaoVazia();

    prepararInstanciaPequena(&instancia);

    if(controllerHeuristicaConstruirSolucao(&instancia,&heuristica,(FatorH) 0,&solucao) != FALSO) {
        printf("[ERRO] Fator h invalido deveria falhar.\n");
        liberarSolucao(&solucao);
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Fator h invalido no controller de heuristica.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do ControllerHeuristica...\n\n");

    quantidadeDeErros += testarConstrucaoDeSolucaoPequena();
    quantidadeDeErros += testarConstrucaoDeSolucaoDaPrimeiraInstanciaSch10();
    quantidadeDeErros += testarConstrucaoDeSolucaoDaPrimeiraInstanciaSch1000();
    quantidadeDeErros += testarInstanciaNulaInvalida();
    quantidadeDeErros += testarHeuristicaNulaInvalida();
    quantidadeDeErros += testarPonteiroDeSolucaoNuloInvalido();
    quantidadeDeErros += testarFatorHInvalido();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do ControllerHeuristica passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes do ControllerHeuristica: %d\n",quantidadeDeErros);

    return 1;
}
