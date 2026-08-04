import csv
import glob
import os
import re
import statistics
import sys

CAMINHO_BENCHMARK = os.path.join("benchmarks","referencias_benchmark.csv")
CAMINHO_RESUMO = os.path.join("resultados","resumo_busca_local_subcubos_particao.csv")
CAMINHO_RESUMO_N_H = os.path.join("resultados","resumo_busca_local_subcubos_particao_por_n_h.csv")
CAMINHO_TABELA_CSV = os.path.join("resultados","tabela_busca_local_subcubos_particao_7x4.csv")
CAMINHO_TABELA_TXT = os.path.join("resultados","tabela_busca_local_subcubos_particao_7x4.txt")
QUANTIDADE_TOTAL_ESPERADA = 280
TAMANHOS = [10,20,50,100,200,500,1000]
FATORES_H = [0.2,0.4,0.6,0.8]
PADRAO_RESULTADO = re.compile(r"^validacao_busca_local_subcubos_particao_[0-9]+_[0-9]{2}_[0-9]{2}\.csv$")
PADRAO_RESULTADO_ANTERIOR = re.compile(r"^validacao_busca_local_troca_k_particao_[0-9]+_[0-9]{2}_[0-9]{2}\.csv$")


def criar_chave(linha):
    return int(linha["n"]),int(linha["idInstancia"]),float(linha["h"])


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
        if(len(linhas) == 1):
            resultados[criar_chave(linhas[0])] = int(linhas[0]["custoBuscaLocal"])
    return resultados


def variacao_percentual(valor,referencia):
    if(float(referencia) == 0.0):
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
        custo_autores = int(referencias[chave]["custoAutores"])
        if(custo_busca_local > custo_construtiva):
            raise ValueError("Busca local pior que a construtiva em: " + str(chave))
        if(chave[0] == 10 and custo_busca_local < custo_autores):
            raise ValueError("Custo abaixo do otimo global conhecido em n=10: " + str(chave))
        linha["custoAutores"] = custo_autores
        linha["tipoAutores"] = referencias[chave]["tipoAutores"]
        linha["melhoriaAbsolutaSobreConstrutiva"] = custo_construtiva - custo_busca_local
        linha["melhoriaPercentualSobreConstrutiva"] = format(-variacao_percentual(custo_busca_local,custo_construtiva),".9f")
        linha["gapPercentualVsConstrutiva"] = format(variacao_percentual(custo_busca_local,custo_construtiva),".9f")
        linha["diferencaAbsolutaParaAutores"] = custo_busca_local - custo_autores
        linha["gapPercentualParaAutores"] = format(variacao_percentual(custo_busca_local,custo_autores),".9f")
        if(chave in resultados_anteriores):
            custo_anterior = resultados_anteriores[chave]
            linha["custoBuscaLocalAnterior"] = custo_anterior
            linha["ganhoAbsolutoSobreBuscaAnterior"] = custo_anterior - custo_busca_local
            linha["ganhoPercentualSobreBuscaAnterior"] = format(-variacao_percentual(custo_busca_local,custo_anterior),".9f")
        else:
            linha["custoBuscaLocalAnterior"] = ""
            linha["ganhoAbsolutoSobreBuscaAnterior"] = ""
            linha["ganhoPercentualSobreBuscaAnterior"] = ""
        enriquecidos.append(linha)
    return enriquecidos


def media(resultados,campo):
    return statistics.fmean(float(linha[campo]) for linha in resultados)


