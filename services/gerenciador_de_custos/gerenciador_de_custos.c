#include "gerenciador_de_custos.h"

#include <stddef.h>
#include <stdlib.h>

static Boolean gerenciadorDeCustosParametrosSaoValidos(const Instancia *instancia,const Solucao *solucao) {
    if(instancia == NULL) {
        return FALSO;
    }

    if(solucao == NULL) {
        return FALSO;
    }

    if((*instancia).tarefas == NULL) {
        return FALSO;
    }

    if((*solucao).sequenciaDeTarefas == NULL) {
        return FALSO;
    }

    if((*instancia).quantidadeDeTarefas == 0) {
        return FALSO;
    }

    if((*solucao).quantidadeDeTarefasAlocadas == 0) {
        return FALSO;
    }

    if((*solucao).quantidadeDeTarefasAlocadas != (*instancia).quantidadeDeTarefas) {
        return FALSO;
    }

    return VERDADEIRO;
}

static const Tarefa *gerenciadorDeCustosBuscarTarefaPorIdentificador(const Instancia *instancia,IdentificadorDeTarefa identificadorDaTarefa) {
    QuantidadeDeTarefas indiceDaTarefa;

    if(instancia == NULL) {
        return NULL;
    }

    if((*instancia).tarefas == NULL) {
        return NULL;
    }

    for(indiceDaTarefa = 0;indiceDaTarefa < (*instancia).quantidadeDeTarefas;indiceDaTarefa++) {
        if((*instancia).tarefas[indiceDaTarefa].identificador == identificadorDaTarefa) {
            return &((*instancia).tarefas[indiceDaTarefa]);
        }
    }

    return NULL;
}

static Boolean gerenciadorDeCustosMontarTarefasNaOrdemDaSolucao(const Instancia *instancia,const Solucao *solucao,const Tarefa **tarefasNaOrdemDaSolucao) {
    QuantidadeDeTarefas indiceDaSequencia;
    IdentificadorDeTarefa identificadorDaTarefa;
    const Tarefa *tarefa;

    if(instancia == NULL) {
        return FALSO;
    }

    if(solucao == NULL) {
        return FALSO;
    }

    if(tarefasNaOrdemDaSolucao == NULL) {
        return FALSO;
    }

    for(indiceDaSequencia = 0;indiceDaSequencia < (*solucao).quantidadeDeTarefasAlocadas;indiceDaSequencia++) {
        identificadorDaTarefa = (*solucao).sequenciaDeTarefas[indiceDaSequencia];
        tarefa = gerenciadorDeCustosBuscarTarefaPorIdentificador(instancia,identificadorDaTarefa);

        if(tarefa == NULL) {
            return FALSO;
        }

        tarefasNaOrdemDaSolucao[indiceDaSequencia] = tarefa;
    }

    return VERDADEIRO;
}

static Custo gerenciadorDeCustosCalcularCustoComTarefasNaOrdem(const Tarefa **tarefasNaOrdemDaSolucao,QuantidadeDeTarefas quantidadeDeTarefas,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits instanteInicial) {
    QuantidadeDeTarefas indiceDaSequencia;
    const Tarefa *tarefa;
    InteiroPositivoDe32Bits instanteAtual;
    InteiroPositivoDe32Bits adiantamento;
    InteiroPositivoDe32Bits atraso;
    Custo custoTotal;

    instanteAtual = instanteInicial;
    custoTotal = 0;

    for(indiceDaSequencia = 0;indiceDaSequencia < quantidadeDeTarefas;indiceDaSequencia++) {
        tarefa = tarefasNaOrdemDaSolucao[indiceDaSequencia];
        instanteAtual += (*tarefa).tempoProcessamento;

        if(instanteAtual < dataDeEntregaComum) {
            adiantamento = dataDeEntregaComum - instanteAtual;
            custoTotal += ((Custo) (*tarefa).penalidadeAdiantamento) * ((Custo) adiantamento);
        }
        else if(instanteAtual > dataDeEntregaComum) {
            atraso = instanteAtual - dataDeEntregaComum;
            custoTotal += ((Custo) (*tarefa).penalidadeAtraso) * ((Custo) atraso);
        }
    }

    return custoTotal;
}

