@echo off

cd /d "%~dp0.."

echo Compilando experimento da busca local hibrida...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 testes\experimento_busca_local_hibrida.c controller\controller_busca_local\controller_busca_local.c controller\controller_busca_local_melhor_melhoria\controller_busca_local_melhor_melhoria.c controller\controller_busca_local_hibrida\controller_busca_local_hibrida.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\resultado_busca_local_hibrida\resultado_busca_local_hibrida.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_de_tempo\gerenciador_de_tempo.c -o testes\experimento_busca_local_hibrida.exe

if errorlevel 1 (
    echo Erro ao compilar o experimento da busca local hibrida.
    exit /b 1
)

echo.
echo Executando fila dinamica com quatro trabalhadores...

powershell -NoProfile -ExecutionPolicy Bypass -File testes\executar_experimento_busca_local_hibrida_paralelo.ps1

if errorlevel 1 (
    echo.
    echo Erro no experimento paralelo da busca local hibrida.
    exit /b 1
)

echo.
echo Experimento paralelo da busca local hibrida finalizado com sucesso.

exit /b 0