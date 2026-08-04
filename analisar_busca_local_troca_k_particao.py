import csv
import glob
import os
import re
import statistics
import sys

CAMINHO_BENCHMARK = os.path.join("benchmarks","referencias_benchmark.csv")
CAMINHO_RESUMO = os.path.join("resultados","resumo_busca_local_troca_k_particao.csv")
CAMINHO_RESUMO_N_H = os.path.join("resultados","resumo_busca_local_troca_k_particao_por_n_h.csv")
QUANTIDADE_TOTAL_ESPERADA = 280
PADRAO_RESULTADO = re.compile(r"^validacao_busca_local_troca_k_particao_[0-9]+_[0-9]{2}_[0-9]{2}\.csv$")
PADRAO_RESULTADO_ANTERIOR = re.compile(r"^validacao_busca_local_particao_estrutural_[0-9]+_[0-9]{2}_[0-9]{2}\.csv$")


def criar_chave(linha):
    return (
        int(linha["n"]),
        int(linha["idInstancia"]),
        float(linha["h"])
    )


def carregar_csv(caminho):
    with open(caminho,"r",encoding="utf-8",newline="") as arquivo:
        return list(csv.DictReader(arquivo))


def localizar_arquivos(padrao):
    caminhos = []

    for caminho in glob.glob(os.path.join("resultados","*.csv")):
        if(padrao.fullmatch(os.path.basename(caminho)) is not None):
            caminhos.append(caminho)

    return sorted(caminhos)


def carregar_resultados():
    caminhos = localizar_arquivos(PADRAO_RESULTADO)
    resultados = []
    chaves = set()

    for caminho in caminhos:
        linhas = carregar_csv(caminho)

        if(len(linhas) != 1):
            raise ValueError("Arquivo com quantidade inesperada de linhas: " + caminho)

        linha = linhas[0]
        chave = criar_chave(linha)

        if(chave in chaves):
            raise ValueError("Resultado duplicado para a chave: " + str(chave))

        chaves.add(chave)
        resultados.append(linha)

    return caminhos,resultados


def carregar_benchmark():
    referencias = {}

    for linha in carregar_csv(CAMINHO_BENCHMARK):
        referencias[criar_chave(linha)] = linha

    return referencias


def carregar_resultados_anteriores():
    resultados = {}

    for caminho in localizar_arquivos(PADRAO_RESULTADO_ANTERIOR):
        linhas = carregar_csv(caminho)

        if(len(linhas) != 1):
            continue

        linha = linhas[0]
        resultados[criar_chave(linha)] = int(linha["custoBuscaLocal"])

    return resultados


def calcular_variacao_percentual(valor,referencia):
    if(referencia == 0):
        return 0.0

    return ((float(valor) - float(referencia)) * 100.0) / float(referencia)


def enriquecer_resultados(resultados,referencias,resultados_anteriores):
    enriquecidos = []

    for linha_original in resultados:
        linha = dict(linha_original)
        chave = criar_chave(linha)

        if(chave not in referencias):
            raise KeyError("Referencia de benchmark ausente para: " + str(chave))

        custo_construtiva = int(linha["custoConstrutiva"])
        custo_busca_local = int(linha["custoBuscaLocal"])
        referencia = referencias[chave]
        custo_autores = int(referencia["custoAutores"])

        if(custo_busca_local > custo_construtiva):
            raise ValueError("Busca local pior que a construtiva em: " + str(chave))

        if(chave[0] == 10 and custo_busca_local < custo_autores):
            raise ValueError("Custo abaixo do otimo global conhecido em n=10: " + str(chave))

        linha["melhoriaAbsolutaSobreConstrutiva"] = custo_construtiva - custo_busca_local
        linha["melhoriaPercentualSobreConstrutiva"] = format(
            ((float(custo_construtiva - custo_busca_local)) * 100.0) / float(custo_construtiva) if(custo_construtiva > 0) else 0.0,
            ".9f"
        )
        linha["custoAutores"] = custo_autores
        linha["tipoAutores"] = referencia["tipoAutores"]
        linha["referenciaEhOtimoGlobal"] = "sim" if(chave[0] == 10) else "nao"
        linha["diferencaAbsolutaParaAutores"] = custo_busca_local - custo_autores
        linha["gapPercentualParaAutores"] = format(
            calcular_variacao_percentual(custo_busca_local,custo_autores),
            ".9f"
        )

        if(chave in resultados_anteriores):
            custo_anterior = resultados_anteriores[chave]
            linha["custoBuscaLocalAnterior"] = custo_anterior
            linha["ganhoAbsolutoSobreBuscaAnterior"] = custo_anterior - custo_busca_local
            linha["ganhoPercentualSobreBuscaAnterior"] = format(
                ((float(custo_anterior - custo_busca_local)) * 100.0) / float(custo_anterior) if(custo_anterior > 0) else 0.0,
                ".9f"
            )
        else:
            linha["custoBuscaLocalAnterior"] = ""
            linha["ganhoAbsolutoSobreBuscaAnterior"] = ""
            linha["ganhoPercentualSobreBuscaAnterior"] = ""

        enriquecidos.append(linha)

    return enriquecidos


