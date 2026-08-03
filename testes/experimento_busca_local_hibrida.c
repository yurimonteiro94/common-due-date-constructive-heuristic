#include "../controller/controller_busca_local_hibrida/controller_busca_local_hibrida.h"
#include "../controller/controller_heuristica/controller_heuristica.h"
#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/heuristica/heuristica.h"
#include "../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_CAMINHO_RESULTADO_HIBRIDO 256

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

    quantidadeDeCaracteres = snprintf(caminhoDoResultado,TAMANHO_CAMINHO_RESULTADO_HIBRIDO,"resultados/validacao_busca_local_hibrida_%s.csv",sufixo);

    if(quantidadeDeCaracteres < 0 || quantidadeDeCaracteres >= TAMANHO_CAMINHO_RESULTADO_HIBRIDO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean criarArquivoDeResultado(const char *caminhoDoResultado) {
    FILE *arquivo;

    if(caminhoDoResultado == NULL) {
        return FALSO;
    }

    arquivo = fopen(caminhoDoResultado,"w");

    if(arquivo == NULL) {
        return FALSO;
    }

    fprintf(arquivo,"arquivo,idInstancia,n,h,raio,custoConstrutiva,custoPrimeira,custoMelhor,custoTrajetoriaA,custoTrajetoriaB,custoHibrida,ganhoHibridaSobrePrimeiraPercentual,ganhoHibridaSobreMelhorPercentual,melhoriaHibridaSobreConstrutivaPercentual,tempoHibridaMs,trajetoriaSelecionada,chamadasBuscaLocal,iteracoesTotais,vizinhosTotais,melhoriasReinsercao,melhoriasTroca\n");
    fclose(arquivo);

    return VERDADEIRO;
}

static Boolean adicionarResultado(const char *caminhoDoResultado,const char *nomeDoArquivo,InteiroPositivoDe16Bits identificadorDaInstancia,QuantidadeDeTarefas quantidadeDeTarefas,FatorH fatorH,QuantidadeDeTarefas raio,Custo custoConstrutiva,TempoComputacionalEmMilissegundos tempoHibridaMs,const ResultadoBuscaLocalHibrida *resultado) {
    FILE *arquivo;
    double ganhoSobrePrimeira;
    double ganhoSobreMelhor;
    double melhoriaSobreConstrutiva;

    if(caminhoDoResultado == NULL || nomeDoArquivo == NULL || resultado == NULL) {
        return FALSO;
    }

    arquivo = fopen(caminhoDoResultado,"a");

    if(arquivo == NULL) {
        return FALSO;
    }

    ganhoSobrePrimeira = 0.0;
    ganhoSobreMelhor = 0.0;
    melhoriaSobreConstrutiva = 0.0;

    if((*resultado).custoPrimeiraMelhoriaIsolada > 0) {
        ganhoSobrePrimeira = ((((double) (*resultado).custoPrimeiraMelhoriaIsolada) - ((double) (*resultado).custoFinal)) * 100.0) / ((double) (*resultado).custoPrimeiraMelhoriaIsolada);
    }

    if((*resultado).custoMelhorMelhoriaIsolada > 0) {
        ganhoSobreMelhor = ((((double) (*resultado).custoMelhorMelhoriaIsolada) - ((double) (*resultado).custoFinal)) * 100.0) / ((double) (*resultado).custoMelhorMelhoriaIsolada);
    }

    if(custoConstrutiva > 0) {
        melhoriaSobreConstrutiva = ((((double) custoConstrutiva) - ((double) (*resultado).custoFinal)) * 100.0) / ((double) custoConstrutiva);
    }

    fprintf(arquivo,"%s,%u,%u,%.1f,%u,%llu,%llu,%llu,%llu,%llu,%llu,%.9f,%.9f,%.9f,%.9f,%u,%u,%u,%llu,%u,%u\n",
        nomeDoArquivo,
        (unsigned int) identificadorDaInstancia,
        (unsigned int) quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        (unsigned int) raio,
        (unsigned long long) custoConstrutiva,
        (unsigned long long) (*resultado).custoPrimeiraMelhoriaIsolada,
        (unsigned long long) (*resultado).custoMelhorMelhoriaIsolada,
        (unsigned long long) (*resultado).custoTrajetoriaPrimeiraMelhorPrimeira,
        (unsigned long long) (*resultado).custoTrajetoriaMelhorPrimeiraMelhor,
        (unsigned long long) (*resultado).custoFinal,
        ganhoSobrePrimeira,
        ganhoSobreMelhor,
        melhoriaSobreConstrutiva,
        tempoHibridaMs,
        (unsigned int) (*resultado).trajetoriaSelecionada,
        (unsigned int) (*resultado).quantidadeDeChamadasDeBuscaLocal,
        (unsigned int) (*resultado).quantidadeTotalDeIteracoes,
        (unsigned long long) (*resultado).quantidadeTotalDeVizinhosAvaliados,
        (unsigned int) (*resultado).quantidadeTotalDeMelhoriasPorReinsercao,
        (unsigned int) (*resultado).quantidadeTotalDeMelhoriasPorTroca
    );

    fclose(arquivo);

    return VERDADEIRO;
}

int main(int quantidadeDeArgumentos,char *argumentos[]) {
    const char *nomeDoArquivo;
    const char *sufixo;
    char caminhoDoResultado[TAMANHO_CAMINHO_RESULTADO_HIBRIDO];
    Instancia instancia;
    Heuristica heuristica;
    Solucao solucaoConstrutiva;
    Solucao solucaoHibrida;
    ResultadoBuscaLocalHibrida resultado;
    FatorH fatorH;
    DataDeEntregaComum dataDeEntregaComum;
    QuantidadeDeTarefas raio;
    Custo custoConstrutiva;
    Custo custoVerificado;
    TempoComputacionalEmSegundos tempoInicial;
    TempoComputacionalEmSegundos tempoFinal;
    TempoComputacionalEmMilissegundos tempoHibridaMs;
    InteiroPositivoDe16Bits identificadorDaInstancia;
    double ganhoSobrePrimeira;
    double ganhoSobreMelhor;

    if(quantidadeDeArgumentos != 5) {
        printf("Uso: experimento_busca_local_hibrida.exe arquivo fatorH idInstancia sufixo\n");

        return 1;
    }

    nomeDoArquivo = argumentos[1];
    identificadorDaInstancia = (InteiroPositivoDe16Bits) strtoul(argumentos[3],NULL,10);
    sufixo = argumentos[4];

    if(obterFatorH(argumentos[2],&fatorH) == FALSO) {
        printf("Fator h invalido.\n");

        return 1;
    }

    if(identificadorDaInstancia == 0) {
        printf("Identificador da instancia invalido.\n");

        return 1;
    }

    if(montarCaminhoDoResultado(caminhoDoResultado,sufixo) == FALSO) {
        printf("Falha ao montar caminho do resultado.\n");

        return 1;
    }

    instancia = criarInstanciaVazia();
    solucaoConstrutiva = criarSolucaoVazia();
    solucaoHibrida = criarSolucaoVazia();
    resultado = criarResultadoBuscaLocalHibridaVazio();
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

    if(controllerBuscaLocalHibridaMelhorarSolucao(&instancia,fatorH,&solucaoConstrutiva,&solucaoHibrida,&resultado,raio,raio) == FALSO) {
        printf("Falha na busca local hibrida.\n");
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    tempoFinal = gerenciadorDeTempoObterTempoAtualEmSegundos();
    tempoHibridaMs = gerenciadorDeTempoConverterSegundosParaMilissegundos(gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicial,tempoFinal));

    if(solucaoEhValida(&solucaoHibrida) == FALSO) {
        printf("Solucao hibrida invalida.\n");
        liberarSolucao(&solucaoHibrida);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(gerenciadorDeCustosCalcularCustoDaSolucao(&instancia,&solucaoHibrida,dataDeEntregaComum,&custoVerificado) == FALSO) {
        printf("Falha ao verificar custo hibrido.\n");
        liberarSolucao(&solucaoHibrida);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(custoVerificado != resultado.custoFinal) {
        printf("Custo hibrido diferente do custo verificado.\n");
        liberarSolucao(&solucaoHibrida);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(resultado.custoFinal > resultado.custoPrimeiraMelhoriaIsolada || resultado.custoFinal > resultado.custoMelhorMelhoriaIsolada) {
        printf("Busca hibrida nao dominou as buscas isoladas.\n");
        liberarSolucao(&solucaoHibrida);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(criarArquivoDeResultado(caminhoDoResultado) == FALSO) {
        printf("Falha ao criar arquivo de resultado.\n");
        liberarSolucao(&solucaoHibrida);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    if(adicionarResultado(caminhoDoResultado,nomeDoArquivo,identificadorDaInstancia,instancia.quantidadeDeTarefas,fatorH,raio,custoConstrutiva,tempoHibridaMs,&resultado) == FALSO) {
        printf("Falha ao gravar resultado.\n");
        liberarSolucao(&solucaoHibrida);
        liberarSolucao(&solucaoConstrutiva);
        liberarInstancia(&instancia);

        return 1;
    }

    ganhoSobrePrimeira = resultado.custoPrimeiraMelhoriaIsolada > 0 ? ((((double) resultado.custoPrimeiraMelhoriaIsolada) - ((double) resultado.custoFinal)) * 100.0) / ((double) resultado.custoPrimeiraMelhoriaIsolada) : 0.0;
    ganhoSobreMelhor = resultado.custoMelhorMelhoriaIsolada > 0 ? ((((double) resultado.custoMelhorMelhoriaIsolada) - ((double) resultado.custoFinal)) * 100.0) / ((double) resultado.custoMelhorMelhoriaIsolada) : 0.0;

    printf("arquivo=%s | instancia=%u | n=%u | h=%.1f | raio=%u | construtiva=%llu | primeira=%llu | melhor=%llu | hibrida=%llu | ganho_primeira=%.4f%% | ganho_melhor=%.4f%% | trajetoria=%u | tempo_ms=%.3f | vizinhos=%llu\n",
        nomeDoArquivo,
        (unsigned int) identificadorDaInstancia,
        (unsigned int) instancia.quantidadeDeTarefas,
        ((double) fatorH) / ((double) FATOR_DE_ESCALA_H),
        (unsigned int) raio,
        (unsigned long long) custoConstrutiva,
        (unsigned long long) resultado.custoPrimeiraMelhoriaIsolada,
        (unsigned long long) resultado.custoMelhorMelhoriaIsolada,
        (unsigned long long) resultado.custoFinal,
        ganhoSobrePrimeira,
        ganhoSobreMelhor,
        (unsigned int) resultado.trajetoriaSelecionada,
        tempoHibridaMs,
        (unsigned long long) resultado.quantidadeTotalDeVizinhosAvaliados
    );

    liberarSolucao(&solucaoHibrida);
    liberarSolucao(&solucaoConstrutiva);
    liberarInstancia(&instancia);

    return 0;
}