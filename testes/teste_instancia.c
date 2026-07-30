#include <stdio.h>
#include <string.h>

#include "../model/entidades/instancia/instancia.h"
#include "../model/entidades/tarefa/tarefa.h"

static int testarCriacaoDeInstanciaVazia(void) {
    Instancia instancia;

    instancia = criarInstanciaVazia();

    if(instancia.nomeDoArquivoDeOrigem[0] != '\0') {
        printf("[ERRO] Nome do arquivo deveria iniciar vazio.\n");

        return 1;
    }

    if(instancia.identificadorDaInstancia != 0) {
        printf("[ERRO] Identificador da instancia vazia deveria ser zero.\n");

        return 1;
    }

    if(instancia.quantidadeDeTarefas != 0) {
        printf("[ERRO] Quantidade de tarefas da instancia vazia deveria ser zero.\n");

        return 1;
    }

    if(instancia.tarefas != NULL) {
        printf("[ERRO] Vetor de tarefas da instancia vazia deveria ser NULL.\n");

        return 1;
    }

    if(instancia.somaDosTemposDeProcessamento != 0) {
        printf("[ERRO] Soma dos tempos da instancia vazia deveria ser zero.\n");

        return 1;
    }

    printf("[OK] Criacao de instancia vazia.\n");

    return 0;
}

