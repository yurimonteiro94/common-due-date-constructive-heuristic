@echo off

cd /d "%~dp0.."

echo Compilando teste da busca local hibrida...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 testes\teste_controller_busca_local_hibrida.c controller\controller_busca_local\controller_busca_local.c controller\controller_busca_local_melhor_melhoria\controller_busca_local_melhor_melhoria.c controller\controller_busca_local_hibrida\controller_busca_local_hibrida.c model\entidades\resultado_busca_local_hibrida\resultado_busca_local_hibrida.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\gerenciador_de_custos\gerenciador_de_custos.c -o testes\teste_controller_busca_local_hibrida.exe

if errorlevel 1 (
    echo Erro ao compilar o teste da busca local hibrida.
    exit /b 1
)

echo.
echo Executando teste da busca local hibrida...

testes\teste_controller_busca_local_hibrida.exe

if errorlevel 1 (
    echo.
    echo Erro no teste da busca local hibrida.
    exit /b 1
)

echo.
echo Teste da busca local hibrida finalizado com sucesso.

exit /b 0