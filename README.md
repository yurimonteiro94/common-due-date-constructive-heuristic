# Heuristica Construtiva para Sequenciamento com Data Comum de Entrega

Este repositorio contem a implementacao de uma heuristica construtiva para o problema de sequenciamento em maquina unica com data comum de entrega, considerando penalidades por adiantamento e atraso.

O projeto foi desenvolvido para a disciplina PRO5826, Estudo de Meta-heuristicas para Problemas de Producao, do Programa de Pos-Graduacao em Engenharia de Producao da USP.

## Problema tratado

O problema considerado envolve um conjunto de tarefas que devem ser processadas em uma unica maquina. Cada tarefa possui um tempo de processamento, uma penalidade por conclusao antes da data comum de entrega e uma penalidade por conclusao depois da data comum de entrega.

Todas as tarefas compartilham a mesma data de entrega. Para cada tarefa, o custo depende da diferenca entre o instante de conclusao e essa data comum. Se a tarefa termina antes da data, ha penalidade por adiantamento. Se termina depois, ha penalidade por atraso.

A funcao objetivo e minimizar a soma ponderada dos adiantamentos e atrasos de todas as tarefas.

## Heuristica implementada

A heuristica implementada e uma heuristica construtiva por insercao temporal.

A solucao comeca com uma sequencia vazia. A cada iteracao, a heuristica escolhe uma tarefa ainda nao inserida e uma posicao de insercao na sequencia parcial corrente. Depois dessa decisao, a tarefa e fixada na sequencia. O processo e repetido ate que todas as tarefas sejam inseridas.

O metodo nao constroi varias solucoes completas para depois escolher a melhor. Tambem nao realiza busca local, trocas entre tarefas, remocao de tarefas ja fixadas ou selecao final entre diferentes heuristicas completas. A construcao segue uma unica trajetoria incremental.

Durante a construcao, sao usados criterios ligados as caracteristicas do problema:

- prioridade por tempo de processamento e maior penalidade;
- avaliacao de posicoes de insercao na sequencia parcial;
- ajuste temporal da sequencia por ociosidade inicial;
- posicoes proximas da data comum de entrega;
- posicoes inspiradas na estrutura V-shaped do problema;
- termo de arrependimento construtivo para favorecer tarefas com menor flexibilidade de insercao.

A heuristica permite ociosidade inicial, mas nao permite intervalos ociosos entre tarefas consecutivas depois que o processamento comeca.

## Por que o metodo e construtivo

O metodo e construtivo porque inicia com uma sequencia vazia e incorpora uma tarefa por iteracao. Em cada passo, sao avaliadas tarefas candidatas e posicoes de insercao apenas na solucao parcial corrente. Apos essa avaliacao, uma unica tarefa e uma unica posicao sao fixadas.

A heuristica nao seleciona a melhor entre varias solucoes completas. Os criterios de prioridade, conclusao proxima da data comum, estrutura V-shaped e arrependimento sao usados somente para orientar a proxima insercao da solucao parcial.

## Instancias

As instancias utilizadas estao na pasta instancias.

O conjunto inclui os arquivos sch10.txt, sch20.txt, sch50.txt, sch100.txt, sch200.txt, sch500.txt e sch1000.txt.

Cada arquivo contem 10 instancias. Para cada instancia, sao avaliados quatro valores do fator associado a data comum de entrega. No total, o experimento completo executa 280 casos.

## Benchmarks

A pasta benchmarks contem o arquivo referencias_benchmark.csv, usado para comparar os resultados obtidos pela heuristica com valores de referencia.

A analise dos resultados compara a solucao obtida com a melhor solucao conhecida cadastrada e tambem com a solucao dos autores, quando aplicavel.

## Como compilar e executar o experimento completo

No Windows, usando CMD:

cd C:\Users\yuri\Documents\MestradoPPGEP\Disciplinas\EstudoDeMetaHeuristicas\ProjetoConstrutivo

executar_experimento_completo.bat

O script compila o projeto e executa o experimento completo.

Os arquivos de saida sao gerados na pasta resultados.

## Como analisar os resultados

Depois de executar o experimento completo, rode:

python analisar_resultados.py

Esse script le os arquivos gerados em resultados e imprime um resumo com medias por tamanho de instancia e fator da data comum, alem da comparacao com os valores de referencia.

## Como rodar os testes

Para compilar e executar a bateria de testes:

testes\executar_testes.bat

Tambem ha scripts especificos para testes menores:

testes\executar_teste_experimento_amostral.bat
testes\executar_teste_controller_benchmark.bat

## Resultados finais da versao atual

A versao atual da heuristica construtiva apresentou os seguintes resultados no experimento completo com 280 execucoes:

Gap medio percentual: 2.212252 por cento
Melhor gap percentual: -5.675164 por cento
Pior gap percentual: 18.032787 por cento

Melhores que a referencia: 29
Iguais a referencia: 26
Piores que a referencia: 225

Esses resultados correspondem a versao com prioridade por maior penalidade, insercao adaptativa, arrependimento construtivo, posicoes proximas da data comum e posicoes inspiradas na estrutura V-shaped.

## Estrutura do repositorio

benchmarks: referencias usadas na comparacao dos resultados.
controller: controladores da heuristica, do experimento e da leitura de benchmark.
instancias: instancias do problema.
model: entidades e DAOs usados no projeto.
resultados: pasta de saida dos experimentos. Os arquivos gerados nao sao versionados.
services: funcoes auxiliares, constantes, calculo de custos, tempo e arquivos.
testes: testes automatizados e scripts de execucao.
view: saida textual do programa.

## Observacao sobre o repositorio anterior

Este repositorio e uma nova implementacao, separada do projeto anterior. A versao atual foi organizada para deixar explicita a construcao incremental da solucao e evitar a escolha entre varias solucoes completas.
