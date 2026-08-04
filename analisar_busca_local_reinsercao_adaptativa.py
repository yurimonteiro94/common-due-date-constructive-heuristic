import csv
import os
import re
import statistics
import sys


PADRAO_ARQUIVO = re.compile(
    r"^validacao_busca_local_reinsercao_adaptativa_"
    r"(10|20|50|100|200|500|1000)_"
    r"(0[1-9]|10)_"
    r"(02|04|06|08)\.csv$"
)

CAMINHO_RESULTADOS = "resultados"
CAMINHO_BENCHMARK = os.path.join("benchmarks","referencias_benchmark.csv")
CAMINHO_RESUMO = os.path.join(CAMINHO_RESULTADOS,"resumo_busca_local_reinsercao_adaptativa.csv")
CAMINHO_RESUMO_N_H = os.path.join(CAMINHO_RESULTADOS,"resumo_busca_local_reinsercao_adaptativa_por_n_h.csv")


def carregar_resultados():
    resultados = []
    caminhos = []

    for nome in sorted(os.listdir(CAMINHO_RESULTADOS)):
        if(PADRAO_ARQUIVO.fullmatch(nome) is None):
            continue

        caminho = os.path.join(CAMINHO_RESULTADOS,nome)

        with open(caminho,"r",encoding="utf-8",newline="") as arquivo:
            linhas = list(csv.DictReader(arquivo))

        if(len(linhas) != 1):
            raise ValueError("Arquivo deve conter exatamente uma execucao: " + caminho)

        linha = linhas[0]
        linha["_arquivo_resultado"] = caminho
        resultados.append(linha)
        caminhos.append(caminho)

    return caminhos,resultados


def carregar_benchmark():
    referencias = {}

    with open(CAMINHO_BENCHMARK,"r",encoding="utf-8",newline="") as arquivo:
        for linha in csv.DictReader(arquivo):
            chave = (
                linha["arquivo"],
                int(linha["idInstancia"]),
                int(linha["n"]),
                format(float(linha["h"]),".1f")
            )
            referencias[chave] = linha

    return referencias


def enriquecer_resultados(resultados,referencias):
    chaves_encontradas = set()

    for linha in resultados:
        chave = (
            linha["arquivo"],
            int(linha["idInstancia"]),
            int(linha["n"]),
            format(float(linha["h"]),".1f")
        )

        if(chave in chaves_encontradas):
            raise ValueError("Execucao duplicada: " + str(chave))

        chaves_encontradas.add(chave)

        if(chave not in referencias):
            raise ValueError("Referencia nao encontrada: " + str(chave))

        referencia = referencias[chave]
        custo_construtiva = int(linha["custoConstrutiva"])
        custo_busca_local = int(linha["custoBuscaLocal"])
        custo_autores = int(referencia["custoAutores"])

        if(custo_busca_local > custo_construtiva):
            raise ValueError("Busca local piorou a construtiva: " + str(chave))

        linha["custoAutores"] = custo_autores
        linha["tipoAutores"] = referencia["tipoAutores"]
        linha["melhoriaAbsolutaSobreConstrutiva"] = custo_construtiva - custo_busca_local
        linha["melhoriaPercentualSobreConstrutiva"] = (
            ((custo_construtiva - custo_busca_local) * 100.0) / custo_construtiva
            if custo_construtiva > 0
            else 0.0
        )
        linha["diferencaAbsolutaParaAutores"] = custo_busca_local - custo_autores
        linha["gapPercentualParaAutores"] = (
            ((custo_busca_local - custo_autores) * 100.0) / custo_autores
            if custo_autores > 0
            else 0.0
        )

    return resultados


def media(resultados,chave):
    return statistics.fmean(float(linha[chave]) for linha in resultados)


