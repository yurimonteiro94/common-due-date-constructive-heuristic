#include "experimento.h"

#include <stddef.h>
#include <stdio.h>

static Boolean fatorHEhValido(FatorH fatorH) {
    if(fatorH == FATOR_H_02) {
        return VERDADEIRO;
    }

    else if(fatorH == FATOR_H_04) {
        return VERDADEIRO;
    }

    else if(fatorH == FATOR_H_06) {
        return VERDADEIRO;
    }

    else if(fatorH == FATOR_H_08) {
        return VERDADEIRO;
    }

    return FALSO;
}

Experimento criarExperimentoVazio(void) {
    Experimento experimento;
    InteiroPositivoDe8Bits indice;

    experimento.identificadorDoExperimento = 0;
    experimento.nomeDoExperimento[0] = '\0';
    experimento.descricaoDoExperimento[0] = '\0';

    experimento.heuristica = criarHeuristicaVazia();

    experimento.quantidadeDeArquivosDeInstancia = 0;

    for(indice = 0; indice < QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA; indice++) {
        experimento.nomesDosArquivosDeInstancia[indice][0] = '\0';
    }

    experimento.quantidadeDeValoresDeH = 0;

    for(indice = 0; indice < QUANTIDADE_DE_VALORES_DE_H; indice++) {
        experimento.fatoresH[indice] = 0;
    }

    return experimento;
}

Boolean inicializarExperimento(Experimento *experimento,IdentificadorDeExperimento identificadorDoExperimento,const char *nomeDoExperimento,const char *descricaoDoExperimento,Heuristica heuristica) {
    if(experimento == NULL) {
        return FALSO;
    }

    else if(identificadorDoExperimento == 0) {
        return FALSO;
    }

    else if(nomeDoExperimento == NULL) {
        return FALSO;
    }

    else if(nomeDoExperimento[0] == '\0') {
        return FALSO;
    }

    else if(descricaoDoExperimento == NULL) {
        return FALSO;
    }

    else if(descricaoDoExperimento[0] == '\0') {
        return FALSO;
    }

    else if(heuristicaEhValida(&heuristica) == FALSO) {
        return FALSO;
    }

    (*experimento) = criarExperimentoVazio();

    (*experimento).identificadorDoExperimento = identificadorDoExperimento;

    snprintf(
        (*experimento).nomeDoExperimento,
        TAMANHO_MAXIMO_DE_NOME_DE_EXPERIMENTO,
        "%s",
        nomeDoExperimento
    );

    snprintf(
        (*experimento).descricaoDoExperimento,
        TAMANHO_MAXIMO_DE_DESCRICAO_DE_EXPERIMENTO,
        "%s",
        descricaoDoExperimento
    );

    (*experimento).heuristica = heuristica;

    return VERDADEIRO;
}

Boolean experimentoAdicionarArquivoDeInstancia(Experimento *experimento,InteiroPositivoDe8Bits indiceDoArquivo,const char *nomeDoArquivoDeInstancia) {
    if(experimento == NULL) {
        return FALSO;
    }

    else if(indiceDoArquivo >= QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA) {
        return FALSO;
    }

    else if(nomeDoArquivoDeInstancia == NULL) {
        return FALSO;
    }

    else if(nomeDoArquivoDeInstancia[0] == '\0') {
        return FALSO;
    }

    if((*experimento).nomesDosArquivosDeInstancia[indiceDoArquivo][0] == '\0') {
        (*experimento).quantidadeDeArquivosDeInstancia++;
    }

    snprintf(
        (*experimento).nomesDosArquivosDeInstancia[indiceDoArquivo],
        TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO,
        "%s",
        nomeDoArquivoDeInstancia
    );

    return VERDADEIRO;
}

Boolean experimentoAdicionarFatorH(Experimento *experimento,InteiroPositivoDe8Bits indiceDoFatorH,FatorH fatorH) {
    if(experimento == NULL) {
        return FALSO;
    }

    else if(indiceDoFatorH >= QUANTIDADE_DE_VALORES_DE_H) {
        return FALSO;
    }

    else if(fatorHEhValido(fatorH) == FALSO) {
        return FALSO;
    }

    if((*experimento).fatoresH[indiceDoFatorH] == 0) {
        (*experimento).quantidadeDeValoresDeH++;
    }

    (*experimento).fatoresH[indiceDoFatorH] = fatorH;

    return VERDADEIRO;
}

Experimento criarExperimentoPadrao(void) {
    Experimento experimento;
    Heuristica heuristica;

    experimento = criarExperimentoVazio();
    heuristica = criarHeuristicaConstrutivaFuzzy();

    inicializarExperimento(&experimento,(IdentificadorDeExperimento) IDENTIFICADOR_EXPERIMENTO_PADRAO,NOME_EXPERIMENTO_PADRAO,DESCRICAO_EXPERIMENTO_PADRAO,heuristica);

    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 0,NOME_ARQUIVO_SCH10);
    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 1,NOME_ARQUIVO_SCH20);
    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 2,NOME_ARQUIVO_SCH50);
    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 3,NOME_ARQUIVO_SCH100);
    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 4,NOME_ARQUIVO_SCH200);
    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 5,NOME_ARQUIVO_SCH500);
    experimentoAdicionarArquivoDeInstancia(&experimento,(InteiroPositivoDe8Bits) 6,NOME_ARQUIVO_SCH1000);

    experimentoAdicionarFatorH(&experimento,(InteiroPositivoDe8Bits) 0,(FatorH) FATOR_H_02);
    experimentoAdicionarFatorH(&experimento,(InteiroPositivoDe8Bits) 1,(FatorH) FATOR_H_04);
    experimentoAdicionarFatorH(&experimento,(InteiroPositivoDe8Bits) 2,(FatorH) FATOR_H_06);
    experimentoAdicionarFatorH(&experimento,(InteiroPositivoDe8Bits) 3,(FatorH) FATOR_H_08);

    return experimento;
}

Boolean experimentoEhValido(const Experimento *experimento) {
    InteiroPositivoDe8Bits indice;

    if(experimento == NULL) {
        return FALSO;
    }

    else if((*experimento).identificadorDoExperimento == 0) {
        return FALSO;
    }

    else if((*experimento).nomeDoExperimento[0] == '\0') {
        return FALSO;
    }

    else if((*experimento).descricaoDoExperimento[0] == '\0') {
        return FALSO;
    }

    else if(heuristicaEhValida(&(*experimento).heuristica) == FALSO) {
        return FALSO;
    }

    else if((*experimento).quantidadeDeArquivosDeInstancia != QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA) {
        return FALSO;
    }

    else if((*experimento).quantidadeDeValoresDeH != QUANTIDADE_DE_VALORES_DE_H) {
        return FALSO;
    }

    for(indice = 0; indice < QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA; indice++) {
        if((*experimento).nomesDosArquivosDeInstancia[indice][0] == '\0') {
            return FALSO;
        }
    }

    for(indice = 0; indice < QUANTIDADE_DE_VALORES_DE_H; indice++) {
        if(fatorHEhValido((*experimento).fatoresH[indice]) == FALSO) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

void limparExperimento(Experimento *experimento) {
    if(experimento == NULL) {
        return;
    }

    (*experimento) = criarExperimentoVazio();
}
