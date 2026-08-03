#ifndef CONTROLLER_BUSCA_LOCAL_ESTRUTURAL_H
#define CONTROLLER_BUSCA_LOCAL_ESTRUTURAL_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

typedef struct ResultadoBuscaLocalEstrutural {
    Custo custoInicial;
    Custo custoAposPrimeiraBuscaComposta;
    Custo custoFinal;
    InteiroPositivoDe32Bits quantidadeDeCiclosEstruturais;
    InteiroPositivoDe32Bits quantidadeDeChamadasDaBuscaComposta;
    InteiroPositivoDe32Bits quantidadeDeVizinhosDaBuscaComposta;
    InteiroPositivoDe32Bits quantidadeDeVizinhosDeBlocoDois;
    InteiroPositivoDe32Bits quantidadeDeVizinhosDeBlocoTres;
    InteiroPositivoDe32Bits quantidadeDeMelhoriasDeBlocoDois;
    InteiroPositivoDe32Bits quantidadeDeMelhoriasDeBlocoTres;
} ResultadoBuscaLocalEstrutural;

ResultadoBuscaLocalEstrutural criarResultadoBuscaLocalEstruturalVazio(void);
Boolean controllerBuscaLocalEstruturalMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalEstrutural *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca,QuantidadeDeTarefas raioDeBlocos);

#endif