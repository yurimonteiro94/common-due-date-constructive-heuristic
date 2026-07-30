#include <stdio.h>
#include <string.h>

#include "../model/dao/instancia_dao/instancia_dao.h"
#include "../model/entidades/instancia/instancia.h"

static int testarLeituraDaQuantidadeDeInstancias(void) {
    InteiroPositivoDe16Bits quantidadeDeInstancias;
    Boolean resultado;

    quantidadeDeInstancias = 0;

    resultado = instanciaDaoLerQuantidadeDeInstanciasDoArquivo(
        NOME_ARQUIVO_SCH10,
        &quantidadeDeInstancias
    );

    if(resultado == FALSO) {
        printf("[ERRO] Quantidade de instancias deveria ser lida.\n");

        return 1;
    }

    if(quantidadeDeInstancias != QUANTIDADE_DE_PROBLEMAS_POR_ARQUIVO) {
        printf("[ERRO] Quantidade de instancias deveria ser 10.\n");

        return 1;
    }

    printf("[OK] Leitura da quantidade de instancias.\n");

    return 0;
}

static int testarLeituraDaPrimeiraInstanciaSch10(void) {
    Instancia instancia;
    Boolean resultado;
    DataDeEntregaComum dataDeEntregaComum02;
    DataDeEntregaComum dataDeEntregaComum06;

    instancia = criarInstanciaVazia();

    resultado = instanciaDaoLerInstanciaPorIdentificador(
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 1,
        &instancia
    );

    if(resultado == FALSO) {
        printf("[ERRO] Primeira instancia do sch10 deveria ser lida.\n");

        return 1;
    }

    if(strcmp(instancia.nomeDoArquivoDeOrigem,NOME_ARQUIVO_SCH10) != 0) {
        printf("[ERRO] Nome do arquivo de origem incorreto.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.identificadorDaInstancia != 1) {
        printf("[ERRO] Identificador da instancia incorreto.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.quantidadeDeTarefas != 10) {
        printf("[ERRO] Quantidade de tarefas deveria ser 10.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.somaDosTemposDeProcessamento != 116) {
        printf("[ERRO] Soma dos tempos de processamento deveria ser 116.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instanciaEhValida(&instancia) == FALSO) {
        printf("[ERRO] Instancia lida deveria ser valida.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    dataDeEntregaComum02 = instanciaCalcularDataDeEntregaComum(&instancia,(FatorH) FATOR_H_02);
    dataDeEntregaComum06 = instanciaCalcularDataDeEntregaComum(&instancia,(FatorH) FATOR_H_06);

    if(dataDeEntregaComum02 != 23) {
        printf("[ERRO] Data de entrega comum para h = 0.2 deveria ser 23.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(dataDeEntregaComum06 != 69) {
        printf("[ERRO] Data de entrega comum para h = 0.6 deveria ser 69.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Leitura da primeira instancia do sch10.\n");

    return 0;
}

static int testarLeituraDaDecimaInstanciaSch10(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = instanciaDaoLerInstanciaPorIdentificador(
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 10,
        &instancia
    );

    if(resultado == FALSO) {
        printf("[ERRO] Decima instancia do sch10 deveria ser lida.\n");

        return 1;
    }

    if(instancia.identificadorDaInstancia != 10) {
        printf("[ERRO] Identificador da decima instancia deveria ser 10.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instancia.quantidadeDeTarefas != 10) {
        printf("[ERRO] Quantidade de tarefas da decima instancia deveria ser 10.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instanciaEhValida(&instancia) == FALSO) {
        printf("[ERRO] Decima instancia deveria ser valida.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Leitura da decima instancia do sch10.\n");

    return 0;
}

static int testarLeituraDaPrimeiraInstanciaSch1000(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = instanciaDaoLerInstanciaPorIdentificador(
        NOME_ARQUIVO_SCH1000,
        (InteiroPositivoDe16Bits) 1,
        &instancia
    );

    if(resultado == FALSO) {
        printf("[ERRO] Primeira instancia do sch1000 deveria ser lida.\n");

        return 1;
    }

    if(instancia.quantidadeDeTarefas != 1000) {
        printf("[ERRO] Quantidade de tarefas deveria ser 1000.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    if(instanciaEhValida(&instancia) == FALSO) {
        printf("[ERRO] Primeira instancia do sch1000 deveria ser valida.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    liberarInstancia(&instancia);

    printf("[OK] Leitura da primeira instancia do sch1000.\n");

    return 0;
}

static int testarArquivoInexistente(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = instanciaDaoLerInstanciaPorIdentificador(
        "arquivo_inexistente.txt",
        (InteiroPositivoDe16Bits) 1,
        &instancia
    );

    if(resultado != FALSO) {
        printf("[ERRO] Leitura de arquivo inexistente deveria falhar.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    printf("[OK] Arquivo inexistente no instancia dao.\n");

    return 0;
}

static int testarIdentificadorZeroInvalido(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = instanciaDaoLerInstanciaPorIdentificador(
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 0,
        &instancia
    );

    if(resultado != FALSO) {
        printf("[ERRO] Identificador zero deveria falhar.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    printf("[OK] Identificador zero invalido no instancia dao.\n");

    return 0;
}

static int testarIdentificadorMaiorQueQuantidadeInvalido(void) {
    Instancia instancia;
    Boolean resultado;

    instancia = criarInstanciaVazia();

    resultado = instanciaDaoLerInstanciaPorIdentificador(
        NOME_ARQUIVO_SCH10,
        (InteiroPositivoDe16Bits) 11,
        &instancia
    );

    if(resultado != FALSO) {
        printf("[ERRO] Identificador maior que a quantidade deveria falhar.\n");
        liberarInstancia(&instancia);

        return 1;
    }

    printf("[OK] Identificador maior que quantidade invalido no instancia dao.\n");

    return 0;
}

static int testarParametrosInvalidos(void) {
    InteiroPositivoDe16Bits quantidadeDeInstancias;

    quantidadeDeInstancias = 0;

    if(instanciaDaoLerQuantidadeDeInstanciasDoArquivo(NULL,&quantidadeDeInstancias) != FALSO) {
        printf("[ERRO] Leitura com nome de arquivo NULL deveria falhar.\n");

        return 1;
    }

    if(instanciaDaoLerQuantidadeDeInstanciasDoArquivo(NOME_ARQUIVO_SCH10,NULL) != FALSO) {
        printf("[ERRO] Leitura com ponteiro de quantidade NULL deveria falhar.\n");

        return 1;
    }

    if(instanciaDaoLerInstanciaPorIdentificador(NOME_ARQUIVO_SCH10,(InteiroPositivoDe16Bits) 1,NULL) != FALSO) {
        printf("[ERRO] Leitura com ponteiro de instancia NULL deveria falhar.\n");

        return 1;
    }

    printf("[OK] Parametros invalidos no instancia dao.\n");

    return 0;
}

int main(void) {
    int quantidadeDeErros;

    quantidadeDeErros = 0;

    printf("Executando testes do InstanciaDAO...\n\n");

    quantidadeDeErros += testarLeituraDaQuantidadeDeInstancias();
    quantidadeDeErros += testarLeituraDaPrimeiraInstanciaSch10();
    quantidadeDeErros += testarLeituraDaDecimaInstanciaSch10();
    quantidadeDeErros += testarLeituraDaPrimeiraInstanciaSch1000();
    quantidadeDeErros += testarArquivoInexistente();
    quantidadeDeErros += testarIdentificadorZeroInvalido();
    quantidadeDeErros += testarIdentificadorMaiorQueQuantidadeInvalido();
    quantidadeDeErros += testarParametrosInvalidos();

    printf("\n");

    if(quantidadeDeErros == 0) {
        printf("Todos os testes do InstanciaDAO passaram.\n");

        return 0;
    }

    printf("Quantidade de erros nos testes do InstanciaDAO: %d\n",quantidadeDeErros);

    return 1;
}
