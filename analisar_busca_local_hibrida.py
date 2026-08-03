import csv
import glob
import os

PADRAO_DOS_ARQUIVOS = "resultados/validacao_busca_local_hibrida_*.csv"

def converter_float(valor):
    return float(valor.replace(",","."))

def carregar_linhas():
    arquivos = sorted(glob.glob(PADRAO_DOS_ARQUIVOS))
    linhas = []

    for caminho in arquivos:
        with open(caminho,"r",encoding="utf-8",newline="") as arquivo:
            leitor = csv.DictReader(arquivo)

            for linha in leitor:
                linha["_arquivoResultado"] = os.path.basename(caminho)
                linhas.append(linha)

    return arquivos,linhas

def main():
    arquivos,linhas = carregar_linhas()

    print("Resumo da busca local hibrida")
    print("Arquivos analisados: " + str(len(arquivos)))
    print("Execucoes: " + str(len(linhas)))
    print("")

    if len(linhas) == 0:
        print("Nenhum resultado encontrado.")
        return

    linhas.sort(key=lambda linha: (int(linha["n"]),converter_float(linha["h"])))

    quantidadeMelhorouPrimeira = 0
    quantidadeIgualouPrimeira = 0
    quantidadeMelhorouMelhor = 0
    quantidadeIgualouMelhor = 0
    quantidadeTrajetoriaA = 0
    quantidadeTrajetoriaB = 0
    somaGanhoPrimeira = 0.0
    somaGanhoMelhor = 0.0
    somaMelhoriaConstrutiva = 0.0
    somaTempo = 0.0
    somaVizinhos = 0

    print("n;h;construtiva;primeira;melhor;trajetoria_a;trajetoria_b;hibrida;ganho_primeira;ganho_melhor;melhoria_construtiva;trajetoria;tempo_ms;vizinhos")

    for linha in linhas:
        custoPrimeira = int(linha["custoPrimeira"])
        custoMelhor = int(linha["custoMelhor"])
        custoHibrida = int(linha["custoHibrida"])
        ganhoPrimeira = converter_float(linha["ganhoHibridaSobrePrimeiraPercentual"])
        ganhoMelhor = converter_float(linha["ganhoHibridaSobreMelhorPercentual"])
        melhoriaConstrutiva = converter_float(linha["melhoriaHibridaSobreConstrutivaPercentual"])
        tempo = converter_float(linha["tempoHibridaMs"])
        vizinhos = int(linha["vizinhosTotais"])
        trajetoria = int(linha["trajetoriaSelecionada"])

        if(custoHibrida < custoPrimeira):
            quantidadeMelhorouPrimeira += 1
        elif(custoHibrida == custoPrimeira):
            quantidadeIgualouPrimeira += 1

        if(custoHibrida < custoMelhor):
            quantidadeMelhorouMelhor += 1
        elif(custoHibrida == custoMelhor):
            quantidadeIgualouMelhor += 1

        if(trajetoria == 1):
            quantidadeTrajetoriaA += 1
        elif(trajetoria == 2):
            quantidadeTrajetoriaB += 1

        somaGanhoPrimeira += ganhoPrimeira
        somaGanhoMelhor += ganhoMelhor
        somaMelhoriaConstrutiva += melhoriaConstrutiva
        somaTempo += tempo
        somaVizinhos += vizinhos

        print(
            linha["n"] + ";" +
            linha["h"] + ";" +
            linha["custoConstrutiva"] + ";" +
            linha["custoPrimeira"] + ";" +
            linha["custoMelhor"] + ";" +
            linha["custoTrajetoriaA"] + ";" +
            linha["custoTrajetoriaB"] + ";" +
            linha["custoHibrida"] + ";" +
            format(ganhoPrimeira,".6f") + ";" +
            format(ganhoMelhor,".6f") + ";" +
            format(melhoriaConstrutiva,".6f") + ";" +
            linha["trajetoriaSelecionada"] + ";" +
            format(tempo,".3f") + ";" +
            str(vizinhos)
        )

    quantidade = len(linhas)

    print("")
    print("Melhorou sobre primeira melhoria: " + str(quantidadeMelhorouPrimeira))
    print("Igualou primeira melhoria: " + str(quantidadeIgualouPrimeira))
    print("Melhorou sobre melhor melhoria: " + str(quantidadeMelhorouMelhor))
    print("Igualou melhor melhoria: " + str(quantidadeIgualouMelhor))
    print("Trajetoria A selecionada: " + str(quantidadeTrajetoriaA))
    print("Trajetoria B selecionada: " + str(quantidadeTrajetoriaB))
    print("Ganho medio sobre primeira melhoria: " + format(somaGanhoPrimeira / quantidade,".6f") + "%")
    print("Ganho medio sobre melhor melhoria: " + format(somaGanhoMelhor / quantidade,".6f") + "%")
    print("Melhoria media sobre construtiva: " + format(somaMelhoriaConstrutiva / quantidade,".6f") + "%")
    print("Tempo medio da busca hibrida: " + format(somaTempo / quantidade,".3f") + " ms")
    print("Tempo maximo da busca hibrida: " + format(max(converter_float(linha["tempoHibridaMs"]) for linha in linhas),".3f") + " ms")
    print("Vizinhos medios avaliados: " + format(somaVizinhos / quantidade,".2f"))

if __name__ == "__main__":
    main()