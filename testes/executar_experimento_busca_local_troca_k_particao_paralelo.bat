@echo off

cd /d "%~dp0.."

echo Compilando experimento da busca local por troca-k da particao...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 -I. testes\experimento_busca_local_troca_k_particao.c controller\controller_busca_local\controller_busca_local.c controller\controller_busca_local_troca_k_particao\controller_busca_local_troca_k_particao.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_de_tempo\gerenciador_de_tempo.c -o testes\experimento_busca_local_troca_k_particao.exe

if errorlevel 1 (
    echo.
    echo Erro ao compilar o experimento da busca local por troca-k da particao.
    exit /b 1
)

echo.
echo Removendo somente resultados anteriores desta estrategia...

del /q resultados\validacao_busca_local_troca_k_particao_*.csv 2>nul
del /q resultados\validacao_busca_local_troca_k_particao_*.log 2>nul
del /q resultados\validacao_busca_local_troca_k_particao_*.log.erro 2>nul
del /q resultados\resumo_busca_local_troca_k_particao*.csv 2>nul

echo.
echo Executando fila dinamica paralela...

powershell -NoProfile -ExecutionPolicy Bypass -File testes\executar_experimento_busca_local_troca_k_particao_paralelo.ps1

if errorlevel 1 (
    echo.
    echo Erro no experimento paralelo da busca local por troca-k da particao.
    exit /b 1
)

echo.
echo Analisando as 280 execucoes...

python analisar_busca_local_troca_k_particao.py

if errorlevel 1 (
    echo.
    echo Erro na analise da busca local por troca-k da particao.
    exit /b 1
)

echo.
echo Experimento e analise finalizados com sucesso.

exit /b 0