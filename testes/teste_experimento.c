#include <stdio.h>
#include <string.h>

#include "../model/entidades/experimento/experimento.h"
#include "../model/entidades/heuristica/heuristica.h"

static int testarCriacaoDeExperimentoVazio(void) {
    Experimento experimento;

    experimento = criarExperimentoVazio();

    if(experimento.identificadorDoExperimento != 0) {
        printf("[ERRO] Identificador do experimento vazio deveria ser zero.\n");

        return 1;
    }

    if(experimento.nomeDoExperimento[0] != '\0') {
        printf("[ERRO] Nome do experimento vazio deveria iniciar vazio.\n");

        return 1;
    }

    if(experimento.descricaoDoExperimento[0] != '\0') {
        printf("[ERRO] Descricao do experimento vazio deveria iniciar vazia.\n");

        return 1;
    }

    if(experimento.quantidadeDeArquivosDeInstancia != 0) {
        printf("[ERRO] Quantidade de arquivos deveria iniciar em zero.\n");

        return 1;
    }

    if(experimento.quantidadeDeValoresDeH != 0) {
        printf("[ERRO] Quantidade de valores de h deveria iniciar em zero.\n");

        return 1;
    }

    if(heuristicaEhValida(&experimento.heuristica) != FALSO) {
        printf("[ERRO] Heuristica do experimento vazio deveria ser invalida.\n");

        return 1;
    }

    printf("[OK] Criacao de experimento vazio.\n");

    return 0;
}

static int testarInicializacaoDeExperimento(void) {
    Experimento experimento;
    Heuristica heuristica;
    Boolean resultado;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    resultado = inicializarExperimento(
        &experimento,
        (IdentificadorDeExperimento) 1,
        "Experimento de Teste",
        "Descricao do experimento de teste.",
        heuristica
    );

    if(resultado == FALSO) {
        printf("[ERRO] Experimento deveria ser inicializado.\n");

        return 1;
    }

    if(experimento.identificadorDoExperimento != 1) {
        printf("[ERRO] Identificador do experimento incorreto.\n");

        return 1;
    }

    if(strcmp(experimento.nomeDoExperimento,"Experimento de Teste") != 0) {
        printf("[ERRO] Nome do experimento incorreto.\n");

        return 1;
    }

    if(strcmp(experimento.descricaoDoExperimento,"Descricao do experimento de teste.") != 0) {
        printf("[ERRO] Descricao do experimento incorreta.\n");

        return 1;
    }

    if(heuristicaEhValida(&experimento.heuristica) == FALSO) {
        printf("[ERRO] Heuristica do experimento deveria ser valida.\n");

        return 1;
    }

    printf("[OK] Inicializacao de experimento.\n");

    return 0;
}

static int testarAdicaoDeArquivosDeInstancia(void) {
    Experimento experimento;
    Heuristica heuristica;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    inicializarExperimento(
        &experimento,
        (IdentificadorDeExperimento) 1,
        "Experimento de Teste",
        "Descricao do experimento de teste.",
        heuristica
    );

    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 0,NOME_ARQUIVO_SCH10);
    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 1,NOME_ARQUIVO_SCH20);

    if(experimento.quantidadeDeArquivosDeInstancia != 2) {
        printf("[ERRO] Quantidade de arquivos deveria ser 2.\n");

        return 1;
    }

    if(strcmp(experimento.nomesDosArquivosDeInstancia[0],NOME_ARQUIVO_SCH10) != 0) {
        printf("[ERRO] Primeiro nome de arquivo incorreto.\n");

        return 1;
    }

    if(strcmp(experimento.nomesDosArquivosDeInstancia[1],NOME_ARQUIVO_SCH20) != 0) {
        printf("[ERRO] Segundo nome de arquivo incorreto.\n");

        return 1;
    }

    printf("[OK] Adicao de arquivos de instancia.\n");

    return 0;
}

static int testarSubstituicaoDeArquivoDeInstancia(void) {
    Experimento experimento;
    Heuristica heuristica;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    inicializarExperimento(
        &experimento,
        (IdentificadorDeExperimento) 1,
        "Experimento de Teste",
        "Descricao do experimento de teste.",
        heuristica
    );

    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 0,NOME_ARQUIVO_SCH10);
    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 0,NOME_ARQUIVO_SCH20);

    if(experimento.quantidadeDeArquivosDeInstancia != 1) {
        printf("[ERRO] Substituicao de arquivo nao deveria aumentar a quantidade.\n");

        return 1;
    }

    if(strcmp(experimento.nomesDosArquivosDeInstancia[0],NOME_ARQUIVO_SCH20) != 0) {
        printf("[ERRO] Arquivo deveria ter sido substituido por sch20.txt.\n");

        return 1;
    }

    printf("[OK] Substituicao de arquivo de instancia.\n");

    return 0;
}