def media(resultados,campo):
    return statistics.fmean(float(linha[campo]) for linha in resultados)


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
        "vizinhosUmaTroca": sum(int(linha["vizinhosUmaTroca"]) for linha in resultados),
        "vizinhosCompostos": sum(int(linha["vizinhosDuasOuTresTrocas"]) for linha in resultados),
        "movimentosUmaTroca": sum(int(linha["movimentosUmaTrocaAceitos"]) for linha in resultados),
        "movimentosCompostos": sum(int(linha["movimentosDuasOuTresTrocasAceitos"]) for linha in resultados)
    }


def resumir_comparacao_anterior(resultados):
    comparaveis = [linha for linha in resultados if linha["custoBuscaLocalAnterior"] != ""]

    if(len(comparaveis) == 0):
        return None

    return {
        "execucoes": len(comparaveis),
        "melhores": sum(int(linha["custoBuscaLocal"]) < int(linha["custoBuscaLocalAnterior"]) for linha in comparaveis),
        "iguais": sum(int(linha["custoBuscaLocal"]) == int(linha["custoBuscaLocalAnterior"]) for linha in comparaveis),
        "piores": sum(int(linha["custoBuscaLocal"]) > int(linha["custoBuscaLocalAnterior"]) for linha in comparaveis),
        "ganhoMedio": statistics.fmean(float(linha["ganhoPercentualSobreBuscaAnterior"]) for linha in comparaveis),
        "melhorGanho": max(float(linha["ganhoPercentualSobreBuscaAnterior"]) for linha in comparaveis),
        "piorGanho": min(float(linha["ganhoPercentualSobreBuscaAnterior"]) for linha in comparaveis)
    }


def agrupar(resultados,chaves):
    grupos = {}

    for linha in resultados:
        chave = tuple(linha[nome] for nome in chaves)
        grupos.setdefault(chave,[]).append(linha)

    return grupos


def imprimir_resumo_geral(resumo):
    print("Resumo geral da busca local por troca-k da particao")
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
    print("Vizinhos de uma troca: " + str(resumo["vizinhosUmaTroca"]))
    print("Vizinhos de duas ou tres trocas: " + str(resumo["vizinhosCompostos"]))
    print("Movimentos de uma troca aceitos: " + str(resumo["movimentosUmaTroca"]))
    print("Movimentos de duas ou tres trocas aceitos: " + str(resumo["movimentosCompostos"]))


def imprimir_otimos_n10(resultados):
    casos = [linha for linha in resultados if int(linha["n"]) == 10]

    if(len(casos) == 0):
        return

    atingidos = sum(int(linha["custoBuscaLocal"]) == int(linha["custoAutores"]) for linha in casos)

    print()
    print("Otimos globais conhecidos para n=10")
    print("Casos avaliados: " + str(len(casos)))
    print("Otimos atingidos: " + str(atingidos))
    print("Otimos nao atingidos: " + str(len(casos) - atingidos))
    print("Gap medio para o otimo: " + format(media(casos,"gapPercentualParaAutores"),".6f") + "%")


def imprimir_comparacao_anterior(resumo):
    if(resumo is None):
        return

    print()
    print("Comparacao diagnostica com a particao estrutural anterior")
    print("Execucoes comparadas: " + str(resumo["execucoes"]))
    print("Melhores: " + str(resumo["melhores"]))
    print("Iguais: " + str(resumo["iguais"]))
    print("Piores: " + str(resumo["piores"]))
    print("Ganho medio: " + format(resumo["ganhoMedio"],".6f") + "%")
    print("Melhor ganho: " + format(resumo["melhorGanho"],".6f") + "%")
    print("Pior ganho: " + format(resumo["piorGanho"],".6f") + "%")


def imprimir_casos_acima_dos_autores(resultados):
    casos = [linha for linha in resultados if int(linha["custoBuscaLocal"]) > int(linha["custoAutores"])]

    print()
    print("Casos que permanecem acima dos autores")

    if(len(casos) == 0):
        print("Nenhum caso.")

        return

    for linha in sorted(casos,key=lambda item: float(item["gapPercentualParaAutores"]),reverse=True):
        print(
            "n=" + linha["n"] +
            " | instancia=" + linha["idInstancia"] +
            " | h=" + linha["h"] +
            " | busca_local=" + linha["custoBuscaLocal"] +
            " | autores=" + str(linha["custoAutores"]) +
            " | gap=" + format(float(linha["gapPercentualParaAutores"]),".6f") + "%"
        )


