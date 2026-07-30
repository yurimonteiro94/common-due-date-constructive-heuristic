from pathlib import Path
import csv
import math
from collections import defaultdict

PASTA_RESULTADOS = Path("resultados")
ARQUIVO_MEDIAS = PASTA_RESULTADOS / "medias_por_n_h.csv"
ARQUIVO_COMPARACAO = PASTA_RESULTADOS / "comparacao_benchmark.csv"
ARQUIVO_SAIDA = PASTA_RESULTADOS / "relatorio_graficos.html"


def ler_csv(caminho):
    with open(caminho, "r", encoding="utf-8-sig", newline="") as arquivo:
        return list(csv.DictReader(arquivo))


def numero_br(valor, casas=3):
    texto = f"{valor:,.{casas}f}"
    texto = texto.replace(",", "X").replace(".", ",").replace("X", ".")
    return texto


def percentual_br(valor, casas=3):
    return numero_br(valor, casas) + "%"


def cor_por_indice(indice):
    cores = [
        "#2563eb",
        "#dc2626",
        "#16a34a",
        "#9333ea",
        "#f97316",
        "#0891b2",
        "#4f46e5",
        "#be123c",
    ]
    return cores[indice % len(cores)]


def gerar_svg_linhas(titulo, series, eixo_x, largura=980, altura=430, escala_log=False, unidade_y=""):
    margem_esq = 85
    margem_dir = 30
    margem_topo = 50
    margem_baixo = 70

    area_x = largura - margem_esq - margem_dir
    area_y = altura - margem_topo - margem_baixo

    todos_y = []
    for serie in series:
        for valor in serie["valores"]:
            if valor is not None:
                todos_y.append(valor)

    if not todos_y:
        return ""

    y_min = min(todos_y)
    y_max = max(todos_y)

    if escala_log:
        y_min = max(y_min, 1e-9)
        log_min = math.log10(y_min)
        log_max = math.log10(y_max)
        if abs(log_max - log_min) < 1e-9:
            log_max = log_min + 1

        def y_para_pixel(valor):
            valor = max(valor, 1e-9)
            return margem_topo + area_y - ((math.log10(valor) - log_min) / (log_max - log_min)) * area_y

        marcas_y = []
        inicio = math.floor(log_min)
        fim = math.ceil(log_max)
        for expoente in range(inicio, fim + 1):
            marcas_y.append(10 ** expoente)
    else:
        if abs(y_max - y_min) < 1e-9:
            y_max = y_min + 1
        folga = (y_max - y_min) * 0.08
        y_min = max(0, y_min - folga)
        y_max = y_max + folga

        def y_para_pixel(valor):
            return margem_topo + area_y - ((valor - y_min) / (y_max - y_min)) * area_y

        marcas_y = []
        for i in range(6):
            marcas_y.append(y_min + (y_max - y_min) * i / 5)

    def x_para_pixel(indice):
        if len(eixo_x) == 1:
            return margem_esq + area_x / 2
        return margem_esq + (indice / (len(eixo_x) - 1)) * area_x

    partes = []
    partes.append(f'<svg class="grafico" viewBox="0 0 {largura} {altura}" role="img">')
    partes.append(f'<text x="{largura/2}" y="26" text-anchor="middle" class="titulo-grafico">{titulo}</text>')

    partes.append(f'<line x1="{margem_esq}" y1="{margem_topo}" x2="{margem_esq}" y2="{margem_topo + area_y}" class="eixo"/>')
    partes.append(f'<line x1="{margem_esq}" y1="{margem_topo + area_y}" x2="{margem_esq + area_x}" y2="{margem_topo + area_y}" class="eixo"/>')

    for marca in marcas_y:
        y = y_para_pixel(marca)
        partes.append(f'<line x1="{margem_esq}" y1="{y:.2f}" x2="{margem_esq + area_x}" y2="{y:.2f}" class="grade"/>')
        if escala_log:
            texto = f"{marca:.0f}"
        else:
            texto = numero_br(marca, 1)
        partes.append(f'<text x="{margem_esq - 10}" y="{y + 4:.2f}" text-anchor="end" class="rotulo">{texto}</text>')

    for indice, x in enumerate(eixo_x):
        px = x_para_pixel(indice)
        partes.append(f'<text x="{px:.2f}" y="{margem_topo + area_y + 28}" text-anchor="middle" class="rotulo">{x}</text>')

    for indice_serie, serie in enumerate(series):
        cor = cor_por_indice(indice_serie)
        pontos = []
        for indice, valor in enumerate(serie["valores"]):
            if valor is not None:
                pontos.append((x_para_pixel(indice), y_para_pixel(valor)))

        if len(pontos) >= 2:
            caminho = " ".join([f"{x:.2f},{y:.2f}" for x, y in pontos])
            partes.append(f'<polyline points="{caminho}" fill="none" stroke="{cor}" stroke-width="3"/>')

        for x, y in pontos:
            partes.append(f'<circle cx="{x:.2f}" cy="{y:.2f}" r="4" fill="{cor}"/>')

    legenda_x = margem_esq
    legenda_y = altura - 20
    deslocamento = 0
    for indice_serie, serie in enumerate(series):
        cor = cor_por_indice(indice_serie)
        x0 = legenda_x + deslocamento
        partes.append(f'<rect x="{x0}" y="{legenda_y - 10}" width="14" height="14" fill="{cor}"/>')
        partes.append(f'<text x="{x0 + 20}" y="{legenda_y + 2}" class="rotulo">{serie["nome"]}</text>')
        deslocamento += 120

    if unidade_y:
        partes.append(f'<text x="{margem_esq}" y="{altura - 45}" class="rotulo">Eixo Y: {unidade_y}</text>')

    partes.append("</svg>")
    return "\n".join(partes)


