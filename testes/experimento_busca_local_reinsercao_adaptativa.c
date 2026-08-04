#include "../controller/controller_busca_local_reinsercao_adaptativa/controller_busca_local_reinsercao_adaptativa.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>
#include <stdlib.h>

static const char *obterTipoDeVizinhanca(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= LIMITE_REINSERCAO_GLOBAL) {
        return "global";
    }

    if(quantidadeDeTarefas <= LIMITE_REINSERCAO_REPRESENTATIVA) {
        return "representativa";
    }

    return "limitada";
}

static QuantidadeDeTarefas obterRaioLocal(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= LIMITE_REINSERCAO_GLOBAL) {
        return (QuantidadeDeTarefas) (quantidadeDeTarefas - 1);
    }

    if(quantidadeDeTarefas <= LIMITE_REINSERCAO_REPRESENTATIVA) {
        return RAIO_LOCAL_REINSERCAO_REPRESENTATIVA;
    }

    return RAIO_LOCAL_REINSERCAO_GRANDE;
}

static QuantidadeDeTarefas obterQuantidadeDeAncoras(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas > LIMITE_REINSERCAO_GLOBAL && quantidadeDeTarefas <= LIMITE_REINSERCAO_REPRESENTATIVA) {
        return QUANTIDADE_ANCORAS_REINSERCAO_REPRESENTATIVA;
    }

    return 0;
}

static QuantidadeDeTarefas obterRaioDaFronteira(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas > LIMITE_REINSERCAO_GLOBAL && quantidadeDeTarefas <= LIMITE_REINSERCAO_REPRESENTATIVA) {
        return RAIO_FRONTEIRA_REINSERCAO_REPRESENTATIVA;
    }

    return 0;
}

