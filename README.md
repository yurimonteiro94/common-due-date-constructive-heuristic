# Constructive Heuristic for Common Due Date Scheduling

This repository contains the implementation of a constructive heuristic for the single-machine common due date scheduling problem with earliness and tardiness penalties.

The project was developed for the course PRO5826, Study of Metaheuristics for Production Problems, at the Graduate Program in Production Engineering of the University of Sao Paulo.

## Problem description

The problem considers a set of jobs that must be processed on a single machine. Each job has a processing time, an earliness penalty and a tardiness penalty.

All jobs share the same due date. For each job, the cost depends on the difference between its completion time and the common due date. If the job finishes before the due date, an earliness penalty is incurred. If it finishes after the due date, a tardiness penalty is incurred.

The objective is to minimize the weighted sum of earliness and tardiness penalties over all jobs.

## Implemented heuristic

The implemented method is a constructive heuristic based on temporal insertion.

The solution starts with an empty sequence. At each iteration, the heuristic selects one job that has not yet been inserted and one insertion position in the current partial sequence. After this decision, the job is fixed in the sequence. The process continues until all jobs have been inserted.

The method does not build several complete solutions and then choose the best one. It also does not perform local search, job swaps, removal of already fixed jobs or final selection among different complete heuristics. The construction follows a single incremental trajectory.

During the construction, the method uses criteria related to the structure of the problem:

- priority based on processing time and the largest penalty;
- evaluation of insertion positions in the partial sequence;
- temporal adjustment through initial idle time;
- positions close to the common due date;
- positions inspired by the V-shaped structure of the problem;
- constructive regret to favor jobs with lower insertion flexibility.

The heuristic allows initial idle time, but it does not allow idle intervals between consecutive jobs after processing starts.

## Why the method is constructive

The method is constructive because it starts with an empty sequence and inserts one job per iteration. At each step, candidate jobs and insertion positions are evaluated only in the current partial solution. After this evaluation, one job and one position are fixed.

The heuristic does not select the best solution among several complete alternatives. The priority rule, due-date proximity, V-shaped positions and regret term are used only to guide the next insertion in the partial sequence.

## Instances

The instances are stored in the instancias folder.

The dataset includes the following files: sch10.txt, sch20.txt, sch50.txt, sch100.txt, sch200.txt, sch500.txt and sch1000.txt.

Each file contains 10 instances. For each instance, four values of the due-date factor are evaluated. The full experiment contains 280 executions.

## Benchmarks

The benchmarks folder contains the file referencias_benchmark.csv, which is used to compare the heuristic results with reference values.

The analysis compares the obtained solution with the best known solution registered in the benchmark and also with the authors solution when applicable.

## How to compile and run the full experiment

From the repository root, run:

executar_experimento_completo.bat

The script compiles the project and runs the full experiment. The output files are generated in the resultados folder.

## How to analyze the results

After running the full experiment, run:

python analisar_resultados.py

This script reads the files generated in resultados and prints a summary with averages by instance size and due-date factor, as well as the comparison with the reference values.

## How to run the tests

To compile and run the full test suite, run:

testes\\executar_testes.bat

There are also smaller test scripts:

testes\\executar_teste_experimento_amostral.bat
testes\\executar_teste_controller_benchmark.bat

## Final results of the current version

The current version of the constructive heuristic produced the following results in the full experiment with 280 executions:

Average percentage gap: 2.212252 percent
Best percentage gap: -5.675164 percent
Worst percentage gap: 18.032787 percent

Better than the reference: 29
Equal to the reference: 26
Worse than the reference: 225

These results correspond to the version with largest-penalty priority, adaptive insertion, constructive regret, positions close to the common due date and positions inspired by the V-shaped structure.

## Repository structure

benchmarks: reference values used in the comparison.
controller: heuristic, experiment and benchmark controllers.
documentation: versioned technical documentation.
instancias: problem instances.
model: entities and DAOs used in the project.
resultados: experiment output folder. Generated files are not versioned.
services: auxiliary functions, constants, cost calculation, time and file services.
testes: automated tests and execution scripts.
view: console output.

## Note about the previous repository

This repository is a new implementation, separate from the previous project. The current version was organized to make the incremental construction explicit and to avoid the selection among several complete solutions.
