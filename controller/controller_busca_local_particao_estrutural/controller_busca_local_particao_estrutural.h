#ifndef CONTROLLER_BUSCA_LOCAL_PARTICAO_ESTRUTURAL_H
#define CONTROLLER_BUSCA_LOCAL_PARTICAO_ESTRUTURAL_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

#define LIMITE_CANDIDATOS_INTERCAMBIO_PARTICAO_100 100
#define LIMITE_CANDIDATOS_INTERCAMBIO_PARTICAO_200 64
#define LIMITE_CANDIDATOS_INTERCAMBIO_PARTICAO_500 24
#define LIMITE_CANDIDATOS_INTERCAMBIO_PARTICAO_1000 16

Boolean controllerBuscaLocalParticaoEstruturalMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal);

#endif