Boolean gerenciadorDeCustosCalcularCustoDaSolucaoComInstanteInicial(const Instancia *instancia,const Solucao *solucao,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits instanteInicial,Custo *custo) {
    const Tarefa **tarefasNaOrdemDaSolucao;

    if(custo == NULL) {
        return FALSO;
    }

    (*custo) = 0;

    if(gerenciadorDeCustosParametrosSaoValidos(instancia,solucao) == FALSO) {
        return FALSO;
    }

    tarefasNaOrdemDaSolucao = (const Tarefa **) malloc(sizeof(Tarefa *) * (*solucao).quantidadeDeTarefasAlocadas);

    if(tarefasNaOrdemDaSolucao == NULL) {
        return FALSO;
    }

    if(gerenciadorDeCustosMontarTarefasNaOrdemDaSolucao(instancia,solucao,tarefasNaOrdemDaSolucao) == FALSO) {
        free(tarefasNaOrdemDaSolucao);
        return FALSO;
    }

    (*custo) = gerenciadorDeCustosCalcularCustoComTarefasNaOrdem(tarefasNaOrdemDaSolucao,(*solucao).quantidadeDeTarefasAlocadas,dataDeEntregaComum,instanteInicial);

    free(tarefasNaOrdemDaSolucao);

    return VERDADEIRO;
}

Boolean gerenciadorDeCustosEncontrarMelhorInstanteInicialDaSolucao(const Instancia *instancia,const Solucao *solucao,DataDeEntregaComum dataDeEntregaComum,InteiroPositivoDe32Bits *melhorInstanteInicial) {
    const Tarefa **tarefasNaOrdemDaSolucao;
    QuantidadeDeTarefas indiceDaSequencia;
    InteiroPositivoDe32Bits instanteInicialCandidato;
    InteiroPositivoDe32Bits melhorInstanteInicialAtual;
    InteiroPositivoDe32Bits somaPrefixadaDosTempos;
    Custo custoCandidato;
    Custo melhorCusto;

    if(melhorInstanteInicial == NULL) {
        return FALSO;
    }

    (*melhorInstanteInicial) = 0;

    if(gerenciadorDeCustosParametrosSaoValidos(instancia,solucao) == FALSO) {
        return FALSO;
    }

    tarefasNaOrdemDaSolucao = (const Tarefa **) malloc(sizeof(Tarefa *) * (*solucao).quantidadeDeTarefasAlocadas);

    if(tarefasNaOrdemDaSolucao == NULL) {
        return FALSO;
    }

    if(gerenciadorDeCustosMontarTarefasNaOrdemDaSolucao(instancia,solucao,tarefasNaOrdemDaSolucao) == FALSO) {
        free(tarefasNaOrdemDaSolucao);
        return FALSO;
    }

    melhorInstanteInicialAtual = 0;
    melhorCusto = gerenciadorDeCustosCalcularCustoComTarefasNaOrdem(tarefasNaOrdemDaSolucao,(*solucao).quantidadeDeTarefasAlocadas,dataDeEntregaComum,0);

    somaPrefixadaDosTempos = 0;

    for(indiceDaSequencia = 0;indiceDaSequencia < (*solucao).quantidadeDeTarefasAlocadas;indiceDaSequencia++) {
        somaPrefixadaDosTempos += (*tarefasNaOrdemDaSolucao[indiceDaSequencia]).tempoProcessamento;

        if(somaPrefixadaDosTempos <= dataDeEntregaComum) {
            instanteInicialCandidato = dataDeEntregaComum - somaPrefixadaDosTempos;
            custoCandidato = gerenciadorDeCustosCalcularCustoComTarefasNaOrdem(tarefasNaOrdemDaSolucao,(*solucao).quantidadeDeTarefasAlocadas,dataDeEntregaComum,instanteInicialCandidato);

            if(custoCandidato < melhorCusto) {
                melhorCusto = custoCandidato;
                melhorInstanteInicialAtual = instanteInicialCandidato;
            }
        }
    }

    (*melhorInstanteInicial) = melhorInstanteInicialAtual;

    free(tarefasNaOrdemDaSolucao);

    return VERDADEIRO;
}

Boolean gerenciadorDeCustosCalcularCustoDaSolucao(const Instancia *instancia,const Solucao *solucao,DataDeEntregaComum dataDeEntregaComum,Custo *custo) {
    InteiroPositivoDe32Bits melhorInstanteInicial;

    if(custo == NULL) {
        return FALSO;
    }

    (*custo) = 0;

    if(gerenciadorDeCustosParametrosSaoValidos(instancia,solucao) == FALSO) {
        return FALSO;
    }

    if(gerenciadorDeCustosEncontrarMelhorInstanteInicialDaSolucao(instancia,solucao,dataDeEntregaComum,&melhorInstanteInicial) == FALSO) {
        return FALSO;
    }

    return gerenciadorDeCustosCalcularCustoDaSolucaoComInstanteInicial(instancia,solucao,dataDeEntregaComum,melhorInstanteInicial,custo);
}
