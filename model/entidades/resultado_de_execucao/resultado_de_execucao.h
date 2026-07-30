#ifndef RESULTADO_DE_EXECUCAO_H
#define RESULTADO_DE_EXECUCAO_H

#include "../../../services/constantes/constantes.h"

typedef struct ResultadoDeExecucao {
    IdentificadorDeExecucao identificadorDaExecucao;
    char nomeDoArquivoDeOrigem[TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO];
    IdentificadorDeHeuristica identificadorDaHeuristica;
    InteiroPositivoDe16Bits identificadorDaInstancia;
    QuantidadeDeTarefas quantidadeDeTarefas;
    FatorH fatorH;
    SomaDosTemposDeProcessamento somaDosTemposDeProcessamento;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custo;
    TempoComputacionalEmSegundos tempoComputacionalEmSegundos;
    TempoComputacionalEmMilissegundos tempoComputacionalEmMilissegundos;
} ResultadoDeExecucao;

ResultadoDeExecucao criarResultadoDeExecucaoVazio(void);
Boolean inicializarResultadoDeExecucao(ResultadoDeExecucao *resultadoDeExecucao,IdentificadorDeExecucao identificadorDaExecucao,const char *nomeDoArquivoDeOrigem,IdentificadorDeHeuristica identificadorDaHeuristica,InteiroPositivoDe16Bits identificadorDaInstancia,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,SomaDosTemposDeProcessamento somaDosTemposDeProcessamento,DataDeEntregaComum dataDeEntregaComum,Custo custo,TempoComputacionalEmSegundos tempoComputacionalEmSegundos);
Boolean resultadoDeExecucaoEhValido(const ResultadoDeExecucao *resultadoDeExecucao);
void limparResultadoDeExecucao(ResultadoDeExecucao *resultadoDeExecucao);

#endif