def resumir(resultados):
    comparaveis = [linha for linha in resultados if linha["custoBuscaLocalAnterior"] != ""]
    return {
        "execucoes": len(resultados),
        "melhoresConstrutiva": sum(int(linha["custoBuscaLocal"]) < int(linha["custoConstrutiva"]) for linha in resultados),
        "iguaisConstrutiva": sum(int(linha["custoBuscaLocal"]) == int(linha["custoConstrutiva"]) for linha in resultados),
        "pioresConstrutiva": sum(int(linha["custoBuscaLocal"]) > int(linha["custoConstrutiva"]) for linha in resultados),
        "melhoriaMediaConstrutiva": media(resultados,"melhoriaPercentualSobreConstrutiva"),
        "abaixoAutores": sum(int(linha["custoBuscaLocal"]) < int(linha["custoAutores"]) for linha in resultados),
        "iguaisAutores": sum(int(linha["custoBuscaLocal"]) == int(linha["custoAutores"]) for linha in resultados),
        "acimaAutores": sum(int(linha["custoBuscaLocal"]) > int(linha["custoAutores"]) for linha in resultados),
        "gapMedioAutores": media(resultados,"gapPercentualParaAutores"),
        "tempoMedioMs": media(resultados,"tempoBuscaLocalMs"),
        "tempoMedianoMs": statistics.median(float(linha["tempoBuscaLocalMs"]) for linha in resultados),
        "tempoMaximoMs": max(float(linha["tempoBuscaLocalMs"]) for linha in resultados),
        "vizinhos": sum(int(linha["vizinhosAvaliados"]) for linha in resultados),
        "vizinhosUmaTroca": sum(int(linha["vizinhosUmaTroca"]) for linha in resultados),
        "vizinhosSubcubos": sum(int(linha["vizinhosSubcubos"]) for linha in resultados),
        "movimentosUmaTroca": sum(int(linha["movimentosUmaTrocaAceitos"]) for linha in resultados),
        "movimentosSubcubos": sum(int(linha["movimentosSubcubosAceitos"]) for linha in resultados),
        "paineis": sum(int(linha["paineisAvaliados"]) for linha in resultados),
        "maiorCardinalidade": max(int(linha["maiorCardinalidadeAceita"]) for linha in resultados),
        "movimentosQuatroOuMais": sum(int(linha["movimentosQuatroOuMaisAceitos"]) for linha in resultados),
        "comparaveis": len(comparaveis),
        "melhoresAnterior": sum(int(linha["custoBuscaLocal"]) < int(linha["custoBuscaLocalAnterior"]) for linha in comparaveis),
        "iguaisAnterior": sum(int(linha["custoBuscaLocal"]) == int(linha["custoBuscaLocalAnterior"]) for linha in comparaveis),
        "pioresAnterior": sum(int(linha["custoBuscaLocal"]) > int(linha["custoBuscaLocalAnterior"]) for linha in comparaveis),
        "ganhoMedioAnterior": statistics.fmean(float(linha["ganhoPercentualSobreBuscaAnterior"]) for linha in comparaveis) if(len(comparaveis) > 0) else None
    }


def agrupar(resultados,campos):
    grupos = {}
    for linha in resultados:
        chave = tuple(linha[campo] for campo in campos)
        grupos.setdefault(chave,[]).append(linha)
    return grupos


def salvar_csv(caminho,campos,linhas):
    with open(caminho,"w",encoding="utf-8",newline="") as arquivo:
        escritor = csv.DictWriter(arquivo,fieldnames=campos)
        escritor.writeheader()
        for linha in linhas:
            escritor.writerow({campo: linha.get(campo,"") for campo in campos})


def salvar_resultados_enriquecidos(resultados):
    campos = [
        "arquivo","idInstancia","n","h","tamanhoSubcubo","quantidadePaineis",
        "custoConstrutiva","custoBuscaLocal","melhoriaAbsolutaSobreConstrutiva",
        "melhoriaPercentualSobreConstrutiva","gapPercentualVsConstrutiva",
        "custoAutores","tipoAutores","diferencaAbsolutaParaAutores","gapPercentualParaAutores",
        "custoBuscaLocalAnterior","ganhoAbsolutoSobreBuscaAnterior","ganhoPercentualSobreBuscaAnterior",
        "tempoConstrutivaMs","tempoBuscaLocalMs","iteracoes","vizinhosAvaliados",
        "vizinhosUmaTroca","vizinhosSubcubos","movimentosUmaTrocaAceitos",
        "movimentosSubcubosAceitos","paineisAvaliados","maiorCardinalidadeAceita",
        "movimentosQuatroOuMaisAceitos"
    ]
    ordenados = sorted(resultados,key=lambda linha: (int(linha["n"]),float(linha["h"]),int(linha["idInstancia"])))
    salvar_csv(CAMINHO_RESUMO,campos,ordenados)