def cor_gap(valor):
    if valor < -5:
        return "#166534"
    if valor < -2:
        return "#22c55e"
    if valor < 0:
        return "#86efac"
    if abs(valor) < 1e-9:
        return "#e5e7eb"
    if valor < 1:
        return "#fde68a"
    if valor < 3:
        return "#fb923c"
    return "#dc2626"


def gerar_heatmap_gap(gaps_por_n_h, ns, hs):
    largura = 980
    altura = 430
    margem_esq = 95
    margem_topo = 60
    celula_largura = 105
    celula_altura = 42

    partes = []
    partes.append(f'<svg class="grafico" viewBox="0 0 {largura} {altura}" role="img">')
    partes.append(f'<text x="{largura/2}" y="28" text-anchor="middle" class="titulo-grafico">Gap médio em relação à referência por n e h</text>')

    for j, h in enumerate(hs):
        x = margem_esq + j * celula_largura
        partes.append(f'<text x="{x + celula_largura/2}" y="{margem_topo - 15}" text-anchor="middle" class="rotulo">h={h}</text>')

    for i, n in enumerate(ns):
        y = margem_topo + i * celula_altura
        partes.append(f'<text x="{margem_esq - 14}" y="{y + 27}" text-anchor="end" class="rotulo">n={n}</text>')

        for j, h in enumerate(hs):
            x = margem_esq + j * celula_largura
            valor = gaps_por_n_h.get((n, h), None)

            if valor is None:
                cor = "#f3f4f6"
                texto = "-"
            else:
                cor = cor_gap(valor)
                texto = percentual_br(valor, 2)

            partes.append(f'<rect x="{x}" y="{y}" width="{celula_largura}" height="{celula_altura}" fill="{cor}" stroke="#ffffff"/>')
            partes.append(f'<text x="{x + celula_largura/2}" y="{y + 26}" text-anchor="middle" class="rotulo">{texto}</text>')

    y_legenda = margem_topo + len(ns) * celula_altura + 45
    legenda = [
        ("<-5%", "#166534"),
        ("-5% a -2%", "#22c55e"),
        ("-2% a 0%", "#86efac"),
        ("0%", "#e5e7eb"),
        ("0% a 1%", "#fde68a"),
        ("1% a 3%", "#fb923c"),
        (">3%", "#dc2626"),
    ]

    x_legenda = margem_esq
    for texto, cor in legenda:
        partes.append(f'<rect x="{x_legenda}" y="{y_legenda}" width="20" height="20" fill="{cor}"/>')
        partes.append(f'<text x="{x_legenda + 26}" y="{y_legenda + 15}" class="rotulo">{texto}</text>')
        x_legenda += 118

    partes.append("</svg>")
    return "\n".join(partes)


def media(lista):
    if not lista:
        return 0.0
    return sum(lista) / len(lista)


