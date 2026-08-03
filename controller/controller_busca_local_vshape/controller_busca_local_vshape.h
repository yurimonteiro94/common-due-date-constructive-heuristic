#ifndef CONTROLLER_BUSCA_LOCAL_VSHAPE_H
#define CONTROLLER_BUSCA_LOCAL_VSHAPE_H

#include "../controller_busca_local/controller_busca_local.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/constantes/constantes.h"

typedef struct ResultadoBuscaLocalVShape {
    Custo custoInicial;
    Custo custoAposPrimeiraBuscaComposta;
    Custo custoFinal;
    InteiroPositivoDe32Bits quantidadeDeCiclos;
    InteiroPositivoDe32Bits quantidadeDeChamadasDaBuscaComposta;
    InteiroPositivoDe32Bits quantidadeDeVizinhosDaBuscaComposta;
    InteiroPositivoDe32Bits quantidadeDeVizinhosDeOrdenacao;
    InteiroPositivoDe32Bits quantidadeDeVizinhosDeFronteira;
    InteiroPositivoDe32Bits quantidadeDeMelhoriasDeOrdenacao;
    InteiroPositivoDe32Bits quantidadeDeMelhoriasDeFronteira;
} ResultadoBuscaLocalVShape;

ResultadoBuscaLocalVShape criarResultadoBuscaLocalVShapeVazio(void);
Boolean controllerBuscaLocalVShapeMelhorarSolucao(const Instancia *instancia,FatorH fatorH,const Solucao *solucaoInicial,Solucao *solucaoFinal,ResultadoBuscaLocalVShape *resultado,QuantidadeDeTarefas raioDeReinsercao,QuantidadeDeTarefas raioDeTroca);

#endif