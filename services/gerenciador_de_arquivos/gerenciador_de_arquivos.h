#ifndef GERENCIADOR_DE_ARQUIVOS_H
#define GERENCIADOR_DE_ARQUIVOS_H

#include "../constantes/constantes.h"

#include <stdio.h>

FILE *gerenciadorDeArquivosAbrirArquivoParaLeitura(const char *caminhoDoArquivo);
FILE *gerenciadorDeArquivosAbrirArquivoParaEscrita(const char *caminhoDoArquivo);
FILE *gerenciadorDeArquivosAbrirArquivoParaAcrescimo(const char *caminhoDoArquivo);
Boolean gerenciadorDeArquivosLerLinha(FILE *arquivo,char *linha,InteiroPositivoDe32Bits tamanhoDaLinha);
Boolean gerenciadorDeArquivosGravarLinha(FILE *arquivo,const char *linha);
Boolean gerenciadorDeArquivosArquivoExiste(const char *caminhoDoArquivo);
void gerenciadorDeArquivosFecharArquivo(FILE *arquivo);

#endif