def criar_resumos_n_h(resultados):
    linhas_resumo = []
    for chave,linhas in sorted(agrupar(resultados,["n","h"]).items(),key=lambda item: (int(item[0][0]),float(item[0][1]))):
        resumo = resumir(linhas)
        linhas_resumo.append({
            "n": chave[0],
            "h": chave[1],
            "execucoes": resumo["execucoes"],
            "custoMedioConstrutiva": format(media(linhas,"custoConstrutiva"),".6f"),
            "custoMedioBuscaLocal": format(media(linhas,"custoBuscaLocal"),".6f"),
            "gapMedioAutores": format(resumo["gapMedioAutores"],".9f"),
            "gapMedioConstrutiva": format(media(linhas,"gapPercentualVsConstrutiva"),".9f"),
            "ganhoMedioTrocaK": format(statistics.fmean(float(linha["ganhoPercentualSobreBuscaAnterior"]) for linha in linhas if linha["ganhoPercentualSobreBuscaAnterior"] != ""),".9f") if(any(linha["ganhoPercentualSobreBuscaAnterior"] != "" for linha in linhas)) else "",
            "tempoMedioMs": format(resumo["tempoMedioMs"],".9f"),
            "abaixoAutores": resumo["abaixoAutores"],
            "iguaisAutores": resumo["iguaisAutores"],
            "acimaAutores": resumo["acimaAutores"]
        })
    return linhas_resumo


def salvar_resumo_n_h(linhas_resumo):
    campos = [
        "n","h","execucoes","custoMedioConstrutiva","custoMedioBuscaLocal",
        "gapMedioAutores","gapMedioConstrutiva","ganhoMedioTrocaK","tempoMedioMs",
        "abaixoAutores","iguaisAutores","acimaAutores"
    ]
    salvar_csv(CAMINHO_RESUMO_N_H,campos,linhas_resumo)


def formatar_valor(metrica,valor):
    if(valor is None):
        return "NA"
    if(metrica in ("GAP vs Lit.","GAP vs Con.","Ganho vs troca-k")):
        return format(valor,"+.2f") + "%"
    if(metrica == "Tempo medio"):
        return format(valor,".3f") + " ms"
    return format(valor,".2f")


def obter_valor_da_metrica(linhas,metrica):
    if(len(linhas) == 0):
        return None
    if(metrica == "Construtiva media"):
        return media(linhas,"custoConstrutiva")
    if(metrica == "Subcubos media"):
        return media(linhas,"custoBuscaLocal")
    if(metrica == "GAP vs Lit."):
        return media(linhas,"gapPercentualParaAutores")
    if(metrica == "GAP vs Con."):
        return media(linhas,"gapPercentualVsConstrutiva")
    if(metrica == "Ganho vs troca-k"):
        comparaveis = [linha for linha in linhas if linha["ganhoPercentualSobreBuscaAnterior"] != ""]
        return statistics.fmean(float(linha["ganhoPercentualSobreBuscaAnterior"]) for linha in comparaveis) if(len(comparaveis) > 0) else None
    if(metrica == "Tempo medio"):
        return media(linhas,"tempoBuscaLocalMs")
    raise ValueError("Metrica desconhecida: " + metrica)


