import csv
import glob

PADRAO = "resultados/validacao_busca_local_estrutural_*.csv"

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
        print("Nenhum resultado estrutural encontrado.")
        return

    linhas.sort(key=lambda linha: (int(linha["n"]),obter_float(linha["h"])))

    print("Resumo da busca local estrutural")
    print("Arquivos analisados: " + str(len(caminhos)))
    print("Execucoes: " + str(len(linhas)))
    print("")
    print("n;h;construtiva;composta;estrutural;ganho_sobre_composta;melhoria_sobre_construtiva;bloco2;bloco3")

    for linha in linhas:
        print(
            linha["n"] + ";" +
            linha["h"] + ";" +
            linha["custoConstrutiva"] + ";" +
            linha["custoComposta"] + ";" +
            linha["custoEstrutural"] + ";" +
            format(obter_float(linha["ganhoEstruturalSobreCompostaPercentual"]),".6f") + ";" +
            format(obter_float(linha["melhoriaEstruturalSobreConstrutivaPercentual"]),".6f") + ";" +
            linha["melhoriasBlocoDois"] + ";" +
            linha["melhoriasBlocoTres"]
        )

    melhorou = sum(1 for linha in linhas if int(linha["custoEstrutural"]) < int(linha["custoComposta"]))
    igualou = sum(1 for linha in linhas if int(linha["custoEstrutural"]) == int(linha["custoComposta"]))
    piorou = sum(1 for linha in linhas if int(linha["custoEstrutural"]) > int(linha["custoComposta"]))
    ganho_medio = sum(obter_float(linha["ganhoEstruturalSobreCompostaPercentual"]) for linha in linhas) / len(linhas)

    print("")
    print("Melhorou sobre composta: " + str(melhorou))
    print("Igualou composta: " + str(igualou))
    print("Piorou em relacao a composta: " + str(piorou))
    print("Ganho medio sobre composta: " + format(ganho_medio,".6f") + "%")

if __name__ == "__main__":
    main()