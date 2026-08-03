#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>

#define QUANTIDADE_ARQUIVOS_COMPOSTA 7
#define CAMINHO_RESULTADO_COMPOSTA "resultados/validacao_busca_local_composta.csv"

static QuantidadeDeTarefas obterRaio(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 200) {
        return 20;
    }

    if(quantidadeDeTarefas <= 500) {
        return 12;
    }

    return 4;
}

static Boolean criarArquivo(void) {
    FILE *arquivo;

    arquivo = fopen(CAMINHO_RESULTADO_COMPOSTA,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(arquivo,"idExecucao,arquivo,n,h,raio,custoConstrutiva,custoReinsercao,custoComposta,melhoriaReinsercaoPercentual,melhoriaCompostaPercentual,ganhoCompostaSobreReinsercaoPercentual,tempoReinsercaoMs,tempoCompostaMs,vizinhosReinsercao,vizinhosComposta,vizinhosCompostaReinsercao,vizinhosCompostaTroca,melhoriasCompostaReinsercao,melhoriasCompostaTroca\n");
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean adicionarLinha(IdentificadorDeExecucao identificadorDaExecucao,const char *nomeDoArquivo,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,QuantidadeDeTarefas raio,Custo custoConstrutiva,Custo custoReinsercao,Custo custoComposta,TempoComputacionalEmMilissegundos tempoReinsercao,TempoComputacionalEmMilissegundos tempoComposta,const ResultadoBuscaLocal *resultadoReinsercao,const ResultadoBuscaLocal *resultadoComposta) {
    FILE *arquivo;
    double melhoriaReinsercao;
    double melhoriaComposta;
    double ganhoComposta;

    arquivo = fopen(CAMINHO_RESULTADO_COMPOSTA,"a");

    if(arquivo == NULL) {
        return FALSO;
    }

    melhoriaReinsercao = custoConstrutiva > 0 ? ((((double) custoConstrutiva) - ((double) custoReinsercao)) * 100.0) / ((double) custoConstrutiva) : 0.0;
    melhoriaComposta = custoConstrutiva > 0 ? ((((double) custoConstrutiva) - ((double) custoComposta)) * 100.0) / ((double) custoConstrutiva) : 0.0;
    ganhoComposta = custoReinsercao > 0 ? ((((double) custoReinsercao) - ((double) custoComposta)) * 100.0) / ((double) custoReinsercao) : 0.0;

    fprintf(arquivo,"%u,%s,%u,%.1f,%u,%llu,%llu,%llu,%.9f,%.9f,%.9f,%.9f,%.9f,%u,%u,%u,%u,%u,%u\n",(unsigned int) identificadorDaExecucao,nomeDoArquivo,(unsigned int) quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) custoConstrutiva,(unsigned long long) custoReinsercao,(unsigned long long) custoComposta,melhoriaReinsercao,melhoriaComposta,ganhoComposta,tempoReinsercao,tempoComposta,(unsigned int) (*resultadoReinsercao).quantidadeDeVizinhosAvaliados,(unsigned int) (*resultadoComposta).quantidadeDeVizinhosAvaliados,(unsigned int) (*resultadoComposta).quantidadeDeVizinhosPorReinsercao,(unsigned int) (*resultadoComposta).quantidadeDeVizinhosPorTroca,(unsigned int) (*resultadoComposta).quantidadeDeMelhoriasPorReinsercao,(unsigned int) (*resultadoComposta).quantidadeDeMelhoriasPorTroca);
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean executarCombinacao(const char *nomeDoArquivo,FatorH fatorH,IdentificadorDeExecucao identificadorDaExecucao,const Heuristica *heuristica) {
    Instancia instancia;
    Solucao solucaoConstrutiva;
    Solucao solucaoReinsercao;
    Solucao solucaoComposta;
    ResultadoBuscaLocal resultadoReinsercao;
    ResultadoBuscaLocal resultadoComposta;
    QuantidadeDeTarefas raio;
    TempoComputacionalEmSegundos inicio;
    TempoComputacionalEmSegundos fim;
    TempoComputacionalEmMilissegundos tempoReinsercao;
    TempoComputacionalEmMilissegundos tempoComposta;
    double ganhoComposta;

    instancia = criarInstanciaVazia();
    solucaoConstrutiva = criarSolucaoVazia();
    solucaoReinsercao = criarSolucaoVazia();
    solucaoComposta = criarSolucaoVazia();
    resultadoReinsercao = criarResultadoBuscaLocalVazio();
    resultadoComposta = criarResultadoBuscaLocalVazio();

    if(instanciaDaoLerInstanciaPorIdentificador(nomeDoArquivo,1,&instancia) == FALSO) {
        return FALSO;
    }

    if(controllerHeuristicaConstruirSolucao(&instancia,heuristica,fatorH,&solucaoConstrutiva) == FALSO) {
        liberarInstancia(&instancia);

        return FALSO;
    }

    raio = obterRaio(instancia.quantidadeDeTarefas);

    inicio = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerBuscaLocalMelhorarSolucaoPorReinsercaoLimitada(&instancia,fatorH,&solucaoConstrutiva,&solucaoReinsercao,&resultadoReinsercao,raio) == FALSO) {
        return FALSO;
    }

    fim = gerenciadorDeTempoObterTempoAtualEmSegundos();
    tempoReinsercao = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(inicio,fim));

    inicio = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(&instancia,fatorH,&solucaoConstrutiva,&solucaoComposta,&resultadoComposta,raio,raio) == FALSO) {
        return FALSO;
    }

    fim = gerenciadorDeTempoObterTempoAtualEmSegundos();
    tempoComposta = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(inicio,fim));

    ganhoComposta = resultadoReinsercao.custoFinal > 0 ? ((((double) resultadoReinsercao.custoFinal) - ((double) resultadoComposta.custoFinal)) * 100.0) / ((double) resultadoReinsercao.custoFinal) : 0.0;

    if(adicionarLinha(identificadorDaExecucao,nomeDoArquivo,instancia.quantidadeDeTarefas,fatorH,raio,resultadoReinsercao.custoInicial,resultadoReinsercao.custoFinal,resultadoComposta.custoFinal,tempoReinsercao,tempoComposta,&resultadoReinsercao,&resultadoComposta) == FALSO) {
        return FALSO;
    }

    printf("Execucao %03u | arquivo=%s | n=%u | h=%.1f | raio=%u | construtiva=%llu | reinsercao=%llu | composta=%llu | ganho_composta=%.4f%% | tempo_reinsercao_ms=%.3f | tempo_composta_ms=%.3f | trocas_aceitas=%u\n",(unsigned int) identificadorDaExecucao,nomeDoArquivo,(unsigned int) instancia.quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) resultadoReinsercao.custoInicial,(unsigned long long) resultadoReinsercao.custoFinal,(unsigned long long) resultadoComposta.custoFinal,ganhoComposta,tempoReinsercao,tempoComposta,(unsigned int) resultadoComposta.quantidadeDeMelhoriasPorTroca);

    liberarSolucao(&solucaoComposta);
    liberarSolucao(&solucaoReinsercao);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return VERDADEIRO;
}

