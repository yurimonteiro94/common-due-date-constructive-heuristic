import csv
import glob

PADRAO = "resultados/validacao_busca_local_melhor_melhoria_*.csv"

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
        print("Nenhum resultado da melhor melhoria encontrado.")
        return

    linhas.sort(key=lambda linha: (int(linha["n"]),obter_float(linha["h"])))

    print("Resumo da busca local por melhor melhoria")
    print("Arquivos analisados: " + str(len(caminhos)))
    print("Execucoes: " + str(len(linhas)))
    print("")
    print("n;h;construtiva;primeira;melhor;ganho_melhor;tempo_primeira_ms;tempo_melhor_ms;iteracoes_primeira;iteracoes_melhor;vizinhos_melhor")

    for linha in linhas:
        print(
            linha["n"] + ";" +
            linha["h"] + ";" +
            linha["custoConstrutiva"] + ";" +
            linha["custoPrimeiraMelhoria"] + ";" +
            linha["custoMelhorMelhoria"] + ";" +
            format(obter_float(linha["ganhoMelhorSobrePrimeiraPercentual"]),".6f") + ";" +
            format(obter_float(linha["tempoPrimeiraMelhoriaMs"]),".3f") + ";" +
            format(obter_float(linha["tempoMelhorMelhoriaMs"]),".3f") + ";" +
            linha["iteracoesPrimeira"] + ";" +
            linha["iteracoesMelhor"] + ";" +
            linha["vizinhosMelhor"]
        )

    melhorou = sum(1 for linha in linhas if int(linha["custoMelhorMelhoria"]) < int(linha["custoPrimeiraMelhoria"]))
    igualou = sum(1 for linha in linhas if int(linha["custoMelhorMelhoria"]) == int(linha["custoPrimeiraMelhoria"]))
    piorou = sum(1 for linha in linhas if int(linha["custoMelhorMelhoria"]) > int(linha["custoPrimeiraMelhoria"]))
    ganho_medio = sum(obter_float(linha["ganhoMelhorSobrePrimeiraPercentual"]) for linha in linhas) / len(linhas)
    tempo_primeira = sum(obter_float(linha["tempoPrimeiraMelhoriaMs"]) for linha in linhas)
    tempo_melhor = sum(obter_float(linha["tempoMelhorMelhoriaMs"]) for linha in linhas)

    print("")
    print("Melhorou sobre primeira melhoria: " + str(melhorou))
    print("Igualou primeira melhoria: " + str(igualou))
    print("Piorou em relacao a primeira melhoria: " + str(piorou))
    print("Ganho medio sobre primeira melhoria: " + format(ganho_medio,".6f") + "%")
    print("Soma dos tempos individuais da primeira melhoria: " + format(tempo_primeira,".3f") + " ms")
    print("Soma dos tempos individuais da melhor melhoria: " + format(tempo_melhor,".3f") + " ms")

if __name__ == "__main__":
    main()