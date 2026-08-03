@echo off

echo Compilando teste da busca local por melhor melhoria...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 testes\teste_controller_busca_local_melhor_melhoria.c controller\controller_busca_local\controller_busca_local.c controller\controller_busca_local_melhor_melhoria\controller_busca_local_melhor_melhoria.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\gerenciador_de_custos\gerenciador_de_custos.c -o testes\teste_controller_busca_local_melhor_melhoria.exe

if errorlevel 1 (
    echo Erro ao compilar o teste da melhor melhoria.
    exit /b 1
)

echo.
echo Executando teste da busca local por melhor melhoria...

testes\teste_controller_busca_local_melhor_melhoria.exe

if errorlevel 1 (
    echo.
    echo Erro no teste da melhor melhoria.
    exit /b 1
)

echo.
echo Teste da melhor melhoria finalizado com sucesso.

exit /b 0