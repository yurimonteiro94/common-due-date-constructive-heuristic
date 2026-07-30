#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <stdint.h>

typedef uint8_t Boolean;

typedef uint8_t InteiroPositivoDe8Bits;
typedef uint16_t InteiroPositivoDe16Bits;
typedef uint32_t InteiroPositivoDe32Bits;
typedef uint64_t InteiroPositivoDe64Bits;

typedef int8_t InteiroComSinalDe8Bits;
typedef int16_t InteiroComSinalDe16Bits;
typedef int32_t InteiroComSinalDe32Bits;
typedef int64_t InteiroComSinalDe64Bits;

typedef InteiroPositivoDe16Bits IdentificadorDeTarefa;
typedef InteiroPositivoDe16Bits IdentificadorDeHeuristica;
typedef InteiroPositivoDe16Bits IdentificadorDeExperimento;
typedef InteiroPositivoDe16Bits IdentificadorDeExecucao;
typedef InteiroPositivoDe16Bits QuantidadeDeTarefas;

typedef InteiroPositivoDe8Bits TempoDeProcessamento;
typedef InteiroPositivoDe8Bits Penalidade;
typedef InteiroPositivoDe8Bits FatorH;

typedef InteiroPositivoDe32Bits SomaDosTemposDeProcessamento;
typedef InteiroPositivoDe32Bits DataDeEntregaComum;
typedef InteiroPositivoDe64Bits Custo;

typedef double TempoComputacionalEmSegundos;
typedef double TempoComputacionalEmMilissegundos;

#define FALSO 0x00
#define VERDADEIRO 0x01

#define CODIGO_ERRO 0x00
#define CODIGO_SUCESSO 0x01

#define QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA 7
#define QUANTIDADE_DE_PROBLEMAS_POR_ARQUIVO 10
#define QUANTIDADE_DE_VALORES_DE_H 4
#define QUANTIDADE_TOTAL_DE_EXECUCOES 280

#define FATOR_H_02 2
#define FATOR_H_04 4
#define FATOR_H_06 6
#define FATOR_H_08 8
#define FATOR_DE_ESCALA_H 10

#define MILISSEGUNDOS_POR_SEGUNDO 1000.0

#define TAMANHO_MAXIMO_DE_LINHA 256
#define TAMANHO_MAXIMO_DE_LINHA_CSV 1024
#define TAMANHO_MAXIMO_DE_CAMINHO 512
#define TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO 128
#define TAMANHO_MAXIMO_DE_NOME_DE_HEURISTICA 128
#define TAMANHO_MAXIMO_DE_DESCRICAO_DE_HEURISTICA 512
#define TAMANHO_MAXIMO_DE_NOME_DE_EXPERIMENTO 128
#define TAMANHO_MAXIMO_DE_DESCRICAO_DE_EXPERIMENTO 512
#define TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA 64
#define TAMANHO_MAXIMO_DE_VALOR_TEXTUAL 64

#define CAMINHO_DA_PASTA_DE_INSTANCIAS "instancias"
#define CAMINHO_DA_PASTA_DE_RESULTADOS "resultados"
#define CAMINHO_DA_PASTA_DE_BENCHMARKS "benchmarks"

#define NOME_ARQUIVO_SCH10 "sch10.txt"
#define NOME_ARQUIVO_SCH20 "sch20.txt"
#define NOME_ARQUIVO_SCH50 "sch50.txt"
#define NOME_ARQUIVO_SCH100 "sch100.txt"
#define NOME_ARQUIVO_SCH200 "sch200.txt"
#define NOME_ARQUIVO_SCH500 "sch500.txt"
#define NOME_ARQUIVO_SCH1000 "sch1000.txt"

#define NOME_ARQUIVO_RESULTADOS_EXECUCOES "resultados_execucoes.csv"
#define NOME_ARQUIVO_MEDIAS_POR_N_H "medias_por_n_h.csv"
#define NOME_ARQUIVO_RELATORIO_HTML "relatorio_resultados.html"
#define NOME_ARQUIVO_REFERENCIAS_BENCHMARK "referencias_benchmark.csv"
#define NOME_ARQUIVO_COMPARACAO_BENCHMARK "comparacao_benchmark.csv"

#define TEXTO_VALOR_NA "NA"
#define TIPO_REFERENCIA_OTIMO "otimo"
#define TIPO_REFERENCIA_MELHOR_CONHECIDA "melhor_conhecida"
#define TIPO_REFERENCIA_UPPER_BOUND "upper_bound"
#define TIPO_REFERENCIA_AUTORES "autores"
#define TIPO_REFERENCIA_DESCONHECIDO "desconhecido"

#define IDENTIFICADOR_HEURISTICA_CONSTRUTIVA_FUZZY 1
#define NOME_HEURISTICA_CONSTRUTIVA_FUZZY "Heuristica Construtiva por Arbitragem Temporal Preditiva"
#define DESCRICAO_HEURISTICA_CONSTRUTIVA_FUZZY "Heuristica construtiva inspirada em arbitragem temporal de recurso unico, com geracao de multiplas sequencias candidatas e escolha da melhor pelo custo da funcao objetivo."

#define IDENTIFICADOR_EXPERIMENTO_PADRAO 1
#define NOME_EXPERIMENTO_PADRAO "Experimento Principal"
#define DESCRICAO_EXPERIMENTO_PADRAO "Experimento com sete arquivos de instancias da OR-Library, dez problemas por arquivo e quatro valores de h."

#endif
