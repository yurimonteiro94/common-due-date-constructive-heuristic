#include "instancia_dao.h"

#include "../../../services/constantes/constantes.h"
#include "../../../services/ferramentas/ferramentas.h"
#include "../../../services/gerenciador_de_arquivos/gerenciador_de_arquivos.h"

#include <stddef.h>
#include <stdio.h>

static Boolean instanciaDaoMontarCaminhoDoArquivo(char *caminhoDoArquivo,InteiroPositivoDe32Bits tamanhoDoCaminho,const char *nomeDoArquivoDeInstancia) {
    return montarCaminhoComArquivo(
        caminhoDoArquivo,
        tamanhoDoCaminho,
        CAMINHO_DA_PASTA_DE_INSTANCIAS,
        nomeDoArquivoDeInstancia
    );
}

static Boolean instanciaDaoLerLinhaUtil(FILE *arquivo,char *linha) {
    while(gerenciadorDeArquivosLerLinha(arquivo,linha,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_LINHA) == VERDADEIRO) {
        removerQuebraDeLinha(linha);

        if(textoEhNuloOuVazio(linha) == FALSO) {
            return VERDADEIRO;
        }
    }

    return FALSO;
}

static Boolean instanciaDaoLerQuantidadeDeTarefas(FILE *arquivo,QuantidadeDeTarefas *quantidadeDeTarefas) {
    char linha[TAMANHO_MAXIMO_DE_LINHA];
    int quantidadeDeTarefasLida;

    if(arquivo == NULL) {
        return FALSO;
    }

    else if(quantidadeDeTarefas == NULL) {
        return FALSO;
    }

    if(instanciaDaoLerLinhaUtil(arquivo,linha) == FALSO) {
        return FALSO;
    }

    if(sscanf(linha,"%d",&quantidadeDeTarefasLida) != 1) {
        return FALSO;
    }

    if(quantidadeDeTarefasLida <= 0) {
        return FALSO;
    }

    (*quantidadeDeTarefas) = (QuantidadeDeTarefas) quantidadeDeTarefasLida;

    return VERDADEIRO;
}

static Boolean instanciaDaoLerTarefa(FILE *arquivo,IdentificadorDeTarefa identificadorDaTarefa,Tarefa *tarefa) {
    char linha[TAMANHO_MAXIMO_DE_LINHA];
    int tempoDeProcessamentoLido;
    int penalidadeAdiantamentoLida;
    int penalidadeAtrasoLida;

    if(arquivo == NULL) {
        return FALSO;
    }

    else if(tarefa == NULL) {
        return FALSO;
    }

    else if(identificadorDaTarefa == 0) {
        return FALSO;
    }

    if(instanciaDaoLerLinhaUtil(arquivo,linha) == FALSO) {
        return FALSO;
    }

    if(sscanf(linha,"%d %d %d",&tempoDeProcessamentoLido,&penalidadeAdiantamentoLida,&penalidadeAtrasoLida) != 3) {
        return FALSO;
    }

    if(tempoDeProcessamentoLido <= 0) {
        return FALSO;
    }

    else if(penalidadeAdiantamentoLida <= 0) {
        return FALSO;
    }

    else if(penalidadeAtrasoLida <= 0) {
        return FALSO;
    }

    (*tarefa) = criarTarefa(
        identificadorDaTarefa,
        (TempoDeProcessamento) tempoDeProcessamentoLido,
        (Penalidade) penalidadeAdiantamentoLida,
        (Penalidade) penalidadeAtrasoLida
    );

    return tarefaEhValida(tarefa);
}

static Boolean instanciaDaoPularTarefas(FILE *arquivo,QuantidadeDeTarefas quantidadeDeTarefas) {
    QuantidadeDeTarefas indiceDaTarefa;
    char linha[TAMANHO_MAXIMO_DE_LINHA];

    if(arquivo == NULL) {
        return FALSO;
    }

    for(indiceDaTarefa = 0; indiceDaTarefa < quantidadeDeTarefas; indiceDaTarefa++) {
        if(instanciaDaoLerLinhaUtil(arquivo,linha) == FALSO) {
            return FALSO;
        }
    }

    return VERDADEIRO;
}

