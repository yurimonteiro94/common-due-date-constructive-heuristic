#ifndef CONTROLLER_BUSCA_LOCAL_INVERSAO_H
#define CONTROLLER_BUSCA_LOCAL_INVERSAO_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/resultado_busca_local_inversao/resultado_busca_local_inversao.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

Boolean controllerBuscaLocalInversaoMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalInversao *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca,QuantidadeDeTarefas raioDeInversao);

#endif