def resumir(resultados):
    melhores_construtiva = sum(int(linha["custoBuscaLocal"]) < int(linha["custoConstrutiva"]) for linha in resultados)
    iguais_construtiva = sum(int(linha["custoBuscaLocal"]) == int(linha["custoConstrutiva"]) for linha in resultados)
    piores_construtiva = sum(int(linha["custoBuscaLocal"]) > int(linha["custoConstrutiva"]) for linha in resultados)
    abaixo_autores = sum(int(linha["custoBuscaLocal"]) < int(linha["custoAutores"]) for linha in resultados)
    iguais_autores = sum(int(linha["custoBuscaLocal"]) == int(linha["custoAutores"]) for linha in resultados)
    acima_autores = sum(int(linha["custoBuscaLocal"]) > int(linha["custoAutores"]) for linha in resultados)

    return {
        "execucoes": len(resultados),
        "melhoresConstrutiva": melhores_construtiva,
        "iguaisConstrutiva": iguais_construtiva,
        "pioresConstrutiva": piores_construtiva,
        "melhoriaMediaConstrutiva": media(resultados,"melhoriaPercentualSobreConstrutiva"),
        "melhorMelhoriaConstrutiva": max(float(linha["melhoriaPercentualSobreConstrutiva"]) for linha in resultados),
        "piorMelhoriaConstrutiva": min(float(linha["melhoriaPercentualSobreConstrutiva"]) for linha in resultados),
        "abaixoAutores": abaixo_autores,
        "iguaisAutores": iguais_autores,
        "acimaAutores": acima_autores,
        "gapMedioAutores": media(resultados,"gapPercentualParaAutores"),
        "melhorGapAutores": min(float(linha["gapPercentualParaAutores"]) for linha in resultados),
        "piorGapAutores": max(float(linha["gapPercentualParaAutores"]) for linha in resultados),
        "tempoMedioMs": media(resultados,"tempoBuscaLocalMs"),
        "tempoMedianoMs": statistics.median(float(linha["tempoBuscaLocalMs"]) for linha in resultados),
        "tempoMaximoMs": max(float(linha["tempoBuscaLocalMs"]) for linha in resultados),
        "vizinhos": sum(int(linha["vizinhosAvaliados"]) for linha in resultados),
        "reinsercoes": sum(int(linha["reinsercoesAceitas"]) for linha in resultados)
    }


def imprimir_resumo(titulo,resumo):
    print(titulo)
    print("Execucoes: " + str(resumo["execucoes"]))
    print()
    print("Busca local versus heuristica construtiva")
    print("Melhores: " + str(resumo["melhoresConstrutiva"]))
    print("Iguais: " + str(resumo["iguaisConstrutiva"]))
    print("Piores: " + str(resumo["pioresConstrutiva"]))
    print("Melhoria media: " + format(resumo["melhoriaMediaConstrutiva"],".6f") + "%")
    print("Melhor melhoria: " + format(resumo["melhorMelhoriaConstrutiva"],".6f") + "%")
    print("Pior melhoria: " + format(resumo["piorMelhoriaConstrutiva"],".6f") + "%")
    print()
    print("Busca local versus resultados dos autores")
    print("Abaixo dos autores: " + str(resumo["abaixoAutores"]))
    print("Iguais aos autores: " + str(resumo["iguaisAutores"]))
    print("Acima dos autores: " + str(resumo["acimaAutores"]))
    print("Gap medio: " + format(resumo["gapMedioAutores"],".6f") + "%")
    print("Melhor gap: " + format(resumo["melhorGapAutores"],".6f") + "%")
    print("Pior gap: " + format(resumo["piorGapAutores"],".6f") + "%")
    print()
    print("Desempenho computacional")
    print("Tempo medio: " + format(resumo["tempoMedioMs"],".3f") + " ms")
    print("Tempo mediano: " + format(resumo["tempoMedianoMs"],".3f") + " ms")
    print("Tempo maximo: " + format(resumo["tempoMaximoMs"],".3f") + " ms")
    print("Vizinhos avaliados: " + str(resumo["vizinhos"]))
    print("Reinsercoes aceitas: " + str(resumo["reinsercoes"]))


def agrupar(resultados,chaves):
    grupos = {}

    for linha in resultados:
        chave = tuple(linha[nome] for nome in chaves)
        grupos.setdefault(chave,[]).append(linha)

    return grupos


def salvar_resultados_enriquecidos(resultados):
    campos = [
        "arquivo",
        "idInstancia",
        "n",
        "h",
        "tipoVizinhanca",
        "raioLocal",
        "quantidadeAncoras",
        "raioFronteira",
        "custoConstrutiva",
        "custoBuscaLocal",
        "melhoriaAbsolutaSobreConstrutiva",
        "melhoriaPercentualSobreConstrutiva",
        "custoAutores",
        "diferencaAbsolutaParaAutores",
        "gapPercentualParaAutores",
        "tempoConstrutivaMs",
        "tempoBuscaLocalMs",
        "iteracoes",
        "vizinhosAvaliados",
        "reinsercoesAceitas"
    ]

    with open(CAMINHO_RESUMO,"w",encoding="utf-8",newline="") as arquivo:
        escritor = csv.DictWriter(arquivo,fieldnames=campos)
        escritor.writeheader()

        for linha in sorted(resultados,key=lambda item: (int(item["n"]),int(item["idInstancia"]),float(item["h"]))):
            escritor.writerow({campo: linha[campo] for campo in campos})


