import csv
from collections import defaultdict

CAMINHO = "resultados/validacao_busca_local_composta.csv"

def obter_float(valor):
    return float(valor.replace(",","."))

def imprimir_resumo(titulo,grupos):
    print("")
    print(titulo)
    print("-" * len(titulo))
    print("grupo;execucoes;ganho_medio_sobre_reinsercao;melhorou_em;igual_em;piorou_em;tempo_medio_reinsercao_ms;tempo_medio_composta_ms;trocas_aceitas")

    for chave in sorted(grupos.keys()):
        linhas = grupos[chave]
        quantidade = len(linhas)
        ganho_medio = sum(obter_float(linha["ganhoCompostaSobreReinsercaoPercentual"]) for linha in linhas) / quantidade
        melhorou = sum(1 for linha in linhas if int(linha["custoComposta"]) < int(linha["custoReinsercao"]))
        igual = sum(1 for linha in linhas if int(linha["custoComposta"]) == int(linha["custoReinsercao"]))
        piorou = sum(1 for linha in linhas if int(linha["custoComposta"]) > int(linha["custoReinsercao"]))
        tempo_reinsercao = sum(obter_float(linha["tempoReinsercaoMs"]) for linha in linhas) / quantidade
        tempo_composta = sum(obter_float(linha["tempoCompostaMs"]) for linha in linhas) / quantidade
        trocas_aceitas = sum(int(linha["melhoriasCompostaTroca"]) for linha in linhas)

        print(str(chave) + ";" + str(quantidade) + ";" + format(ganho_medio,".6f") + ";" + str(melhorou) + ";" + str(igual) + ";" + str(piorou) + ";" + format(tempo_reinsercao,".3f") + ";" + format(tempo_composta,".3f") + ";" + str(trocas_aceitas))

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
    por_h = defaultdict(list)

    for linha in linhas:
        por_n[int(linha["n"])].append(linha)
        por_h[linha["h"]].append(linha)

    melhorou = sum(1 for linha in linhas if int(linha["custoComposta"]) < int(linha["custoReinsercao"]))
    igual = sum(1 for linha in linhas if int(linha["custoComposta"]) == int(linha["custoReinsercao"]))
    piorou = sum(1 for linha in linhas if int(linha["custoComposta"]) > int(linha["custoReinsercao"]))

    print("Resumo da busca local composta")
    print("Arquivo analisado: " + CAMINHO)
    print("Execucoes: " + str(len(linhas)))
    print("Melhorou sobre reinsercao: " + str(melhorou))
    print("Igualou reinsercao: " + str(igual))
    print("Piorou em relacao a reinsercao: " + str(piorou))

    imprimir_resumo("Resumo por n",por_n)
    imprimir_resumo("Resumo por h",por_h)

if __name__ == "__main__":
    main()