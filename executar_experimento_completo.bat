@echo off

echo Compilando experimento completo...
gcc -std=c17 -Wall -Wextra -Wpedantic -O2 main.c view\view_console\view_console.c controller\controller_benchmark\controller_benchmark.c controller\controller_experimento\controller_experimento.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\dao\resultado_dao\resultado_dao.c model\entidades\experimento\experimento.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\resultado_de_execucao\resultado_de_execucao.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_de_tempo\gerenciador_de_tempo.c services\gerenciador_fuzzy\gerenciador_fuzzy.c -o heuristica_construtiva.exe

if errorlevel 1 (
    echo Erro ao compilar o experimento completo.
    exit /b 1
)

echo.
echo Executando experimento completo...
heuristica_construtiva.exe

if errorlevel 1 (
    echo.
    echo Erro durante a execucao do experimento completo.
    exit /b 1
)

echo.
echo Experimento completo finalizado com sucesso.
echo Arquivos esperados:
echo resultados\resultados_execucoes.csv
echo resultados\medias_por_n_h.csv
echo resultados\comparacao_benchmark.csv

exit /b 0