def salvar_resumo_por_n_h(resultados):
    campos = [
        "n",
        "h",
        "execucoes",
        "melhoriaMediaConstrutiva",
        "gapMedioAutores",
        "abaixoAutores",
        "iguaisAutores",
        "acimaAutores",
        "tempoMedioMs"
    ]

    with open(CAMINHO_RESUMO_N_H,"w",encoding="utf-8",newline="") as arquivo:
        escritor = csv.DictWriter(arquivo,fieldnames=campos)
        escritor.writeheader()

        for chave,linhas in sorted(agrupar(resultados,["n","h"]).items(),key=lambda item: (int(item[0][0]),float(item[0][1]))):
            resumo = resumir(linhas)
            escritor.writerow({
                "n": chave[0],
                "h": chave[1],
                "execucoes": resumo["execucoes"],
                "melhoriaMediaConstrutiva": format(resumo["melhoriaMediaConstrutiva"],".9f"),
                "gapMedioAutores": format(resumo["gapMedioAutores"],".9f"),
                "abaixoAutores": resumo["abaixoAutores"],
                "iguaisAutores": resumo["iguaisAutores"],
                "acimaAutores": resumo["acimaAutores"],
                "tempoMedioMs": format(resumo["tempoMedioMs"],".9f")
            })


def main():
    caminhos,resultados = carregar_resultados()

    if(len(resultados) == 0):
        print("Nenhum resultado da busca local por reinsercao adaptativa foi encontrado.")

        return 1

    referencias = carregar_benchmark()
    resultados = enriquecer_resultados(resultados,referencias)

    imprimir_resumo("Resumo geral da busca local por reinsercao adaptativa",resumir(resultados))
    print()
    print("Resumo por tamanho")

    for chave,linhas in sorted(agrupar(resultados,["n"]).items(),key=lambda item: int(item[0][0])):
        resumo = resumir(linhas)
        print(
            "n=" + chave[0] +
            " | melhoria_construtiva=" + format(resumo["melhoriaMediaConstrutiva"],".6f") + "%" +
            " | gap_autores=" + format(resumo["gapMedioAutores"],".6f") + "%" +
            " | abaixo/igual/acima=" +
            str(resumo["abaixoAutores"]) +
            "/" +
            str(resumo["iguaisAutores"]) +
            "/" +
            str(resumo["acimaAutores"]) +
            " | tempo_ms=" +
            format(resumo["tempoMedioMs"],".3f")
        )

    print()
    print("Resumo por h")

    for chave,linhas in sorted(agrupar(resultados,["h"]).items(),key=lambda item: float(item[0][0])):
        resumo = resumir(linhas)
        print(
            "h=" + chave[0] +
            " | melhoria_construtiva=" + format(resumo["melhoriaMediaConstrutiva"],".6f") + "%" +
            " | gap_autores=" + format(resumo["gapMedioAutores"],".6f") + "%" +
            " | abaixo/igual/acima=" +
            str(resumo["abaixoAutores"]) +
            "/" +
            str(resumo["iguaisAutores"]) +
            "/" +
            str(resumo["acimaAutores"]) +
            " | tempo_ms=" +
            format(resumo["tempoMedioMs"],".3f")
        )

    salvar_resultados_enriquecidos(resultados)
    salvar_resumo_por_n_h(resultados)

    print()
    print("Arquivos individuais analisados: " + str(len(caminhos)))
    print("Resumo completo: " + CAMINHO_RESUMO)
    print("Resumo por n e h: " + CAMINHO_RESUMO_N_H)

    if(len(resultados) != 280):
        print()
        print("Atencao: eram esperadas 280 execucoes.")

    return 0


if(__name__ == "__main__"):
    try:
        sys.exit(main())
    except (OSError,ValueError,KeyError) as excecao:
        print("Falha na analise: " + str(excecao))

        sys.exit(1)