def main():
    if not ARQUIVO_MEDIAS.exists():
        raise FileNotFoundError(f"Arquivo nao encontrado: {ARQUIVO_MEDIAS}")

    if not ARQUIVO_COMPARACAO.exists():
        raise FileNotFoundError(f"Arquivo nao encontrado: {ARQUIVO_COMPARACAO}")

    linhas_medias = ler_csv(ARQUIVO_MEDIAS)
    linhas_comparacao = ler_csv(ARQUIVO_COMPARACAO)

    for linha in linhas_medias:
        linha["n"] = int(linha["n"])
        linha["h"] = float(linha["h"])
        linha["custoMedio"] = float(linha["custoMedio"])
        linha["tempoMedioMilissegundos"] = float(linha["tempoMedioMilissegundos"])

    for linha in linhas_comparacao:
        linha["n"] = int(linha["n"])
        linha["h"] = float(linha["h"])
        linha["custoHeuristica"] = float(linha["custoHeuristica"])
        linha["custoMelhorConhecido"] = float(linha["custoMelhorConhecido"])
        linha["gapMelhorSolucaoConhecidaPercentual"] = float(linha["gapMelhorSolucaoConhecidaPercentual"])

    ns = sorted(set(linha["n"] for linha in linhas_medias))
    hs = sorted(set(linha["h"] for linha in linhas_medias))

    medias_por_n_h = {}
    for linha in linhas_medias:
        medias_por_n_h[(linha["n"], linha["h"])] = linha

    series_custo = []
    series_tempo = []

    for h in hs:
        valores_custo = []
        valores_tempo = []

        for n in ns:
            linha = medias_por_n_h.get((n, h))
            if linha is None:
                valores_custo.append(None)
                valores_tempo.append(None)
            else:
                valores_custo.append(linha["custoMedio"])
                valores_tempo.append(linha["tempoMedioMilissegundos"])

        series_custo.append({"nome": f"h={h}", "valores": valores_custo})
        series_tempo.append({"nome": f"h={h}", "valores": valores_tempo})

    gaps = defaultdict(list)
    melhores = defaultdict(int)
    iguais = defaultdict(int)
    piores = defaultdict(int)

    for linha in linhas_comparacao:
        chave = (linha["n"], linha["h"])
        gap = linha["gapMelhorSolucaoConhecidaPercentual"]
        gaps[chave].append(gap)

        if gap < -1e-9:
            melhores[chave] += 1
        elif gap > 1e-9:
            piores[chave] += 1
        else:
            iguais[chave] += 1

    gaps_medios = {}
    for chave, lista in gaps.items():
        gaps_medios[chave] = media(lista)

    svg_custo = gerar_svg_linhas(
        "Custo médio por tamanho da instância",
        series_custo,
        [str(n) for n in ns],
        escala_log=True,
        unidade_y="custo médio em escala logarítmica",
    )

    svg_tempo = gerar_svg_linhas(
        "Tempo médio por tamanho da instância",
        series_tempo,
        [str(n) for n in ns],
        escala_log=False,
        unidade_y="milissegundos",
    )

    svg_gap = gerar_heatmap_gap(gaps_medios, ns, hs)

    total_execucoes = len(linhas_comparacao)
    total_melhores = sum(1 for linha in linhas_comparacao if linha["gapMelhorSolucaoConhecidaPercentual"] < -1e-9)
    total_iguais = sum(1 for linha in linhas_comparacao if abs(linha["gapMelhorSolucaoConhecidaPercentual"]) <= 1e-9)
    total_piores = sum(1 for linha in linhas_comparacao if linha["gapMelhorSolucaoConhecidaPercentual"] > 1e-9)
    gap_medio_global = media([linha["gapMelhorSolucaoConhecidaPercentual"] for linha in linhas_comparacao])

    linhas_tabela = []
    for n in ns:
        for h in hs:
            linha_media = medias_por_n_h[(n, h)]
            chave = (n, h)
            lista_gaps = gaps.get(chave, [])
            gap_medio = media(lista_gaps)
            gap_min = min(lista_gaps) if lista_gaps else 0.0
            gap_max = max(lista_gaps) if lista_gaps else 0.0

            linhas_tabela.append(
                f"""
                <tr>
                    <td>{n}</td>
                    <td>{h}</td>
                    <td>{numero_br(linha_media["custoMedio"], 3)}</td>
                    <td>{numero_br(linha_media["tempoMedioMilissegundos"], 3)}</td>
                    <td>{percentual_br(gap_medio, 3)}</td>
                    <td>{percentual_br(gap_min, 3)}</td>
                    <td>{percentual_br(gap_max, 3)}</td>
                    <td>{melhores[chave]}</td>
                    <td>{iguais[chave]}</td>
                    <td>{piores[chave]}</td>
                </tr>
                """
            )

    html = f"""<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <title>Prazo Nebuloso - Relatório de Gráficos</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            margin: 28px;
            background: #f8fafc;
            color: #111827;
        }}

        h1, h2 {{
            color: #111827;
        }}

        .subtitulo {{
            color: #4b5563;
            margin-top: -8px;
        }}

        .cards {{
            display: grid;
            grid-template-columns: repeat(4, minmax(150px, 1fr));
            gap: 14px;
            margin: 24px 0;
        }}

        .card {{
            background: white;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            padding: 16px;
            box-shadow: 0 1px 4px rgba(0,0,0,0.06);
        }}

        .card .numero {{
            font-size: 26px;
            font-weight: bold;
            margin-top: 8px;
        }}

        .card .rotulo-card {{
            color: #6b7280;
            font-size: 14px;
        }}

        .bloco {{
            background: white;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            padding: 18px;
            margin: 22px 0;
            box-shadow: 0 1px 4px rgba(0,0,0,0.06);
        }}

        .grafico {{
            width: 100%;
            height: auto;
            display: block;
        }}

        .titulo-grafico {{
            font-size: 20px;
            font-weight: bold;
            fill: #111827;
        }}

        .eixo {{
            stroke: #111827;
            stroke-width: 1.5;
        }}

        .grade {{
            stroke: #e5e7eb;
            stroke-width: 1;
        }}

        .rotulo {{
            font-size: 13px;
            fill: #374151;
        }}

        table {{
            width: 100%;
            border-collapse: collapse;
            background: white;
            font-size: 13px;
        }}

        th, td {{
            border: 1px solid #e5e7eb;
            padding: 8px;
            text-align: right;
        }}

        th {{
            background: #f3f4f6;
        }}

        td:first-child, th:first-child {{
            text-align: center;
        }}

        td:nth-child(2), th:nth-child(2) {{
            text-align: center;
        }}

        .observacao {{
            color: #4b5563;
            line-height: 1.5;
        }}
    </style>
</head>
<body>
    <h1>Prazo Nebuloso - Relatório de Gráficos</h1>
    <p class="subtitulo">Relatório gerado automaticamente a partir dos arquivos CSV da pasta resultados.</p>

    <div class="cards">
        <div class="card">
            <div class="rotulo-card">Execuções comparadas</div>
            <div class="numero">{total_execucoes}</div>
        </div>

        <div class="card">
            <div class="rotulo-card">Melhores que a referência</div>
            <div class="numero">{total_melhores}</div>
        </div>

        <div class="card">
            <div class="rotulo-card">Iguais à referência</div>
            <div class="numero">{total_iguais}</div>
        </div>

        <div class="card">
            <div class="rotulo-card">Gap médio global</div>
            <div class="numero">{percentual_br(gap_medio_global, 3)}</div>
        </div>
    </div>

    <div class="bloco">
        {svg_custo}
        <p class="observacao">
            O custo médio usa escala logarítmica porque os valores crescem muito entre n=10 e n=1000.
        </p>
    </div>

    <div class="bloco">
        {svg_tempo}
        <p class="observacao">
            Mesmo nas instâncias com 1000 tarefas, o tempo médio permanece abaixo de 0,1 segundo.
        </p>
    </div>

    <div class="bloco">
        {svg_gap}
        <p class="observacao">
            Valores negativos indicam que a heurística encontrou custo menor do que a referência cadastrada.
            Valores positivos indicam custo acima da referência.
        </p>
    </div>

    <div class="bloco">
        <h2>Tabela consolidada por n e h</h2>
        <table>
            <thead>
                <tr>
                    <th>n</th>
                    <th>h</th>
                    <th>Custo médio</th>
                    <th>Tempo médio (ms)</th>
                    <th>Gap médio</th>
                    <th>Melhor gap</th>
                    <th>Pior gap</th>
                    <th>Melhores</th>
                    <th>Iguais</th>
                    <th>Piores</th>
                </tr>
            </thead>
            <tbody>
                {''.join(linhas_tabela)}
            </tbody>
        </table>
    </div>
</body>
</html>
"""

    PASTA_RESULTADOS.mkdir(exist_ok=True)
    with open(ARQUIVO_SAIDA, "w", encoding="utf-8") as arquivo:
        arquivo.write(html)

    print(f"Relatorio gerado com sucesso: {ARQUIVO_SAIDA}")


if __name__ == "__main__":
    main()