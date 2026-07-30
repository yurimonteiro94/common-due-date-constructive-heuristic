#include "controller_benchmark.h"

#include "../../services/ferramentas/ferramentas.h"
#include "../../services/gerenciador_de_arquivos/gerenciador_de_arquivos.h"

#include <stdio.h>
#include <string.h>

typedef struct RegistroDeResultadoParaBenchmark {
    IdentificadorDeExecucao identificadorDaExecucao;
    char nomeDoArquivoDeOrigem[TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO];
    InteiroPositivoDe16Bits identificadorDaInstancia;
    QuantidadeDeTarefas quantidadeDeTarefas;
    FatorH fatorH;
    IdentificadorDeHeuristica identificadorDaHeuristica;
    Custo custoHeuristica;
} RegistroDeResultadoParaBenchmark;

typedef struct RegistroDeReferenciaBenchmark {
    char nomeDoArquivoDeOrigem[TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO];
    InteiroPositivoDe16Bits identificadorDaInstancia;
    QuantidadeDeTarefas quantidadeDeTarefas;
    FatorH fatorH;
    Custo custoMelhorConhecido;
    char tipoMelhorConhecido[TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA];
    Custo custoAutores;
    char tipoAutores[TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA];
} RegistroDeReferenciaBenchmark;

static void controllerBenchmarkLimparReferenciaComoDesconhecida(RegistroDeReferenciaBenchmark *referencia) {
    if(referencia == NULL) {
        return;
    }

    (*referencia).nomeDoArquivoDeOrigem[0] = '\0';
    (*referencia).identificadorDaInstancia = 0;
    (*referencia).quantidadeDeTarefas = 0;
    (*referencia).fatorH = 0;
    (*referencia).custoMelhorConhecido = 0;
    (*referencia).custoAutores = 0;
    copiarTexto((*referencia).tipoMelhorConhecido,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA,TIPO_REFERENCIA_DESCONHECIDO);
    copiarTexto((*referencia).tipoAutores,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA,TIPO_REFERENCIA_DESCONHECIDO);
}

static Boolean controllerBenchmarkConverterHDecimalParaFatorH(double hDecimal,FatorH *fatorH) {
    if(fatorH == NULL) {
        return FALSO;
    }

    if(hDecimal > 0.19 && hDecimal < 0.21) {
        (*fatorH) = FATOR_H_02;
        return VERDADEIRO;
    }

    if(hDecimal > 0.39 && hDecimal < 0.41) {
        (*fatorH) = FATOR_H_04;
        return VERDADEIRO;
    }

    if(hDecimal > 0.59 && hDecimal < 0.61) {
        (*fatorH) = FATOR_H_06;
        return VERDADEIRO;
    }

    if(hDecimal > 0.79 && hDecimal < 0.81) {
        (*fatorH) = FATOR_H_08;
        return VERDADEIRO;
    }

    return FALSO;
}

static Boolean controllerBenchmarkLerRegistroDeResultado(const char *linha,RegistroDeResultadoParaBenchmark *registro) {
    char nomeDoArquivo[TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO];
    unsigned int identificadorDaExecucao;
    unsigned int identificadorDaInstancia;
    unsigned int quantidadeDeTarefas;
    unsigned int identificadorDaHeuristica;
    unsigned long somaDosTemposDeProcessamento;
    unsigned long dataDeEntregaComum;
    unsigned long long custoHeuristica;
    double hDecimal;
    double tempoSegundos;
    double tempoMilissegundos;
    double tempoPorTarefaMilissegundos;
    FatorH fatorH;

    if(textoEhNuloOuVazio(linha) == VERDADEIRO) {
        return FALSO;
    }

    if(registro == NULL) {
        return FALSO;
    }

    if(sscanf(linha,"%u,%127[^,],%u,%u,%lf,%lu,%lu,%u,%llu,%lf,%lf,%lf",&identificadorDaExecucao,nomeDoArquivo,&identificadorDaInstancia,&quantidadeDeTarefas,&hDecimal,&somaDosTemposDeProcessamento,&dataDeEntregaComum,&identificadorDaHeuristica,&custoHeuristica,&tempoSegundos,&tempoMilissegundos,&tempoPorTarefaMilissegundos) != 12) {
        return FALSO;
    }

    if(controllerBenchmarkConverterHDecimalParaFatorH(hDecimal,&fatorH) == FALSO) {
        return FALSO;
    }

    if(copiarTexto((*registro).nomeDoArquivoDeOrigem,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO,nomeDoArquivo) == FALSO) {
        return FALSO;
    }

    (*registro).identificadorDaExecucao = (IdentificadorDeExecucao) identificadorDaExecucao;
    (*registro).identificadorDaInstancia = (InteiroPositivoDe16Bits) identificadorDaInstancia;
    (*registro).quantidadeDeTarefas = (QuantidadeDeTarefas) quantidadeDeTarefas;
    (*registro).fatorH = fatorH;
    (*registro).identificadorDaHeuristica = (IdentificadorDeHeuristica) identificadorDaHeuristica;
    (*registro).custoHeuristica = (Custo) custoHeuristica;

    return VERDADEIRO;
}