def imprimir_casos_piores_que_anterior(resultados):
    casos = [
        linha for linha in resultados
        if(linha["custoBuscaLocalAnterior"] != "" and int(linha["custoBuscaLocal"]) > int(linha["custoBuscaLocalAnterior"]))
    ]

    print()
    print("Casos piores que a particao estrutural anterior")

    if(len(casos) == 0):
        print("Nenhum caso.")

        return

    for linha in sorted(casos,key=lambda item: float(item["ganhoPercentualSobreBuscaAnterior"])):
        print(
            "n=" + linha["n"] +
            " | instancia=" + linha["idInstancia"] +
            " | h=" + linha["h"] +
            " | troca_k=" + linha["custoBuscaLocal"] +
            " | anterior=" + str(linha["custoBuscaLocalAnterior"]) +
            " | ganho=" + format(float(linha["ganhoPercentualSobreBuscaAnterior"]),".6f") + "%"
        )


def salvar_resultados_enriquecidos(resultados):
    campos = [
        "arquivo",
        "idInstancia",
        "n",
        "h",
        "raioTroca",
        "limiteCandidatosDuasTrocasPorLado",
        "limiteCandidatosTresTrocasPorLado",
        "custoConstrutiva",
        "custoBuscaLocal",
        "melhoriaAbsolutaSobreConstrutiva",
        "melhoriaPercentualSobreConstrutiva",
        "custoAutores",
        "tipoAutores",
        "referenciaEhOtimoGlobal",
        "diferencaAbsolutaParaAutores",
        "gapPercentualParaAutores",
        "custoBuscaLocalAnterior",
        "ganhoAbsolutoSobreBuscaAnterior",
        "ganhoPercentualSobreBuscaAnterior",
        "tempoConstrutivaMs",
        "tempoBuscaLocalMs",
        "iteracoes",
        "vizinhosAvaliados",
        "vizinhosUmaTroca",
        "vizinhosDuasOuTresTrocas",
        "movimentosUmaTrocaAceitos",
        "movimentosDuasOuTresTrocasAceitos"
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
        print("Nenhum resultado da busca local por troca-k da particao foi encontrado.")

        return 1

    referencias = carregar_benchmark()
    resultados_anteriores = carregar_resultados_anteriores()
    resultados = enriquecer_resultados(resultados,referencias,resultados_anteriores)
    resumo_geral = resumir(resultados)

    imprimir_resumo_geral(resumo_geral)
    imprimir_otimos_n10(resultados)
    imprimir_comparacao_anterior(resumir_comparacao_anterior(resultados))
    print()
    print("Resumo por tamanho")

    for chave,linhas in sorted(agrupar(resultados,["n"]).items(),key=lambda item: int(item[0][0])):
        resumo = resumir(linhas)
        print(
            "n=" + chave[0] +
            " | melhoria_construtiva=" + format(resumo["melhoriaMediaConstrutiva"],".6f") + "%" +
            " | gap_autores=" + format(resumo["gapMedioAutores"],".6f") + "%" +
            " | abaixo/igual/acima=" + str(resumo["abaixoAutores"]) + "/" + str(resumo["iguaisAutores"]) + "/" + str(resumo["acimaAutores"]) +
            " | tempo_ms=" + format(resumo["tempoMedioMs"],".3f")
        )

    print()
    print("Resumo por h")

    for chave,linhas in sorted(agrupar(resultados,["h"]).items(),key=lambda item: float(item[0][0])):
        resumo = resumir(linhas)
        print(
            "h=" + chave[0] +
            " | melhoria_construtiva=" + format(resumo["melhoriaMediaConstrutiva"],".6f") + "%" +
            " | gap_autores=" + format(resumo["gapMedioAutores"],".6f") + "%" +
            " | abaixo/igual/acima=" + str(resumo["abaixoAutores"]) + "/" + str(resumo["iguaisAutores"]) + "/" + str(resumo["acimaAutores"]) +
            " | tempo_ms=" + format(resumo["tempoMedioMs"],".3f")
        )

    imprimir_casos_acima_dos_autores(resultados)
    imprimir_casos_piores_que_anterior(resultados)
    salvar_resultados_enriquecidos(resultados)
    salvar_resumo_por_n_h(resultados)

    print()
    print("Arquivos individuais analisados: " + str(len(caminhos)))
    print("Resumo completo: " + CAMINHO_RESUMO)
    print("Resumo por n e h: " + CAMINHO_RESUMO_N_H)

    if(len(resultados) != QUANTIDADE_TOTAL_ESPERADA):
        print()
        print("Atencao: eram esperadas " + str(QUANTIDADE_TOTAL_ESPERADA) + " execucoes.")

    return 0


if(__name__ == "__main__"):
    try:
        sys.exit(main())
    except (OSError,ValueError,KeyError) as excecao:
        print("Falha na analise: " + str(excecao))

        sys.exit(1)