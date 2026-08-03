#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_busca_local_melhor_melhoria/controller_busca_local_melhor_melhoria.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>
#include <stdlib.h>

static QuantidadeDeTarefas obterRaio(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 200) {
        return 20;
    }

    if(quantidadeDeTarefas <= 500) {
        return 12;
    }

    return 4;
}

static Boolean criarArquivo(const char *caminho) {
    FILE *arquivo;

    arquivo = fopen(caminho,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(arquivo,"arquivo,n,h,raio,custoConstrutiva,custoPrimeiraMelhoria,custoMelhorMelhoria,ganhoMelhorSobrePrimeiraPercentual,melhoriaPrimeiraSobreConstrutivaPercentual,melhoriaMelhorSobreConstrutivaPercentual,tempoPrimeiraMelhoriaMs,tempoMelhorMelhoriaMs,iteracoesPrimeira,iteracoesMelhor,vizinhosPrimeira,vizinhosMelhor,melhoriasReinsercaoMelhor,melhoriasTrocaMelhor\n");
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean adicionarLinha(const char *caminho,const char *nomeDoArquivo,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,QuantidadeDeTarefas raio,const ResultadoBuscaLocal *resultadoPrimeira,const ResultadoBuscaLocal *resultadoMelhor,TempoComputacionalEmMilissegundos tempoPrimeira,TempoComputacionalEmMilissegundos tempoMelhor) {
    FILE *arquivo;
    double ganhoMelhor;
    double melhoriaPrimeira;
    double melhoriaMelhor;

    arquivo = fopen(caminho,"a");

    if(arquivo == NULL) {
        return FALSO;
    }

    ganhoMelhor = (*resultadoPrimeira).custoFinal > 0 ? ((((double) (*resultadoPrimeira).custoFinal) - ((double) (*resultadoMelhor).custoFinal)) * 100.0) / ((double) (*resultadoPrimeira).custoFinal) : 0.0;
    melhoriaPrimeira = (*resultadoPrimeira).custoInicial > 0 ? ((((double) (*resultadoPrimeira).custoInicial) - ((double) (*resultadoPrimeira).custoFinal)) * 100.0) / ((double) (*resultadoPrimeira).custoInicial) : 0.0;
    melhoriaMelhor = (*resultadoMelhor).custoInicial > 0 ? ((((double) (*resultadoMelhor).custoInicial) - ((double) (*resultadoMelhor).custoFinal)) * 100.0) / ((double) (*resultadoMelhor).custoInicial) : 0.0;

    fprintf(arquivo,"%s,%u,%.1f,%u,%llu,%llu,%llu,%.9f,%.9f,%.9f,%.9f,%.9f,%u,%u,%u,%u,%u,%u\n",nomeDoArquivo,(unsigned int) quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) (*resultadoPrimeira).custoInicial,(unsigned long long) (*resultadoPrimeira).custoFinal,(unsigned long long) (*resultadoMelhor).custoFinal,ganhoMelhor,melhoriaPrimeira,melhoriaMelhor,tempoPrimeira,tempoMelhor,(unsigned int) (*resultadoPrimeira).quantidadeDeIteracoes,(unsigned int) (*resultadoMelhor).quantidadeDeIteracoes,(unsigned int) (*resultadoPrimeira).quantidadeDeVizinhosAvaliados,(unsigned int) (*resultadoMelhor).quantidadeDeVizinhosAvaliados,(unsigned int) (*resultadoMelhor).quantidadeDeMelhoriasPorReinsercao,(unsigned int) (*resultadoMelhor).quantidadeDeMelhoriasPorTroca);
    fclose(arquivo);

    return VERDADEIRO;
}

int main(int argc,char **argv) {
    Instancia instancia;
    Heuristica heuristica;
    Solucao solucaoConstrutiva;
    Solucao solucaoPrimeira;
    Solucao solucaoMelhor;
    ResultadoBuscaLocal resultadoPrimeira;
    ResultadoBuscaLocal resultadoMelhor;
    FatorH fatorH;
    QuantidadeDeTarefas raio;
    DataDeEntregaComum dataDeEntregaComum;
    Custo custoVerificado;
    TempoComputacionalEmSegundos inicio;
    TempoComputacionalEmSegundos fim;
    TempoComputacionalEmMilissegundos tempoPrimeira;
    TempoComputacionalEmMilissegundos tempoMelhor;
    double ganhoMelhor;

    if(argc != 4) {
        printf("Uso: experimento_busca_local_melhor_melhoria.exe arquivo fator_h caminho_saida\n");

        return 1;
    }

    fatorH = (FatorH) atoi(argv[2]);

    if(fatorH != FATOR_H_02 && fatorH != FATOR_H_04) {
        printf("Fator h invalido.\n");

        return 1;
    }

    instancia = criarInstanciaVazia();
    solucaoConstrutiva = criarSolucaoVazia();
    solucaoPrimeira = criarSolucaoVazia();
    solucaoMelhor = criarSolucaoVazia();
    resultadoPrimeira = criarResultadoBuscaLocalVazio();
    resultadoMelhor = criarResultadoBuscaLocalVazio();
    heuristica = criarHeuristicaPorInsercaoTemporal();

    if(heuristicaEhValida(&heuristica) == FALSO) {
        return 1;
    }

    if(criarArquivo(argv[3]) == FALSO) {
        return 1;
    }

    if(instanciaDaoLerInstanciaPorIdentificador(argv[1],1,&instancia) == FALSO) {
        return 1;
    }

    if(controllerHeuristicaConstruirSolucao(&instancia,&heuristica,fatorH,&solucaoConstrutiva) == FALSO) {
        liberarInstancia(&instancia);

        return 1;
    }

    raio = obterRaio(instancia.quantidadeDeTarefas);

    inicio = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(&instancia,fatorH,&solucaoConstrutiva,&solucaoPrimeira,&resultadoPrimeira,raio,raio) == FALSO) {
        return 1;
    }

    fim = gerenciadorDeTempoObterTempoAtualEmSegundos();
    tempoPrimeira = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(inicio,fim));

    inicio = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerBuscaLocalMelhorarSolucaoComMelhorMelhoria(&instancia,fatorH,&solucaoConstrutiva,&solucaoMelhor,&resultadoMelhor,raio,raio) == FALSO) {
        return 1;
    }

    fim = gerenciadorDeTempoObterTempoAtualEmSegundos();
    tempoMelhor = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(inicio,fim));

    if(solucaoEhValida(&solucaoMelhor) == FALSO) {
        printf("Solucao final invalida.\n");

        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,fatorH);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoMelhor,dataDeEntregaComum,&custoVerificado) == FALSO) {
        return 1;
    }

    if(custoVerificado != resultadoMelhor.custoFinal) {
        printf("Custo da melhor melhoria diferente do custo verificado.\n");

        return 1;
    }

    if(adicionarLinha(argv[3],argv[1],instancia.quantidadeDeTarefas,fatorH,raio,&resultadoPrimeira,&resultadoMelhor,tempoPrimeira,tempoMelhor) == FALSO) {
        return 1;
    }

    ganhoMelhor = resultadoPrimeira.custoFinal > 0 ? ((((double) resultadoPrimeira.custoFinal) - ((double) resultadoMelhor.custoFinal)) * 100.0) / ((double) resultadoPrimeira.custoFinal) : 0.0;

    printf("arquivo=%s | n=%u | h=%.1f | raio=%u | construtiva=%llu | primeira=%llu | melhor=%llu | ganho_melhor=%.4f%% | tempo_primeira_ms=%.3f | tempo_melhor_ms=%.3f | iteracoes_melhor=%u | vizinhos_melhor=%u\n",argv[1],(unsigned int) instancia.quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) resultadoMelhor.custoInicial,(unsigned long long) resultadoPrimeira.custoFinal,(unsigned long long) resultadoMelhor.custoFinal,ganhoMelhor,tempoPrimeira,tempoMelhor,(unsigned int) resultadoMelhor.quantidadeDeIteracoes,(unsigned int) resultadoMelhor.quantidadeDeVizinhosAvaliados);

    liberarSolucao(&solucaoMelhor);
    liberarSolucao(&solucaoPrimeira);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return 0;
}