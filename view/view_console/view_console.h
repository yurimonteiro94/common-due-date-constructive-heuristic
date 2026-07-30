#ifndef VIEW_CONSOLE_H
#define VIEW_CONSOLE_H

#include "../../services/constantes/constantes.h"

void viewConsoleExibirCabecalho(void);
void viewConsoleExibirMensagem(const char *mensagem);
void viewConsoleExibirErro(const char *mensagem);
void viewConsoleExibirInicioDoExperimento(void);
void viewConsoleExibirFimDoExperimento(const char *caminhoArquivoResultadosDeExecucoes,const char *caminhoArquivoMediasPorNH);
void viewConsoleExibirFalhaDoExperimento(void);

#endif