static Boolean controllerBenchmarkLerRegistroDeReferencia(const char *linha,RegistroDeReferenciaBenchmark *registro) {
    char nomeDoArquivo[TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO];
    char tipoMelhorConhecido[TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA];
    char tipoAutores[TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA];
    unsigned int identificadorDaInstancia;
    unsigned int quantidadeDeTarefas;
    unsigned long long custoMelhorConhecido;
    unsigned long long custoAutores;
    double hDecimal;
    FatorH fatorH;

    if(textoEhNuloOuVazio(linha) == VERDADEIRO) {
        return FALSO;
    }

    if(registro == NULL) {
        return FALSO;
    }

    if(sscanf(linha,"%127[^,],%u,%u,%lf,%llu,%63[^,],%llu,%63[^\n]",nomeDoArquivo,&identificadorDaInstancia,&quantidadeDeTarefas,&hDecimal,&custoMelhorConhecido,tipoMelhorConhecido,&custoAutores,tipoAutores) != 8) {
        return FALSO;
    }

    removerQuebraDeLinha(tipoMelhorConhecido);
    removerQuebraDeLinha(tipoAutores);

    if(controllerBenchmarkConverterHDecimalParaFatorH(hDecimal,&fatorH) == FALSO) {
        return FALSO;
    }

    if(copiarTexto((*registro).nomeDoArquivoDeOrigem,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO,nomeDoArquivo) == FALSO) {
        return FALSO;
    }

    if(copiarTexto((*registro).tipoMelhorConhecido,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA,tipoMelhorConhecido) == FALSO) {
        return FALSO;
    }

    if(copiarTexto((*registro).tipoAutores,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_TIPO_DE_REFERENCIA,tipoAutores) == FALSO) {
        return FALSO;
    }

    (*registro).identificadorDaInstancia = (InteiroPositivoDe16Bits) identificadorDaInstancia;
    (*registro).quantidadeDeTarefas = (QuantidadeDeTarefas) quantidadeDeTarefas;
    (*registro).fatorH = fatorH;
    (*registro).custoMelhorConhecido = (Custo) custoMelhorConhecido;
    (*registro).custoAutores = (Custo) custoAutores;

    return VERDADEIRO;
}

