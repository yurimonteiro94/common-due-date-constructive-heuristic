@echo off

cd /d "%~dp0.."

echo Compilando teste da busca local por reinsercao adaptativa...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 -I. testes\teste_controller_busca_local_reinsercao_adaptativa.c controller\controller_busca_local\controller_busca_local.c controller\controller_busca_local_reinsercao_adaptativa\controller_busca_local_reinsercao_adaptativa.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\gerenciador_de_custos\gerenciador_de_custos.c -o testes\teste_controller_busca_local_reinsercao_adaptativa.exe

if errorlevel 1 (
    echo.
    echo Erro ao compilar o teste da busca local por reinsercao adaptativa.
    exit /b 1
)

echo.
echo Executando teste da busca local por reinsercao adaptativa...

testes\teste_controller_busca_local_reinsercao_adaptativa.exe

if errorlevel 1 (
    echo.
    echo Erro no teste da busca local por reinsercao adaptativa.
    exit /b 1
)

echo.
echo Teste da busca local por reinsercao adaptativa finalizado com sucesso.

exit /b 0