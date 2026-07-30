#include <stdio.h>

#include "../model/entidades/instancia/instancia.h"
#include "../model/entidades/solucao/solucao.h"
#include "../model/entidades/tarefa/tarefa.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"

static void montarInstanciaDeTeste(Instancia *instancia,Tarefa *tarefas,Solucao *solucao,IdentificadorDeTarefa *sequencia) {
    tarefas[0] = criarTarefa(1,3,1,5);
    tarefas[1] = criarTarefa(2,2,1,5);

    sequencia[0] = 1;
    sequencia[1] = 2;

    (*instancia).identificadorDaInstancia = 1;
    (*instancia).quantidadeDeTarefas = 2;
    (*instancia).tarefas = tarefas;
    (*instancia).somaDosTemposDeProcessamento = 5;

    (*solucao).quantidadeDeTarefas = 2;
    (*solucao).quantidadeDeTarefasAlocadas = 2;
    (*solucao).sequenciaDeTarefas = sequencia;
}

static int testarCustoComInstanteInicialZero(void) {
    Instancia instancia;
    Tarefa tarefas[2];
    Solucao solucao;
    IdentificadorDeTarefa sequencia[2];
    Custo custo;

    montarInstanciaDeTeste(&instancia,tarefas,&solucao,sequencia);

    if(gerenciadorDeCustosCalcularCustoDaSolucaoComInstanteInicial(&instancia,&solucao,10,0,&custo) == FALSO) {
        printf("[ERRO] Calculo de custo com instante inicial zero falhou.\n");
        return 1;
    }

    if(custo != 12) {
        printf("[ERRO] Custo com instante inicial zero deveria ser 12, mas foi %llu.\n",(unsigned long long) custo);
        return 1;
    }

    printf("[OK] Custo com instante inicial zero.\n");

    return 0;
}

static int testarMelhorInstanteInicial(void) {
    Instancia instancia;
    Tarefa tarefas[2];
    Solucao solucao;
    IdentificadorDeTarefa sequencia[2];
    InteiroPositivoDe32Bits melhorInstanteInicial;

    montarInstanciaDeTeste(&instancia,tarefas,&solucao,sequencia);

    if(gerenciadorDeCustosEncontrarMelhorInstanteInicialDaSolucao(&instancia,&solucao,10,&melhorInstanteInicial) == FALSO) {
        printf("[ERRO] Busca do melhor instante inicial falhou.\n");
        return 1;
    }

    if(melhorInstanteInicial != 5) {
        printf("[ERRO] Melhor instante inicial deveria ser 5, mas foi %u.\n",(unsigned int) melhorInstanteInicial);
        return 1;
    }

    printf("[OK] Melhor instante inicial da solucao.\n");

    return 0;
}

static int testarCustoComMelhorDeslocamento(void) {
    Instancia instancia;
    Tarefa tarefas[2];
    Solucao solucao;
    IdentificadorDeTarefa sequencia[2];
    Custo custo;

    montarInstanciaDeTeste(&instancia,tarefas,&solucao,sequencia);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucao,10,&custo) == FALSO) {
        printf("[ERRO] Calculo de custo com melhor deslocamento falhou.\n");
        return 1;
    }

    if(custo != 2) {
        printf("[ERRO] Custo com melhor deslocamento deveria ser 2, mas foi %llu.\n",(unsigned long long) custo);
        return 1;
    }

    printf("[OK] Custo com melhor deslocamento temporal.\n");

    return 0;
}

static int testarParametrosInvalidos(void) {
    Instancia instancia;
    Tarefa tarefas[2];
    Solucao solucao;
    IdentificadorDeTarefa sequencia[2];
    Custo custo;

    montarInstanciaDeTeste(&instancia,tarefas,&solucao,sequencia);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(NULL,&solucao,10,&custo) != FALSO) {
        printf("[ERRO] Instancia NULL deveria falhar.\n");
        return 1;
    }

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,NULL,10,&custo) != FALSO) {
        printf("[ERRO] Solucao NULL deveria falhar.\n");
        return 1;
    }

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucao,10,NULL) != FALSO) {
        printf("[ERRO] Ponteiro de custo NULL deveria falhar.\n");
        return 1;
    }

    printf("[OK] Parametros invalidos do gerenciador de custos.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do GerenciadorDeCustos...\n\n");

    quantidadeDeErros += testarCustoComInstanteInicialZero();
    quantidadeDeErros += testarMelhorInstanteInicial();
    quantidadeDeErros += testarCustoComMelhorDeslocamento();
    quantidadeDeErros += testarParametrosInvalidos();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do GerenciadorDeCustos passaram.\n");
        return 0;
    }

    printf("Quantidade de erros nos testes do GerenciadorDeCustos: %d\n",quantidadeDeErros);

    return 1;
}
