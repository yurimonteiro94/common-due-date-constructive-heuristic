#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_busca_local_estrutural/controller_busca_local_estrutural.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"

#include <stdio.h>

static QuantidadeDeTarefas obterRaio(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 200) {
        return 20;
    }

    if(quantidadeDeTarefas <= 500) {
        return 12;
    }

    return 6;
}

static Boolean criarArquivo(const char *caminho) {
    FILE *arquivo;

    arquivo = fopen(caminho,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(arquivo,"arquivo,n,h,raio,custoConstrutiva,custoComposta,custoEstrutural,ganhoEstruturalSobreCompostaPercentual,melhoriaEstruturalSobreConstrutivaPercentual,chamadasComposta,vizinhosComposta,vizinhosBlocoDois,vizinhosBlocoTres,melhoriasBlocoDois,melhoriasBlocoTres\n");
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean adicionarLinha(const char *caminho,const char *nomeDoArquivo,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,QuantidadeDeTarefas raio,const ResultadoBuscaLocal *resultadoComposta,const ResultadoBuscaLocalEstrutural *resultadoEstrutural) {
    FILE *arquivo;
    double ganhoEstrutural;
    double melhoriaEstrutural;

    arquivo = fopen(caminho,"a");

    if(arquivo == NULL) {
        return FALSO;
    }

    ganhoEstrutural = resultadoComposta->custoFinal > 0 ? ((((double) resultadoComposta->custoFinal) - ((double) resultadoEstrutural->custoFinal)) * 100.0) / ((double) resultadoComposta->custoFinal) : 0.0;
    melhoriaEstrutural = resultadoEstrutural->custoInicial > 0 ? ((((double) resultadoEstrutural->custoInicial) - ((double) resultadoEstrutural->custoFinal)) * 100.0) / ((double) resultadoEstrutural->custoInicial) : 0.0;

    fprintf(arquivo,"%s,%u,%.1f,%u,%llu,%llu,%llu,%.9f,%.9f,%u,%u,%u,%u,%u,%u\n",nomeDoArquivo,(unsigned int) quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) resultadoEstrutural->custoInicial,(unsigned long long) resultadoComposta->custoFinal,(unsigned long long) resultadoEstrutural->custoFinal,ganhoEstrutural,melhoriaEstrutural,(unsigned int) resultadoEstrutural->quantidadeDeChamadasDaBuscaComposta,(unsigned int) resultadoEstrutural->quantidadeDeVizinhosDaBuscaComposta,(unsigned int) resultadoEstrutural->quantidadeDeVizinhosDeBlocoDois,(unsigned int) resultadoEstrutural->quantidadeDeVizinhosDeBlocoTres,(unsigned int) resultadoEstrutural->quantidadeDeMelhoriasDeBlocoDois,(unsigned int) resultadoEstrutural->quantidadeDeMelhoriasDeBlocoTres);
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean executarCombinacao(const char *nomeDoArquivo,const char *caminhoDeSaida,FatorH fatorH,const Heuristica *heuristica) {
    Instancia instancia;
    Solucao solucaoConstrutiva;
    Solucao solucaoComposta;
    Solucao solucaoEstrutural;
    ResultadoBuscaLocal resultadoComposta;
    ResultadoBuscaLocalEstrutural resultadoEstrutural;
    QuantidadeDeTarefas raio;
    double ganhoEstrutural;

    instancia = criarInstanciaVazia();
    solucaoConstrutiva = criarSolucaoVazia();
    solucaoComposta = criarSolucaoVazia();
    solucaoEstrutural = criarSolucaoVazia();
    resultadoComposta = criarResultadoBuscaLocalVazio();
    resultadoEstrutural = criarResultadoBuscaLocalEstruturalVazio();

    if(instanciaDaoLerInstanciaPorIdentificador(nomeDoArquivo,1,&instancia) == FALSO) {
        return FALSO;
    }

    if(controllerHeuristicaConstruirSolucao(&instancia,heuristica,fatorH,&solucaoConstrutiva) == FALSO) {
        liberarInstancia(&instancia);

        return FALSO;
    }

    raio = obterRaio(instancia.quantidadeDeTarefas);

    if(controllerBuscaLocalMelhorarSolucaoComVizinhancaComposta(&instancia,fatorH,&solucaoConstrutiva,&solucaoComposta,&resultadoComposta,raio,raio) == FALSO) {
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    if(controllerBuscaLocalEstruturalMelhorarSolucao(&instancia,fatorH,&solucaoConstrutiva,&solucaoEstrutural,&resultadoEstrutural,raio,raio,raio) == FALSO) {
        liberarSolucao(&solucaoComposta);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    if(resultadoEstrutural.custoFinal > resultadoComposta.custoFinal) {
        printf("Erro: busca estrutural pior que a busca composta.\n");
        liberarSolucao(&solucaoEstrutural);
        liberarSolucao(&solucaoComposta);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    if(adicionarLinha(caminhoDeSaida,nomeDoArquivo,instancia.quantidadeDeTarefas,fatorH,raio,&resultadoComposta,&resultadoEstrutural) == FALSO) {
        liberarSolucao(&solucaoEstrutural);
        liberarSolucao(&solucaoComposta);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    ganhoEstrutural = resultadoComposta.custoFinal > 0 ? ((((double) resultadoComposta.custoFinal) - ((double) resultadoEstrutural.custoFinal)) * 100.0) / ((double) resultadoComposta.custoFinal) : 0.0;

    printf("arquivo=%s | n=%u | h=%.1f | raio=%u | construtiva=%llu | composta=%llu | estrutural=%llu | ganho_estrutural=%.4f%% | bloco2=%u | bloco3=%u\n",nomeDoArquivo,(unsigned int) instancia.quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) resultadoEstrutural.custoInicial,(unsigned long long) resultadoComposta.custoFinal,(unsigned long long) resultadoEstrutural.custoFinal,ganhoEstrutural,(unsigned int) resultadoEstrutural.quantidadeDeMelhoriasDeBlocoDois,(unsigned int) resultadoEstrutural.quantidadeDeMelhoriasDeBlocoTres);

    liberarSolucao(&solucaoEstrutural);
    liberarSolucao(&solucaoComposta);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return VERDADEIRO;
}

int main(int argc,char **argv) {
    Heuristica heuristica;

    if(argc != 3) {
        printf("Uso: experimento_busca_local_estrutural.exe arquivo_sch caminho_saida\n");

        return 1;
    }

    heuristica = criarHeuristicaPorInsercaoTemporal();

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("Heuristica invalida.\n");

        return 1;
    }

    if(criarArquivo(argv[2]) == FALSO) {
        printf("Nao foi possivel criar o arquivo %s.\n",argv[2]);

        return 1;
    }

    if(executarCombinacao(argv[1],argv[2],FATOR_H_02,&heuristica) == FALSO) {
        return 1;
    }

    if(executarCombinacao(argv[1],argv[2],FATOR_H_04,&heuristica) == FALSO) {
        return 1;
    }

    return 0;
}