static int testarAdicaoDeFatoresH(void) {
    Experimento experimento;
    Heuristica heuristica;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    inicializarExperimento(
        &experimento,
        (IdentificadorDeExperimento) 1,
        "Experimento de Teste",
        "Descricao do experimento de teste.",
        heuristica
    );

    experimentoAdicionarFatorH(&experimento,(InteiroPositivoDe8Bits) 0,(FatorH) FATOR_H_02);
    experimentoAdicionarFatorH(&experimento,(InteiroPositivoDe8Bits) 1,(FatorH) FATOR_H_04);

    if(experimento.quantidadeDeValoresDeH != 2) {
        printf("[ERRO] Quantidade de valores de h deveria ser 2.\n");

        return 1;
    }

    if(experimento.fatoresH[0] != FATOR_H_02) {
        printf("[ERRO] Primeiro fator h incorreto.\n");

        return 1;
    }

    if(experimento.fatoresH[1] != FATOR_H_04) {
        printf("[ERRO] Segundo fator h incorreto.\n");

        return 1;
    }

    printf("[OK] Adicao de fatores h.\n");

    return 0;
}

static int testarSubstituicaoDeFatorH(void) {
    Experimento experimento;
    Heuristica heuristica;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    inicializarExperimento(
        &experimento,
        (IdentificadorDeExperimento) 1,
        "Experimento de Teste",
        "Descricao do experimento de teste.",
        heuristica
    );

    experimentoAdicionarFatorH(&experimento,(InteiroPositivoDe8Bits) 0,(FatorH) FATOR_H_02);
    experimentoAdicionarFatorH(&experimento,(InteiroPositivoDe8Bits) 0,(FatorH) FATOR_H_08);

    if(experimento.quantidadeDeValoresDeH != 1) {
        printf("[ERRO] Substituicao de fator h nao deveria aumentar a quantidade.\n");

        return 1;
    }

    if(experimento.fatoresH[0] != FATOR_H_08) {
        printf("[ERRO] Fator h deveria ter sido substituido por 0.8.\n");

        return 1;
    }

    printf("[OK] Substituicao de fator h.\n");

    return 0;
}

static int testarCriacaoDeExperimentoPadrao(void) {
    Experimento experimento;

    experimento = criarExperimentoPadrao();

    if(experimento.identificadorDoExperimento != IDENTIFICADOR_EXPERIMENTO_PADRAO) {
        printf("[ERRO] Identificador do experimento padrao incorreto.\n");

        return 1;
    }

    if(strcmp(experimento.nomeDoExperimento,NOME_EXPERIMENTO_PADRAO) != 0) {
        printf("[ERRO] Nome do experimento padrao incorreto.\n");

        return 1;
    }

    if(experimento.quantidadeDeArquivosDeInstancia != QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA) {
        printf("[ERRO] Quantidade de arquivos do experimento padrao incorreta.\n");

        return 1;
    }

    if(experimento.quantidadeDeValoresDeH != QUANTIDADE_DE_VALORES_DE_H) {
        printf("[ERRO] Quantidade de valores de h do experimento padrao incorreta.\n");

        return 1;
    }

    if(strcmp(experimento.nomesDosArquivosDeInstancia[0],NOME_ARQUIVO_SCH10) != 0) {
        printf("[ERRO] Primeiro arquivo do experimento padrao incorreto.\n");

        return 1;
    }

    if(strcmp(experimento.nomesDosArquivosDeInstancia[6],NOME_ARQUIVO_SCH1000) != 0) {
        printf("[ERRO] Ultimo arquivo do experimento padrao incorreto.\n");

        return 1;
    }

    if(experimento.fatoresH[0] != FATOR_H_02) {
        printf("[ERRO] Primeiro fator h do experimento padrao incorreto.\n");

        return 1;
    }

    if(experimento.fatoresH[3] != FATOR_H_08) {
        printf("[ERRO] Ultimo fator h do experimento padrao incorreto.\n");

        return 1;
    }

    if(experimentoEhValido(&experimento) == FALSO) {
        printf("[ERRO] Experimento padrao deveria ser valido.\n");

        return 1;
    }

    printf("[OK] Criacao de experimento padrao.\n");

    return 0;
}

