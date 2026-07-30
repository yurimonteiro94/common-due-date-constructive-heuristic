#include <stdio.h>
#include <string.h>

#include "../controller/controller_benchmark/controller_benchmark.h"
#include "../services/gerenciador_de_arquivos/gerenciador_de_arquivos.h"

#define CAMINHO_TESTE_RESULTADOS_EXECUCOES "resultados/teste_resultados_para_benchmark.csv"
#define CAMINHO_TESTE_REFERENCIAS "benchmarks/teste_referencias_benchmark.csv"
#define CAMINHO_TESTE_COMPARACAO "resultados/teste_comparacao_benchmark.csv"

static Boolean criarArquivoDeResultadosParaTeste(void) {
    FILE *arquivo;

    arquivo = gerenciadorDeArquivosAbrirArquivoParaEscrita(CAMINHO_TESTE_RESULTADOS_EXECUCOES);

    if(arquivo == NULL) {
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,"idExecucao,arquivo,idInstancia,n,h,somaP,d,idHeuristica,custo,tempoSegundos,tempoMilissegundos,tempoPorTarefaMilissegundos\n") == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,"1,sch10.txt,1,10,0.6,116,69,1,1337,0.001000000,1.000000000,0.100000000\n") == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,"2,sch20.txt,1,20,0.6,217,130,1,6924,0.001000000,1.000000000,0.050000000\n") == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);
        return FALSO;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return VERDADEIRO;
}

static Boolean criarArquivoDeReferenciasParaTeste(void) {
    FILE *arquivo;

    arquivo = gerenciadorDeArquivosAbrirArquivoParaEscrita(CAMINHO_TESTE_REFERENCIAS);

    if(arquivo == NULL) {
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,"arquivo,idInstancia,n,h,custoMelhorConhecido,tipoMelhorConhecido,custoAutores,tipoAutores\n") == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,"sch10.txt,1,10,0.6,841,otimo,841,autores\n") == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);
        return FALSO;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return VERDADEIRO;
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

static Boolean arquivoContemTexto(const char *caminhoDoArquivo,const char *textoProcurado) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(caminhoDoArquivo);

    if(arquivo == NULL) {
        return FALSO;
    }

    while(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA_CSV) == VERDADEIRO) {
        if(strstr(linha,textoProcurado) != NULL) {
            gerenciadorDeArquivosFecharArquivo(arquivo);
            return VERDADEIRO;
        }
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return FALSO;
}

static int testarGeracaoDeComparacaoComBenchmark(void) {
    int quantidadeDeLinhas;

    if(criarArquivoDeResultadosParaTeste() == FALSO) {
        printf("[ERRO] Arquivo de resultados para benchmark nao foi criado.\n");
        return 1;
    }

    if(criarArquivoDeReferenciasParaTeste() == FALSO) {
        printf("[ERRO] Arquivo de referencias para teste nao foi criado.\n");
        return 1;
    }

    if(controllerBenchmarkGerarComparacao(CAMINHO_TESTE_RESULTADOS_EXECUCOES,CAMINHO_TESTE_REFERENCIAS,CAMINHO_TESTE_COMPARACAO) == FALSO) {
        printf("[ERRO] Comparacao com benchmark deveria ser gerada.\n");
        return 1;
    }

    quantidadeDeLinhas = contarLinhasDoArquivo(CAMINHO_TESTE_COMPARACAO);

    if(quantidadeDeLinhas != 3) {
        printf("[ERRO] Arquivo de comparacao deveria ter 3 linhas.\n");
        return 1;
    }

    if(arquivoContemTexto(CAMINHO_TESTE_COMPARACAO,"gapMelhorSolucaoConhecidaPercentual") == FALSO) {
        printf("[ERRO] Cabecalho deveria conter gapMelhorSolucaoConhecidaPercentual.\n");
        return 1;
    }

    if(arquivoContemTexto(CAMINHO_TESTE_COMPARACAO,"gapSolucaoAutoresPercentual") == FALSO) {
        printf("[ERRO] Cabecalho deveria conter gapSolucaoAutoresPercentual.\n");
        return 1;
    }

    if(arquivoContemTexto(CAMINHO_TESTE_COMPARACAO,"58.977407848") == FALSO) {
        printf("[ERRO] Gap esperado para sch10.txt instancia 1 h 0.6 nao foi encontrado.\n");
        return 1;
    }

    if(arquivoContemTexto(CAMINHO_TESTE_COMPARACAO,"desconhecido,NA,0,desconhecido,NA") == FALSO) {
        printf("[ERRO] Linha sem referencia deveria receber valores desconhecidos e NA.\n");
        return 1;
    }

    printf("[OK] Geracao de comparacao com benchmark.\n");

    return 0;
}

static int testarParametrosInvalidos(void) {
    if(controllerBenchmarkGerarComparacao(NULL,CAMINHO_TESTE_REFERENCIAS,CAMINHO_TESTE_COMPARACAO) != FALSO) {
        printf("[ERRO] Caminho de resultados NULL deveria falhar.\n");
        return 1;
    }

    if(controllerBenchmarkGerarComparacao(CAMINHO_TESTE_RESULTADOS_EXECUCOES,NULL,CAMINHO_TESTE_COMPARACAO) != FALSO) {
        printf("[ERRO] Caminho de referencias NULL deveria falhar.\n");
        return 1;
    }

    if(controllerBenchmarkGerarComparacao(CAMINHO_TESTE_RESULTADOS_EXECUCOES,CAMINHO_TESTE_REFERENCIAS,NULL) != FALSO) {
        printf("[ERRO] Caminho de comparacao NULL deveria falhar.\n");
        return 1;
    }

    printf("[OK] Parametros invalidos do controller de benchmark.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do ControllerBenchmark...\n\n");

    quantidadeDeErros += testarGeracaoDeComparacaoComBenchmark();
    quantidadeDeErros += testarParametrosInvalidos();

    remove(CAMINHO_TESTE_RESULTADOS_EXECUCOES);
    remove(CAMINHO_TESTE_REFERENCIAS);
    remove(CAMINHO_TESTE_COMPARACAO);

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do ControllerBenchmark passaram.\n");
        return 0;
    }

    printf("Quantidade de erros nos testes do ControllerBenchmark: %d\n",quantidadeDeErros);

    return 1;
}
