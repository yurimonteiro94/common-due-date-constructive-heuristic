@echo off

echo Compilando teste_tarefa...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_tarefa.c model\entidades\tarefa\tarefa.c -o testes\teste_tarefa.exe

if errorlevel 1 (
    echo Erro ao compilar teste_tarefa.
    exit /b 1
)

echo.
echo Executando teste_tarefa...
testes\teste_tarefa.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_tarefa.
    exit /b 1
)

echo.
echo Compilando teste_instancia...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_instancia.c model\entidades\tarefa\tarefa.c model\entidades\instancia\instancia.c -o testes\teste_instancia.exe

if errorlevel 1 (
    echo Erro ao compilar teste_instancia.
    exit /b 1
)

echo.
echo Executando teste_instancia...
testes\teste_instancia.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_instancia.
    exit /b 1
)

echo.
echo Compilando teste_solucao...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_solucao.c model\entidades\solucao\solucao.c -o testes\teste_solucao.exe

if errorlevel 1 (
    echo Erro ao compilar teste_solucao.
    exit /b 1
)

echo.
echo Executando teste_solucao...
testes\teste_solucao.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_solucao.
    exit /b 1
)

echo.
echo Compilando teste_heuristica...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_heuristica.c model\entidades\heuristica\heuristica.c -o testes\teste_heuristica.exe

if errorlevel 1 (
    echo Erro ao compilar teste_heuristica.
    exit /b 1
)

echo.
echo Executando teste_heuristica...
testes\teste_heuristica.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_heuristica.
    exit /b 1
)

echo.
echo Compilando teste_resultado_de_execucao...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_resultado_de_execucao.c model\entidades\resultado_de_execucao\resultado_de_execucao.c -o testes\teste_resultado_de_execucao.exe

if errorlevel 1 (
    echo Erro ao compilar teste_resultado_de_execucao.
    exit /b 1
)

echo.
echo Executando teste_resultado_de_execucao...
testes\teste_resultado_de_execucao.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_resultado_de_execucao.
    exit /b 1
)

echo.
echo Compilando teste_experimento...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_experimento.c model\entidades\heuristica\heuristica.c model\entidades\experimento\experimento.c -o testes\teste_experimento.exe

if errorlevel 1 (
    echo Erro ao compilar teste_experimento.
    exit /b 1
)

echo.
echo Executando teste_experimento...
testes\teste_experimento.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_experimento.
    exit /b 1
)

echo.
echo Compilando teste_ferramentas...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_ferramentas.c services\ferramentas\ferramentas.c -o testes\teste_ferramentas.exe

if errorlevel 1 (
    echo Erro ao compilar teste_ferramentas.
    exit /b 1
)

echo.
echo Executando teste_ferramentas...
testes\teste_ferramentas.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_ferramentas.
    exit /b 1
)

echo.
echo Compilando teste_gerenciador_de_arquivos...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_gerenciador_de_arquivos.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c -o testes\teste_gerenciador_de_arquivos.exe

if errorlevel 1 (
    echo Erro ao compilar teste_gerenciador_de_arquivos.
    exit /b 1
)

echo.
echo Executando teste_gerenciador_de_arquivos...
testes\teste_gerenciador_de_arquivos.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_gerenciador_de_arquivos.
    exit /b 1
)

echo.
echo Compilando teste_instancia_dao...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_instancia_dao.c model\dao\instancia_dao\instancia_dao.c model\entidades\instancia\instancia.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c -o testes\teste_instancia_dao.exe

if errorlevel 1 (
    echo Erro ao compilar teste_instancia_dao.
    exit /b 1
)

echo.
echo Executando teste_instancia_dao...
testes\teste_instancia_dao.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_instancia_dao.
    exit /b 1
)

echo.
echo Compilando teste_gerenciador_de_custos...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_gerenciador_de_custos.c services\gerenciador_de_custos\gerenciador_de_custos.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c -o testes\teste_gerenciador_de_custos.exe

if errorlevel 1 (
    echo Erro ao compilar teste_gerenciador_de_custos.
    exit /b 1
)

