#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../model/entidades/instancia/instancia.h"
#include "../model/entidades/solucao/solucao.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>

#define QUANTIDADE_ARQUIVOS_RAIO 2
#define QUANTIDADE_RAIOS_TESTADOS 4
#define CAMINHO_RESULTADO_VARREDURA_RAIO "resultados/varredura_raio_busca_local.csv"

static Boolean experimentoRaioCriarArquivoDeSaida(void) {
    FILE *arquivo;

    arquivo = fopen(CAMINHO_RESULTADO_VARREDURA_RAIO,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(arquivo,"idExecucao,arquivo,idInstancia,n,h,raio,custoConstrutiva,custoBuscaLocal,melhoriaAbsoluta,melhoriaPercentual,tempoConstrutivaMs,tempoBuscaLocalMs,tempoTotalMs,iteracoesBuscaLocal,vizinhosAvaliados\n");
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean experimentoRaioAdicionarLinha(IdentificadorDeExecucao identificadorDaExecucao,const char *nomeDoArquivo,InteiroPositivoDe16Bits identificadorDaInstancia,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,QuantidadeDeTarefas raio,Custo custoConstrutiva,Custo custoBuscaLocal,TempoComputacionalEmMilissegundos tempoConstrutivaMs,TempoComputacionalEmMilissegundos tempoBuscaLocalMs,const ResultadoBuscaLocal *resultadoBuscaLocal) {
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

    arquivo = fopen(CAMINHO_RESULTADO_VARREDURA_RAIO,"a");

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

static Boolean experimentoRaioExecutarCombinacao(const char *nomeDoArquivo,InteiroPositivoDe16Bits identificadorDaInstancia,FatorH fatorH,IdentificadorDeExecucao *identificadorDaExecucao,const Heuristica *heuristica,const QuantidadeDeTarefas *raios,InteiroPositivoDe8Bits quantidadeDeRaios) {
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
    InteiroPositivoDe8Bits indiceDoRaio;
    QuantidadeDeTarefas raio;

    instancia = criarInstanciaVazia();
    solucaoConstrutiva = criarSolucaoVazia();
    custoConstrutiva = 0;

    if(nomeDoArquivo == NULL) {
        return FALSO;
    }

    if(identificadorDaExecucao == NULL) {
        return FALSO;
    }

    if(heuristica == NULL) {
        return FALSO;
    }

    if(raios == NULL) {
        return FALSO;
    }

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

    tempoConstrutivaMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicialConstrutiva,tempoFinalConstrutiva));

    for(indiceDoRaio = 0;indiceDoRaio < quantidadeDeRaios;indiceDoRaio++) {
        raio = raios[indiceDoRaio];
        solucaoBuscaLocal = criarSolucaoVazia();
        resultadoBuscaLocal = criarResultadoBuscaLocalVazio();
        custoBuscaLocal = 0;

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

        tempoBuscaLocalMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicialBuscaLocal,tempoFinalBuscaLocal));

        if(experimentoRaioAdicionarLinha((*identificadorDaExecucao),nomeDoArquivo,identificadorDaInstancia,instancia.quantidadeDeTarefas,fatorH,raio,custoConstrutiva,custoBuscaLocal,tempoConstrutivaMs,tempoBuscaLocalMs,&resultadoBuscaLocal) == FALSO) {
            liberarSolucao(&solucaoBuscaLocal);
            liberarSolucao(&solucaoConstrutiva);
            liberarInstancia(&instancia);

            return FALSO;
        }

        printf("Execucao %03u | arquivo=%s | instancia=%u | n=%u | h=%.1f | raio=%u | construtiva=%llu | busca_local=%llu | melhora=%.4f%% | tempo_bl_ms=%.3f\n",(unsigned int) (*identificadorDaExecucao),nomeDoArquivo,(unsigned int) identificadorDaInstancia,(unsigned int) instancia.quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) custoConstrutiva,(unsigned long long) custoBuscaLocal,custoConstrutiva > 0 ? ((((double) custoConstrutiva) - ((double) custoBuscaLocal)) * 100.0) / ((double) custoConstrutiva) : 0.0,tempoBuscaLocalMs);

        (*identificadorDaExecucao)++;
        liberarSolucao(&solucaoBuscaLocal);
    }

    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return VERDADEIRO;
}

