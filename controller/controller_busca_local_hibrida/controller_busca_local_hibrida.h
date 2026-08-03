#ifndef CONTROLLER_BUSCA_LOCAL_HIBRIDA_H
#define CONTROLLER_BUSCA_LOCAL_HIBRIDA_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/resultado_busca_local_hibrida/resultado_busca_local_hibrida.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

Boolean controllerBuscaLocalHibridaMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalHibrida *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca);

#endif