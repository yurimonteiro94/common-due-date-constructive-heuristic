#ifndef EXPERIMENTO_H
#define EXPERIMENTO_H

#include "../../../services/constantes/constantes.h"
#include "../heuristica/heuristica.h"

typedef struct Experimento {
    IdentificadorDeExperimento identificadorDoExperimento;
    char nomeDoExperimento[TAMANHO_MAXIMO_DE_NOME_DE_EXPERIMENTO];
    char descricaoDoExperimento[TAMANHO_MAXIMO_DE_DESCRICAO_DE_EXPERIMENTO];

    Heuristica heuristica;

    InteiroPositivoDe8Bits quantidadeDeArquivosDeInstancia;
    char nomesDosArquivosDeInstancia[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO];

    InteiroPositivoDe8Bits quantidadeDeValoresDeH;
    FatorH fatoresH[QUANTIDADE_DE_VALORES_DE_H];
} Experimento;

Experimento criarExperimentoVazio(void);
Boolean inicializarExperimento(Experimento *experimento,IdentificadorDeExperimento identificadorDoExperimento,const char *nomeDoExperimento,const char *descricaoDoExperimento,Heuristica heuristica);
Boolean experimentoAdicionarArquivoDeInstancia(Experimento *experimento,InteiroPositivoDe8Bits indiceDoArquivo,const char *nomeDoArquivoDeInstancia);
Boolean experimentoAdicionarFatorH(Experimento *experimento,InteiroPositivoDe8Bits indiceDoFatorH,FatorH fatorH);
Experimento criarExperimentoPadrao(void);
Boolean experimentoEhValido(const Experimento *experimento);
void limparExperimento(Experimento *experimento);

#endif
