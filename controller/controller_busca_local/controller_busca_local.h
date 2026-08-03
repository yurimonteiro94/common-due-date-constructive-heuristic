#ifndef CONTROLLER_BUSCA_LOCAL_H
#define CONTROLLER_BUSCA_LOCAL_H

#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

#define RAIO_REINSERCAO_LIMITADA_PADRAO 12

typedef struct ResultadoBuscaLocal {
    Custo custoInicial;
    Custo custoFinal;
    InteiroPositivoDe32Bits quantidadeDeIteracoes;
    InteiroPositivoDe32Bits quantidadeDeVizinhosAvaliados;
} ResultadoBuscaLocal;

ResultadoBuscaLocal criarResultadoBuscaLocalVazio(void);
Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal);
Boolean controllerBuscaLocalMelhorarSolucaoPorReinsercaoLimitada(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao);

#endif