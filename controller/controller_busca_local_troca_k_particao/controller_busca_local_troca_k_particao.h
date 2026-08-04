#ifndef CONTROLLER_BUSCA_LOCAL_TROCA_K_PARTICAO_H
#define CONTROLLER_BUSCA_LOCAL_TROCA_K_PARTICAO_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

#define LIMITE_CANDIDATOS_DUAS_TROCAS_100 100
#define LIMITE_CANDIDATOS_DUAS_TROCAS_200 200
#define LIMITE_CANDIDATOS_DUAS_TROCAS_500 64
#define LIMITE_CANDIDATOS_DUAS_TROCAS_1000 48

#define LIMITE_CANDIDATOS_TRES_TROCAS_20 20
#define LIMITE_CANDIDATOS_TRES_TROCAS_50 15
#define LIMITE_CANDIDATOS_TRES_TROCAS_100 12
#define LIMITE_CANDIDATOS_TRES_TROCAS_200 10
#define LIMITE_CANDIDATOS_TRES_TROCAS_500 8
#define LIMITE_CANDIDATOS_TRES_TROCAS_1000 6

Boolean controllerBuscaLocalTrocaKParticaoMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal);

#endif