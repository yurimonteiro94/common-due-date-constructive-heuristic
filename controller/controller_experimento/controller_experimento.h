#ifndef CONTROLLER_EXPERIMENTO_H
#define CONTROLLER_EXPERIMENTO_H

#include "../../services/constantes/constantes.h"
#include "../../model/entidades/experimento/experimento.h"

Boolean controllerExperimentoExecutarExperimento(const Experimento *experimento,const char *caminhoArquivoResultadosDeExecucoes,const char *caminhoArquivoMediasPorNH);

#endif
