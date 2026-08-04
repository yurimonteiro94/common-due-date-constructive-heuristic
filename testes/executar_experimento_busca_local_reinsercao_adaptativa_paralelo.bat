@echo off

cd /d "%~dp0.."

echo Compilando experimento da busca local por reinsercao adaptativa...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 -I. testes\experimento_busca_local_reinsercao_adaptativa.c controller\controller_busca_local\controller_busca_local.c controller\controller_busca_local_reinsercao_adaptativa\controller_busca_local_reinsercao_adaptativa.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_de_tempo\gerenciador_de_tempo.c -o testes\experimento_busca_local_reinsercao_adaptativa.exe

if errorlevel 1 (
    echo.
    echo Erro ao compilar o experimento da busca local por reinsercao adaptativa.
    exit /b 1
)

echo.
echo Removendo somente resultados anteriores desta nova estrategia...

del /q resultados\validacao_busca_local_reinsercao_adaptativa_*.csv 2>nul
del /q resultados\validacao_busca_local_reinsercao_adaptativa_*.log 2>nul
del /q resultados\validacao_busca_local_reinsercao_adaptativa_*.log.erro 2>nul
del /q resultados\resumo_busca_local_reinsercao_adaptativa*.csv 2>nul

echo.
echo Executando fila dinamica com quatro trabalhadores...

powershell -NoProfile -ExecutionPolicy Bypass -File testes\executar_experimento_busca_local_reinsercao_adaptativa_paralelo.ps1

if errorlevel 1 (
    echo.
    echo Erro no experimento paralelo da busca local por reinsercao adaptativa.
    exit /b 1
)

echo.
echo Analisando as 280 execucoes...

python analisar_busca_local_reinsercao_adaptativa.py

if errorlevel 1 (
    echo.
    echo Erro na analise da busca local por reinsercao adaptativa.
    exit /b 1
)

echo.
echo Experimento e analise finalizados com sucesso.

exit /b 0