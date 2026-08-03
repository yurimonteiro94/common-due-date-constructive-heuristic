#ifndef CONTROLLER_BUSCA_LOCAL_H
#define CONTROLLER_BUSCA_LOCAL_H

#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

#define RAIO_REINSERCAO_LIMITADA_PADRAO 12
#define RAIO_TROCA_LIMITADA_PADRAO 12

typedef struct ResultadoBuscaLocal {
    Custo custoInicial;
    Custo custoFinal;
    InteiroPositivoDe32Bits quantidadeDeIteracoes;
    InteiroPositivoDe32Bits quantidadeDeVizinhosAvaliados;
    InteiroPositivoDe32Bits quantidadeDeVizinhosPorReinsercao;
    InteiroPositivoDe32Bits quantidadeDeVizinhosPorTroca;
    InteiroPositivoDe32Bits quantidadeDeMelhoriasPorReinsercao;
    InteiroPositivoDe32Bits quantidadeDeMelhoriasPorTroca;
} ResultadoBuscaLocal;

ResultadoBuscaLocal criarResultadoBuscaLocalVazio(void);
Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal);
Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercaoLimitada(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao);
Boolean controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca);

#endif