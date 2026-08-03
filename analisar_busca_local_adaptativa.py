import csv
from collections import defaultdict

CAMINHO = "resultados/validacao_busca_local_adaptativa.csv"

def converter_float(valor):
    return float(valor.replace(",","."))

def imprimir_resumo(titulo,grupos):
    print("")
    print(titulo)
    print("-" * len(titulo))
    print("grupo;qtd;melhoria_media_percentual;tempo_medio_ms;tempo_maximo_ms;vizinhos_medios;melhorou_em")

    for chave in sorted(grupos.keys()):
        linhas = grupos[chave]
        quantidade = len(linhas)
        melhoria_media = sum(converter_float(linha["melhoriaPercentual"]) for linha in linhas) / quantidade
        tempo_medio = sum(converter_float(linha["tempoBuscaLocalMs"]) for linha in linhas) / quantidade
        tempo_maximo = max(converter_float(linha["tempoBuscaLocalMs"]) for linha in linhas)
        vizinhos_medios = sum(int(linha["vizinhosAvaliados"]) for linha in linhas) / quantidade
        melhorou_em = sum(1 for linha in linhas if converter_float(linha["melhoriaAbsoluta"]) > 0.0)

        print(str(chave) + ";" + str(quantidade) + ";" + format(melhoria_media,".6f") + ";" + format(tempo_medio,".6f") + ";" + format(tempo_maximo,".6f") + ";" + format(vizinhos_medios,".2f") + ";" + str(melhorou_em))

def main():
    linhas = []

    with open(CAMINHO,"r",encoding="utf-8",newline="") as arquivo:
        leitor = csv.DictReader(arquivo)

        for linha in leitor:
            linhas.append(linha)

    if len(linhas) == 0:
        print("Arquivo sem dados.")
        return

    por_n = defaultdict(list)
    por_raio = defaultdict(list)
    por_h = defaultdict(list)

    for linha in linhas:
        por_n[int(linha["n"])].append(linha)
        por_raio[int(linha["raio"])].append(linha)
        por_h[linha["h"]].append(linha)

    print("Resumo da busca local adaptativa")
    print("Arquivo analisado: " + CAMINHO)
    print("Execucoes: " + str(len(linhas)))

    imprimir_resumo("Resumo por n",por_n)
    imprimir_resumo("Resumo por raio",por_raio)
    imprimir_resumo("Resumo por h",por_h)

if __name__ == "__main__":
    main()