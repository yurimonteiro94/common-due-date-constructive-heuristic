#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../model/entidades/instancia/instancia.h"
#include "../model/entidades/solucao/solucao.h"
#include "../services/constantes/constantes.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>

#define QUANTIDADE_DE_ARQUIVOS_AMOSTRAIS 5
#define QUANTIDADE_DE_INSTANCIAS_AMOSTRAIS 3

static void testeAmostralImprimirCabecalho(void) {
    printf("Executando teste amostral da heuristica construtiva por insercao temporal...\n\n");
    printf("arquivo;instancia;n;h;d;custo;tempo_ms\n");
}

static double testeAmostralConverterFatorHParaDecimal(FatorH fatorH) {
    return ((double) fatorH) / ((double) FATOR_DE_ESCALA_H);
}

static Boolean testeAmostralExecutarInstancia(const char *nomeDoArquivo,InteiroPositivoDe16Bits identificadorDaInstancia,FatorH fatorH,Heuristica *heuristica,TempoComputacionalEmMilissegundos *tempoTotalEmMilissegundos,InteiroPositivoDe32Bits *quantidadeDeExecucoes,Custo *maiorCustoEncontrado) {
    Instancia instancia;
    Solucao solucao;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custo;
    TempoComputacionalEmSegundos tempoInicial;
    TempoComputacionalEmSegundos tempoFinal;
    TempoComputacionalEmSegundos duracaoEmSegundos;
    TempoComputacionalEmMilissegundos duracaoEmMilissegundos;

    instancia = criarInstanciaVazia();
    solucao = criarSolucaoVazia();

    if(nomeDoArquivo == NULL) {
        printf("[ERRO] Nome de arquivo nulo.\n");
        return FALSO;
    }

    if(heuristica == NULL) {
        printf("[ERRO] Heuristica nula.\n");
        return FALSO;
    }

    if(instanciaDaoLerInstanciaPorIdentificador(nomeDoArquivo,identificadorDaInstancia,&instancia) == FALSO) {
        printf("[ERRO] Nao foi possivel ler %s instancia %u.\n",nomeDoArquivo,(unsigned int) identificadorDaInstancia);
        return FALSO;
    }

    dataDeEntregaComum = (DataDeEntregaComum) (((SomaDosTemposDeProcessamento) instancia.somaDosTemposDeProcessamento * ((SomaDosTemposDeProcessamento) fatorH)) / ((SomaDosTemposDeProcessamento) FATOR_DE_ESCALA_H));

    tempoInicial = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerHeuristicaConstruirSolucao(&instancia,heuristica,fatorH,&solucao) == FALSO) {
        printf("[ERRO] Heuristica falhou em %s instancia %u h %.1f.\n",nomeDoArquivo,(unsigned int) identificadorDaInstancia,testeAmostralConverterFatorHParaDecimal(fatorH));
        liberarSolucao(&solucao);
        return FALSO;
    }

    tempoFinal = gerenciadorDeTempoObterTempoAtualEmSegundos();
    duracaoEmSegundos = gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicial,tempoFinal);
    duracaoEmMilissegundos = gerenciadorDeTempoConverterSegundosParaMilissegundos(duracaoEmSegundos);

    if(solucaoEhValida(&solucao) == FALSO) {
        printf("[ERRO] Solucao invalida em %s instancia %u h %.1f.\n",nomeDoArquivo,(unsigned int) identificadorDaInstancia,testeAmostralConverterFatorHParaDecimal(fatorH));
        liberarSolucao(&solucao);
        return FALSO;
    }

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucao,dataDeEntregaComum,&custo) == FALSO) {
        printf("[ERRO] Calculo de custo falhou em %s instancia %u h %.1f.\n",nomeDoArquivo,(unsigned int) identificadorDaInstancia,testeAmostralConverterFatorHParaDecimal(fatorH));
        liberarSolucao(&solucao);
        return FALSO;
    }

    printf(
        "%s;%u;%u;%.1f;%u;%llu;%.6f\n",
        nomeDoArquivo,
        (unsigned int) identificadorDaInstancia,
        (unsigned int) instancia.quantidadeDeTarefas,
        testeAmostralConverterFatorHParaDecimal(fatorH),
        (unsigned int) dataDeEntregaComum,
        (unsigned long long) custo,
        duracaoEmMilissegundos
    );

    if(tempoTotalEmMilissegundos != NULL) {
        (*tempoTotalEmMilissegundos) += duracaoEmMilissegundos;
    }

    if(quantidadeDeExecucoes != NULL) {
        (*quantidadeDeExecucoes) = (*quantidadeDeExecucoes) + 1;
    }

    if(maiorCustoEncontrado != NULL) {
        if(custo > (*maiorCustoEncontrado)) {
            (*maiorCustoEncontrado) = custo;
        }
    }

    liberarSolucao(&solucao);

    return VERDADEIRO;
}

