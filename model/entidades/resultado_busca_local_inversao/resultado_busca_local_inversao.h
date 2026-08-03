#ifndef RESULTADO_BUSCA_LOCAL_INVERSAO_H
#define RESULTADO_BUSCA_LOCAL_INVERSAO_H

#include "../../../services/constantes/constantes.h"

typedef struct ResultadoBuscaLocalInversao {
    Custo custoInicial;
    Custo custoAposBuscaHibrida;
    Custo custoFinal;
    InteiroPositivoDe32Bits quantidadeDeCiclos;
    InteiroPositivoDe32Bits quantidadeDeChamadasDaBuscaComposta;
    InteiroPositivoDe32Bits quantidadeDeIteracoesDeInversao;
    InteiroPositivoDe64Bits quantidadeDeVizinhosPorInversao;
    InteiroPositivoDe32Bits quantidadeDeInversoesAceitas;
    InteiroPositivoDe64Bits quantidadeDeVizinhosDaBuscaComposta;
    InteiroPositivoDe32Bits quantidadeDeMelhoriasPorReinsercao;
    InteiroPositivoDe32Bits quantidadeDeMelhoriasPorTroca;
} ResultadoBuscaLocalInversao;

ResultadoBuscaLocalInversao criarResultadoBuscaLocalInversaoVazio(void);

#endif