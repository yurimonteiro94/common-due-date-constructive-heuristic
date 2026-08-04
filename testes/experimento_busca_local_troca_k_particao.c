#include "../controller/controller_busca_local_troca_k_particao/controller_busca_local_troca_k_particao.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>
#include <stdlib.h>

static QuantidadeDeTarefas obterLimiteDeCandidatosParaDuasTrocas(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 100) {
        return LIMITE_CANDIDATOS_DUAS_TROCAS_100;
    }

    if(quantidadeDeTarefas <= 200) {
        return LIMITE_CANDIDATOS_DUAS_TROCAS_200;
    }

    if(quantidadeDeTarefas <= 500) {
        return LIMITE_CANDIDATOS_DUAS_TROCAS_500;
    }

    return LIMITE_CANDIDATOS_DUAS_TROCAS_1000;
}

static QuantidadeDeTarefas obterLimiteDeCandidatosParaTresTrocas(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 20) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_20;
    }

    if(quantidadeDeTarefas <= 50) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_50;
    }

    if(quantidadeDeTarefas <= 100) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_100;
    }

    if(quantidadeDeTarefas <= 200) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_200;
    }

    if(quantidadeDeTarefas <= 500) {
        return LIMITE_CANDIDATOS_TRES_TROCAS_500;
    }

    return LIMITE_CANDIDATOS_TRES_TROCAS_1000;
}

static Boolean criarArquivoDeResultado(const char *caminho) {
    FILE *arquivo;

    arquivo = fopen(caminho,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(
        arquivo,
        "arquivo,idInstancia,n,h,raioTroca,limiteCandidatosDuasTrocasPorLado,limiteCandidatosTresTrocasPorLado,custoConstrutiva,custoBuscaLocal,melhoriaSobreConstrutivaPercentual,tempoConstrutivaMs,tempoBuscaLocalMs,iteracoes,vizinhosAvaliados,vizinhosUmaTroca,vizinhosDuasOuTresTrocas,movimentosUmaTrocaAceitos,movimentosDuasOuTresTrocasAceitos\n"
    );

    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean adicionarResultado(const char *caminho,const Instancia *instancia,FatorH fatorH,Custo custoConstrutiva,Custo custoBuscaLocal,TempoComputacionalEmMilissegundos tempoConstrutivaMs,TempoComputacionalEmMilissegundos tempoBuscaLocalMs,const ResultadoBuscaLocal *resultadoBuscaLocal) {
    FILE *arquivo;
    double melhoriaPercentual;

    if(caminho == NULL || instancia == NULL || resultadoBuscaLocal == NULL) {
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
        "%s,%u,%u,%.1f,3,%u,%u,%llu,%llu,%.9f,%.9f,%.9f,%u,%u,%u,%u,%u,%u\n",
        (*instancia).nomeDoArquivoDeOrigem,
        (unsigned int) (*instancia).identificadorDaInstancia,
        (unsigned int) (*instancia).quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        (unsigned int) obterLimiteDeCandidatosParaDuasTrocas((*instancia).quantidadeDeTarefas),
        (unsigned int) obterLimiteDeCandidatosParaTresTrocas((*instancia).quantidadeDeTarefas),
        (unsigned long long) custoConstrutiva,
        (unsigned long long) custoBuscaLocal,
        melhoriaPercentual,
        tempoConstrutivaMs,
        tempoBuscaLocalMs,
        (unsigned int) (*resultadoBuscaLocal).quantidadeDeIteracoes,
        (unsigned int) (*resultadoBuscaLocal).quantidadeDeVizinhosAvaliados,
        (unsigned int) (*resultadoBuscaLocal).quantidadeDeVizinhosPorReinsercao,
        (unsigned int) (*resultadoBuscaLocal).quantidadeDeVizinhosPorTroca,
        (unsigned int) (*resultadoBuscaLocal).quantidadeDeMelhoriasPorReinsercao,
        (unsigned int) (*resultadoBuscaLocal).quantidadeDeMelhoriasPorTroca
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
        printf("Uso: experimento_busca_local_troca_k_particao.exe arquivo fator_h id_instancia sufixo\n");

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

    if(snprintf(caminhoResultado,sizeof(caminhoResultado),"resultados/validacao_busca_local_troca_k_particao_%s.csv",argv[4]) < 0) {
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

    if(controllerBuscaLocalTrocaKParticaoMelhorarSolucao(&instancia,fatorH,&solucaoConstrutiva,&solucaoBuscaLocal,&resultadoBuscaLocal) == FALSO) {
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
        "arquivo=%s | instancia=%u | n=%u | h=%.1f | raio=3 | candidatos_2_por_lado=%u | candidatos_3_por_lado=%u | construtiva=%llu | busca_local=%llu | melhora=%.6f%% | tempo_ms=%.3f | iteracoes=%u | vizinhos=%u | simples=%u | compostos=%u\n",
        argv[1],
        (unsigned int) identificadorDaInstancia,
        (unsigned int) instancia.quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        (unsigned int) obterLimiteDeCandidatosParaDuasTrocas(instancia.quantidadeDeTarefas),
        (unsigned int) obterLimiteDeCandidatosParaTresTrocas(instancia.quantidadeDeTarefas),
        (unsigned long long) custoConstrutiva,
        (unsigned long long) custoBuscaLocal,
        custoConstrutiva > 0 ? ((((double) custoConstrutiva) - ((double) custoBuscaLocal)) * 100.0) / ((double) custoConstrutiva) : 0.0,
        tempoBuscaLocalMs,
        (unsigned int) resultadoBuscaLocal.quantidadeDeIteracoes,
        (unsigned int) resultadoBuscaLocal.quantidadeDeVizinhosAvaliados,
        (unsigned int) resultadoBuscaLocal.quantidadeDeMelhoriasPorReinsercao,
        (unsigned int) resultadoBuscaLocal.quantidadeDeMelhoriasPorTroca
    );

    liberarSolucao(&solucaoBuscaLocal);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return 0;
}