static Boolean testeAmostralExecutarArquivo(const char *nomeDoArquivo,Heuristica *heuristica,TempoComputacionalEmMilissegundos *tempoTotalEmMilissegundos,InteiroPositivoDe32Bits *quantidadeDeExecucoes,Custo *maiorCustoEncontrado) {
    InteiroPositivoDe16Bits identificadorDaInstancia;
    FatorH fatoresH[QUANTIDADE_DE_VALORES_DE_H];
    InteiroPositivoDe8Bits indiceDoFator;

    fatoresH[0] = FATOR_H_02;
    fatoresH[1] = FATOR_H_04;
    fatoresH[2] = FATOR_H_06;
    fatoresH[3] = FATOR_H_08;

    for(identificadorDaInstancia = 1;identificadorDaInstancia <= QUANTIDADE_DE_INSTANCIAS_AMOSTRAIS;identificadorDaInstancia++) {
        for(indiceDoFator = 0;indiceDoFator < QUANTIDADE_DE_VALORES_DE_H;indiceDoFator++) {
            if(testeAmostralExecutarInstancia(nomeDoArquivo,identificadorDaInstancia,fatoresH[indiceDoFator],heuristica,tempoTotalEmMilissegundos,quantidadeDeExecucoes,maiorCustoEncontrado) == FALSO) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

int main(void) {
    Heuristica heuristica;
    const char *arquivos[QUANTIDADE_DE_ARQUIVOS_AMOSTRAIS];
    InteiroPositivoDe8Bits indiceDoArquivo;
    TempoComputacionalEmMilissegundos tempoTotalEmMilissegundos;
    TempoComputacionalEmMilissegundos tempoMedioEmMilissegundos;
    InteiroPositivoDe32Bits quantidadeDeExecucoes;
    Custo maiorCustoEncontrado;

    arquivos[0] = NOME_ARQUIVO_SCH10;
    arquivos[1] = NOME_ARQUIVO_SCH20;
    arquivos[2] = NOME_ARQUIVO_SCH50;
    arquivos[3] = NOME_ARQUIVO_SCH100;
    arquivos[4] = NOME_ARQUIVO_SCH200;

    heuristica = criarHeuristicaPorInsercaoTemporal();

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("[ERRO] Heuristica invalida.\n");
        return 1;
    }

    tempoTotalEmMilissegundos = 0.0;
    quantidadeDeExecucoes = 0;
    maiorCustoEncontrado = 0;

    testeAmostralImprimirCabecalho();

    for(indiceDoArquivo = 0;indiceDoArquivo < QUANTIDADE_DE_ARQUIVOS_AMOSTRAIS;indiceDoArquivo++) {
        if(testeAmostralExecutarArquivo(arquivos[indiceDoArquivo],&heuristica,&tempoTotalEmMilissegundos,&quantidadeDeExecucoes,&maiorCustoEncontrado) == FALSO) {
            printf("\nQuantidade de execucoes concluidas antes da falha: %u\n",(unsigned int) quantidadeDeExecucoes);
            return 1;
        }
    }

    if(quantidadeDeExecucoes == 0) {
        printf("[ERRO] Nenhuma execucao realizada.\n");
        return 1;
    }

    tempoMedioEmMilissegundos = tempoTotalEmMilissegundos / ((TempoComputacionalEmMilissegundos) quantidadeDeExecucoes);

    printf("\nResumo do teste amostral:\n");
    printf("Execucoes realizadas: %u\n",(unsigned int) quantidadeDeExecucoes);
    printf("Tempo total ms: %.6f\n",tempoTotalEmMilissegundos);
    printf("Tempo medio ms: %.6f\n",tempoMedioEmMilissegundos);
    printf("Maior custo encontrado: %llu\n",(unsigned long long) maiorCustoEncontrado);
    printf("\nTodos os testes amostrais passaram.\n");

    return 0;
}