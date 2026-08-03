import csv
import glob
import os
import sys


def carregar_resultados():
    caminhos = sorted(glob.glob("resultados/validacao_busca_local_inversao_*.csv"))
    resultados = []

    for caminho in caminhos:
        with open(caminho,"r",encoding="utf-8",newline="") as arquivo:
            leitor = csv.DictReader(arquivo)

            for linha in leitor:
                linha["_arquivo_resultado"] = caminho
                resultados.append(linha)

    return caminhos,resultados


def numero(linha,chave):
    return float(linha[chave])


def inteiro(linha,chave):
    return int(linha[chave])


def main():
    caminhos,resultados = carregar_resultados()

    print("Resumo da busca local por inversao")
    print("Arquivos analisados: " + str(len(caminhos)))
    print("Execucoes: " + str(len(resultados)))
    print()

    if(len(resultados) == 0):
        print("Nenhum resultado encontrado.")
        return 1

    resultados.sort(key=lambda linha: (inteiro(linha,"n"),numero(linha,"h")))

    print("n;h;construtiva;hibrida;inversao;ganho_inversao;melhoria_construtiva;tempo_ms;ciclos;iteracoes_inversao;vizinhos_inversao;inversoes")
    for linha in resultados:
        print(
            linha["n"] + ";" +
            linha["h"] + ";" +
            linha["custoConstrutiva"] + ";" +
            linha["custoHibrida"] + ";" +
            linha["custoInversao"] + ";" +
            format(numero(linha,"ganhoInversaoSobreHibridaPercentual"),".6f") + ";" +
            format(numero(linha,"melhoriaInversaoSobreConstrutivaPercentual"),".6f") + ";" +
            format(numero(linha,"tempoInversaoMs"),".3f") + ";" +
            linha["ciclos"] + ";" +
            linha["iteracoesInversao"] + ";" +
            linha["vizinhosInversao"] + ";" +
            linha["inversoesAceitas"]
        )

    melhorou = sum(1 for linha in resultados if inteiro(linha,"custoInversao") < inteiro(linha,"custoHibrida"))
    igualou = sum(1 for linha in resultados if inteiro(linha,"custoInversao") == inteiro(linha,"custoHibrida"))
    piorou = sum(1 for linha in resultados if inteiro(linha,"custoInversao") > inteiro(linha,"custoHibrida"))
    ganho_medio = sum(numero(linha,"ganhoInversaoSobreHibridaPercentual") for linha in resultados) / len(resultados)
    melhoria_media = sum(numero(linha,"melhoriaInversaoSobreConstrutivaPercentual") for linha in resultados) / len(resultados)
    tempo_medio = sum(numero(linha,"tempoInversaoMs") for linha in resultados) / len(resultados)
    tempo_maximo = max(numero(linha,"tempoInversaoMs") for linha in resultados)
    inversoes = sum(inteiro(linha,"inversoesAceitas") for linha in resultados)
    vizinhos = sum(inteiro(linha,"vizinhosInversao") for linha in resultados)

    print()
    print("Melhorou sobre hibrida: " + str(melhorou))
    print("Igualou hibrida: " + str(igualou))
    print("Piorou em relacao a hibrida: " + str(piorou))
    print("Ganho medio sobre hibrida: " + format(ganho_medio,".6f") + "%")
    print("Melhoria media sobre construtiva: " + format(melhoria_media,".6f") + "%")
    print("Tempo medio da busca com inversao: " + format(tempo_medio,".3f") + " ms")
    print("Tempo maximo da busca com inversao: " + format(tempo_maximo,".3f") + " ms")
    print("Inversoes aceitas: " + str(inversoes))
    print("Vizinhos de inversao avaliados: " + str(vizinhos))

    return 0


if(__name__ == "__main__"):
    sys.exit(main())
