#ifndef FERRAMENTAS_H
#define FERRAMENTAS_H

#include "../constantes/constantes.h"

Boolean textoEhNuloOuVazio(const char *texto);
void removerQuebraDeLinha(char *texto);
Boolean copiarTexto(char *destino,InteiroPositivoDe32Bits tamanhoDoDestino,const char *origem);
Boolean montarCaminhoComArquivo(char *destino,InteiroPositivoDe32Bits tamanhoDoDestino,const char *caminhoDaPasta,const char *nomeDoArquivo);
double fatorHParaNumeroDecimal(FatorH fatorH);

#endif
