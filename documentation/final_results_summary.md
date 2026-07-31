# Final Results Summary

This file records the final results of the constructive heuristic based on temporal insertion.

The resultados folder is used only as experiment output and is not versioned in Git. For this reason, this summary stores the main numbers obtained by the final version of the method.

## Final heuristic configuration

The final version uses the following elements:

- largest-penalty priority;
- adaptive insertion with two candidate jobs per iteration;
- evaluation of positions in the partial solution;
- candidate position based on completion close to the common due date;
- candidate positions inspired by the V-shaped structure;
- constructive regret in the insertion choice;
- temporal displacement through initial idle time;
- no idle intervals between jobs after processing starts.

## Final parameters

QUANTIDADE_MAXIMA_DE_TAREFAS_CANDIDATAS = 2
PESO_ARREPENDIMENTO_PERCENTUAL = 45
RAIO_DA_JANELA_CONCLUSAO_NA_DATA = 40
RAIO_DA_JANELA_V_SHAPED = 40

## Overall result

Full experiment with 280 executions.

Average percentage gap: 2.212252 percent
Best percentage gap: -5.675164 percent
Worst percentage gap: 18.032787 percent

Better than the reference: 29
Equal to the reference: 26
Worse than the reference: 225

## Interpretation

The current heuristic is constructive because it builds a single solution through successive insertions. It does not generate a set of complete solutions for later comparison and it does not perform local search.

The results show that the method finds solutions better than the reference in part of the cases, ties in other cases and remains above the reference in most instances. The main goal of this version is to preserve the methodological adequacy of a constructive heuristic, avoiding mechanisms that select among complete solutions.

## Discarded exploratory tests

During development, several variations were tested, including different numbers of candidate jobs, different weights for the regret term, different radii for temporal windows, local temporal targets, partial use of V-shaped positions, removal of V-shaped positions and an alternative architecture based on incremental two-block partitioning.

These variations did not bring enough improvement to replace the final configuration.
