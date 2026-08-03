#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../model/entidades/instancia/instancia.h"
#include "../model/entidades/solucao/solucao.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>

#define QUANTIDADE_ARQUIVOS_AMOSTRAIS 3
#define CAMINHO_RESULTADO_BUSCA_LOCAL_AMOSTRAL "resultados/resultados_busca_local_amostral.csv"

static Boolean experimentoBuscaLocalCriarArquivoDeSaida(void) {
    FILE *arquivo;

    arquivo = fopen(CAMINHO_RESULTADO_BUSCA_LOCAL_AMOSTRAL,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(
        arquivo,
        "idExecucao,arquivo,idInstancia,n,h,custoConstrutiva,custoBuscaLocal,melhoriaAbsoluta,melhoriaPercentual,tempoConstrutivaMs,tempoBuscaLocalMs,tempoTotalMs,iteracoesBuscaLocal,vizinhosAvaliados\n"
    );

    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean experimentoBuscaLocalAdicionarLinha(
    IdentificadorDeExecucao identificadorDaExecucao,
    const char *nomeDoArquivo,
    InteiroPositivoDe16Bits identificadorDaInstancia,
    QuantidadeDeTarefas quantidadeDeTarefas,
    FatorH fatorH,
    Custo custoConstrutiva,
    Custo custoBuscaLocal,
    TempoComputacionalEmMilissegundos tempoConstrutivaMs,
    TempoComputacionalEmMilissegundos tempoBuscaLocalMs,
    const ResultadoBuscaLocal *resultadoBuscaLocal
) {
    FILE *arquivo;
    double hDecimal;
    double melhoriaAbsoluta;
    double melhoriaPercentual;
    double tempoTotalMs;

    if(nomeDoArquivo == NULL) {
        return FALSO;
    }

    if(resultadoBuscaLocal == NULL) {
        return FALSO;
    }

    arquivo = fopen(CAMINHO_RESULTADO_BUSCA_LOCAL_AMOSTRAL,"a");

    if(arquivo == NULL) {
        return FALSO;
    }

    hDecimal = ((double) fatorH) / ((double) FATOR_DE_ESCALA_H);
    melhoriaAbsoluta = ((double) custoConstrutiva) - ((double) custoBuscaLocal);
    melhoriaPercentual = 0.0;
    tempoTotalMs = tempoConstrutivaMs + tempoBuscaLocalMs;

    if(custoConstrutiva > 0) {
        melhoriaPercentual = (melhoriaAbsoluta * 100.0) / ((double) custoConstrutiva);
    }

    fprintf(
        arquivo,
        "%u,%s,%u,%u,%.1f,%llu,%llu,%.9f,%.9f,%.9f,%.9f,%.9f,%u,%u\n",
        (unsigned int) identificadorDaExecucao,
        nomeDoArquivo,
        (unsigned int) identificadorDaInstancia,
        (unsigned int) quantidadeDeTarefas,
        hDecimal,
        (unsigned long long) custoConstrutiva,
        (unsigned long long) custoBuscaLocal,
        melhoriaAbsoluta,
        melhoriaPercentual,
        tempoConstrutivaMs,
        tempoBuscaLocalMs,
        tempoTotalMs,
        (unsigned int) (*resultadoBuscaLocal).quantidadeDeIteracoes,
        (unsigned int) (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados
    );

    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean experimentoBuscaLocalExecutarCombinacao(
    const char *nomeDoArquivo,
    InteiroPositivoDe16Bits identificadorDaInstancia,
    FatorH fatorH,
    IdentificadorDeExecucao identificadorDaExecucao,
    const Heuristica *heuristica
) {
    Instancia instancia;
    Solucao solucaoConstrutiva;
    Solucao solucaoBuscaLocal;
    ResultadoBuscaLocal resultadoBuscaLocal;
    Custo custoConstrutiva;
    Custo custoBuscaLocal;
    TempoComputacionalEmSegundos tempoInicialConstrutiva;
    TempoComputacionalEmSegundos tempoFinalConstrutiva;
    TempoComputacionalEmSegundos tempoInicialBuscaLocal;
    TempoComputacionalEmSegundos tempoFinalBuscaLocal;
    TempoComputacionalEmMilissegundos tempoConstrutivaMs;
    TempoComputacionalEmMilissegundos tempoBuscaLocalMs;
    DataDeEntregaComum dataDeEntregaComum;

    instancia = criarInstanciaVazia();
    solucaoConstrutiva = criarSolucaoVazia();
    solucaoBuscaLocal = criarSolucaoVazia();
    resultadoBuscaLocal = criarResultadoBuscaLocalVazio();
    custoConstrutiva = 0;
    custoBuscaLocal = 0;

    if(instanciaDaoLerInstanciaPorIdentificador(nomeDoArquivo,identificadorDaInstancia,&instancia) == FALSO) {
        liberarInstancia(&instancia);

        return FALSO;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        liberarInstancia(&instancia);

        return FALSO;
    }

    tempoInicialConstrutiva = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerHeuristicaConstruirSolucao(&instancia,heuristica,fatorH,&solucaoConstrutiva) == FALSO) {
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    tempoFinalConstrutiva = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoConstrutiva,dataDeEntregaComum,&custoConstrutiva) == FALSO) {
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    tempoInicialBuscaLocal = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerBuscaLocalMelhorarSolucaoPorReinsercao(&instancia,fatorH,&solucaoConstrutiva,&solucaoBuscaLocal,&resultadoBuscaLocal) == FALSO) {
        liberarSolucao(&solucaoBuscaLocal);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    tempoFinalBuscaLocal = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoBuscaLocal,dataDeEntregaComum,&custoBuscaLocal) == FALSO) {
        liberarSolucao(&solucaoBuscaLocal);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    if(custoBuscaLocal > custoConstrutiva) {
        printf("Erro: busca local piorou a solucao.\n");
        liberarSolucao(&solucaoBuscaLocal);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    tempoConstrutivaMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(
        gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicialConstrutiva,tempoFinalConstrutiva)
    );

    tempoBuscaLocalMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(
        gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicialBuscaLocal,tempoFinalBuscaLocal)
    );

    if(experimentoBuscaLocalAdicionarLinha(
        identificadorDaExecucao,
        nomeDoArquivo,
        identificadorDaInstancia,
        instancia.quantidadeDeTarefas,
        fatorH,
        custoConstrutiva,
        custoBuscaLocal,
        tempoConstrutivaMs,
        tempoBuscaLocalMs,
        &resultadoBuscaLocal
    ) == FALSO) {
        liberarSolucao(&solucaoBuscaLocal);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    printf(
        "Execucao %03u | arquivo=%s | instancia=%u | n=%u | h=%.1f | construtiva=%llu | busca_local=%llu | melhora=%.4f%% | tempo_bl_ms=%.3f\n",
        (unsigned int) identificadorDaExecucao,
        nomeDoArquivo,
        (unsigned int) identificadorDaInstancia,
        (unsigned int) instancia.quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        (unsigned long long) custoConstrutiva,
        (unsigned long long) custoBuscaLocal,
        custoConstrutiva > 0 ? ((((double) custoConstrutiva) - ((double) custoBuscaLocal)) * 100.0) / ((double) custoConstrutiva) : 0.0,
        tempoBuscaLocalMs
    );

    liberarSolucao(&solucaoBuscaLocal);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return VERDADEIRO;
}

int main(void) {
    const char *nomesDosArquivos[QUANTIDADE_ARQUIVOS_AMOSTRAIS];
    FatorH fatoresH[QUANTIDADE_DE_VALORES_DE_H];
    Heuristica heuristica;
    IdentificadorDeExecucao identificadorDaExecucao;
    InteiroPositivoDe8Bits indiceDoArquivo;
    InteiroPositivoDe8Bits indiceDoFatorH;
    InteiroPositivoDe16Bits quantidadeDeInstancias;
    InteiroPositivoDe16Bits identificadorDaInstancia;

    nomesDosArquivos[0] = "instancias/sch10.txt";
    nomesDosArquivos[1] = "instancias/sch20.txt";
    nomesDosArquivos[2] = "instancias/sch50.txt";

    fatoresH[0] = FATOR_H_02;
    fatoresH[1] = FATOR_H_04;
    fatoresH[2] = FATOR_H_06;
    fatoresH[3] = FATOR_H_08;

    heuristica = criarHeuristicaPorInsercaoTemporal();

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("Heuristica invalida.\n");

        return 1;
    }

    if(experimentoBuscaLocalCriarArquivoDeSaida() == FALSO) {
        printf("Nao foi possivel criar o arquivo de saida.\n");

        return 1;
    }

    identificadorDaExecucao = 1;

    printf("Experimento amostral da busca local por reinsercao\n");

    for(indiceDoArquivo = 0;indiceDoArquivo < QUANTIDADE_ARQUIVOS_AMOSTRAIS;indiceDoArquivo++) {
        quantidadeDeInstancias = 0;

        if(instanciaDaoLerQuantidadeDeInstanciasDoArquivo(nomesDosArquivos[indiceDoArquivo],&quantidadeDeInstancias) == FALSO) {
            printf("Nao foi possivel ler a quantidade de instancias do arquivo %s.\n",nomesDosArquivos[indiceDoArquivo]);

            return 1;
        }

        for(identificadorDaInstancia = 1;identificadorDaInstancia <= quantidadeDeInstancias;identificadorDaInstancia++) {
            for(indiceDoFatorH = 0;indiceDoFatorH < QUANTIDADE_DE_VALORES_DE_H;indiceDoFatorH++) {
                if(experimentoBuscaLocalExecutarCombinacao(
                    nomesDosArquivos[indiceDoArquivo],
                    identificadorDaInstancia,
                    fatoresH[indiceDoFatorH],
                    identificadorDaExecucao,
                    &heuristica
                ) == FALSO) {
                    printf("Falha na execucao %u.\n",(unsigned int) identificadorDaExecucao);

                    return 1;
                }

                identificadorDaExecucao++;
            }
        }
    }

    printf("Arquivo gerado: %s\n",CAMINHO_RESULTADO_BUSCA_LOCAL_AMOSTRAL);
    printf("Experimento amostral finalizado com sucesso.\n");

    return 0;
}