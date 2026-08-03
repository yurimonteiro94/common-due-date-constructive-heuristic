#include "resultado_busca_local_inversao.h"

ResultadoBuscaLocalInversao criarResultadoBuscaLocalInversaoVazio(void) {
    ResultadoBuscaLocalInversao resultado;

    resultado.custoInicial = 0;
    resultado.custoAposBuscaHibrida = 0;
    resultado.custoFinal = 0;
    resultado.quantidadeDeCiclos = 0;
    resultado.quantidadeDeChamadasDaBuscaComposta = 0;
    resultado.quantidadeDeIteracoesDeInversao = 0;
    resultado.quantidadeDeVizinhosPorInversao = 0;
    resultado.quantidadeDeInversoesAceitas = 0;
    resultado.quantidadeDeVizinhosDaBuscaComposta = 0;
    resultado.quantidadeDeMelhoriasPorReinsercao = 0;
    resultado.quantidadeDeMelhoriasPorTroca = 0;

    return resultado;
}