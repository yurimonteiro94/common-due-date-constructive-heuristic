#include "gerenciador_de_tempo.h"

#include <time.h>

TempoComputacionalEmSegundos gerenciadorDeTempoObterTempoAtualEmSegundos(void) {
    return (TempoComputacionalEmSegundos) clock() / (TempoComputacionalEmSegundos) CLOCKS_PER_SEC;
}

TempoComputacionalEmSegundos gerenciadorDeTempoCalcularDuracaoEmSegundos(TempoComputacionalEmSegundos tempoInicialEmSegundos,TempoComputacionalEmSegundos tempoFinalEmSegundos) {
    if(tempoInicialEmSegundos < 0.0) {
        return 0.0;
    }

    else if(tempoFinalEmSegundos < 0.0) {
        return 0.0;
    }

    else if(tempoFinalEmSegundos < tempoInicialEmSegundos) {
        return 0.0;
    }

    return tempoFinalEmSegundos - tempoInicialEmSegundos;
}

TempoComputacionalEmMilissegundos gerenciadorDeTempoConverterSegundosParaMilissegundos(TempoComputacionalEmSegundos tempoEmSegundos) {
    if(tempoEmSegundos < 0.0) {
        return 0.0;
    }

    return tempoEmSegundos * MILISSEGUNDOS_POR_SEGUNDO;
}

Boolean gerenciadorDeTempoValorEhValido(TempoComputacionalEmSegundos tempoEmSegundos) {
    if(tempoEmSegundos < 0.0) {
        return FALSO;
    }

    return VERDADEIRO;
}
