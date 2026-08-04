#ifndef CONTROLLER_BUSCA_LOCAL_REINSERCAO_ADAPTATIVA_H
#define CONTROLLER_BUSCA_LOCAL_REINSERCAO_ADAPTATIVA_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

#define LIMITE_REINSERCAO_GLOBAL 200
#define LIMITE_REINSERCAO_REPRESENTATIVA 500
#define QUANTIDADE_ANCORAS_REINSERCAO_REPRESENTATIVA 32
#define RAIO_LOCAL_REINSERCAO_REPRESENTATIVA 4
#define RAIO_FRONTEIRA_REINSERCAO_REPRESENTATIVA 8
#define RAIO_LOCAL_REINSERCAO_GRANDE 6

Boolean controllerBuscaLocalReinsercaoAdaptativaMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal);

#endif