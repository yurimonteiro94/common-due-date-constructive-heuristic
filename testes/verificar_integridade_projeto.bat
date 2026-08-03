@echo off

cd /d "%~dp0.."

echo ============================================================
echo VERIFICACAO DE INTEGRIDADE DO PROJETO
echo ============================================================

python verificar_integridade_projeto.py

if errorlevel 1 (
    echo.
    echo A verificacao de integridade encontrou problemas.
    exit /b 1
)

echo.
echo ============================================================
echo COMPILACAO DOS TESTES PRINCIPAIS
echo ============================================================

call testes\executar_teste_controller_busca_local.bat

if errorlevel 1 exit /b 1

call testes\executar_teste_busca_local_composta.bat

if errorlevel 1 exit /b 1

call testes\executar_teste_controller_busca_local_estrutural.bat

if errorlevel 1 exit /b 1

call testes\executar_teste_controller_busca_local_vshape.bat

if errorlevel 1 exit /b 1

call testes\executar_teste_controller_busca_local_melhor_melhoria.bat

if errorlevel 1 exit /b 1

echo.
echo ============================================================
echo INTEGRIDADE E TESTES APROVADOS
echo ============================================================

exit /b 0