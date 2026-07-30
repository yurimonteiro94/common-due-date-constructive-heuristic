#ifndef RESULTADO_DAO_H
#define RESULTADO_DAO_H

#include "../../entidades/resultado_de_execucao/resultado_de_execucao.h"

Boolean resultadoDaoCriarArquivoDeResultadosDeExecucoes(const char *caminhoDoArquivo);
Boolean resultadoDaoAdicionarResultadoDeExecucao(const char *caminhoDoArquivo,const ResultadoDeExecucao *resultadoDeExecucao);

Boolean resultadoDaoCriarArquivoDeMediasPorNH(const char *caminhoDoArquivo);
Boolean resultadoDaoAdicionarMediaPorNH(const char *caminhoDoArquivo,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,InteiroPositivoDe16Bits quantidadeDeExecucoes,double custoMedio,double tempoMedioEmSegundos,double tempoMedioEmMilissegundos,double tempoMedioPorTarefaEmMilissegundos);

#endif
