import csv
import glob

PADRAO = "resultados/validacao_busca_local_vshape_*.csv"

def obter_float(valor):
    return float(valor.replace(",","."))

def main():
    caminhos = sorted(glob.glob(PADRAO))
    linhas = []

    for caminho in caminhos:
        with open(caminho,"r",encoding="utf-8",newline="") as arquivo:
            leitor = csv.DictReader(arquivo)

            for linha in leitor:
                linhas.append(linha)

    if len(linhas) == 0:
        print("Nenhum resultado V-shaped encontrado.")
        return

    linhas.sort(key=lambda linha: (int(linha["n"]),obter_float(linha["h"])))

    print("Resumo da busca local V-shaped")
    print("Arquivos analisados: " + str(len(caminhos)))
    print("Execucoes: " + str(len(linhas)))
    print("")
    print("n;h;construtiva;composta;vshape;ganho_sobre_composta;melhoria_sobre_construtiva;ordenacao;fronteira")

    for linha in linhas:
        print(
            linha["n"] + ";" +
            linha["h"] + ";" +
            linha["custoConstrutiva"] + ";" +
            linha["custoComposta"] + ";" +
            linha["custoVShape"] + ";" +
            format(obter_float(linha["ganhoVShapeSobreCompostaPercentual"]),".6f") + ";" +
            format(obter_float(linha["melhoriaVShapeSobreConstrutivaPercentual"]),".6f") + ";" +
            linha["melhoriasOrdenacao"] + ";" +
            linha["melhoriasFronteira"]
        )

    melhorou = sum(1 for linha in linhas if int(linha["custoVShape"]) < int(linha["custoComposta"]))
    igualou = sum(1 for linha in linhas if int(linha["custoVShape"]) == int(linha["custoComposta"]))
    piorou = sum(1 for linha in linhas if int(linha["custoVShape"]) > int(linha["custoComposta"]))
    ganho_medio = sum(obter_float(linha["ganhoVShapeSobreCompostaPercentual"]) for linha in linhas) / len(linhas)
    melhorias_ordenacao = sum(int(linha["melhoriasOrdenacao"]) for linha in linhas)
    melhorias_fronteira = sum(int(linha["melhoriasFronteira"]) for linha in linhas)

    print("")
    print("Melhorou sobre composta: " + str(melhorou))
    print("Igualou composta: " + str(igualou))
    print("Piorou em relacao a composta: " + str(piorou))
    print("Ganho medio sobre composta: " + format(ganho_medio,".6f") + "%")
    print("Melhorias de ordenacao aceitas: " + str(melhorias_ordenacao))
    print("Melhorias de fronteira aceitas: " + str(melhorias_fronteira))

if __name__ == "__main__":
    main()