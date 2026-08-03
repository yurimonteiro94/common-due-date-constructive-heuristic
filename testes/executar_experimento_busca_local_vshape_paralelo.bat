@echo off

echo Compilando experimento da busca local V-shaped...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 testes\experimento_busca_local_vshape.c controller\controller_busca_local\controller_busca_local.c controller\controller_busca_local_vshape\controller_busca_local_vshape.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c -o testes\experimento_busca_local_vshape.exe

if errorlevel 1 (
    echo Erro ao compilar o experimento V-shaped.
    exit /b 1
)

echo.
echo Executando amostra V-shaped em paralelo...

powershell -NoProfile -ExecutionPolicy Bypass -File testes\executar_experimento_busca_local_vshape_paralelo.ps1

if errorlevel 1 (
    echo.
    echo Erro no experimento V-shaped paralelo.
    exit /b 1
)

echo.
echo Experimento V-shaped paralelo finalizado com sucesso.

exit /b 0