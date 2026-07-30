#ifndef GERENCIADOR_FUZZY_H
#define GERENCIADOR_FUZZY_H

#include "../constantes/constantes.h"
#include "../../model/entidades/tarefa/tarefa.h"

Boolean gerenciadorFuzzyCalcularPertinenciaCrescente(double valor,double limiteInferior,double limiteSuperior,double *pertinencia);
Boolean gerenciadorFuzzyCalcularPertinenciaDecrescente(double valor,double limiteInferior,double limiteSuperior,double *pertinencia);
Boolean gerenciadorFuzzyCalcularPertinenciaTriangular(double valor,double limiteInferior,double limiteCentral,double limiteSuperior,double *pertinencia);
Boolean gerenciadorFuzzyCalcularPrioridadeAntesDaDataDeEntrega(const Tarefa *tarefa,DataDeEntregaComum dataDeEntregaComum,SomaDosTemposDeProcessamento tempoJaAlocadoAntesDaDataDeEntrega,double *prioridade);
Boolean gerenciadorFuzzyCalcularPrioridadeDepoisDaDataDeEntrega(const Tarefa *tarefa,double *prioridade);

#endif
