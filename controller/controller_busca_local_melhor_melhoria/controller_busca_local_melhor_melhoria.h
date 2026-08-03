#ifndef CONTROLLER_BUSCA_LOCAL_MELHOR_MELHORIA_H
#define CONTROLLER_BUSCA_LOCAL_MELHOR_MELHORIA_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

Boolean controllerBuscaLocalMelhorarSolucaoComMelhorMelhoria(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocal *resultadoBuscaLocal,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca);

#endif