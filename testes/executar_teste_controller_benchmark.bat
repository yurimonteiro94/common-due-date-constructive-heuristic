@echo off

echo Compilando teste_controller_benchmark...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_controller_benchmark.c controller\controller_benchmark\controller_benchmark.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c -o testes\teste_controller_benchmark.exe

if errorlevel 1 (
    echo Erro ao compilar teste_controller_benchmark.
    exit /b 1
)

echo.
echo Executando teste_controller_benchmark...
testes\teste_controller_benchmark.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_controller_benchmark.
    exit /b 1
)

echo.
echo Teste controller_benchmark passou.
exit /b 0