static int testarInicializacaoDeInstancia(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = inicializarInstancia(
        &instancia,
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 3
    );

    if(resultado == FALSO) {
        printf("[ERRO] Instancia deveria ser inicializada.\n");

        return 1;
    }

    if(strcmp(instancia.nomeDoArquivoDeOrigem,NOME_ARQUIVO_SCH10) != 0) {
        printf("[ERRO] Nome do arquivo de origem incorreto.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.identificadorDaInstancia != 1) {
        printf("[ERRO] Identificador da instancia incorreto.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.quantidadeDeTarefas != 3) {
        printf("[ERRO] Quantidade de tarefas incorreta.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.tarefas == NULL) {
        printf("[ERRO] Vetor de tarefas nao deveria ser NULL.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.somaDosTemposDeProcessamento != 0) {
        printf("[ERRO] Soma dos tempos deveria iniciar em zero.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Inicializacao de instancia.\n");

    return 0;
}

static int testarAdicaoDeTarefasNaInstancia(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = inicializarInstancia(
        &instancia,
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 3
    );

    if(resultado == FALSO) {
        printf("[ERRO] Instancia nao foi inicializada.\n");

        return 1;
    }

    resultado = instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 0,
        criarTarefa(
            (IdentificadorDeTarefa) 1,
            (TempoDeProcessamento) 10,
            (Penalidade) 4,
            (Penalidade) 8
        )
    );

    if(resultado == FALSO) {
        printf("[ERRO] Tarefa 1 nao foi adicionada.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    resultado = instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 1,
        criarTarefa(
            (IdentificadorDeTarefa) 2,
            (TempoDeProcessamento) 20,
            (Penalidade) 5,
            (Penalidade) 6
        )
    );

    if(resultado == FALSO) {
        printf("[ERRO] Tarefa 2 nao foi adicionada.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    resultado = instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 2,
        criarTarefa(
            (IdentificadorDeTarefa) 3,
            (TempoDeProcessamento) 30,
            (Penalidade) 7,
            (Penalidade) 9
        )
    );

    if(resultado == FALSO) {
        printf("[ERRO] Tarefa 3 nao foi adicionada.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.somaDosTemposDeProcessamento != 60) {
        printf("[ERRO] Soma dos tempos deveria ser 60.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instanciaEhValida(&instancia) == FALSO) {
        printf("[ERRO] Instancia deveria ser valida apos adicionar todas as tarefas.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Adicao de tarefas na instancia.\n");

    return 0;
}

static int testarSubstituicaoDeTarefaNaInstancia(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = inicializarInstancia(
        &instancia,
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 2
    );

    if(resultado == FALSO) {
        printf("[ERRO] Instancia nao foi inicializada.\n");

        return 1;
    }

    instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 0,
        criarTarefa(
            (IdentificadorDeTarefa) 1,
            (TempoDeProcessamento) 10,
            (Penalidade) 4,
            (Penalidade) 8
        )
    );

    instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 1,
        criarTarefa(
            (IdentificadorDeTarefa) 2,
            (TempoDeProcessamento) 20,
            (Penalidade) 5,
            (Penalidade) 6
        )
    );

    if(instancia.somaDosTemposDeProcessamento != 30) {
        printf("[ERRO] Soma inicial deveria ser 30.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    resultado = instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 1,
        criarTarefa(
            (IdentificadorDeTarefa) 2,
            (TempoDeProcessamento) 40,
            (Penalidade) 5,
            (Penalidade) 6
        )
    );

    if(resultado == FALSO) {
        printf("[ERRO] Substituicao da tarefa deveria funcionar.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.somaDosTemposDeProcessamento != 50) {
        printf("[ERRO] Soma apos substituicao deveria ser 50.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Substituicao de tarefa na instancia.\n");

    return 0;
}

static int testarCalculoDaDataDeEntregaComum(void) {
    Instancia instancia;
    DataDeEntregaComum dataDeEntregaComum;

    instancia = criarInstanciaVazia();

    inicializarInstancia(
        &instancia,
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 3
    );

    instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 0,
        criarTarefa(
            (IdentificadorDeTarefa) 1,
            (TempoDeProcessamento) 10,
            (Penalidade) 4,
            (Penalidade) 8
        )
    );

    instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 1,
        criarTarefa(
            (IdentificadorDeTarefa) 2,
            (TempoDeProcessamento) 20,
            (Penalidade) 5,
            (Penalidade) 6
        )
    );

    instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 2,
        criarTarefa(
            (IdentificadorDeTarefa) 3,
            (TempoDeProcessamento) 30,
            (Penalidade) 7,
            (Penalidade) 9
        )
    );

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,FATOR_H_06);

    if(dataDeEntregaComum != 36) {
        printf("[ERRO] Data de entrega comum deveria ser 36.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Calculo da data de entrega comum.\n");

    return 0;
}

static int testarInstanciaNulaInvalida(void) {
    if(instanciaEhValida(NULL) != FALSO) {
        printf("[ERRO] Instancia NULL deveria ser invalida.\n");

        return 1;
    }

    printf("[OK] Instancia nula invalida.\n");

    return 0;
}

static int testarInicializacaoComQuantidadeZeroInvalida(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = inicializarInstancia(
        &instancia,
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 0
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com quantidade zero deveria falhar.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    printf("[OK] Inicializacao com quantidade zero invalida.\n");

    return 0;
}

static int testarIndiceInvalidoAoAdicionarTarefa(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    inicializarInstancia(
        &instancia,
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 2
    );

    resultado = instanciaAdicionarTarefa(
        &instancia,
        (QuantidadeDeTarefas) 2,
        criarTarefa(
            (IdentificadorDeTarefa) 1,
            (TempoDeProcessamento) 10,
            (Penalidade) 4,
            (Penalidade) 8
        )
    );

    if(resultado != FALSO) {
        printf("[ERRO] Adicao em indice invalido deveria falhar.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Indice invalido ao adicionar tarefa.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes da entidade Instancia...\n\n");

    quantidadeDeErros += testarCriacaoDeInstanciaVazia();
    quantidadeDeErros += testarInicializacaoDeInstancia();
    quantidadeDeErros += testarAdicaoDeTarefasNaInstancia();
    quantidadeDeErros += testarSubstituicaoDeTarefaNaInstancia();
    quantidadeDeErros += testarCalculoDaDataDeEntregaComum();
    quantidadeDeErros += testarInstanciaNulaInvalida();
    quantidadeDeErros += testarInicializacaoComQuantidadeZeroInvalida();
    quantidadeDeErros += testarIndiceInvalidoAoAdicionarTarefa();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes da entidade Instancia passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes da entidade Instancia: %d\n",quantidadeDeErros);

    return 1;
}
