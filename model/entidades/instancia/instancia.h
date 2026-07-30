#ifndef INSTANCIA_H
#define INSTANCIA_H

#include "../../../services/constantes/constantes.h"
#include "../tarefa/tarefa.h"

typedef struct Instancia {
    char nomeDoArquivoDeOrigem[TAMANHO_MAXIMO_DE_NOME_DE_ARQUIVO];
    InteiroPositivoDe16Bits identificadorDaInstancia;
    QuantidadeDeTarefas quantidadeDeTarefas;
    Tarefa *tarefas;
    SomaDosTemposDeProcessamento somaDosTemposDeProcessamento;
} Instancia;

Instancia criarInstanciaVazia(void);
Boolean inicializarInstancia(Instancia *instancia,const char *nomeDoArquivoDeOrigem,InteiroPositivoDe16Bits identificadorDaInstancia,QuantidadeDeTarefas quantidadeDeTarefas);
Boolean instanciaAdicionarTarefa(Instancia *instancia,QuantidadeDeTarefas indiceDaTarefa,Tarefa tarefa);
Boolean instanciaEhValida(const Instancia *instancia);
DataDeEntregaComum instanciaCalcularDataDeEntregaComum(const Instancia *instancia,FatorH fatorH);
void liberarInstancia(Instancia *instancia);

#endif
