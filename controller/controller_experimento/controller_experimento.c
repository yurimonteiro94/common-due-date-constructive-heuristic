#include "controller_experimento.h"

#include "../controller_heuristica/controller_heuristica.h"
#include "../../model/dao/instancia_dao/instancia_dao.h"
#include "../../model/dao/resultado_dao/resultado_dao.h"
#include "../../model/entidades/instancia/instancia.h"
#include "../../model/entidades/resultado_de_execucao/resultado_de_execucao.h"
#include "../../model/entidades/solucao/solucao.h"
#include "../../services/ferramentas/ferramentas.h"
#include "../../services/gerenciador_de_custos/gerenciador_de_custos.h"
#include "../../services/gerenciador_de_tempo/gerenciador_de_tempo.h"

#include <stddef.h>

static void controllerExperimentoInicializarAcumuladores(double somasDosCustos[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H],double somasDosTemposEmSegundos[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H],InteiroPositivoDe16Bits quantidadesDeExecucoes[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H],QuantidadeDeTarefas quantidadesDeTarefasPorArquivo[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA]) {
    InteiroPositivoDe8Bits indiceDoArquivo;
    InteiroPositivoDe8Bits indiceDoFatorH;

    for(indiceDoArquivo = 0; indiceDoArquivo < QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA; indiceDoArquivo++) {
        quantidadesDeTarefasPorArquivo[indiceDoArquivo] = 0;

        for(indiceDoFatorH = 0; indiceDoFatorH < QUANTIDADE_DE_VALORES_DE_H; indiceDoFatorH++) {
            somasDosCustos[indiceDoArquivo][indiceDoFatorH] = 0.0;
            somasDosTemposEmSegundos[indiceDoArquivo][indiceDoFatorH] = 0.0;
            quantidadesDeExecucoes[indiceDoArquivo][indiceDoFatorH] = 0;
        }
    }
}