int main(void) {
    const char *arquivos[QUANTIDADE_ARQUIVOS_COMPOSTA];
    FatorH fatoresH[QUANTIDADE_DE_VALORES_DE_H];
    Heuristica heuristica;
    IdentificadorDeExecucao identificadorDaExecucao;
    InteiroPositivoDe8Bits indiceDoArquivo;
    InteiroPositivoDe8Bits indiceDoFatorH;

    arquivos[0] = "sch10.txt";
    arquivos[1] = "sch20.txt";
    arquivos[2] = "sch50.txt";
    arquivos[3] = "sch100.txt";
    arquivos[4] = "sch200.txt";
    arquivos[5] = "sch500.txt";
    arquivos[6] = "sch1000.txt";

    fatoresH[0] = FATOR_H_02;
    fatoresH[1] = FATOR_H_04;
    fatoresH[2] = FATOR_H_06;
    fatoresH[3] = FATOR_H_08;

    heuristica = criarHeuristicaPorInsercaoTemporal();

    if(heuristicaEhValida(&heuristica) == FALSO || criarArquivo() == FALSO) {
        return 1;
    }

    identificadorDaExecucao = 1;

    printf("Validacao da busca local com vizinhanca composta\n");

    for(indiceDoArquivo = 0;indiceDoArquivo < QUANTIDADE_ARQUIVOS_COMPOSTA;indiceDoArquivo++) {
        for(indiceDoFatorH = 0;indiceDoFatorH < QUANTIDADE_DE_VALORES_DE_H;indiceDoFatorH++) {
            if(executarCombinacao(arquivos[indiceDoArquivo],fatoresH[indiceDoFatorH],identificadorDaExecucao,&heuristica) == FALSO) {
                printf("Falha na execucao %u.\n",(unsigned int) identificadorDaExecucao);

                return 1;
            }

            identificadorDaExecucao++;
        }
    }

    printf("Arquivo gerado: %s\n",CAMINHO_RESULTADO_COMPOSTA);
    printf("Validacao composta finalizada com sucesso.\n");

    return 0;
}