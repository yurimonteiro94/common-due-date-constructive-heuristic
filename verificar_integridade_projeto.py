import os
import subprocess
import sys

ARQUIVOS_OBRIGATORIOS = [
    ".gitignore",
    "README.md",
    "main.c",
    "controller/controller_busca_local/controller_busca_local.c",
    "controller/controller_busca_local/controller_busca_local.h",
    "controller/controller_busca_local_estrutural/controller_busca_local_estrutural.c",
    "controller/controller_busca_local_estrutural/controller_busca_local_estrutural.h",
    "controller/controller_busca_local_vshape/controller_busca_local_vshape.c",
    "controller/controller_busca_local_vshape/controller_busca_local_vshape.h",
    "controller/controller_busca_local_melhor_melhoria/controller_busca_local_melhor_melhoria.c",
    "controller/controller_busca_local_melhor_melhoria/controller_busca_local_melhor_melhoria.h",
    "testes/teste_controller_busca_local.c",
    "testes/teste_busca_local_composta.c",
    "testes/teste_controller_busca_local_estrutural.c",
    "testes/teste_controller_busca_local_vshape.c",
    "testes/teste_controller_busca_local_melhor_melhoria.c",
    "testes/experimento_busca_local_composta.c",
    "testes/experimento_busca_local_estrutural.c",
    "testes/experimento_busca_local_vshape.c",
    "testes/experimento_busca_local_melhor_melhoria.c",
    "testes/executar_teste_controller_busca_local.bat",
    "testes/executar_teste_busca_local_composta.bat",
    "testes/executar_teste_controller_busca_local_estrutural.bat",
    "testes/executar_teste_controller_busca_local_vshape.bat",
    "testes/executar_teste_controller_busca_local_melhor_melhoria.bat",
    "testes/executar_experimento_busca_local_melhor_melhoria_paralelo.bat",
    "testes/executar_experimento_busca_local_melhor_melhoria_paralelo.ps1",
    "analisar_busca_local_adaptativa.py",
    "analisar_busca_local_composta.py",
    "analisar_busca_local_estrutural.py",
    "analisar_busca_local_vshape.py",
    "analisar_busca_local_melhor_melhoria.py"
]

EXTENSOES_DE_TEXTO = {".c",".h",".py",".bat",".ps1",".md"}

def verificar_arquivos():
    quantidade_de_erros = 0

    print("Verificacao de existencia e conteudo")
    print("-----------------------------------")

    for caminho in ARQUIVOS_OBRIGATORIOS:
        if not os.path.isfile(caminho):
            print("[FALTA] " + caminho)
            quantidade_de_erros += 1
            continue

        tamanho = os.path.getsize(caminho)

        if tamanho == 0:
            print("[VAZIO] " + caminho)
            quantidade_de_erros += 1
            continue

        extensao = os.path.splitext(caminho)[1].lower()

        if extensao in EXTENSOES_DE_TEXTO:
            with open(caminho,"r",encoding="utf-8",errors="replace") as arquivo:
                quantidade_de_linhas = sum(1 for _ in arquivo)

            print("[OK] " + caminho + " | bytes=" + str(tamanho) + " | linhas=" + str(quantidade_de_linhas))
        else:
            print("[OK] " + caminho + " | bytes=" + str(tamanho))

    return quantidade_de_erros

def verificar_arquivos_suspeitos():
    quantidade_de_erros = 0
    nomes_suspeitos = ["nulcd","nulcopy","nulgit","nulpython"]

    print("")
    print("Verificacao de arquivos suspeitos")
    print("---------------------------------")

    encontrados = []

    for raiz,_,arquivos in os.walk("."):
        for nome_do_arquivo in arquivos:
            nome_em_minusculas = nome_do_arquivo.lower()

            if nome_em_minusculas in nomes_suspeitos or nome_em_minusculas.startswith("nulcd"):
                caminho = os.path.join(raiz,nome_do_arquivo)
                encontrados.append(caminho)

    if len(encontrados) == 0:
        print("[OK] Nenhum arquivo suspeito encontrado.")
    else:
        for caminho in encontrados:
            print("[SUSPEITO] " + caminho)
            quantidade_de_erros += 1

    return quantidade_de_erros

def verificar_sintaxe_python():
    print("")
    print("Verificacao de sintaxe Python")
    print("-----------------------------")

    arquivos_python = [
        caminho
        for caminho in ARQUIVOS_OBRIGATORIOS
        if caminho.endswith(".py") and os.path.isfile(caminho)
    ]

    if len(arquivos_python) == 0:
        print("[ERRO] Nenhum arquivo Python encontrado.")
        return 1

    comando = [sys.executable,"-m","py_compile"] + arquivos_python
    resultado = subprocess.run(comando,capture_output=True,text=True)

    if resultado.returncode != 0:
        print("[ERRO] Falha de sintaxe Python.")
        print(resultado.stdout)
        print(resultado.stderr)
        return 1

    print("[OK] Sintaxe dos arquivos Python validada.")

    return 0

def verificar_git():
    print("")
    print("Estado do Git")
    print("-------------")

    resultado = subprocess.run(
        ["git","status","--short"],
        capture_output=True,
        text=True
    )

    if resultado.returncode != 0:
        print("[ERRO] Nao foi possivel consultar o Git.")
        print(resultado.stderr)
        return 1

    conteudo = resultado.stdout.strip()

    if conteudo == "":
        print("[OK] Arvore de trabalho limpa.")
    else:
        print("Alteracoes atuais:")
        print(conteudo)

    return 0

def main():
    quantidade_de_erros = 0

    quantidade_de_erros += verificar_arquivos()
    quantidade_de_erros += verificar_arquivos_suspeitos()
    quantidade_de_erros += verificar_sintaxe_python()
    quantidade_de_erros += verificar_git()

    print("")
    print("Resultado final")
    print("---------------")

    if quantidade_de_erros > 0:
        print("Integridade reprovada. Erros encontrados: " + str(quantidade_de_erros))
        sys.exit(1)

    print("Integridade estrutural aprovada.")
    sys.exit(0)

if __name__ == "__main__":
    main()