static Boolean controllerBenchmarkRegistrosSaoCorrespondentes(const RegistroDeResultadoParaBenchmark *resultado,const RegistroDeReferenciaBenchmark *referencia) {
    if(resultado == NULL) {
        return FALSO;
    }

    if(referencia == NULL) {
        return FALSO;
    }

    if(strcmp((*resultado).nomeDoArquivoDeOrigem,(*referencia).nomeDoArquivoDeOrigem) != 0) {
        return FALSO;
    }

    if((*resultado).identificadorDaInstancia != (*referencia).identificadorDaInstancia) {
        return FALSO;
    }

    if((*resultado).quantidadeDeTarefas != (*referencia).quantidadeDeTarefas) {
        return FALSO;
    }

    if((*resultado).fatorH != (*referencia).fatorH) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean controllerBenchmarkBuscarReferencia(const char *caminhoArquivoReferenciasBenchmark,const RegistroDeResultadoParaBenchmark *resultado,RegistroDeReferenciaBenchmark *referenciaEncontrada) {
    FILE *arquivoReferencias;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];
    RegistroDeReferenciaBenchmark referencia;
    Boolean cabecalhoJaFoiIgnorado;

    if(textoEhNuloOuVazio(caminhoArquivoReferenciasBenchmark) == VERDADEIRO) {
        return FALSO;
    }

    if(resultado == NULL) {
        return FALSO;
    }

    if(referenciaEncontrada == NULL) {
        return FALSO;
    }

    arquivoReferencias = gerenciadorDeArquivosAbrirArquivoParaLeitura(caminhoArquivoReferenciasBenchmark);

    if(arquivoReferencias == NULL) {
        return FALSO;
    }

    cabecalhoJaFoiIgnorado = FALSO;

    while(gerenciadorDeArquivosLerLinha(arquivoReferencias,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA_CSV) == VERDADEIRO) {
        if(cabecalhoJaFoiIgnorado == FALSO) {
            cabecalhoJaFoiIgnorado = VERDADEIRO;
            continue;
        }

        if(controllerBenchmarkLerRegistroDeReferencia(linha,&referencia) == FALSO) {
            continue;
        }

        if(controllerBenchmarkRegistrosSaoCorrespondentes(resultado,&referencia) == VERDADEIRO) {
            (*referenciaEncontrada) = referencia;
            gerenciadorDeArquivosFecharArquivo(arquivoReferencias);
            return VERDADEIRO;
        }
    }

    gerenciadorDeArquivosFecharArquivo(arquivoReferencias);

    return FALSO;
}

static Boolean controllerBenchmarkFormatarGap(Custo custoHeuristica,Custo custoReferencia,char *textoDoGap,InteiroPositivoDe32Bits tamanhoDoTextoDoGap) {
    double gapPercentual;
    int quantidadeDeCaracteresEscritos;

    if(textoDoGap == NULL) {
        return FALSO;
    }

    if(tamanhoDoTextoDoGap == 0) {
        return FALSO;
    }

    if(custoReferencia == 0) {
        return copiarTexto(textoDoGap,tamanhoDoTextoDoGap,TEXTO_VALOR_NA);
    }

    gapPercentual = (((double) custoHeuristica - (double) custoReferencia) / (double) custoReferencia) * 100.0;
    quantidadeDeCaracteresEscritos = snprintf(textoDoGap,tamanhoDoTextoDoGap,"%.9f",gapPercentual);

    if(quantidadeDeCaracteresEscritos < 0) {
        return FALSO;
    }

    if((InteiroPositivoDe32Bits) quantidadeDeCaracteresEscritos >= tamanhoDoTextoDoGap) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean controllerBenchmarkGravarCabecalho(FILE *arquivoComparacao) {
    if(arquivoComparacao == NULL) {
        return FALSO;
    }

    return gerenciadorDeArquivosGravarLinha(arquivoComparacao,"idExecucao,arquivo,idInstancia,n,h,idHeuristica,custoHeuristica,custoMelhorConhecido,tipoMelhorConhecido,gapMelhorSolucaoConhecidaPercentual,custoAutores,tipoAutores,gapSolucaoAutoresPercentual\n");
}

static Boolean controllerBenchmarkGravarLinhaDeComparacao(FILE *arquivoComparacao,const RegistroDeResultadoParaBenchmark *resultado,const RegistroDeReferenciaBenchmark *referencia) {
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];
    char gapMelhorConhecido[TAMANHO_MAXIMO_DE_VALOR_TEXTUAL];
    char gapAutores[TAMANHO_MAXIMO_DE_VALOR_TEXTUAL];
    double hDecimal;
    int quantidadeDeCaracteresEscritos;

    if(arquivoComparacao == NULL) {
        return FALSO;
    }

    if(resultado == NULL) {
        return FALSO;
    }

    if(referencia == NULL) {
        return FALSO;
    }

    if(controllerBenchmarkFormatarGap((*resultado).custoHeuristica,(*referencia).custoMelhorConhecido,gapMelhorConhecido,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_VALOR_TEXTUAL) == FALSO) {
        return FALSO;
    }

    if(controllerBenchmarkFormatarGap((*resultado).custoHeuristica,(*referencia).custoAutores,gapAutores,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_VALOR_TEXTUAL) == FALSO) {
        return FALSO;
    }

    hDecimal = fatorHParaNumeroDecimal((*resultado).fatorH);
    quantidadeDeCaracteresEscritos = snprintf(linha,TAMANHO_MAXIMO_DE_LINHA_CSV,"%u,%s,%u,%u,%.1f,%u,%llu,%llu,%s,%s,%llu,%s,%s\n",(unsigned int) (*resultado).identificadorDaExecucao,(*resultado).nomeDoArquivoDeOrigem,(unsigned int) (*resultado).identificadorDaInstancia,(unsigned int) (*resultado).quantidadeDeTarefas,hDecimal,(unsigned int) (*resultado).identificadorDaHeuristica,(unsigned long long) (*resultado).custoHeuristica,(unsigned long long) (*referencia).custoMelhorConhecido,(*referencia).tipoMelhorConhecido,gapMelhorConhecido,(unsigned long long) (*referencia).custoAutores,(*referencia).tipoAutores,gapAutores);

    if(quantidadeDeCaracteresEscritos < 0) {
        return FALSO;
    }

    if(quantidadeDeCaracteresEscritos >= TAMANHO_MAXIMO_DE_LINHA_CSV) {
        return FALSO;
    }

    return gerenciadorDeArquivosGravarLinha(arquivoComparacao,linha);
}

