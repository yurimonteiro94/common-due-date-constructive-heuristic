#include "../controller/controller_busca_local/controller_busca_local.h"
#include "../controller/controller_busca_local_vshape/controller_busca_local_vshape.h"
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

    fprintf(arquivo,"arquivo,n,h,raio,custoConstrutiva,custoComposta,custoVShape,ganhoVShapeSobreCompostaPercentual,melhoriaVShapeSobreConstrutivaPercentual,chamadasComposta,vizinhosComposta,vizinhosOrdenacao,vizinhosFronteira,melhoriasOrdenacao,melhoriasFronteira\n");
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean adicionarLinha(const char *caminho,const char *nomeDoArquivo,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,QuantidadeDeTarefas raio,const ResultadoBuscaLocal *resultadoComposta,const ResultadoBuscaLocalVShape *resultadoVShape) {
    FILE *arquivo;
    double ganhoVShape;
    double melhoriaVShape;

    arquivo = fopen(caminho,"a");

    if(arquivo == NULL) {
        return FALSO;
    }

    ganhoVShape = resultadoComposta->custoFinal > 0 ? ((((double) resultadoComposta->custoFinal) - ((double) resultadoVShape->custoFinal)) * 100.0) / ((double) resultadoComposta->custoFinal) : 0.0;
    melhoriaVShape = resultadoVShape->custoInicial > 0 ? ((((double) resultadoVShape->custoInicial) - ((double) resultadoVShape->custoFinal)) * 100.0) / ((double) resultadoVShape->custoInicial) : 0.0;

    fprintf(arquivo,"%s,%u,%.1f,%u,%llu,%llu,%llu,%.9f,%.9f,%u,%u,%u,%u,%u,%u\n",nomeDoArquivo,(unsigned int) quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) resultadoVShape->custoInicial,(unsigned long long) resultadoComposta->custoFinal,(unsigned long long) resultadoVShape->custoFinal,ganhoVShape,melhoriaVShape,(unsigned int) resultadoVShape->quantidadeDeChamadasDaBuscaComposta,(unsigned int) resultadoVShape->quantidadeDeVizinhosDaBuscaComposta,(unsigned int) resultadoVShape->quantidadeDeVizinhosDeOrdenacao,(unsigned int) resultadoVShape->quantidadeDeVizinhosDeFronteira,(unsigned int) resultadoVShape->quantidadeDeMelhoriasDeOrdenacao,(unsigned int) resultadoVShape->quantidadeDeMelhoriasDeFronteira);
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean executarCombinacao(const char *nomeDoArquivo,const char *caminho,FatorH fatorH,const Heuristica *heuristica) {
    Instancia instancia;
    Solucao solucaoConstrutiva;
    Solucao solucaoComposta;
    Solucao solucaoVShape;
    ResultadoBuscaLocal resultadoComposta;
    ResultadoBuscaLocalVShape resultadoVShape;
    QuantidadeDeTarefas raio;
    double ganhoVShape;

    instancia = criarInstanciaVazia();
    solucaoConstrutiva = criarSolucaoVazia();
    solucaoComposta = criarSolucaoVazia();
    solucaoVShape = criarSolucaoVazia();
    resultadoComposta = criarResultadoBuscaLocalVazio();
    resultadoVShape = criarResultadoBuscaLocalVShapeVazio();

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

    if(controllerBuscaLocalVShapeMelhorarSolucao(&instancia,fatorH,&solucaoConstrutiva,&solucaoVShape,&resultadoVShape,raio,raio) == FALSO) {
        liberarSolucao(&solucaoComposta);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    if(resultadoVShape.custoFinal > resultadoComposta.custoFinal) {
        printf("Erro: busca V-shaped pior que a busca composta.\n");
        liberarSolucao(&solucaoVShape);
        liberarSolucao(&solucaoComposta);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    if(adicionarLinha(caminho,nomeDoArquivo,instancia.quantidadeDeTarefas,fatorH,raio,&resultadoComposta,&resultadoVShape) == FALSO) {
        liberarSolucao(&solucaoVShape);
        liberarSolucao(&solucaoComposta);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return FALSO;
    }

    ganhoVShape = resultadoComposta.custoFinal > 0 ? ((((double) resultadoComposta.custoFinal) - ((double) resultadoVShape.custoFinal)) * 100.0) / ((double) resultadoComposta.custoFinal) : 0.0;

    printf("arquivo=%s | n=%u | h=%.1f | raio=%u | construtiva=%llu | composta=%llu | vshape=%llu | ganho_vshape=%.4f%% | ordenacao=%u | fronteira=%u\n",nomeDoArquivo,(unsigned int) instancia.quantidadeDeTarefas,((double) fatorH) / ((double) FATOR_DE_ESCALA_H),(unsigned int) raio,(unsigned long long) resultadoVShape.custoInicial,(unsigned long long) resultadoComposta.custoFinal,(unsigned long long) resultadoVShape.custoFinal,ganhoVShape,(unsigned int) resultadoVShape.quantidadeDeMelhoriasDeOrdenacao,(unsigned int) resultadoVShape.quantidadeDeMelhoriasDeFronteira);

    liberarSolucao(&solucaoVShape);
    liberarSolucao(&solucaoComposta);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return VERDADEIRO;
}

int main(int argc,char **argv) {
    Heuristica heuristica;

    if(argc != 3) {
        printf("Uso: experimento_busca_local_vshape.exe arquivo_sch caminho_saida\n");

        return 1;
    }

    heuristica = criarHeuristicaPorInsercaoTemporal();

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("Heuristica invalida.\n");

        return 1;
    }

    if(criarArquivo(argv[2]) == FALSO) {
        printf("Nao foi possivel criar o arquivo de saida.\n");

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