static int testarExperimentoNuloInvalido(void) {
    if(experimentoEhValido(NULL) != FALSO) {
        printf("[ERRO] Experimento NULL deveria ser invalido.\n");

        return 1;
    }

    printf("[OK] Experimento nulo invalido.\n");

    return 0;
}

static int testarInicializacaoComHeuristicaInvalida(void) {
    Experimento experimento;
    Heuristica heuristica;
    Boolean resultado;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaVazia();

    resultado = inicializarExperimento(
        &experimento,
        (IdentificadorDeExperimento) 1,
        "Experimento de Teste",
        "Descricao do experimento de teste.",
        heuristica
    );

    if(resultado != FALSO) {
        printf("[ERRO] Inicializacao com heuristica invalida deveria falhar.\n");

        return 1;
    }

    printf("[OK] Inicializacao com heuristica invalida.\n");

    return 0;
}

static int testarArquivoComIndiceInvalido(void) {
    Experimento experimento;
    Heuristica heuristica;
    Boolean resultado;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    inicializarExperimento(
        &experimento,
        (IdentificadorDeExperimento) 1,
        "Experimento de Teste",
        "Descricao do experimento de teste.",
        heuristica
    );

    resultado = experimentoAdicionarArquivoDeInstancia(
        &experimento,
        (InteiroPositivoDe8Bits) QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA,
        NOME_ARQUIVO_SCH10
    );

    if(resultado != FALSO) {
        printf("[ERRO] Arquivo com indice invalido deveria falhar.\n");

        return 1;
    }

    printf("[OK] Arquivo com indice invalido.\n");

    return 0;
}

static int testarFatorHInvalido(void) {
    Experimento experimento;
    Heuristica heuristica;
    Boolean resultado;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    inicializarExperimento(
        &experimento,
        (IdentificadorDeExperimento) 1,
        "Experimento de Teste",
        "Descricao do experimento de teste.",
        heuristica
    );

    resultado = experimentoAdicionarFatorH(
        &experimento,
        (InteiroPositivoDe8Bits) 0,
        (FatorH) 5
    );

    if(resultado != FALSO) {
        printf("[ERRO] Fator h invalido deveria falhar.\n");

        return 1;
    }

    printf("[OK] Fator h invalido.\n");

    return 0;
}

static int testarLimpezaDeExperimento(void) {
    Experimento experimento;

    experimento = criarExperimentoPadrao();

    limparExperimento(&experimento);

    if(experimento.identificadorDoExperimento != 0) {
        printf("[ERRO] Identificador deveria ser zero apos limpar.\n");

        return 1;
    }

    if(experimento.nomeDoExperimento[0] != '\0') {
        printf("[ERRO] Nome deveria ficar vazio apos limpar.\n");

        return 1;
    }

    if(experimento.quantidadeDeArquivosDeInstancia != 0) {
        printf("[ERRO] Quantidade de arquivos deveria ser zero apos limpar.\n");

        return 1;
    }

    if(experimento.quantidadeDeValoresDeH != 0) {
        printf("[ERRO] Quantidade de valores de h deveria ser zero apos limpar.\n");

        return 1;
    }

    printf("[OK] Limpeza de experimento.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes da entidade Experimento...\n\n");

    quantidadeDeErros += testarCriacaoDeExperimentoVazio();
    quantidadeDeErros += testarInicializacaoDeExperimento();
    quantidadeDeErros += testarAdicaoDeArquivosDeInstancia();
    quantidadeDeErros += testarSubstituicaoDeArquivoDeInstancia();
    quantidadeDeErros += testarAdicaoDeFatoresH();
    quantidadeDeErros += testarSubstituicaoDeFatorH();
    quantidadeDeErros += testarCriacaoDeExperimentoPadrao();
    quantidadeDeErros += testarExperimentoNuloInvalido();
    quantidadeDeErros += testarInicializacaoComHeuristicaInvalida();
    quantidadeDeErros += testarArquivoComIndiceInvalido();
    quantidadeDeErros += testarFatorHInvalido();
    quantidadeDeErros += testarLimpezaDeExperimento();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes da entidade Experimento passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes da entidade Experimento: %d\n",quantidadeDeErros);

    return 1;
}
