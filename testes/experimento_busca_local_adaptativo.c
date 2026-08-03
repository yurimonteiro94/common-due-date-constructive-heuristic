#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../model/entidades/instancia/instancia.h"
#include "../model/entidades/solucao/solucao.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>

#define QUANTIDADE_ARQUIVOS_ADAPTATIVO 7
#define CAMINHO_RESULTADO_ADAPTATIVO "resultados/validacao_busca_local_adaptativa.csv"

static QuantidadeDeTarefas experimentoAdaptativoObterRaio(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 200) {
        return 20;
    }

    if(quantidadeDeTarefas <= 500) {
        return 12;
    }

    return 4;
}

static Boolean experimentoAdaptativoCriarArquivoDeSaida(void) {
    FILE *arquivo;

    arquivo = fopen(CAMINHO_RESULTADO_ADAPTATIVO,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(arquivo,"idExecucao,arquivo,idInstancia,n,h,raio,custoConstrutiva,custoBuscaLocal,melhoriaAbsoluta,melhoriaPercentual,tempoConstrutivaMs,tempoBuscaLocalMs,tempoTotalMs,iteracoesBuscaLocal,vizinhosAvaliados\n");
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean experimentoAdaptativoAdicionarLinha(IdentificadorDeExecucao identificadorDaExecucao,const char *nomeDoArquivo,InteiroPositivoDe16Bits identificadorDaInstancia,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,QuantidadeDeTarefas raio,Custo custoConstrutiva,Custo custoBuscaLocal,TempoComputacionalEmMilissegundos tempoConstrutivaMs,TempoComputacionalEmMilissegundos tempoBuscaLocalMs,const ResultadoBuscaLocal *resultadoBuscaLocal) {
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

    arquivo = fopen(CAMINHO_RESULTADO_ADAPTATIVO,"a");

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

    fprintf(arquivo,"%u,%s,%u,%u,%.1f,%u,%llu,%llu,%.9f,%.9f,%.9f,%.9f,%.9f,%u,%u\n",(unsigned int) identificadorDaExecucao,nomeDoArquivo,(unsigned int) identificadorDaInstancia,(unsigned int) quantidadeDeTarefas,hDecimal,(unsigned int) raio,(unsigned long long) custoConstrutiva,(unsigned long long) custoBuscaLocal,melhoriaAbsoluta,melhoriaPercentual,tempoConstrutivaMs,tempoBuscaLocalMs,tempoTotalMs,(unsigned int) (*resultadoBuscaLocal).quantidadeDeIteracoes,(unsigned int) (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados);
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean experimentoAdaptativoExecutarCombinacao(const char *nomeDoArquivo,InteiroPositivoDe16Bits identificadorDaInstancia,FatorH fatorH,IdentificadorDeExecucao identificadorDaExecucao,const Heuristica *heuristica) {
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
    QuantidadeDeTarefas raio;

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

    raio = experimentoAdaptativoObterRaio(instancia.quantidadeDeTarefas);
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

    if(controllerBuscaLocalMelhorarSolucaoPorReinsercaoLimitada(&instancia,fatorH,&solucaoConstrutiva,&solucaoBuscaLocal,&resultadoBuscaLocal,raio) == FALSO) {
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

    if(custoBuscaLocal != resultadoBuscaLocal.custoFinal) {
        printf("Erro: custo rapido diferente do custo verificado.\n");
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

    tempoConstrutivaMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicialConstrutiva,tempoFinalConstrutiva));
    tempoBuscaLocalMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicialBuscaLocal,tempoFinalBuscaLocal));

    if(experimentoAdaptativoAdicionarLinha(identificadorDaExecucao,nomeDoArquivo,identificadorDaInstancia,instancia.quantidadeDeTarefas,fatorH,raio,custoConstrutiva,custoBuscaLocal,tempoConstrutivaMs,tempoBuscaLocalMs,&resultadoBuscaLocal) == FALSO) {
        liberarSolucao(&solucaoBuscaLocal);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    printf("Execucao %03u | arquivo=%s | n=%u | h=%.1f | raio=%u | construtiva=%llu | busca_local=%llu | melhora=%.4f%% | tempo_bl_ms=%.3f | vizinhos=%u\n",(unsigned int) identificadorDaExecucao,nomeDoArquivo,(unsigned int) instancia.quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) custoConstrutiva,(unsigned long long) custoBuscaLocal,custoConstrutiva > 0 ? ((((double) custoConstrutiva) - ((double) custoBuscaLocal)) * 100.0) / ((double) custoConstrutiva) : 0.0,tempoBuscaLocalMs,(unsigned int) resultadoBuscaLocal.quantidadeDeVizinhosAvaliados);

    liberarSolucao(&solucaoBuscaLocal);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return VERDADEIRO;
}

int main(void) {
    const char *nomesDosArquivos[QUANTIDADE_ARQUIVOS_ADAPTATIVO];
    FatorH fatoresH[QUANTIDADE_DE_VALORES_DE_H];
    Heuristica heuristica;
    IdentificadorDeExecucao identificadorDaExecucao;
    InteiroPositivoDe8Bits indiceDoArquivo;
    InteiroPositivoDe8Bits indiceDoFatorH;

    nomesDosArquivos[0] = "sch10.txt";
    nomesDosArquivos[1] = "sch20.txt";
    nomesDosArquivos[2] = "sch50.txt";
    nomesDosArquivos[3] = "sch100.txt";
    nomesDosArquivos[4] = "sch200.txt";
    nomesDosArquivos[5] = "sch500.txt";
    nomesDosArquivos[6] = "sch1000.txt";

    fatoresH[0] = FATOR_H_02;
    fatoresH[1] = FATOR_H_04;
    fatoresH[2] = FATOR_H_06;
    fatoresH[3] = FATOR_H_08;

    heuristica = criarHeuristicaPorInsercaoTemporal();

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("Heuristica invalida.\n");

        return 1;
    }

    if(experimentoAdaptativoCriarArquivoDeSaida() == FALSO) {
        printf("Nao foi possivel criar o arquivo de saida.\n");

        return 1;
    }

    identificadorDaExecucao = 1;

    printf("Validacao da busca local adaptativa\n");

    for(indiceDoArquivo = 0;indiceDoArquivo < QUANTIDADE_ARQUIVOS_ADAPTATIVO;indiceDoArquivo++) {
        for(indiceDoFatorH = 0;indiceDoFatorH < QUANTIDADE_DE_VALORES_DE_H;indiceDoFatorH++) {
            if(experimentoAdaptativoExecutarCombinacao(nomesDosArquivos[indiceDoArquivo],1,fatoresH[indiceDoFatorH],identificadorDaExecucao,&heuristica) == FALSO) {
                printf("Falha na execucao %u.\n",(unsigned int) identificadorDaExecucao);

                return 1;
            }

            identificadorDaExecucao++;
        }
    }

    printf("Arquivo gerado: %s\n",CAMINHO_RESULTADO_ADAPTATIVO);
    printf("Validacao adaptativa finalizada com sucesso.\n");

    return 0;
}