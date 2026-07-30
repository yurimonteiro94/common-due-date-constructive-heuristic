#include "resultado_dao.h"

#include "../../../services/ferramentas/ferramentas.h"
#include "../../../services/gerenciador_de_arquivos/gerenciador_de_arquivos.h"

#include <stddef.h>
#include <stdio.h>

static Boolean resultadoDaoCaminhoEhValido(const char *caminhoDoArquivo) {
    if(caminhoDoArquivo == NULL) {
        return FALSO;
    }

    else if(caminhoDoArquivo[0] == '\0') {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean resultadoDaoFatorHEhValido(FatorH fatorH) {
    if(fatorH == FATOR_H_02) {
        return VERDADEIRO;
    }

    else if(fatorH == FATOR_H_04) {
        return VERDADEIRO;
    }

    else if(fatorH == FATOR_H_06) {
        return VERDADEIRO;
    }

    else if(fatorH == FATOR_H_08) {
        return VERDADEIRO;
    }

    return FALSO;
}

Boolean resultadoDaoCriarArquivoDeResultadosDeExecucoes(const char *caminhoDoArquivo) {
    FILE *arquivo;

    if(resultadoDaoCaminhoEhValido(caminhoDoArquivo) == FALSO) {
        return FALSO;
    }

    arquivo = gerenciadorDeArquivosAbrirArquivoParaEscrita(caminhoDoArquivo);

    if(arquivo == NULL) {
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(
        arquivo,
        "idExecucao,arquivo,idInstancia,n,h,somaP,d,idHeuristica,custo,tempoSegundos,tempoMilissegundos,tempoPorTarefaMilissegundos\n"
    ) == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return FALSO;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return VERDADEIRO;
}

Boolean resultadoDaoAdicionarResultadoDeExecucao(const char *caminhoDoArquivo,const ResultadoDeExecucao *resultadoDeExecucao) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];
    double hDecimal;
    double tempoPorTarefaEmMilissegundos;
    int quantidadeDeCaracteresEscritos;

    if(resultadoDaoCaminhoEhValido(caminhoDoArquivo) == FALSO) {
        return FALSO;
    }

    else if(resultadoDeExecucaoEhValido(resultadoDeExecucao) == FALSO) {
        return FALSO;
    }

    hDecimal = fatorHParaNumeroDecimal((*resultadoDeExecucao).fatorH);

    if(hDecimal == 0.0) {
        return FALSO;
    }

    tempoPorTarefaEmMilissegundos = (*resultadoDeExecucao).tempoComputacionalEmMilissegundos / (double) (*resultadoDeExecucao).quantidadeDeTarefas;

    quantidadeDeCaracteresEscritos = snprintf(
        linha,
        TAMANHO_MAXIMO_DE_LINHA_CSV,
        "%u,%s,%u,%u,%.1f,%lu,%lu,%u,%llu,%.9f,%.9f,%.9f\n",
        (unsigned int) (*resultadoDeExecucao).identificadorDaExecucao,
        (*resultadoDeExecucao).nomeDoArquivoDeOrigem,
        (unsigned int) (*resultadoDeExecucao).identificadorDaInstancia,
        (unsigned int) (*resultadoDeExecucao).quantidadeDeTarefas,
        hDecimal,
        (unsigned long) (*resultadoDeExecucao).somaDosTemposDeProcessamento,
        (unsigned long) (*resultadoDeExecucao).dataDeEntregaComum,
        (unsigned int) (*resultadoDeExecucao).identificadorDaHeuristica,
        (unsigned long long) (*resultadoDeExecucao).custo,
        (*resultadoDeExecucao).tempoComputacionalEmSegundos,
        (*resultadoDeExecucao).tempoComputacionalEmMilissegundos,
        tempoPorTarefaEmMilissegundos
    );

    if(quantidadeDeCaracteresEscritos < 0) {
        return FALSO;
    }

    else if(quantidadeDeCaracteresEscritos >= TAMANHO_MAXIMO_DE_LINHA_CSV) {
        return FALSO;
    }

    arquivo = gerenciadorDeArquivosAbrirArquivoParaAcrescimo(caminhoDoArquivo);

    if(arquivo == NULL) {
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,linha) == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return FALSO;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return VERDADEIRO;
}

Boolean resultadoDaoCriarArquivoDeMediasPorNH(const char *caminhoDoArquivo) {
    FILE *arquivo;

    if(resultadoDaoCaminhoEhValido(caminhoDoArquivo) == FALSO) {
        return FALSO;
    }

    arquivo = gerenciadorDeArquivosAbrirArquivoParaEscrita(caminhoDoArquivo);

    if(arquivo == NULL) {
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(
        arquivo,
        "n,h,quantidadeExecucoes,custoMedio,tempoMedioSegundos,tempoMedioMilissegundos,tempoMedioPorTarefaMilissegundos\n"
    ) == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return FALSO;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return VERDADEIRO;
}

Boolean resultadoDaoAdicionarMediaPorNH(const char *caminhoDoArquivo,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,InteiroPositivoDe16Bits quantidadeDeExecucoes,double custoMedio,double tempoMedioEmSegundos,double tempoMedioEmMilissegundos,double tempoMedioPorTarefaEmMilissegundos) {
    FILE *arquivo;
    char linha[TAMANHO_MAXIMO_DE_LINHA_CSV];
    double hDecimal;
    int quantidadeDeCaracteresEscritos;

    if(resultadoDaoCaminhoEhValido(caminhoDoArquivo) == FALSO) {
        return FALSO;
    }

    else if(quantidadeDeTarefas == 0) {
        return FALSO;
    }

    else if(resultadoDaoFatorHEhValido(fatorH) == FALSO) {
        return FALSO;
    }

    else if(quantidadeDeExecucoes == 0) {
        return FALSO;
    }

    else if(custoMedio < 0.0) {
        return FALSO;
    }

    else if(tempoMedioEmSegundos < 0.0) {
        return FALSO;
    }

    else if(tempoMedioEmMilissegundos < 0.0) {
        return FALSO;
    }

    else if(tempoMedioPorTarefaEmMilissegundos < 0.0) {
        return FALSO;
    }

    hDecimal = fatorHParaNumeroDecimal(fatorH);

    if(hDecimal == 0.0) {
        return FALSO;
    }

    quantidadeDeCaracteresEscritos = snprintf(
        linha,
        TAMANHO_MAXIMO_DE_LINHA_CSV,
        "%u,%.1f,%u,%.9f,%.9f,%.9f,%.9f\n",
        (unsigned int) quantidadeDeTarefas,
        hDecimal,
        (unsigned int) quantidadeDeExecucoes,
        custoMedio,
        tempoMedioEmSegundos,
        tempoMedioEmMilissegundos,
        tempoMedioPorTarefaEmMilissegundos
    );

    if(quantidadeDeCaracteresEscritos < 0) {
        return FALSO;
    }

    else if(quantidadeDeCaracteresEscritos >= TAMANHO_MAXIMO_DE_LINHA_CSV) {
        return FALSO;
    }

    arquivo = gerenciadorDeArquivosAbrirArquivoParaAcrescimo(caminhoDoArquivo);

    if(arquivo == NULL) {
        return FALSO;
    }

    if(gerenciadorDeArquivosGravarLinha(arquivo,linha) == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return FALSO;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return VERDADEIRO;
}