Boolean instanciaDaoLerQuantidadeDeInstanciasDoArquivo(const char *nomeDoArquivoDeInstancia,InteiroPositivoDe16Bits *quantidadeDeInstancias) {
    FILE *arquivo;
    char caminhoDoArquivo[TAMANHO_MAXIMO_DE_CAMINHO];
    char linha[TAMANHO_MAXIMO_DE_LINHA];
    int quantidadeDeInstanciasLida;

    if(textoEhNuloOuVazio(nomeDoArquivoDeInstancia) == VERDADEIRO) {
        return FALSO;
    }

    else if(quantidadeDeInstancias == NULL) {
        return FALSO;
    }

    (*quantidadeDeInstancias) = 0;

    if(instanciaDaoMontarCaminhoDoArquivo(caminhoDoArquivo,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_CAMINHO,nomeDoArquivoDeInstancia) == FALSO) {
        return FALSO;
    }

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(caminhoDoArquivo);

    if(arquivo == NULL) {
        return FALSO;
    }

    if(instanciaDaoLerLinhaUtil(arquivo,linha) == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return FALSO;
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    if(sscanf(linha,"%d",&quantidadeDeInstanciasLida) != 1) {
        return FALSO;
    }

    if(quantidadeDeInstanciasLida <= 0) {
        return FALSO;
    }

    (*quantidadeDeInstancias) = (InteiroPositivoDe16Bits) quantidadeDeInstanciasLida;

    return VERDADEIRO;
}

Boolean instanciaDaoLerInstanciaPorIdentificador(const char *nomeDoArquivoDeInstancia,InteiroPositivoDe16Bits identificadorDaInstancia,Instancia *instancia) {
    FILE *arquivo;
    char caminhoDoArquivo[TAMANHO_MAXIMO_DE_CAMINHO];
    InteiroPositivoDe16Bits quantidadeDeInstancias;
    InteiroPositivoDe16Bits indiceDaInstancia;
    QuantidadeDeTarefas quantidadeDeTarefas;
    QuantidadeDeTarefas indiceDaTarefa;
    Tarefa tarefa;
    Instancia instanciaTemporaria;

    if(textoEhNuloOuVazio(nomeDoArquivoDeInstancia) == VERDADEIRO) {
        return FALSO;
    }

    else if(identificadorDaInstancia == 0) {
        return FALSO;
    }

    else if(instancia == NULL) {
        return FALSO;
    }

    quantidadeDeInstancias = 0;
    instanciaTemporaria = criarInstanciaVazia();

    if(instanciaDaoMontarCaminhoDoArquivo(caminhoDoArquivo,(InteiroPositivoDe32Bits) TAMANHO_MAXIMO_DE_CAMINHO,nomeDoArquivoDeInstancia) == FALSO) {
        return FALSO;
    }

    arquivo = gerenciadorDeArquivosAbrirArquivoParaLeitura(caminhoDoArquivo);

    if(arquivo == NULL) {
        return FALSO;
    }

    if(instanciaDaoLerQuantidadeDeInstanciasDoArquivo(nomeDoArquivoDeInstancia,&quantidadeDeInstancias) == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return FALSO;
    }

    if(identificadorDaInstancia > quantidadeDeInstancias) {
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return FALSO;
    }

    if(instanciaDaoLerLinhaUtil(arquivo,caminhoDoArquivo) == FALSO) {
        gerenciadorDeArquivosFecharArquivo(arquivo);

        return FALSO;
    }

    for(indiceDaInstancia = 1; indiceDaInstancia <= quantidadeDeInstancias; indiceDaInstancia++) {
        if(instanciaDaoLerQuantidadeDeTarefas(arquivo,&quantidadeDeTarefas) == FALSO) {
            gerenciadorDeArquivosFecharArquivo(arquivo);

            return FALSO;
        }

        if(indiceDaInstancia == identificadorDaInstancia) {
            if(inicializarInstancia(&instanciaTemporaria,nomeDoArquivoDeInstancia,identificadorDaInstancia,quantidadeDeTarefas) == FALSO) {
                gerenciadorDeArquivosFecharArquivo(arquivo);

                return FALSO;
            }

            for(indiceDaTarefa = 0; indiceDaTarefa < quantidadeDeTarefas; indiceDaTarefa++) {
                if(instanciaDaoLerTarefa(arquivo,(IdentificadorDeTarefa) (indiceDaTarefa + 1),&tarefa) == FALSO) {
                    liberarInstancia(&instanciaTemporaria);
                    gerenciadorDeArquivosFecharArquivo(arquivo);

                    return FALSO;
                }

                if(instanciaAdicionarTarefa(&instanciaTemporaria,indiceDaTarefa,tarefa) == FALSO) {
                    liberarInstancia(&instanciaTemporaria);
                    gerenciadorDeArquivosFecharArquivo(arquivo);

                    return FALSO;
                }
            }

            if(instanciaEhValida(&instanciaTemporaria) == FALSO) {
                liberarInstancia(&instanciaTemporaria);
                gerenciadorDeArquivosFecharArquivo(arquivo);

                return FALSO;
            }

            (*instancia) = instanciaTemporaria;

            gerenciadorDeArquivosFecharArquivo(arquivo);

            return VERDADEIRO;
        }

        if(instanciaDaoPularTarefas(arquivo,quantidadeDeTarefas) == FALSO) {
            gerenciadorDeArquivosFecharArquivo(arquivo);

            return FALSO;
        }
    }

    gerenciadorDeArquivosFecharArquivo(arquivo);

    return FALSO;
}
