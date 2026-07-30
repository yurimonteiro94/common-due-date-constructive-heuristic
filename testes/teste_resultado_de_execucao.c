#include <stdio.h>
#include <string.h>

#include "../model/entidades/resultado_de_execucao/resultado_de_execucao.h"

static int testarCriacaoDeResultadoDeExecucaoVazio(void) {
    ResultadoDeExecucao resultadoDeExecucao;

    resultadoDeExecucao = criarResultadoDeExecucaoVazio();

    if(resultadoDeExecucao.identificadorDaExecucao != 0) {
        printf("[ERRO] Identificador da execucao deveria iniciar em zero.\n");

        return 1;
    }

    if(resultadoDeExecucao.nomeDoArquivoDeOrigem[0] != '\0') {
        printf("[ERRO] Nome do arquivo deveria iniciar vazio.\n");

        return 1;
    }

    if(resultadoDeExecucao.custo != 0) {
        printf("[ERRO] Custo deveria iniciar em zero.\n");

        return 1;
    }

    printf("[OK] Criacao de resultado de execucao vazio.\n");

    return 0;
}

static int testarInicializacaoDeResultadoDeExecucao(void) {
    ResultadoDeExecucao resultadoDeExecucao;
    Boolean resultado;

    resultadoDeExecucao = criarResultadoDeExecucaoVazio();

    resultado = inicializarResultadoDeExecucao(
        &resultadoDeExecucao,
        (IdentificadorDeExecucao) 1,
        NOME_ARQUIVO_SCH10,
        (IdentificadorDeHeuristica) IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 10,
        (FatorH) FATOR_H_06,
        (SomaDosTemposDeProcessamento) 116,
        (DataDeEntregaComum) 69,
        (Custo) 1234,
        (TempoComputacionalEmSegundos) 0.025
    );

    if(resultado == FALSO) {
        printf("[ERRO] Resultado de execucao deveria ser inicializado.\n");

        return 1;
    }

    if(resultadoDeExecucao.identificadorDaExecucao != 1) {
        printf("[ERRO] Identificador da execucao incorreto.\n");

        return 1;
    }

    if(strcmp(resultadoDeExecucao.nomeDoArquivoDeOrigem,NOME_ARQUIVO_SCH10) != 0) {
        printf("[ERRO] Nome do arquivo de origem incorreto.\n");

        return 1;
    }

    if(resultadoDeExecucao.identificadorDaHeuristica != IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY) {
        printf("[ERRO] Identificador da heuristica incorreto.\n");

        return 1;
    }

    if(resultadoDeExecucao.identificadorDaInstancia != 1) {
        printf("[ERRO] Identificador da instancia incorreto.\n");

        return 1;
    }

    if(resultadoDeExecucao.quantidadeDeTarefas != 10) {
        printf("[ERRO] Quantidade de tarefas incorreta.\n");

        return 1;
    }

    if(resultadoDeExecucao.fatorH != FATOR_H_06) {
        printf("[ERRO] Fator h incorreto.\n");

        return 1;
    }

    if(resultadoDeExecucao.somaDosTemposDeProcessamento != 116) {
        printf("[ERRO] Soma dos tempos de processamento incorreta.\n");

        return 1;
    }

    if(resultadoDeExecucao.dataDeEntregaComum != 69) {
        printf("[ERRO] Data de entrega comum incorreta.\n");

        return 1;
    }

    if(resultadoDeExecucao.custo != 1234) {
        printf("[ERRO] Custo incorreto.\n");

        return 1;
    }

    if(resultadoDeExecucao.tempoComputacionalEmSegundos != 0.025) {
        printf("[ERRO] Tempo computacional em segundos incorreto.\n");

        return 1;
    }

    if(resultadoDeExecucao.tempoComputacionalEmMilissegundos != 25.0) {
        printf("[ERRO] Tempo computacional em milissegundos incorreto.\n");

        return 1;
    }

    if(resultadoDeExecucaoEhValido(&resultadoDeExecucao) == FALSO) {
        printf("[ERRO] Resultado de execucao deveria ser valido.\n");

        return 1;
    }

    printf("[OK] Inicializacao de resultado de execucao.\n");

    return 0;
}

static int testarResultadoDeExecucaoNuloInvalido(void) {
    if(resultadoDeExecucaoEhValido(NULL) != FALSO) {
        printf("[ERRO] Resultado de execucao NULL deveria ser invalido.\n");

        return 1;
    }

    printf("[OK] Resultado de execucao nulo invalido.\n");

    return 0;
}

