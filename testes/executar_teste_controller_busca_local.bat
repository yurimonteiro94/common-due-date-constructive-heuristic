@echo off

echo Compilando teste da busca local...
gcc -std=c17 -Wall -Wextra -Wpedantic -O2 testes\teste_controller_busca_local.c controller\controller_busca_local\controller_busca_local.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\gerenciador_de_custos\gerenciador_de_custos.c -o testes\teste_controller_busca_local.exe

if errorlevel 1 (
    echo Erro ao compilar o teste da busca local.
    exit /b 1
)

echo.
echo Executando teste da busca local...
testes\teste_controller_busca_local.exe

if errorlevel 1 (
    echo.
    echo Erro no teste da busca local.
    exit /b 1
)

echo.
echo Teste da busca local finalizado com sucesso.

exit /b 0
