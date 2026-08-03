#include "../controller/controller_busca_local_inversao/controller_busca_local_inversao.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_CAMINHO_RESULTADO_INVERSAO 256

static Boolean textoEhIgual(const char *primeiroTexto,const char *segundoTexto) {
    if(primeiroTexto == NULL || segundoTexto == NULL) {
        return FALSO;
    }

    return strcmp(primeiroTexto,segundoTexto) == 0 ? VERDADEIRO : FALSO;
}

static Boolean obterFatorH(const char *texto,FatorH *fatorH) {
    if(texto == NULL || fatorH == NULL) {
        return FALSO;
    }

    if(textoEhIgual(texto,"2") == VERDADEIRO || textoEhIgual(texto,"0.2") == VERDADEIRO) {
        (*fatorH) = FATOR_H_02;
        return VERDADEIRO;
    }

    if(textoEhIgual(texto,"4") == VERDADEIRO || textoEhIgual(texto,"0.4") == VERDADEIRO) {
        (*fatorH) = FATOR_H_04;
        return VERDADEIRO;
    }

    if(textoEhIgual(texto,"6") == VERDADEIRO || textoEhIgual(texto,"0.6") == VERDADEIRO) {
        (*fatorH) = FATOR_H_06;
        return VERDADEIRO;
    }

    if(textoEhIgual(texto,"8") == VERDADEIRO || textoEhIgual(texto,"0.8") == VERDADEIRO) {
        (*fatorH) = FATOR_H_08;
        return VERDADEIRO;
    }

    return FALSO;
}

static QuantidadeDeTarefas obterRaio(QuantidadeDeTarefas quantidadeDeTarefas) {
    if(quantidadeDeTarefas <= 200) {
        return 20;
    }

    if(quantidadeDeTarefas <= 500) {
        return 12;
    }

    return 4;
}