static int testarInicializacaoComIdentificadorZeroInvalida(void) {
    ResultadoDeExecucao resultadoDeExecucao;
    Boolean resultado;

    resultadoDeExecucao = criarResultadoDeExecucaoVazio();

    resultado = inicializarResultadoDeExecucao(
        &resultadoDeExecucao,
        (IdentificadorDeExecucao) 0,
        NOME_ARQUIVO_SCH10,
        (IdentificadorDeHeuristica) IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 10,
        (FatorH) FATOR_H_06,
        (SomaDosTemposDeProcessamento) 116,
        (DataDeEntregaComum) 69,
        (Custo) 1234,
        (TempoComputacionalEmSegundos) 0.025
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com identificador zero deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com identificador zero invalida.\n");

    return 0;
}

static int testarInicializacaoComNomeDeArquivoNuloInvalida(void) {
    ResultadoDeExecucao resultadoDeExecucao;
    Boolean resultado;

    resultadoDeExecucao = criarResultadoDeExecucaoVazio();

    resultado = inicializarResultadoDeExecucao(
        &resultadoDeExecucao,
        (IdentificadorDeExecucao) 1,
        NULL,
        (IdentificadorDeHeuristica) IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 10,
        (FatorH) FATOR_H_06,
        (SomaDosTemposDeProcessamento) 116,
        (DataDeEntregaComum) 69,
        (Custo) 1234,
        (TempoComputacionalEmSegundos) 0.025
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com nome de arquivo NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com nome de arquivo nulo invalida.\n");

    return 0;
}

static int testarInicializacaoComFatorHInvalido(void) {
    ResultadoDeExecucao resultadoDeExecucao;
    Boolean resultado;

    resultadoDeExecucao = criarResultadoDeExecucaoVazio();

    resultado = inicializarResultadoDeExecucao(
        &resultadoDeExecucao,
        (IdentificadorDeExecucao) 1,
        NOME_ARQUIVO_SCH10,
        (IdentificadorDeHeuristica) IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 10,
        (FatorH) 5,
        (SomaDosTemposDeProcessamento) 116,
        (DataDeEntregaComum) 69,
        (Custo) 1234,
        (TempoComputacionalEmSegundos) 0.025
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com fator h invalido deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com fator h invalido.\n");

    return 0;
}

static int testarInicializacaoComTempoNegativoInvalida(void) {
    ResultadoDeExecucao resultadoDeExecucao;
    Boolean resultado;

    resultadoDeExecucao = criarResultadoDeExecucaoVazio();

    resultado = inicializarResultadoDeExecucao(
        &resultadoDeExecucao,
        (IdentificadorDeExecucao) 1,
        NOME_ARQUIVO_SCH10,
        (IdentificadorDeHeuristica) IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 10,
        (FatorH) FATOR_H_06,
        (SomaDosTemposDeProcessamento) 116,
        (DataDeEntregaComum) 69,
        (Custo) 1234,
        (TempoComputacionalEmSegundos) -0.025
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com tempo negativo deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com tempo negativo invalida.\n");

    return 0;
}

static int testarResultadoDeExecucaoComCustoZeroValido(void) {
    ResultadoDeExecucao resultadoDeExecucao;
    Boolean resultado;

    resultadoDeExecucao = criarResultadoDeExecucaoVazio();

    resultado = inicializarResultadoDeExecucao(
        &resultadoDeExecucao,
        (IdentificadorDeExecucao) 1,
        NOME_ARQUIVO_SCH10,
        (IdentificadorDeHeuristica) IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 10,
        (FatorH) FATOR_H_06,
        (SomaDosTemposDeProcessamento) 116,
        (DataDeEntregaComum) 69,
        (Custo) 0,
        (TempoComputacionalEmSegundos) 0.025
    );

    if(resultado == FALSO) {
        printf("[ERRO] Resultado com custo zero deveria poder ser inicializado.\n");

        return 1;
    }

    if(resultadoDeExecucaoEhValido(&resultadoDeExecucao) == FALSO) {
        printf("[ERRO] Resultado com custo zero deveria ser valido.\n");

        return 1;
    }

    printf("[OK] Resultado de execucao com custo zero valido.\n");

    return 0;
}

static int testarLimpezaDeResultadoDeExecucao(void) {
    ResultadoDeExecucao resultadoDeExecucao;

    resultadoDeExecucao = criarResultadoDeExecucaoVazio();

    inicializarResultadoDeExecucao(
        &resultadoDeExecucao,
        (IdentificadorDeExecucao) 1,
        NOME_ARQUIVO_SCH10,
        (IdentificadorDeHeuristica) IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY,
        (InteiroPositivoDe16Bits) 1,
        (QuantidadeDeTarefas) 10,
        (FatorH) FATOR_H_06,
        (SomaDosTemposDeProcessamento) 116,
        (DataDeEntregaComum) 69,
        (Custo) 1234,
        (TempoComputacionalEmSegundos) 0.025
    );

    limparResultadoDeExecucao(&resultadoDeExecucao);

    if(resultadoDeExecucao.identificadorDaExecucao != 0) {
        printf("[ERRO] Identificador deveria ser zero apos limpar.\n");

        return 1;
    }

    if(resultadoDeExecucao.nomeDoArquivoDeOrigem[0] != '\0') {
        printf("[ERRO] Nome do arquivo deveria ficar vazio apos limpar.\n");

        return 1;
    }

    if(resultadoDeExecucao.tempoComputacionalEmSegundos != 0.0) {
        printf("[ERRO] Tempo em segundos deveria ser zero apos limpar.\n");

        return 1;
    }

    printf("[OK] Limpeza de resultado de execucao.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes da entidade ResultadoDeExecucao...\n\n");

    quantidadeDeErros += testarCriacaoDeResultadoDeExecucaoVazio();
    quantidadeDeErros += testarInicializacaoDeResultadoDeExecucao();
    quantidadeDeErros += testarResultadoDeExecucaoNuloInvalido();
    quantidadeDeErros += testarInicializacaoComIdentificadorZeroInvalida();
    quantidadeDeErros += testarInicializacaoComNomeDeArquivoNuloInvalida();
    quantidadeDeErros += testarInicializacaoComFatorHInvalido();
    quantidadeDeErros += testarInicializacaoComTempoNegativoInvalida();
    quantidadeDeErros += testarResultadoDeExecucaoComCustoZeroValido();
    quantidadeDeErros += testarLimpezaDeResultadoDeExecucao();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes da entidade ResultadoDeExecucao passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes da entidade ResultadoDeExecucao: %d\n",quantidadeDeErros);

    return 1;
}
