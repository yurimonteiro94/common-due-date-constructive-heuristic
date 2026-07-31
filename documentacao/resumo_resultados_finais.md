# Resumo dos Resultados Finais

Este arquivo registra os resultados finais da heuristica construtiva por insercao temporal.

A pasta resultados e usada apenas como saida dos experimentos e nao e versionada no Git. Por isso, este resumo guarda os principais numeros obtidos na versao final do metodo.

## Configuracao final da heuristica

A versao final utiliza os seguintes elementos:

- prioridade por maior penalidade;
- insercao adaptativa com duas tarefas candidatas por iteracao;
- avaliacao de posicoes na solucao parcial;
- posicao candidata por conclusao proxima da data comum;
- posicoes candidatas inspiradas na estrutura V-shaped;
- arrependimento construtivo na escolha da insercao;
- deslocamento temporal por ociosidade inicial;
- ausencia de intervalos ociosos entre tarefas depois do inicio do processamento.

## Parametros finais

QUANTIDADE_MAXIMA_DE_TAREFAS_CANDIDATAS = 2
PESO_ARREPENDIMENTO_PERCENTUAL = 45
RAIO_DA_JANELA_CONCLUSAO_NA_DATA = 40
RAIO_DA_JANELA_V_SHAPED = 40

## Resultado geral

Experimento completo com 280 execucoes.

Gap medio percentual: 2.212252 por cento
Melhor gap percentual: -5.675164 por cento
Pior gap percentual: 18.032787 por cento

Melhores que a referencia: 29
Iguais a referencia: 26
Piores que a referencia: 225

## Interpretacao

A heuristica atual e construtiva porque constroi uma unica solucao por insercoes sucessivas. Ela nao gera um conjunto de solucoes completas para posterior comparacao e nao realiza busca local.

Os resultados mostram que o metodo encontra solucoes melhores que a referencia em parte dos casos, empata em outros e fica acima da referencia na maioria das instancias. O objetivo principal desta versao e manter a adequacao metodologica a proposta de heuristica construtiva, evitando mecanismos de selecao entre solucoes completas.

## Testes exploratorios descartados

Durante o desenvolvimento, foram testadas variacoes com diferentes quantidades de tarefas candidatas, diferentes pesos para o termo de arrependimento, diferentes raios para janelas temporais, alvos temporais locais, uso parcial das posicoes V-shaped, remocao das posicoes V-shaped e arquitetura alternativa por particao incremental em dois blocos.

Essas variacoes nao trouxeram ganho suficiente para substituir a configuracao final.
