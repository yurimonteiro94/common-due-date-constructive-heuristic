#ifndef GERENCIADOR_DE_CUSTOS_H
#define GERENCIADOR_DE_CUSTOS_H

#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

Boolean gerenciadorDeCustosCalcularCustoDaSolucaoComInstanteInicial(const Instancia *instancia,const Solucao *solucao,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits instanteInicial,Custo *custo);
Boolean gerenciadorDeCustosEncontrarMelhorInstanteInicialDaSolucao(const Instancia *instancia,const Solucao *solucao,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *melhorInstanteInicial);
Boolean gerenciadorDeCustosCalcularCustoDaSolucao(const Instancia *instancia,const Solucao *solucao,DataDeEntregaComum dataDeEntregaComum,Custo *custo);

#endif
