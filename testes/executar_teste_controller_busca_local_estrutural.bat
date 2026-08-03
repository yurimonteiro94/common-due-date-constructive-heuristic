@echo off

echo Compilando teste da busca local estrutural...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 testes\teste_controller_busca_local_estrutural.c controller\controller_busca_local\controller_busca_local.c controller\controller_busca_local_estrutural\controller_busca_local_estrutural.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\gerenciador_de_custos\gerenciador_de_custos.c -o testes\teste_controller_busca_local_estrutural.exe

if errorlevel 1 (
    echo Erro ao compilar o teste da busca local estrutural.
    exit /b 1
)

echo.
echo Executando teste da busca local estrutural...

testes\teste_controller_busca_local_estrutural.exe

if errorlevel 1 (
    echo.
    echo Erro no teste da busca local estrutural.
    exit /b 1
)

echo.
echo Teste da busca local estrutural finalizado com sucesso.

exit /b 0