static Boolean montarCaminhoDoResultado(char *caminhoDoResultado,const char *sufixo) {
    int quantidadeDeCaracteres;

    if(caminhoDoResultado == NULL || sufixo == NULL) {
        return FALSO;
    }

    quantidadeDeCaracteres = snprintf(caminhoDoResultado,TAMANHO_CAMINHO_RESULTADO_INVERSAO,"resultados/validacao_busca_local_inversao_%s.csv",sufixo);

    if(quantidadeDeCaracteres < 0 || quantidadeDeCaracteres >= TAMANHO_CAMINHO_RESULTADO_INVERSAO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean criarArquivoDeResultado(const char *caminhoDoResultado) {
    FILE *arquivo;

    arquivo = fopen(caminhoDoResultado,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(arquivo,"arquivo,idInstancia,n,h,raio,custoConstrutiva,custoHibrida,custoInversao,ganhoInversaoSobreHibridaPercentual,melhoriaInversaoSobreConstrutivaPercentual,tempoInversaoMs,ciclos,iteracoesInversao,vizinhosInversao,inversoesAceitas,chamadasBuscaComposta,vizinhosBuscaComposta,melhoriasReinsercao,melhoriasTroca\n");
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean adicionarResultado(const char *caminhoDoResultado,const char *nomeDoArquivo,InteiroPositivoDe16Bits identificadorDaInstancia,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,QuantidadeDeTarefas raio,TempoComputacionalEmMilissegundos tempoInversaoMs,const ResultadoBuscaLocalInversao *resultado) {
    FILE *arquivo;
    double ganhoSobreHibrida;
    double melhoriaSobreConstrutiva;

    if(caminhoDoResultado == NULL || nomeDoArquivo == NULL || resultado == NULL) {
        return FALSO;
    }

    arquivo = fopen(caminhoDoResultado,"a");

    if(arquivo == NULL) {
        return FALSO;
    }

    ganhoSobreHibrida = 0.0;
    melhoriaSobreConstrutiva = 0.0;

    if((*resultado).custoAposBuscaHibrida > 0) {
        ganhoSobreHibrida = ((((double) (*resultado).custoAposBuscaHibrida) - ((double) (*resultado).custoFinal)) * 100.0) / ((double) (*resultado).custoAposBuscaHibrida);
    }

    if((*resultado).custoInicial > 0) {
        melhoriaSobreConstrutiva = ((((double) (*resultado).custoInicial) - ((double) (*resultado).custoFinal)) * 100.0) / ((double) (*resultado).custoInicial);
    }

    fprintf(arquivo,"%s,%u,%u,%.1f,%u,%llu,%llu,%llu,%.9f,%.9f,%.9f,%u,%u,%llu,%u,%u,%llu,%u,%u\n",
        nomeDoArquivo,
        (unsigned int) identificadorDaInstancia,
        (unsigned int) quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        (unsigned int) raio,
        (unsigned long long) (*resultado).custoInicial,
        (unsigned long long) (*resultado).custoAposBuscaHibrida,
        (unsigned long long) (*resultado).custoFinal,
        ganhoSobreHibrida,
        melhoriaSobreConstrutiva,
        tempoInversaoMs,
        (unsigned int) (*resultado).quantidadeDeCiclos,
        (unsigned int) (*resultado).quantidadeDeIteracoesDeInversao,
        (unsigned long long) (*resultado).quantidadeDeVizinhosPorInversao,
        (unsigned int) (*resultado).quantidadeDeInversoesAceitas,
        (unsigned int) (*resultado).quantidadeDeChamadasDaBuscaComposta,
        (unsigned long long) (*resultado).quantidadeDeVizinhosDaBuscaComposta,
        (unsigned int) (*resultado).quantidadeDeMelhoriasPorReinsercao,
        (unsigned int) (*resultado).quantidadeDeMelhoriasPorTroca
    );

    fclose(arquivo);

    return VERDADEIRO;
}

int main(int quantidadeDeArgumentos,char *argumentos[]) {
    const char *nomeDoArquivo;
    const char *sufixo;
    char caminhoDoResultado[TAMANHO_CAMINHO_RESULTADO_INVERSAO];
    Instancia instancia;
    Heuristica heuristica;
    Solucao solucaoConstrutiva;
    Solucao solucaoInversao;
    ResultadoBuscaLocalInversao resultado;
    FatorH fatorH;
    DataDeEntregaComum dataDeEntregaComum;
    QuantidadeDeTarefas raio;
    Custo custoConstrutiva;
    Custo custoVerificado;
    TempoComputacionalEmSegundos tempoInicial;
    TempoComputacionalEmSegundos tempoFinal;
    TempoComputacionalEmMilissegundos tempoInversaoMs;
    InteiroPositivoDe16Bits identificadorDaInstancia;
    double ganhoSobreHibrida;

    if(quantidadeDeArgumentos != 5) {
        printf("Uso: experimento_busca_local_inversao.exe arquivo fatorH idInstancia sufixo\n");
        return 1;
    }

    nomeDoArquivo = argumentos[1];
    identificadorDaInstancia = (InteiroPositivoDe16Bits) strtoul(argumentos[3],NULL,10);
    sufixo = argumentos[4];

    if(obterFatorH(argumentos[2],&fatorH) == FALSO || identificadorDaInstancia == 0) {
        printf("Parametros invalidos.\n");
        return 1;
    }

    if(montarCaminhoDoResultado(caminhoDoResultado,sufixo) == FALSO) {
        printf("Falha ao montar caminho do resultado.\n");
        return 1;
    }

    instancia = criarInstanciaVazia();
    solucaoConstrutiva = criarSolucaoVazia();
    solucaoInversao = criarSolucaoVazia();
    resultado = criarResultadoBuscaLocalInversaoVazio();
    heuristica = criarHeuristicaPorInsercaoTemporal();
    custoConstrutiva = 0;
    custoVerificado = 0;

    if(heuristicaEhValida(&heuristica) == FALSO) {
        printf("Heuristica invalida.\n");
        return 1;
    }

    if(instanciaDaoLerInstanciaPorIdentificador(nomeDoArquivo,identificadorDaInstancia,&instancia) == FALSO) {
        printf("Falha ao ler instancia.\n");
        return 1;
    }

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(&instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        printf("Data de entrega comum invalida.\n");
        liberarInstancia(&instancia);
        return 1;
    }

    if(controllerHeuristicaConstruirSolucao(&instancia,&heuristica,fatorH,&solucaoConstrutiva) == FALSO) {
        printf("Falha na heuristica construtiva.\n");
        liberarInstancia(&instancia);
        return 1;
    }

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoConstrutiva,dataDeEntregaComum,&custoConstrutiva) == FALSO) {
        printf("Falha ao calcular custo construtivo.\n");
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);
        return 1;
    }

    raio = obterRaio(instancia.quantidadeDeTarefas);
    tempoInicial = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerBuscaLocalInversaoMelhorarSolucao(&instancia,fatorH,&solucaoConstrutiva,&solucaoInversao,&resultado,raio,raio,raio) == FALSO) {
        printf("Falha na busca local por inversao.\n");
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);
        return 1;
    }

    tempoFinal = gerenciadorDeTempoObterTempoAtualEmSegundos();
    tempoInversaoMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicial,tempoFinal));

    if(solucaoEhValida(&solucaoInversao) == FALSO) {
        printf("Solucao por inversao invalida.\n");
        liberarSolucao(&solucaoInversao);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);
        return 1;
    }

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoInversao,dataDeEntregaComum,&custoVerificado) == FALSO || custoVerificado != resultado.custoFinal) {
        printf("Custo final invalido.\n");
        liberarSolucao(&solucaoInversao);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);
        return 1;
    }

    if(resultado.custoInicial != custoConstrutiva) {
        printf("Custo inicial interno diferente do custo construtivo calculado.\n");
        printf("Custo inicial interno: %llu\n",(unsigned long long) resultado.custoInicial);
        printf("Custo construtivo calculado: %llu\n",(unsigned long long) custoConstrutiva);
        liberarSolucao(&solucaoInversao);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(resultado.custoFinal > resultado.custoAposBuscaHibrida) {
        printf("A busca por inversao piorou a solucao hibrida.\n");
        printf("Custo apos busca hibrida: %llu\n",(unsigned long long) resultado.custoAposBuscaHibrida);
        printf("Custo final da inversao: %llu\n",(unsigned long long) resultado.custoFinal);
        liberarSolucao(&solucaoInversao);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(criarArquivoDeResultado(caminhoDoResultado) == FALSO || adicionarResultado(caminhoDoResultado,nomeDoArquivo,identificadorDaInstancia,instancia.quantidadeDeTarefas,fatorH,raio,tempoInversaoMs,&resultado) == FALSO) {
        printf("Falha ao gravar resultado.\n");
        liberarSolucao(&solucaoInversao);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);
        return 1;
    }

    ganhoSobreHibrida = resultado.custoAposBuscaHibrida > 0 ? ((((double) resultado.custoAposBuscaHibrida) - ((double) resultado.custoFinal)) * 100.0) / ((double) resultado.custoAposBuscaHibrida) : 0.0;

    printf("arquivo=%s | instancia=%u | n=%u | h=%.1f | raio=%u | construtiva=%llu | hibrida=%llu | inversao=%llu | ganho_inversao=%.4f%% | tempo_ms=%.3f | ciclos=%u | vizinhos_inversao=%llu | inversoes=%u\n",
        nomeDoArquivo,
        (unsigned int) identificadorDaInstancia,
        (unsigned int) instancia.quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        (unsigned int) raio,
        (unsigned long long) resultado.custoInicial,
        (unsigned long long) resultado.custoAposBuscaHibrida,
        (unsigned long long) resultado.custoFinal,
        ganhoSobreHibrida,
        tempoInversaoMs,
        (unsigned int) resultado.quantidadeDeCiclos,
        (unsigned long long) resultado.quantidadeDeVizinhosPorInversao,
        (unsigned int) resultado.quantidadeDeInversoesAceitas
    );

    liberarSolucao(&solucaoInversao);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return 0;
}