static Boolean controllerExperimentoCaminhosSaoValidos(const char *caminhoArquivoResultadosDeExecucoes,const char *caminhoArquivoMediasPorNH) {
    if(textoEhNuloOuVazio(caminhoArquivoResultadosDeExecucoes) == VERDADEIRO) {
        return FALSO;
    }

    else if(textoEhNuloOuVazio(caminhoArquivoMediasPorNH) == VERDADEIRO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean controllerExperimentoPrepararArquivosDeSaida(const char *caminhoArquivoResultadosDeExecucoes,const char *caminhoArquivoMediasPorNH) {
    if(resultadoDaoCriarArquivoDeResultadosDeExecucoes(caminhoArquivoResultadosDeExecucoes) == FALSO) {
        return FALSO;
    }

    if(resultadoDaoCriarArquivoDeMediasPorNH(caminhoArquivoMediasPorNH) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}

static Boolean controllerExperimentoExecutarUmaCombinacao(const Experimento *experimento,const Instancia *instancia,const char *caminhoArquivoResultadosDeExecucoes,FatorH fatorH,IdentificadorDeExecucao identificadorDaExecucao,ResultadoDeExecucao *resultadoDeExecucao) {
    Solucao solucao;
    Custo custo;
    DataDeEntregaComum dataDeEntregaComum;
    TempoComputacionalEmSegundos tempoInicialEmSegundos;
    TempoComputacionalEmSegundos tempoFinalEmSegundos;
    TempoComputacionalEmSegundos duracaoEmSegundos;

    if(experimento == NULL) {
        return FALSO;
    }

    else if(instanciaEhValida(instancia) == FALSO) {
        return FALSO;
    }

    else if(textoEhNuloOuVazio(caminhoArquivoResultadosDeExecucoes) == VERDADEIRO) {
        return FALSO;
    }

    else if(identificadorDaExecucao == 0) {
        return FALSO;
    }

    else if(resultadoDeExecucao == NULL) {
        return FALSO;
    }

    solucao = criarSolucaoVazia();
    custo = 0;

    dataDeEntregaComum = instanciaCalcularDataDeEntregaComum(instancia,fatorH);

    if(dataDeEntregaComum == 0) {
        return FALSO;
    }

    tempoInicialEmSegundos = gerenciadorDeTempoObterTempoAtualEmSegundos();

    if(controllerHeuristicaConstruirSolucao(instancia,&(*experimento).heuristica,fatorH,&solucao) == FALSO) {
        liberarSolucao(&solucao);

        return FALSO;
    }

    tempoFinalEmSegundos = gerenciadorDeTempoObterTempoAtualEmSegundos();
    duracaoEmSegundos = gerenciadorDeTempoCalcularDuracaoEmSegundos(tempoInicialEmSegundos,tempoFinalEmSegundos);

    if(gerenciadorDeCustosCalcularCustoDaSolucao(instancia,&solucao,dataDeEntregaComum,&custo) == FALSO) {
        liberarSolucao(&solucao);

        return FALSO;
    }

    (*resultadoDeExecucao) = criarResultadoDeExecucaoVazio();

    if(inicializarResultadoDeExecucao(
        resultadoDeExecucao,
        identificadorDaExecucao,
        (*instancia).nomeDoArquivoDeOrigem,
        (*experimento).heuristica.identificadorDaHeuristica,
        (*instancia).identificadorDaInstancia,
        (*instancia).quantidadeDeTarefas,
        fatorH,
        (*instancia).somaDosTemposDeProcessamento,
        dataDeEntregaComum,
        custo,
        duracaoEmSegundos
    ) == FALSO) {
        liberarSolucao(&solucao);

        return FALSO;
    }

    if(resultadoDaoAdicionarResultadoDeExecucao(caminhoArquivoResultadosDeExecucoes,resultadoDeExecucao) == FALSO) {
        liberarSolucao(&solucao);

        return FALSO;
    }

    liberarSolucao(&solucao);

    return VERDADEIRO;
}

static Boolean controllerExperimentoGravarMedias(const char *caminhoArquivoMediasPorNH,const Experimento *experimento,double somasDosCustos[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H],double somasDosTemposEmSegundos[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H],InteiroPositivoDe16Bits quantidadesDeExecucoes[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H],QuantidadeDeTarefas quantidadesDeTarefasPorArquivo[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA]) {
    InteiroPositivoDe8Bits indiceDoArquivo;
    InteiroPositivoDe8Bits indiceDoFatorH;

    QuantidadeDeTarefas quantidadeDeTarefas;
    FatorH fatorH;
    InteiroPositivoDe16Bits quantidadeDeExecucoes;

    double custoMedio;
    double tempoMedioEmSegundos;
    double tempoMedioEmMilissegundos;
    double tempoMedioPorTarefaEmMilissegundos;

    if(textoEhNuloOuVazio(caminhoArquivoMediasPorNH) == VERDADEIRO) {
        return FALSO;
    }

    else if(experimentoEhValido(experimento) == FALSO) {
        return FALSO;
    }

    for(indiceDoArquivo = 0; indiceDoArquivo < (*experimento).quantidadeDeArquivosDeInstancia; indiceDoArquivo++) {
        quantidadeDeTarefas = quantidadesDeTarefasPorArquivo[indiceDoArquivo];

        if(quantidadeDeTarefas == 0) {
            return FALSO;
        }

        for(indiceDoFatorH = 0; indiceDoFatorH < (*experimento).quantidadeDeValoresDeH; indiceDoFatorH++) {
            fatorH = (*experimento).fatoresH[indiceDoFatorH];
            quantidadeDeExecucoes = quantidadesDeExecucoes[indiceDoArquivo][indiceDoFatorH];

            if(quantidadeDeExecucoes == 0) {
                return FALSO;
            }

            custoMedio = somasDosCustos[indiceDoArquivo][indiceDoFatorH] / (double) quantidadeDeExecucoes;
            tempoMedioEmSegundos = somasDosTemposEmSegundos[indiceDoArquivo][indiceDoFatorH] / (double) quantidadeDeExecucoes;
            tempoMedioEmMilissegundos = gerenciadorDeTempoConverterSegundosParaMilissegundos(tempoMedioEmSegundos);
            tempoMedioPorTarefaEmMilissegundos = tempoMedioEmMilissegundos / (double) quantidadeDeTarefas;

            if(resultadoDaoAdicionarMediaPorNH(
                caminhoArquivoMediasPorNH,
                quantidadeDeTarefas,
                fatorH,
                quantidadeDeExecucoes,
                custoMedio,
                tempoMedioEmSegundos,
                tempoMedioEmMilissegundos,
                tempoMedioPorTarefaEmMilissegundos
            ) == FALSO) {
                return FALSO;
            }
        }
    }

    return VERDADEIRO;
}

Boolean controllerExperimentoExecutarExperimento(const Experimento *experimento,const char *caminhoArquivoResultadosDeExecucoes,const char *caminhoArquivoMediasPorNH) {
    double somasDosCustos[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H];
    double somasDosTemposEmSegundos[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H];
    InteiroPositivoDe16Bits quantidadesDeExecucoes[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA][QUANTIDADE_DE_VALORES_DE_H];
    QuantidadeDeTarefas quantidadesDeTarefasPorArquivo[QUANTIDADE_DE_ARQUIVOS_DE_INSTANCIA];

    InteiroPositivoDe8Bits indiceDoArquivo;
    InteiroPositivoDe8Bits indiceDoFatorH;
    InteiroPositivoDe16Bits quantidadeDeInstanciasNoArquivo;
    InteiroPositivoDe16Bits identificadorDaInstancia;
    IdentificadorDeExecucao identificadorDaExecucao;

    Instancia instancia;
    ResultadoDeExecucao resultadoDeExecucao;
    FatorH fatorH;

    if(experimentoEhValido(experimento) == FALSO) {
        return FALSO;
    }

    else if(controllerExperimentoCaminhosSaoValidos(caminhoArquivoResultadosDeExecucoes,caminhoArquivoMediasPorNH) == FALSO) {
        return FALSO;
    }

    if(controllerExperimentoPrepararArquivosDeSaida(caminhoArquivoResultadosDeExecucoes,caminhoArquivoMediasPorNH) == FALSO) {
        return FALSO;
    }

    controllerExperimentoInicializarAcumuladores(
        somasDosCustos,
        somasDosTemposEmSegundos,
        quantidadesDeExecucoes,
        quantidadesDeTarefasPorArquivo
    );

    identificadorDaExecucao = 1;

    for(indiceDoArquivo = 0; indiceDoArquivo < (*experimento).quantidadeDeArquivosDeInstancia; indiceDoArquivo++) {
        quantidadeDeInstanciasNoArquivo = 0;

        if(instanciaDaoLerQuantidadeDeInstanciasDoArquivo((*experimento).nomesDosArquivosDeInstancia[indiceDoArquivo],&quantidadeDeInstanciasNoArquivo) == FALSO) {
            return FALSO;
        }

        for(identificadorDaInstancia = 1; identificadorDaInstancia <= quantidadeDeInstanciasNoArquivo; identificadorDaInstancia++) {
            instancia = criarInstanciaVazia();

            if(instanciaDaoLerInstanciaPorIdentificador(
                (*experimento).nomesDosArquivosDeInstancia[indiceDoArquivo],
                identificadorDaInstancia,
                &instancia
            ) == FALSO) {
                liberarInstancia(&instancia);

                return FALSO;
            }

            if(quantidadesDeTarefasPorArquivo[indiceDoArquivo] == 0) {
                quantidadesDeTarefasPorArquivo[indiceDoArquivo] = instancia.quantidadeDeTarefas;
            }

            for(indiceDoFatorH = 0; indiceDoFatorH < (*experimento).quantidadeDeValoresDeH; indiceDoFatorH++) {
                fatorH = (*experimento).fatoresH[indiceDoFatorH];

                resultadoDeExecucao = criarResultadoDeExecucaoVazio();

                if(controllerExperimentoExecutarUmaCombinacao(
                    experimento,
                    &instancia,
                    caminhoArquivoResultadosDeExecucoes,
                    fatorH,
                    identificadorDaExecucao,
                    &resultadoDeExecucao
                ) == FALSO) {
                    liberarInstancia(&instancia);

                    return FALSO;
                }

                somasDosCustos[indiceDoArquivo][indiceDoFatorH] += (double) resultadoDeExecucao.custo;
                somasDosTemposEmSegundos[indiceDoArquivo][indiceDoFatorH] += resultadoDeExecucao.tempoComputacionalEmSegundos;
                quantidadesDeExecucoes[indiceDoArquivo][indiceDoFatorH]++;

                identificadorDaExecucao++;
            }

            liberarInstancia(&instancia);
        }
    }

    if(controllerExperimentoGravarMedias(
        caminhoArquivoMediasPorNH,
        experimento,
        somasDosCustos,
        somasDosTemposEmSegundos,
        quantidadesDeExecucoes,
        quantidadesDeTarefasPorArquivo
    ) == FALSO) {
        return FALSO;
    }

    return VERDADEIRO;
}