def gerar_tabela_7x4(resultados):
    metricas = ["Construtiva media","Subcubos media","GAP vs Lit.","GAP vs Con.","Ganho vs troca-k","Tempo medio"]
    linhas_csv = []
    linhas_texto = []
    largura_rotulo = 22
    largura_coluna = 15
    cabecalho = "Metrica".ljust(largura_rotulo) + " | " + " | ".join(("n=" + str(n)).rjust(largura_coluna) for n in TAMANHOS) + " | " + "Media".rjust(largura_coluna)
    separador = "-" * len(cabecalho)
    linhas_texto.append("=" * len(cabecalho))
    linhas_texto.append("BUSCA LOCAL POR SUBCUBOS EXATOS DA PARTICAO".center(len(cabecalho)))
    linhas_texto.append("Cada casa representa a media das 10 instancias da combinacao n x h".center(len(cabecalho)))
    linhas_texto.append("=" * len(cabecalho))
    for metrica in metricas:
        linhas_texto.append("")
        linhas_texto.append(metrica.upper())
        linhas_texto.append(cabecalho)
        linhas_texto.append(separador)
        for h in FATORES_H:
            valores = []
            for n in TAMANHOS:
                grupo = [linha for linha in resultados if int(linha["n"]) == n and abs(float(linha["h"]) - h) < 0.000001]
                valores.append(obter_valor_da_metrica(grupo,metrica))
            valores_validos = [valor for valor in valores if(valor is not None)]
            media_linha = statistics.fmean(valores_validos) if(len(valores_validos) > 0) else None
            linha_texto = ("h=" + format(h,".1f")).ljust(largura_rotulo) + " | " + " | ".join(formatar_valor(metrica,valor).rjust(largura_coluna) for valor in valores) + " | " + formatar_valor(metrica,media_linha).rjust(largura_coluna)
            linhas_texto.append(linha_texto)
            linha_csv = {"metrica": metrica,"h": format(h,".1f")}
            for indice,n in enumerate(TAMANHOS):
                linha_csv["n" + str(n)] = "" if(valores[indice] is None) else format(valores[indice],".9f")
            linha_csv["media"] = "" if(media_linha is None) else format(media_linha,".9f")
            linhas_csv.append(linha_csv)
        valores_resumo = []
        for n in TAMANHOS:
            grupo = [linha for linha in resultados if int(linha["n"]) == n]
            valores_resumo.append(obter_valor_da_metrica(grupo,metrica))
        valores_validos = [valor for valor in valores_resumo if(valor is not None)]
        media_geral = statistics.fmean(valores_validos) if(len(valores_validos) > 0) else None
        linhas_texto.append(separador)
        linhas_texto.append("Res".ljust(largura_rotulo) + " | " + " | ".join(formatar_valor(metrica,valor).rjust(largura_coluna) for valor in valores_resumo) + " | " + formatar_valor(metrica,media_geral).rjust(largura_coluna))
        linha_csv = {"metrica": metrica,"h": "Res"}
        for indice,n in enumerate(TAMANHOS):
            linha_csv["n" + str(n)] = "" if(valores_resumo[indice] is None) else format(valores_resumo[indice],".9f")
        linha_csv["media"] = "" if(media_geral is None) else format(media_geral,".9f")
        linhas_csv.append(linha_csv)
    linhas_texto.append("")
    linhas_texto.append("GAP negativo indica custo menor. Ganho positivo contra a troca-k indica melhoria.")
    linhas_texto.append("=" * len(cabecalho))
    campos = ["metrica","h"] + ["n" + str(n) for n in TAMANHOS] + ["media"]
    salvar_csv(CAMINHO_TABELA_CSV,campos,linhas_csv)
    with open(CAMINHO_TABELA_TXT,"w",encoding="utf-8") as arquivo:
        arquivo.write("\n".join(linhas_texto) + "\n")
    return linhas_texto


