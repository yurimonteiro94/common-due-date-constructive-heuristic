#include "resultado_de_execucao.h"

#include <stddef.h>
#include <stdio.h>

static Boolean fatorHEhValido(FatorH fatorH) {
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

ResultadoDeExecucao criarResultadoDeExecucaoVazio(void) {
    ResultadoDeExecucao resultadoDeExecucao;

    resultadoDeExecucao.identificadorDaExecucao = 0;
    resultadoDeExecucao.nomeDoArquivoDeOrigem[0] = '\0';
    resultadoDeExecucao.identificadorDaHeuristica = 0;
    resultadoDeExecucao.identificadorDaInstancia = 0;
    resultadoDeExecucao.quantidadeDeTarefas = 0;
    resultadoDeExecucao.fatorH = 0;
    resultadoDeExecucao.somaDosTemposDeProcessamento = 0;
    resultadoDeExecucao.dataDeEntregaComum = 0;
    resultadoDeExecucao.custo = 0;
    resultadoDeExecucao.tempoComputacionalEmSegundos = 0.0;
    resultadoDeExecucao.tempoComputacionalEmMilissegundos = 0.0;

    return resultadoDeExecucao;
}

Boolean inicializarResultadoDeExecucao(ResultadoDeExecucao *resultadoDeExecucao,IdentificadorDeExecucao identificadorDaExecucao,const char *nomeDoArquivoDeOrigem,IdentificadorDeHeuristica identificadorDaHeuristica,InteiroPositivoDe16Bits identificadorDaInstancia,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,SomaDosTemposDeProcessamento somaDosTemposDeProcessamento,DataDeEntregaComum dataDeEntregaComum,Custo custo,TempoComputacionalEmSegundos tempoComputacionalEmSegundos) {
    if(resultadoDeExecucao == NULL) {
        return FALSO;
    }

    else if(identificadorDaExecucao == 0) {
        return FALSO;
    }

    else if(nomeDoArquivoDeOrigem == NULL) {
        return FALSO;
    }

    else if(nomeDoArquivoDeOrigem[0] == '\0') {
        return FALSO;
    }

    else if(identificadorDaHeuristica == 0) {
        return FALSO;
    }

    else if(identificadorDaInstancia == 0) {
        return FALSO;
    }

    else if(quantidadeDeTarefas == 0) {
        return FALSO;
    }

    else if(fatorHEhValido(fatorH) == FALSO) {
        return FALSO;
    }

    else if(somaDosTemposDeProcessamento == 0) {
        return FALSO;
    }

    else if(dataDeEntregaComum == 0) {
        return FALSO;
    }

    else if(tempoComputacionalEmSegundos < 0.0) {
        return FALSO;
    }

    (*resultadoDeExecucao) = criarResultadoDeExecucaoVazio();

    (*resultadoDeExecucao).identificadorDaExecucao = identificadorDaExecucao;

    snprintf(
        (*resultadoDeExecucao).nomeDoArquivoDeOrigem,
        TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO,
        "%s",
        nomeDoArquivoDeOrigem
    );

    (*resultadoDeExecucao).identificadorDaHeuristica = identificadorDaHeuristica;
    (*resultadoDeExecucao).identificadorDaInstancia = identificadorDaInstancia;
    (*resultadoDeExecucao).quantidadeDeTarefas = quantidadeDeTarefas;
    (*resultadoDeExecucao).fatorH = fatorH;
    (*resultadoDeExecucao).somaDosTemposDeProcessamento = somaDosTemposDeProcessamento;
    (*resultadoDeExecucao).dataDeEntregaComum = dataDeEntregaComum;
    (*resultadoDeExecucao).custo = custo;
    (*resultadoDeExecucao).tempoComputacionalEmSegundos = tempoComputacionalEmSegundos;
    (*resultadoDeExecucao).tempoComputacionalEmMilissegundos = tempoComputacionalEmSegundos * MILISSEGUNDOS_POR_SEGUNDO;

    return VERDADEIRO;
}

Boolean resultadoDeExecucaoEhValido(const ResultadoDeExecucao *resultadoDeExecucao) {
    if(resultadoDeExecucao == NULL) {
        return FALSO;
    }

    else if((*resultadoDeExecucao).identificadorDaExecucao == 0) {
        return FALSO;
    }

    else if((*resultadoDeExecucao).nomeDoArquivoDeOrigem[0] == '\0') {
        return FALSO;
    }

    else if((*resultadoDeExecucao).identificadorDaHeuristica == 0) {
        return FALSO;
    }

    else if((*resultadoDeExecucao).identificadorDaInstancia == 0) {
        return FALSO;
    }

    else if((*resultadoDeExecucao).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    else if(fatorHEhValido((*resultadoDeExecucao).fatorH) == FALSO) {
        return FALSO;
    }

    else if((*resultadoDeExecucao).somaDosTemposDeProcessamento == 0) {
        return FALSO;
    }

    else if((*resultadoDeExecucao).dataDeEntregaComum == 0) {
        return FALSO;
    }

    else if((*resultadoDeExecucao).tempoComputacionalEmSegundos < 0.0) {
        return FALSO;
    }

    else if((*resultadoDeExecucao).tempoComputacionalEmMilissegundos < 0.0) {
        return FALSO;
    }

    return VERDADEIRO;
}

void limparResultadoDeExecucao(ResultadoDeExecucao *resultadoDeExecucao) {
    if(resultadoDeExecucao == NULL) {
        return;
    }

    (*resultadoDeExecucao) = criarResultadoDeExecucaoVazio();
}
