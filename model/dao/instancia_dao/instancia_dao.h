#ifndef INSTANCIA_DAO_H
#define INSTANCIA_DAO_H

#include "../../entidades/instancia/instancia.h"

Boolean instanciaDaoLerQuantidadeDeInstanciasDoArquivo(const char *nomeDoArquivoDeInstancia,InteiroPositivoDe16Bits *quantidadeDeInstancias);
Boolean instanciaDaoLerInstanciaPorIdentificador(const char *nomeDoArquivoDeInstancia,InteiroPositivoDe16Bits identificadorDaInstancia,Instancia *instancia);

#endif