def imprimir_resumo(resultados):
    resumo = resumir(resultados)
    print("Resumo geral da busca local por subcubos da particao")
    print("Execucoes: " + str(resumo["execucoes"]))
    print()
    print("Busca local versus heuristica construtiva")
    print("Melhores: " + str(resumo["melhoresConstrutiva"]))
    print("Iguais: " + str(resumo["iguaisConstrutiva"]))
    print("Piores: " + str(resumo["pioresConstrutiva"]))
    print("Melhoria media: " + format(resumo["melhoriaMediaConstrutiva"],".6f") + "%")
    print()
    print("Busca local versus resultados dos autores")
    print("Abaixo dos autores: " + str(resumo["abaixoAutores"]))
    print("Iguais aos autores: " + str(resumo["iguaisAutores"]))
    print("Acima dos autores: " + str(resumo["acimaAutores"]))
    print("Gap medio: " + format(resumo["gapMedioAutores"],".6f") + "%")
    print()
    print("Comparacao com a troca-k")
    print("Execucoes comparadas: " + str(resumo["comparaveis"]))
    print("Melhores: " + str(resumo["melhoresAnterior"]))
    print("Iguais: " + str(resumo["iguaisAnterior"]))
    print("Piores: " + str(resumo["pioresAnterior"]))
    if(resumo["ganhoMedioAnterior"] is not None):
        print("Ganho medio: " + format(resumo["ganhoMedioAnterior"],".6f") + "%")
    print()
    print("Desempenho computacional")
    print("Tempo medio: " + format(resumo["tempoMedioMs"],".3f") + " ms")
    print("Tempo mediano: " + format(resumo["tempoMedianoMs"],".3f") + " ms")
    print("Tempo maximo: " + format(resumo["tempoMaximoMs"],".3f") + " ms")
    print("Vizinhos avaliados: " + str(resumo["vizinhos"]))
    print("Vizinhos de uma troca: " + str(resumo["vizinhosUmaTroca"]))
    print("Vizinhos de subcubos: " + str(resumo["vizinhosSubcubos"]))
    print("Paineis avaliados: " + str(resumo["paineis"]))
    print("Maior cardinalidade aceita: " + str(resumo["maiorCardinalidade"]))
    print("Movimentos com quatro ou mais trocas aceitos: " + str(resumo["movimentosQuatroOuMais"]))
    casos_n10 = [linha for linha in resultados if int(linha["n"]) == 10]
    if(len(casos_n10) > 0):
        atingidos = sum(int(linha["custoBuscaLocal"]) == int(linha["custoAutores"]) for linha in casos_n10)
        print()
        print("Otimos globais conhecidos para n=10")
        print("Otimos atingidos: " + str(atingidos) + "/" + str(len(casos_n10)))
    print()
    print("Resumo por tamanho")
    for chave,linhas in sorted(agrupar(resultados,["n"]).items(),key=lambda item: int(item[0][0])):
        resumo_grupo = resumir(linhas)
        print("n=" + chave[0] + " | melhoria_construtiva=" + format(resumo_grupo["melhoriaMediaConstrutiva"],".6f") + "% | gap_autores=" + format(resumo_grupo["gapMedioAutores"],".6f") + "% | abaixo/igual/acima=" + str(resumo_grupo["abaixoAutores"]) + "/" + str(resumo_grupo["iguaisAutores"]) + "/" + str(resumo_grupo["acimaAutores"]) + " | tempo_ms=" + format(resumo_grupo["tempoMedioMs"],".3f"))
    print()
    print("Resumo por h")
    for chave,linhas in sorted(agrupar(resultados,["h"]).items(),key=lambda item: float(item[0][0])):
        resumo_grupo = resumir(linhas)
        print("h=" + chave[0] + " | melhoria_construtiva=" + format(resumo_grupo["melhoriaMediaConstrutiva"],".6f") + "% | gap_autores=" + format(resumo_grupo["gapMedioAutores"],".6f") + "% | abaixo/igual/acima=" + str(resumo_grupo["abaixoAutores"]) + "/" + str(resumo_grupo["iguaisAutores"]) + "/" + str(resumo_grupo["acimaAutores"]) + " | tempo_ms=" + format(resumo_grupo["tempoMedioMs"],".3f"))
    print()
    casos_acima = [linha for linha in resultados if int(linha["custoBuscaLocal"]) > int(linha["custoAutores"])]
    print("Casos que permanecem acima dos autores")
    if(len(casos_acima) == 0):
        print("Nenhum caso.")
    else:
        for linha in sorted(casos_acima,key=lambda item: float(item["gapPercentualParaAutores"]),reverse=True):
            print("n=" + linha["n"] + " | instancia=" + linha["idInstancia"] + " | h=" + linha["h"] + " | subcubos=" + linha["custoBuscaLocal"] + " | autores=" + str(linha["custoAutores"]) + " | gap=" + format(float(linha["gapPercentualParaAutores"]),".6f") + "%")


def main():
    permitir_parcial = "--parcial" in sys.argv[1:]
    caminhos,resultados = carregar_resultados()
    if(len(resultados) == 0):
        print("Nenhum resultado da busca local por subcubos da particao foi encontrado.")
        return 1
    if(len(resultados) != QUANTIDADE_TOTAL_ESPERADA and permitir_parcial is False):
        print("Foram encontradas " + str(len(resultados)) + " execucoes. Esperado: " + str(QUANTIDADE_TOTAL_ESPERADA) + ".")
        print("Use --parcial apenas para analisar testes de fumaca.")
        return 1
    referencias = carregar_benchmark()
    resultados_anteriores = carregar_resultados_anteriores()
    resultados = enriquecer_resultados(resultados,referencias,resultados_anteriores)
    imprimir_resumo(resultados)
    salvar_resultados_enriquecidos(resultados)
    linhas_resumo = criar_resumos_n_h(resultados)
    salvar_resumo_n_h(linhas_resumo)
    gerar_tabela_7x4(resultados)
    print()
    print("Arquivos individuais analisados: " + str(len(caminhos)))
    print("Resumo completo: " + CAMINHO_RESUMO)
    print("Resumo por n e h: " + CAMINHO_RESUMO_N_H)
    print("Tabela 7 por 4 em CSV: " + CAMINHO_TABELA_CSV)
    print("Tabela 7 por 4 em texto: " + CAMINHO_TABELA_TXT)
    return 0


if(__name__ == "__main__"):
    try:
        sys.exit(main())
    except (OSError,ValueError,KeyError) as excecao:
        print("Falha na analise: " + str(excecao))
        sys.exit(1)