int main(void) {
    const char *nomesDosArquivos[QUANTIDADE_ARQUIVOS_RAIO];
    InteiroPositivoDe16Bits limitesDeInstancias[QUANTIDADE_ARQUIVOS_RAIO];
    FatorH fatoresH[QUANTIDADE_DE_VALORES_DE_H];
    QuantidadeDeTarefas raios[QUANTIDADE_RAIOS_TESTADOS];
    Heuristica heuristica;
    IdentificadorDeExecucao identificadorDaExecucao;
    InteiroPositivoDe8Bits indiceDoArquivo;
    InteiroPositivoDe8Bits indiceDoFatorH;
    InteiroPositivoDe16Bits quantidadeDeInstancias;
    InteiroPositivoDe16Bits quantidadeDeInstanciasExecutadas;
    InteiroPositivoDe16Bits identificadorDaInstancia;

    nomesDosArquivos[0] = "sch50.txt";
    nomesDosArquivos[1] = "sch100.txt";

    limitesDeInstancias[0] = 10;
    limitesDeInstancias[1] = 3;

    fatoresH[0] = FATOR_H_02;
    fatoresH[1] = FATOR_H_04;
    fatoresH[2] = FATOR_H_06;
    fatoresH[3] = FATOR_H_08;

    raios[0] = 4;
    raios[1] = 8;
    raios[2] = 12;
    raios[3] = 20;

    heuristica = criarHeuristicaPorInsercaoTemporal();

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("Heuristica invalida.\n");

        return 1;
    }

    if(experimentoRaioCriarArquivoDeSaida() == FALSO) {
        printf("Nao foi possivel criar o arquivo de saida.\n");

        return 1;
    }

    identificadorDaExecucao = 1;

    printf("Varredura de raio da busca local por reinsercao\n");

    for(indiceDoArquivo = 0;indiceDoArquivo < QUANTIDADE_ARQUIVOS_RAIO;indiceDoArquivo++) {
        quantidadeDeInstancias = 0;

        if(instanciaDaoLerQuantidadeDeInstanciasDoArquivo(nomesDosArquivos[indiceDoArquivo],&quantidadeDeInstancias) == FALSO) {
            printf("Nao foi possivel ler a quantidade de instancias do arquivo %s.\n",nomesDosArquivos[indiceDoArquivo]);

            return 1;
        }

        quantidadeDeInstanciasExecutadas = quantidadeDeInstancias;

        if(quantidadeDeInstanciasExecutadas > limitesDeInstancias[indiceDoArquivo]) {
            quantidadeDeInstanciasExecutadas = limitesDeInstancias[indiceDoArquivo];
        }

        for(identificadorDaInstancia = 1;identificadorDaInstancia <= quantidadeDeInstanciasExecutadas;identificadorDaInstancia++) {
            for(indiceDoFatorH = 0;indiceDoFatorH < QUANTIDADE_DE_VALORES_DE_H;indiceDoFatorH++) {
                if(experimentoRaioExecutarCombinacao(nomesDosArquivos[indiceDoArquivo],identificadorDaInstancia,fatoresH[indiceDoFatorH],&identificadorDaExecucao,&heuristica,raios,QUANTIDADE_RAIOS_TESTADOS) == FALSO) {
                    printf("Falha na execucao %u.\n",(unsigned int) identificadorDaExecucao);

                    return 1;
                }
            }
        }
    }

    printf("Arquivo gerado: %s\n",CAMINHO_RESULTADO_VARREDURA_RAIO);
    printf("Varredura de raio finalizada com sucesso.\n");

    return 0;
}