@echo off

echo Compilando varredura de raio da busca local...

gcc -std=c17 -Wall -Wextra -Wpedantic -O2 testes\experimento_busca_local_raio.c controller\controller_busca_local\controller_busca_local.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_de_tempo\gerenciador_de_tempo.c -o testes\experimento_busca_local_raio.exe

if errorlevel 1 (
    echo Erro ao compilar a varredura de raio da busca local.
    exit /b 1
)

echo.
echo Executando varredura de raio da busca local...

testes\experimento_busca_local_raio.exe

if errorlevel 1 (
    echo.
    echo Erro na varredura de raio da busca local.
    exit /b 1
)

echo.
echo Varredura de raio da busca local finalizada com sucesso.

exit /b 0