echo.
echo Executando teste_gerenciador_de_custos...
testes\teste_gerenciador_de_custos.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_gerenciador_de_custos.
    exit /b 1
)

echo.
echo Compilando teste_gerenciador_de_tempo...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_gerenciador_de_tempo.c services\gerenciador_de_tempo\gerenciador_de_tempo.c -o testes\teste_gerenciador_de_tempo.exe

if errorlevel 1 (
    echo Erro ao compilar teste_gerenciador_de_tempo.
    exit /b 1
)

echo.
echo Executando teste_gerenciador_de_tempo...
testes\teste_gerenciador_de_tempo.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_gerenciador_de_tempo.
    exit /b 1
)

echo.
echo Compilando teste_gerenciador_fuzzy...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_gerenciador_fuzzy.c services\gerenciador_fuzzy\gerenciador_fuzzy.c model\entidades\tarefa\tarefa.c -o testes\teste_gerenciador_fuzzy.exe

if errorlevel 1 (
    echo Erro ao compilar teste_gerenciador_fuzzy.
    exit /b 1
)

echo.
echo Executando teste_gerenciador_fuzzy...
testes\teste_gerenciador_fuzzy.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_gerenciador_fuzzy.
    exit /b 1
)

echo.
echo Compilando teste_controller_heuristica...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_controller_heuristica.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_fuzzy\gerenciador_fuzzy.c -o testes\teste_controller_heuristica.exe

if errorlevel 1 (
    echo Erro ao compilar teste_controller_heuristica.
    exit /b 1
)

echo.
echo Executando teste_controller_heuristica...
testes\teste_controller_heuristica.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_controller_heuristica.
    exit /b 1
)

echo.
echo Compilando teste_resultado_dao...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_resultado_dao.c model\dao\resultado_dao\resultado_dao.c model\entidades\resultado_de_execucao\resultado_de_execucao.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c -o testes\teste_resultado_dao.exe

if errorlevel 1 (
    echo Erro ao compilar teste_resultado_dao.
    exit /b 1
)

echo.
echo Executando teste_resultado_dao...
testes\teste_resultado_dao.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_resultado_dao.
    exit /b 1
)

echo.
echo Compilando teste_controller_experimento...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 testes\teste_controller_experimento.c controller\controller_experimento\controller_experimento.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\dao\resultado_dao\resultado_dao.c model\entidades\experimento\experimento.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\resultado_de_execucao\resultado_de_execucao.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_de_tempo\gerenciador_de_tempo.c services\gerenciador_fuzzy\gerenciador_fuzzy.c -o testes\teste_controller_experimento.exe

if errorlevel 1 (
    echo Erro ao compilar teste_controller_experimento.
    exit /b 1
)

echo.
echo Executando teste_controller_experimento...
testes\teste_controller_experimento.exe

if errorlevel 1 (
    echo.
    echo Existem testes com erro em teste_controller_experimento.
    exit /b 1
)

echo.
echo Compilando programa principal...
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 main.c view\view_console\view_console.c controller\controller_experimento\controller_experimento.c controller\controller_heuristica\controller_heuristica.c model\dao\instancia_dao\instancia_dao.c model\dao\resultado_dao\resultado_dao.c model\entidades\experimento\experimento.c model\entidades\heuristica\heuristica.c model\entidades\instancia\instancia.c model\entidades\resultado_de_execucao\resultado_de_execucao.c model\entidades\solucao\solucao.c model\entidades\tarefa\tarefa.c services\ferramentas\ferramentas.c services\gerenciador_de_arquivos\gerenciador_de_arquivos.c services\gerenciador_de_custos\gerenciador_de_custos.c services\gerenciador_de_tempo\gerenciador_de_tempo.c services\gerenciador_fuzzy\gerenciador_fuzzy.c -o prazo_nebuloso.exe

if errorlevel 1 (
    echo Erro ao compilar programa principal.
    exit /b 1
)

echo.
echo Todos os testes passaram.
echo Programa principal compilado: prazo_nebuloso.exe
exit /b 0