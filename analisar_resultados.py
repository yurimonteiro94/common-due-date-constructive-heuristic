from pathlib import Path
import csv

CAMINHO_MEDIAS = Path("resultados/medias_por_n_h.csv")
CAMINHO_COMPARACAO = Path("resultados/comparacao_benchmark.csv")
CAMINHO_SAIDA = Path("resultados/resumo_analise_resultados.txt")


def ler_csv(caminho):
    with caminho.open("r", encoding="utf-8", newline="") as arquivo:
        return list(csv.DictReader(arquivo))


def converter_float(valor):
    if(valor is None):
        return None

    valor = str(valor).strip()

    if(valor == "" or valor.upper() == "NA"):
        return None

    return float(valor.replace(",", "."))


def analisar_medias(linhas):
    tempos = []
    custos = []

    for linha in linhas:
        custo = converter_float(linha.get("custoMedio"))
        tempo = converter_float(linha.get("tempoMedioMilissegundos"))

        if(custo is not None):
            custos.append(custo)

        if(tempo is not None):
            tempos.append(tempo)

    saida = []
    saida.append("MEDIAS POR N E H")
    saida.append("")
    saida.append("Quantidade de linhas: {}".format(len(linhas)))

    if(custos):
        saida.append("Menor custo medio: {:.6f}".format(min(custos)))
        saida.append("Maior custo medio: {:.6f}".format(max(custos)))

    if(tempos):
        saida.append("Menor tempo medio ms: {:.6f}".format(min(tempos)))
        saida.append("Maior tempo medio ms: {:.6f}".format(max(tempos)))
        saida.append("Tempo medio geral ms: {:.6f}".format(sum(tempos) / len(tempos)))

    saida.append("")
    saida.append("Tabela de medias:")
    saida.append("n;h;execucoes;custoMedio;tempoMedioMs")

    for linha in linhas:
        saida.append("{};{};{};{};{}".format(
            linha.get("n", "NA"),
            linha.get("h", "NA"),
            linha.get("quantidadeExecucoes", "NA"),
            linha.get("custoMedio", "NA"),
            linha.get("tempoMedioMilissegundos", "NA")
        ))

    return saida


def comparar_com_coluna(linhas,nomeDaComparacao,colunaReferencia,colunaTipoReferencia,colunaGap):
    quantidadeComReferencia = 0
    quantidadeSemReferencia = 0
    quantidadeMelhor = 0
    quantidadeIgual = 0
    quantidadePior = 0
    gaps = []
    distribuicaoTipos = {}

    for linha in linhas:
        custoHeuristica = converter_float(linha.get("custoHeuristica"))
        custoReferencia = converter_float(linha.get(colunaReferencia))
        gap = converter_float(linha.get(colunaGap))
        tipo = str(linha.get(colunaTipoReferencia, "")).strip()

        if(tipo != ""):
            if(tipo not in distribuicaoTipos):
                distribuicaoTipos[tipo] = 0

            distribuicaoTipos[tipo] = distribuicaoTipos[tipo] + 1

        if(custoHeuristica is None or custoReferencia is None):
            quantidadeSemReferencia = quantidadeSemReferencia + 1
            continue

        quantidadeComReferencia = quantidadeComReferencia + 1

        if(custoHeuristica < custoReferencia):
            quantidadeMelhor = quantidadeMelhor + 1
        elif(custoHeuristica == custoReferencia):
            quantidadeIgual = quantidadeIgual + 1
        else:
            quantidadePior = quantidadePior + 1

        if(gap is None and custoReferencia != 0):
            gap = ((custoHeuristica - custoReferencia) / custoReferencia) * 100.0

        if(gap is not None):
            gaps.append(gap)

    saida = []
    saida.append("")
    saida.append(nomeDaComparacao)
    saida.append("")
    saida.append("Total de linhas: {}".format(len(linhas)))
    saida.append("Referencias numericas: {}".format(quantidadeComReferencia))
    saida.append("Sem referencia numerica: {}".format(quantidadeSemReferencia))

    if(distribuicaoTipos):
        saida.append("")
        saida.append("Distribuicao dos tipos de referencia:")

        for tipo in sorted(distribuicaoTipos.keys()):
            saida.append("{}: {}".format(tipo,distribuicaoTipos[tipo]))

    saida.append("")
    saida.append("Melhores que a referencia: {}".format(quantidadeMelhor))
    saida.append("Iguais a referencia: {}".format(quantidadeIgual))
    saida.append("Piores que a referencia: {}".format(quantidadePior))

    if(quantidadeComReferencia > 0):
        saida.append("")
        saida.append("Percentual melhor: {:.6f}%".format(100.0 * quantidadeMelhor / quantidadeComReferencia))
        saida.append("Percentual igual: {:.6f}%".format(100.0 * quantidadeIgual / quantidadeComReferencia))
        saida.append("Percentual pior: {:.6f}%".format(100.0 * quantidadePior / quantidadeComReferencia))

    if(gaps):
        saida.append("")
        saida.append("Gap medio percentual: {:.6f}%".format(sum(gaps) / len(gaps)))
        saida.append("Melhor gap percentual: {:.6f}%".format(min(gaps)))
        saida.append("Pior gap percentual: {:.6f}%".format(max(gaps)))

    return saida


def main():
    linhasMedias = ler_csv(CAMINHO_MEDIAS)
    linhasComparacao = ler_csv(CAMINHO_COMPARACAO)

    saida = []
    saida.append("RESUMO DA ANALISE DOS RESULTADOS")
    saida.append("=" * 60)
    saida.append("")

    saida.extend(analisar_medias(linhasMedias))

    saida.extend(comparar_com_coluna(
        linhasComparacao,
        "COMPARACAO COM MELHOR SOLUCAO CONHECIDA",
        "custoMelhorConhecido",
        "tipoMelhorConhecido",
        "gapMelhorSolucaoConhecidaPercentual"
    ))

    saida.extend(comparar_com_coluna(
        linhasComparacao,
        "COMPARACAO COM SOLUCAO DOS AUTORES",
        "custoAutores",
        "tipoAutores",
        "gapSolucaoAutoresPercentual"
    ))

    texto = "\n".join(saida)

    CAMINHO_SAIDA.write_text(texto, encoding="utf-8")

    print(texto)
    print("")
    print("Resumo salvo em: {}".format(CAMINHO_SAIDA))

    return 0


if(__name__ == "__main__"):
    raise SystemExit(main())