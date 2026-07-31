@echo off

echo Compilando teste_experimento_amostral...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_experimento_amostral.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_de_tempo\gerenciador_de_tempo.c -o testes\teste_experimento_amostral.exe

if errorlevel 1 (
    echo Erro ao compilar teste_experimento_amostral.
    exit /b 1
)

echo.
echo Executando teste_experimento_amostral...
testes\teste_experimento_amostral.exe

if errorlevel 1 (
    echo.
    echo Existem erros no teste_experimento_amostral.
    exit /b 1
)

echo.
echo Teste amostral finalizado com sucesso.
exit /b 0