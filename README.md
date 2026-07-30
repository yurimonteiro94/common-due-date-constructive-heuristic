# Common Due Date Constructive Heuristic

Projeto desenvolvido para a disciplina PRO5826 - Estudo de Meta-heurísticas para Problemas de Produção.

O objetivo deste repositório é implementar uma heurística construtiva para o problema de sequenciamento em máquina única com data comum de entrega, considerando penalidades por adiantamento e atraso.

Este projeto é uma segunda implementação independente, criada para tratar explicitamente a construção incremental da solução. A solução será construída passo a passo, a partir de uma sequência inicialmente vazia, por meio de inserções sucessivas de tarefas.

## Problema tratado

Cada tarefa possui:

- tempo de processamento
- penalidade por adiantamento
- penalidade por atraso

Todas as tarefas compartilham uma data comum de entrega. A função objetivo considera a soma das penalidades de adiantamento e atraso.

## Método

O método a ser implementado será uma heurística construtiva por inserção temporal. A sequência será construída progressivamente, escolhendo tarefas e posições de inserção com base no custo da solução parcial.

A heurística permite ociosidade inicial, mas não permite intervalos ociosos entre tarefas consecutivas depois que o processamento começa.

## Estrutura

O projeto reaproveita parte da infraestrutura de leitura de instâncias, cálculo de custos, execução de experimentos e comparação com benchmarks, mas o núcleo da heurística será substituído por uma construção incremental.

## Repositório anterior

O projeto anterior foi mantido separado e não será apagado. Este repositório corresponde a uma nova implementação.