static Boolean criarArquivoDeResultado(const char *caminho) {
    FILE *arquivo;

    arquivo = fopen(caminho,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(
        arquivo,
        "arquivo,idInstancia,n,h,tipoVizinhanca,raioLocal,quantidadeAncoras,raioFronteira,custoConstrutiva,custoBuscaLocal,melhoriaSobreConstrutivaPercentual,tempoConstrutivaMs,tempoBuscaLocalMs,iteracoes,vizinhosAvaliados,reinsercoesAceitas\n"
    );

    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean adicionarResultado(const char *caminho,const Instancia *instancia,FatorH fatorH,Custo custoConstrutiva,Custo custoBuscaLocal,TempoComputacionalEmMilissegundos tempoConstrutivaMs,TempoComputacionalEmMilissegundos tempoBuscaLocalMs,const ResultadoBuscaLocal *resultado) {
    FILE *arquivo;
    double melhoriaPercentual;

    if(caminho == NULL || instancia == NULL || resultado == NULL) {
        return FALSO;
    }

    arquivo = fopen(caminho,"a");

    if(arquivo == NULL) {
        return FALSO;
    }

    melhoriaPercentual = 0.0;

    if(custoConstrutiva > 0) {
        melhoriaPercentual = ((((double) custoConstrutiva) - ((double) custoBuscaLocal)) * 100.0) / ((double) custoConstrutiva);
    }

    fprintf(
        arquivo,
        "%s,%u,%u,%.1f,%s,%u,%u,%u,%llu,%llu,%.9f,%.9f,%.9f,%u,%u,%u\n",
        (*instancia).nomeDoArquivoDeOrigem,
        (unsigned int) (*instancia).identificadorDaInstancia,
        (unsigned int) (*instancia).quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        obterTipoDeVizinhanca((*instancia).quantidadeDeTarefas),
        (unsigned int) obterRaioLocal((*instancia).quantidadeDeTarefas),
        (unsigned int) obterQuantidadeDeAncoras((*instancia).quantidadeDeTarefas),
        (unsigned int) obterRaioDaFronteira((*instancia).quantidadeDeTarefas),
        (unsigned long long) custoConstrutiva,
        (unsigned long long) custoBuscaLocal,
        melhoriaPercentual,
        tempoConstrutivaMs,
        tempoBuscaLocalMs,
        (unsigned int) (*resultado).quantidadeDeIteracoes,
        (unsigned int) (*resultado).quantidadeDeVizinhosAvaliados,
        (unsigned int) (*resultado).quantidadeDeMelhoriasPorReinsercao
    );

    fclose(arquivo);

    return VERDADEIRO;
}

int main(int argc,char **argv) {
    Instancia instancia;
    Heuristica heuristica;
    Solucao solucaoConstrutiva;
    Solucao solucaoBuscaLocal;
    ResultadoBuscaLocal resultadoBuscaLocal;
    FatorH fatorH;
    InteiroPositivoDe16Bits identificadorDaInstancia;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoConstrutiva;
    Custo custoBuscaLocal;
    TempoComputacionalEmSegundos inicio;
    TempoComputacionalEmSegundos fim;
    TempoComputacionalEmMilissegundos tempoConstrutivaMs;
    TempoComputacionalEmMilissegundos tempoBuscaLocalMs;
    char caminhoResultado[TAMANHO_MAXIMO_DE_CAMINHO];

    if(argc != 5) {
        printf("Uso: experimento_busca_local_reinsercao_adaptativa.exe arquivo fator_h id_instancia sufixo\n");

        return 1;
    }

    fatorH = (FatorH) atoi(argv[2]);
    identificadorDaInstancia = (InteiroPositivoDe16Bits) atoi(argv[3]);

    if(fatorH != FATOR_H_02 && fatorH != FATOR_H_04 && fatorH != FATOR_H_06 && fatorH != FATOR_H_08) {
        printf("Fator h invalido.\n");

        return 1;
    }

    if(identificadorDaInstancia == 0 || identificadorDaInstancia > QUANTIDADE_DE_PROBLEMAS_POR_ARQUIVO) {
        printf("Identificador de instancia invalido.\n");

        return 1;
    }

    if(snprintf(caminhoResultado,sizeof(caminhoResultado),"resultados/validacao_busca_local_reinsercao_adaptativa_%s.csv",argv[4]) < 0) {
        return 1;
    }

    instancia = criarInstanciaVazia();
    heuristica = criarHeuristicaPorInsercaoTemporal();
    solucaoConstrutiva = criarSolucaoVazia();
    solucaoBuscaLocal = criarSolucaoVazia();
    resultadoBuscaLocal = criarResultadoBuscaLocalVazio();
    custoConstrutiva = 0;
    custoBuscaLocal = 0;

    if(heuristicaEhValida(&heuristica) == FALSO) {
        return 1;
    }

    if(instanciaDaoLerInstanciaPorIdentificador(argv[1],identificadorDaInstancia,&instancia) == FALSO) {
        printf("Falha ao carregar a instancia.\n");

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        liberarInstancia(&instancia);

        return 1;
    }

    inicio = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerHeuristicaConstruirSolucao(&instancia,&heuristica,fatorH,&solucaoConstrutiva) == FALSO) {
        liberarInstancia(&instancia);

        return 1;
    }

    fim = gerenciadorDeTempoObterTempoAtualEmSegundos();
    tempoConstrutivaMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(inicio,fim));

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoConstrutiva,dataDeEntregaComum,&custoConstrutiva) == FALSO) {
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    inicio = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerBuscaLocalReinsercaoAdaptativaMelhorarSolucao(&instancia,fatorH,&solucaoConstrutiva,&solucaoBuscaLocal,&resultadoBuscaLocal) == FALSO) {
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    fim = gerenciadorDeTempoObterTempoAtualEmSegundos();
    tempoBuscaLocalMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(inicio,fim));

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoBuscaLocal,dataDeEntregaComum,&custoBuscaLocal) == FALSO) {
        liberarSolucao(&solucaoBuscaLocal);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(custoBuscaLocal != resultadoBuscaLocal.custoFinal || custoBuscaLocal > custoConstrutiva) {
        printf("Falha na validacao independente do custo.\n");
        printf("Construtiva: %llu\n",(unsigned long long) custoConstrutiva);
        printf("Busca local registrada: %llu\n",(unsigned long long) resultadoBuscaLocal.custoFinal);
        printf("Busca local verificada: %llu\n",(unsigned long long) custoBuscaLocal);
        liberarSolucao(&solucaoBuscaLocal);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(criarArquivoDeResultado(caminhoResultado) == FALSO || adicionarResultado(caminhoResultado,&instancia,fatorH,custoConstrutiva,custoBuscaLocal,tempoConstrutivaMs,tempoBuscaLocalMs,&resultadoBuscaLocal) == FALSO) {
        liberarSolucao(&solucaoBuscaLocal);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    printf(
        "arquivo=%s | instancia=%u | n=%u | h=%.1f | vizinhanca=%s | construtiva=%llu | busca_local=%llu | melhora=%.6f%% | tempo_ms=%.3f | iteracoes=%u | vizinhos=%u | reinsercoes=%u\n",
        argv[1],
        (unsigned int) identificadorDaInstancia,
        (unsigned int) instancia.quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        obterTipoDeVizinhanca(instancia.quantidadeDeTarefas),
        (unsigned long long) custoConstrutiva,
        (unsigned long long) custoBuscaLocal,
        custoConstrutiva > 0 ? ((((double) custoConstrutiva) - ((double) custoBuscaLocal)) * 100.0) / ((double) custoConstrutiva) : 0.0,
        tempoBuscaLocalMs,
        (unsigned int) resultadoBuscaLocal.quantidadeDeIteracoes,
        (unsigned int) resultadoBuscaLocal.quantidadeDeVizinhosAvaliados,
        (unsigned int) resultadoBuscaLocal.quantidadeDeMelhoriasPorReinsercao
    );

    liberarSolucao(&solucaoBuscaLocal);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return 0;
}