#ifndef CONTROLLER_BUSCA_LOCAL_SUBCUBOS_PARTICAO_H
#define CONTROLLER_BUSCA_LOCAL_SUBCUBOS_PARTICAO_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

#define TAMANHO_SUBCUBO_20 20
#define TAMANHO_SUBCUBO_50 15
#define TAMANHO_SUBCUBO_100 14
#define TAMANHO_SUBCUBO_200 13
#define TAMANHO_SUBCUBO_500 12
#define TAMANHO_SUBCUBO_1000 11

#define QUANTIDADE_PAINEIS_20 1
#define QUANTIDADE_PAINEIS_50 4
#define QUANTIDADE_PAINEIS_100 4
#define QUANTIDADE_PAINEIS_200 4
#define QUANTIDADE_PAINEIS_500 4
#define QUANTIDADE_PAINEIS_1000 4

typedef struct ResultadoBuscaLocalSubcubosParticao {
    ResultadoBuscaLocal resultadoBase;
    InteiroPositivoDe32Bits quantidadeDePaineisAvaliados;
    QuantidadeDeTarefas maiorCardinalidadeAceita;
    InteiroPositivoDe32Bits quantidadeDeMovimentosComQuatroOuMaisAceitos;
} ResultadoBuscaLocalSubcubosParticao;

QuantidadeDeTarefas controllerBuscaLocalSubcubosParticaoObterTamanhoDoSubcubo(QuantidadeDeTarefas quantidadeDeTarefas);
InteiroPositivoDe8Bits controllerBuscaLocalSubcubosParticaoObterQuantidadeDePaineis(QuantidadeDeTarefas quantidadeDeTarefas);
ResultadoBuscaLocalSubcubosParticao criarResultadoBuscaLocalSubcubosParticaoVazio(void);
Boolean controllerBuscaLocalSubcubosParticaoMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalSubcubosParticao *resultadoBuscaLocal);

#endif