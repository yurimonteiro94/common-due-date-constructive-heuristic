#ifndef RESULTADO_BUSCA_LOCAL_HIBRIDA_H
#define RESULTADO_BUSCA_LOCAL_HIBRIDA_H

#include "../../../services/constantes/constantes.h"

#define TRAJETORIA_HIBRIDA_NENHUMA 0
#define TRAJETORIA_HIBRIDA_PRIMEIRA_MELHOR_PRIMEIRA 1
#define TRAJETORIA_HIBRIDA_MELHOR_PRIMEIRA_MELHOR 2

typedef struct ResultadoBuscaLocalHibrida {
    Custo custoInicial;
    Custo custoPrimeiraMelhoriaIsolada;
    Custo custoMelhorMelhoriaIsolada;
    Custo custoTrajetoriaPrimeiraMelhorPrimeira;
    Custo custoTrajetoriaMelhorPrimeiraMelhor;
    Custo custoFinal;
    InteiroPositivoDe8Bits trajetoriaSelecionada;
    InteiroPositivoDe32Bits quantidadeDeChamadasDeBuscaLocal;
    InteiroPositivoDe64Bits quantidadeTotalDeVizinhosAvaliados;
    InteiroPositivoDe32Bits quantidadeTotalDeIteracoes;
    InteiroPositivoDe32Bits quantidadeTotalDeMelhoriasPorReinsercao;
    InteiroPositivoDe32Bits quantidadeTotalDeMelhoriasPorTroca;
} ResultadoBuscaLocalHibrida;

ResultadoBuscaLocalHibrida criarResultadoBuscaLocalHibridaVazio(void);

#endif