#include "resultado_busca_local_hibrida.h"

ResultadoBuscaLocalHibrida criarResultadoBuscaLocalHibridaVazio(void) {
    ResultadoBuscaLocalHibrida resultado;

    resultado.custoInicial = 0;
    resultado.custoPrimeiraMelhoriaIsolada = 0;
    resultado.custoMelhorMelhoriaIsolada = 0;
    resultado.custoTrajetoriaPrimeiraMelhorPrimeira = 0;
    resultado.custoTrajetoriaMelhorPrimeiraMelhor = 0;
    resultado.custoFinal = 0;
    resultado.trajetoriaSelecionada = TRAJETORIA_HIBRIDA_NENHUMA;
    resultado.quantidadeDeChamadasDeBuscaLocal = 0;
    resultado.quantidadeTotalDeVizinhosAvaliados = 0;
    resultado.quantidadeTotalDeIteracoes = 0;
    resultado.quantidadeTotalDeMelhoriasPorReinsercao = 0;
    resultado.quantidadeTotalDeMelhoriasPorTroca = 0;

    return resultado;
}