Boolean controllerBenchmarkGerarComparacao(const char *caminhoArquivoResultadosDeExecucoes,const char *caminhoArquivoReferenciasBenchmark,const char *caminhoArquivoComparacaoBenchmark) {
    FILE *arquivoResultados;
    FILE *arquivoComparacao;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];
    RegistroDeResultadoParaBenchmark resultado;
    RegistroDeReferenciaBenchmark referencia;
    Boolean cabecalhoJaFoiIgnorado;

    if(textoEhNuloOuVazio(caminhoArquivoResultadosDeExecucoes) == VERDADEIRO) {
        return FALSO;
    }

    if(textoEhNuloOuVazio(caminhoArquivoReferenciasBenchmark) == VERDADEIRO) {
        return FALSO;
    }

    if(textoEhNuloOuVazio(caminhoArquivoComparacaoBenchmark) == VERDADEIRO) {
        return FALSO;
    }

    arquivoResultados = gerenciadorDeArquivosAbrirArquivoParaLeitura(caminhoArquivoResultadosDeExecucoes);

    if(arquivoResultados == NULL) {
        return FALSO;
    }

    arquivoComparacao = gerenciadorDeArquivosAbrirArquivoParaEscrita(caminhoArquivoComparacaoBenchmark);

    if(arquivoComparacao == NULL) {
        gerenciadorDeArquivosFecharArquivo(arquivoResultados);
        return FALSO;
    }

    if(controllerBenchmarkGravarCabecalho(arquivoComparacao) == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivoComparacao);
        gerenciadorDeArquivosFecharArquivo(arquivoResultados);
        return FALSO;
    }

    cabecalhoJaFoiIgnorado = FALSO;

    while(gerenciadorDeArquivosLerLinha(arquivoResultados,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA_CSV) == VERDADEIRO) {
        if(cabecalhoJaFoiIgnorado == FALSO) {
            cabecalhoJaFoiIgnorado = VERDADEIRO;
            continue;
        }

        if(controllerBenchmarkLerRegistroDeResultado(linha,&resultado) == FALSO) {
            gerenciadorDeArquivosFecharArquivo(arquivoComparacao);
            gerenciadorDeArquivosFecharArquivo(arquivoResultados);
            return FALSO;
        }

        controllerBenchmarkLimparReferenciaComoDesconhecida(&referencia);
        controllerBenchmarkBuscarReferencia(caminhoArquivoReferenciasBenchmark,&resultado,&referencia);

        if(controllerBenchmarkGravarLinhaDeComparacao(arquivoComparacao,&resultado,&referencia) == FALSO) {
            gerenciadorDeArquivosFecharArquivo(arquivoComparacao);
            gerenciadorDeArquivosFecharArquivo(arquivoResultados);
            return FALSO;
        }
    }

    gerenciadorDeArquivosFecharArquivo(arquivoComparacao);
    gerenciadorDeArquivosFecharArquivo(arquivoResultados);

    return VERDADEIRO;
}
