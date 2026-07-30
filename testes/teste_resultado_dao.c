#include <stdio.h>
#include <string.h>

#include "../model/dao/resultado_dao/resultado_dao.h"
#include "../model/entidades/resultado_de_execucao/resultado_de_execucao.h"
#include "../services/ferramentas/ferramentas.h"
#include "../services/gerenciador_de_arquivos/gerenciador_de_arquivos.h"

#define CAMINHO_TESTE_RESULTADOS_EXECUCOES "resultados/teste_resultados_execucoes.csv"
#define CAMINHO_TESTE_MEDIAS_POR_N_H "resultados/teste_medias_por_n_h.csv"

static ResultadoDeExecucao criarResultadoDeExecucaoParaTeste(void) {
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

    return resultadoDeExecucao;
}

static int contarLinhasDoArquivo(const char *caminhoDoArquivo) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];
    int quantidadeDeLinhas;

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(caminhoDoArquivo);

    if(arquivo == NULL) {
        return -1;
    }

    quantidadeDeLinhas = 0;

    while(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA_CSV) == VERDADEIRO) {
        quantidadeDeLinhas++;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return quantidadeDeLinhas;
}

static int testarCriacaoDeArquivoDeResultadosDeExecucoes(void) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];

    if(resultadoDaoCriarArquivoDeResultadosDeExecucoes(CAMINHO_TESTE_RESULTADOS_EXECUCOES) == FALSO) {
        printf("[ERRO] Arquivo de resultados de execucoes deveria ser criado.\n");

        return 1;
    }

    if(gerenciadorDeArquivosArquivoExiste(CAMINHO_TESTE_RESULTADOS_EXECUCOES) == FALSO) {
        printf("[ERRO] Arquivo de resultados de execucoes deveria existir.\n");

        return 1;
    }

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(CAMINHO_TESTE_RESULTADOS_EXECUCOES);

    if(arquivo == NULL) {
        printf("[ERRO] Arquivo de resultados de execucoes deveria abrir para leitura.\n");

        return 1;
    }

    if(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA_CSV) == FALSO) {
        printf("[ERRO] Cabecalho de resultados de execucoes deveria ser lido.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    removerQuebraDeLinha(linha);

    if(strcmp(linha,"idExecucao,arquivo,idInstancia,n,h,somaP,d,idHeuristica,custo,tempoSegundos,tempoMilissegundos,tempoPorTarefaMilissegundos") != 0) {
        printf("[ERRO] Cabecalho de resultados de execucoes incorreto.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    printf("[OK] Criacao de arquivo de resultados de execucoes.\n");

    return 0;
}

static int testarAdicaoDeResultadoDeExecucao(void) {
    ResultadoDeExecucao resultadoDeExecucao;
    int quantidadeDeLinhas;

    resultadoDeExecucao = criarResultadoDeExecucaoParaTeste();

    if(resultadoDaoCriarArquivoDeResultadosDeExecucoes(CAMINHO_TESTE_RESULTADOS_EXECUCOES) == FALSO) {
        printf("[ERRO] Arquivo de resultados de execucoes deveria ser criado.\n");

        return 1;
    }

    if(resultadoDaoAdicionarResultadoDeExecucao(CAMINHO_TESTE_RESULTADOS_EXECUCOES,&resultadoDeExecucao) == FALSO) {
        printf("[ERRO] Resultado de execucao deveria ser adicionado.\n");

        return 1;
    }

    if(resultadoDaoAdicionarResultadoDeExecucao(CAMINHO_TESTE_RESULTADOS_EXECUCOES,&resultadoDeExecucao) == FALSO) {
        printf("[ERRO] Segundo resultado de execucao deveria ser adicionado.\n");

        return 1;
    }

    quantidadeDeLinhas = contarLinhasDoArquivo(CAMINHO_TESTE_RESULTADOS_EXECUCOES);

    if(quantidadeDeLinhas != 3) {
        printf("[ERRO] Arquivo de resultados de execucoes deveria ter 3 linhas.\n");

        return 1;
    }

    printf("[OK] Adicao de resultado de execucao.\n");

    return 0;
}

static int testarCriacaoDeArquivoDeMediasPorNH(void) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];

    if(resultadoDaoCriarArquivoDeMediasPorNH(CAMINHO_TESTE_MEDIAS_POR_N_H) == FALSO) {
        printf("[ERRO] Arquivo de medias por n e h deveria ser criado.\n");

        return 1;
    }

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(CAMINHO_TESTE_MEDIAS_POR_N_H);

    if(arquivo == NULL) {
        printf("[ERRO] Arquivo de medias por n e h deveria abrir para leitura.\n");

        return 1;
    }

    if(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA_CSV) == FALSO) {
        printf("[ERRO] Cabecalho de medias por n e h deveria ser lido.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    removerQuebraDeLinha(linha);

    if(strcmp(linha,"n,h,quantidadeExecucoes,custoMedio,tempoMedioSegundos,tempoMedioMilissegundos,tempoMedioPorTarefaMilissegundos") != 0) {
        printf("[ERRO] Cabecalho de medias por n e h incorreto.\n");
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return 1;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    printf("[OK] Criacao de arquivo de medias por n e h.\n");

    return 0;
}

static int testarAdicaoDeMediaPorNH(void) {
    int quantidadeDeLinhas;

    if(resultadoDaoCriarArquivoDeMediasPorNH(CAMINHO_TESTE_MEDIAS_POR_N_H) == FALSO) {
        printf("[ERRO] Arquivo de medias por n e h deveria ser criado.\n");

        return 1;
    }

    if(resultadoDaoAdicionarMediaPorNH(
        CAMINHO_TESTE_MEDIAS_POR_N_H,
        (QuantidadeDeTarefas) 10,
        (FatorH) FATOR_H_06,
        (InteiroPositivoDe16Bits) 10,
        1234.5,
        0.025,
        25.0,
        2.5
    ) == FALSO) {
        printf("[ERRO] Media por n e h deveria ser adicionada.\n");

        return 1;
    }

    quantidadeDeLinhas = contarLinhasDoArquivo(CAMINHO_TESTE_MEDIAS_POR_N_H);

    if(quantidadeDeLinhas != 2) {
        printf("[ERRO] Arquivo de medias deveria ter 2 linhas.\n");

        return 1;
    }

    printf("[OK] Adicao de media por n e h.\n");

    return 0;
}

static int testarParametrosInvalidos(void) {
    ResultadoDeExecucao resultadoDeExecucao;

    resultadoDeExecucao = criarResultadoDeExecucaoParaTeste();

    if(resultadoDaoCriarArquivoDeResultadosDeExecucoes(NULL) != FALSO) {
        printf("[ERRO] Criacao de arquivo de resultados com caminho NULL deveria falhar.\n");

        return 1;
    }

    if(resultadoDaoAdicionarResultadoDeExecucao(CAMINHO_TESTE_RESULTADOS_EXECUCOES,NULL) != FALSO) {
        printf("[ERRO] Adicao de resultado NULL deveria falhar.\n");

        return 1;
    }

    if(resultadoDaoAdicionarResultadoDeExecucao(NULL,&resultadoDeExecucao) != FALSO) {
        printf("[ERRO] Adicao de resultado com caminho NULL deveria falhar.\n");

        return 1;
    }

    if(resultadoDaoCriarArquivoDeMediasPorNH(NULL) != FALSO) {
        printf("[ERRO] Criacao de arquivo de medias com caminho NULL deveria falhar.\n");

        return 1;
    }

    if(resultadoDaoAdicionarMediaPorNH(CAMINHO_TESTE_MEDIAS_POR_N_H,(QuantidadeDeTarefas) 0,(FatorH) FATOR_H_06,(InteiroPositivoDe16Bits) 10,1.0,1.0,1.0,1.0) != FALSO) {
        printf("[ERRO] Media com quantidade de tarefas zero deveria falhar.\n");

        return 1;
    }

    if(resultadoDaoAdicionarMediaPorNH(CAMINHO_TESTE_MEDIAS_POR_N_H,(QuantidadeDeTarefas) 10,(FatorH) 5,(InteiroPositivoDe16Bits) 10,1.0,1.0,1.0,1.0) != FALSO) {
        printf("[ERRO] Media com fator h invalido deveria falhar.\n");

        return 1;
    }

    if(resultadoDaoAdicionarMediaPorNH(CAMINHO_TESTE_MEDIAS_POR_N_H,(QuantidadeDeTarefas) 10,(FatorH) FATOR_H_06,(InteiroPositivoDe16Bits) 0,1.0,1.0,1.0,1.0) != FALSO) {
        printf("[ERRO] Media com quantidade de execucoes zero deveria falhar.\n");

        return 1;
    }

    printf("[OK] Parametros invalidos do resultado dao.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do ResultadoDAO...\n\n");

    quantidadeDeErros += testarCriacaoDeArquivoDeResultadosDeExecucoes();
    quantidadeDeErros += testarAdicaoDeResultadoDeExecucao();
    quantidadeDeErros += testarCriacaoDeArquivoDeMediasPorNH();
    quantidadeDeErros += testarAdicaoDeMediaPorNH();
    quantidadeDeErros += testarParametrosInvalidos();

    remove(CAMINHO_TESTE_RESULTADOS_EXECUCOES);
    remove(CAMINHO_TESTE_MEDIAS_POR_N_H);

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do ResultadoDAO passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes do ResultadoDAO: %d\n",quantidadeDeErros);

    return 1;
}
