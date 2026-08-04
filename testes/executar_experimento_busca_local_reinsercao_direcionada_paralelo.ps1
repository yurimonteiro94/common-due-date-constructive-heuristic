$ErrorActionPreference = "Stop"

$diretorioProjeto = Split-Path -Parent $PSScriptRoot
Set-Location $diretorioProjeto

$quantidadeMaximaDeTrabalhadores = 6

if($null -ne $env:PRO5826_TRABALHADORES -and $env:PRO5826_TRABALHADORES -ne "") {
    $quantidadeInformada = 0

    if([int]::TryParse($env:PRO5826_TRABALHADORES,[ref]$quantidadeInformada) -and $quantidadeInformada -ge 1 -and $quantidadeInformada -le 12) {
        $quantidadeMaximaDeTrabalhadores = $quantidadeInformada
    }
}

$arquivos = @(
    [PSCustomObject]@{Arquivo="sch1000.txt"; Quantidade=1000},
    [PSCustomObject]@{Arquivo="sch500.txt"; Quantidade=500},
    [PSCustomObject]@{Arquivo="sch200.txt"; Quantidade=200},
    [PSCustomObject]@{Arquivo="sch100.txt"; Quantidade=100},
    [PSCustomObject]@{Arquivo="sch50.txt"; Quantidade=50},
    [PSCustomObject]@{Arquivo="sch20.txt"; Quantidade=20},
    [PSCustomObject]@{Arquivo="sch10.txt"; Quantidade=10}
)

$fatoresH = @(
    [PSCustomObject]@{Argumento="2"; Texto="0.2"; Sufixo="02"},
    [PSCustomObject]@{Argumento="4"; Texto="0.4"; Sufixo="04"},
    [PSCustomObject]@{Argumento="6"; Texto="0.6"; Sufixo="06"},
    [PSCustomObject]@{Argumento="8"; Texto="0.8"; Sufixo="08"}
)

$tarefas = [System.Collections.Generic.List[object]]::new()

foreach($arquivo in $arquivos) {
    foreach($fatorH in $fatoresH) {
        for($identificadorDaInstancia = 1;$identificadorDaInstancia -le 10;$identificadorDaInstancia++) {
            $sufixo = $arquivo.Quantidade.ToString() +
                "_" +
                $identificadorDaInstancia.ToString().PadLeft(2,"0") +
                "_" +
                $fatorH.Sufixo

            $peso = 1

            if($arquivo.Quantidade -eq 1000) {
                if($fatorH.Argumento -eq "2") {
                    $peso = 140
                }
                elseif($fatorH.Argumento -eq "4") {
                    $peso = 130
                }
                else {
                    $peso = 65
                }
            }
            elseif($arquivo.Quantidade -eq 500) {
                if($fatorH.Argumento -eq "2") {
                    $peso = 55
                }
                elseif($fatorH.Argumento -eq "4") {
                    $peso = 48
                }
                else {
                    $peso = 24
                }
            }
            elseif($arquivo.Quantidade -eq 200) {
                if($fatorH.Argumento -eq "2" -or $fatorH.Argumento -eq "4") {
                    $peso = 12
                }
                else {
                    $peso = 6
                }
            }
            elseif($arquivo.Quantidade -eq 100) {
                if($fatorH.Argumento -eq "2" -or $fatorH.Argumento -eq "4") {
                    $peso = 4
                }
                else {
                    $peso = 2
                }
            }
            elseif($arquivo.Quantidade -eq 50) {
                $peso = 2
            }

            $tarefas.Add(
                [PSCustomObject]@{
                    Arquivo=$arquivo.Arquivo
                    Quantidade=$arquivo.Quantidade
                    H=$fatorH.Argumento
                    TextoH=$fatorH.Texto
                    Instancia=$identificadorDaInstancia
                    Sufixo=$sufixo
                    Peso=$peso
                }
            )
        }
    }
}

$fila = [System.Collections.Generic.Queue[object]]::new()

foreach($tarefa in ($tarefas | Sort-Object Peso -Descending)) {
    $fila.Enqueue($tarefa)
}

$trabalhosAtivos = [System.Collections.Generic.List[object]]::new()
$houveFalha = $false
$quantidadeConcluida = 0
$quantidadeComSucesso = 0
$quantidadeComFalha = 0
$quantidadeTotal = $tarefas.Count
$tempoInicial = Get-Date
$instanteUltimoProgresso = Get-Date

function Iniciar-Tarefa {
    param(
        [Parameter(Mandatory=$true)]
        [object]$Tarefa
    )

    $caminhoLog = Join-Path $diretorioProjeto (
        "resultados\validacao_busca_local_reinsercao_direcionada_" +
        $Tarefa.Sufixo +
        ".log"
    )

    $caminhoErro = Join-Path $diretorioProjeto (
        "resultados\validacao_busca_local_reinsercao_direcionada_" +
        $Tarefa.Sufixo +
        ".log.erro"
    )

    Remove-Item $caminhoLog -Force -ErrorAction SilentlyContinue
    Remove-Item $caminhoErro -Force -ErrorAction SilentlyContinue

    $trabalho = Start-Job -ScriptBlock {
        param(
            $diretorioProjetoDoTrabalho,
            $arquivo,
            $fatorH,
            $instancia,
            $sufixo,
            $caminhoLog,
            $caminhoErro
        )

        Set-Location $diretorioProjetoDoTrabalho

        & ".\testes\experimento_busca_local_reinsercao_direcionada.exe" `
            $arquivo `
            $fatorH `
            $instancia `
            $sufixo `
            1> $caminhoLog `
            2> $caminhoErro

        $codigoDeSaida = $LASTEXITCODE

        [PSCustomObject]@{
            CodigoDeSaida=$codigoDeSaida
        }
    } -ArgumentList `
        $diretorioProjeto,`
        $Tarefa.Arquivo,`
        $Tarefa.H,`
        $Tarefa.Instancia,`
        $Tarefa.Sufixo,`
        $caminhoLog,`
        $caminhoErro

    Write-Host (
        "Iniciada: n=" +
        $Tarefa.Quantidade +
        " instancia=" +
        $Tarefa.Instancia +
        " h=" +
        $Tarefa.TextoH +
        " Job=" +
        $trabalho.Id
    )

    return [PSCustomObject]@{
        Tarefa=$Tarefa
        Trabalho=$trabalho
        Log=$caminhoLog
        Erro=$caminhoErro
    }
}

while($fila.Count -gt 0 -or $trabalhosAtivos.Count -gt 0) {
    while($fila.Count -gt 0 -and $trabalhosAtivos.Count -lt $quantidadeMaximaDeTrabalhadores) {
        $tarefa = $fila.Dequeue()
        $item = Iniciar-Tarefa -Tarefa $tarefa
        $trabalhosAtivos.Add($item)
    }

    Start-Sleep -Milliseconds 500

    for($indice = $trabalhosAtivos.Count - 1;$indice -ge 0;$indice--) {
        $item = $trabalhosAtivos[$indice]
        $estado = $item.Trabalho.State

        if($estado -eq "Completed" -or $estado -eq "Failed" -or $estado -eq "Stopped") {
            $retornoDoTrabalho = @(
                Receive-Job $item.Trabalho -ErrorAction SilentlyContinue
            )

            $codigoDeSaida = 1

            if($estado -eq "Completed" -and $retornoDoTrabalho.Count -gt 0) {
                $ultimoRetorno = $retornoDoTrabalho[$retornoDoTrabalho.Count - 1]

                if($null -ne $ultimoRetorno.CodigoDeSaida) {
                    $codigoDeSaida = [int] $ultimoRetorno.CodigoDeSaida
                }
            }

            $quantidadeConcluida++

            if($codigoDeSaida -ne 0) {
                $houveFalha = $true
                $quantidadeComFalha++

                Write-Host (
                    "Falha " +
                    $quantidadeConcluida +
                    "/" +
                    $quantidadeTotal +
                    ": n=" +
                    $item.Tarefa.Quantidade +
                    " instancia=" +
                    $item.Tarefa.Instancia +
                    " h=" +
                    $item.Tarefa.TextoH +
                    " codigo=" +
                    $codigoDeSaida
                )

                if(Test-Path $item.Log) {
                    Get-Content $item.Log
                }

                if(Test-Path $item.Erro) {
                    $arquivoDeErro = Get-Item $item.Erro

                    if($arquivoDeErro.Length -gt 0) {
                        Get-Content $item.Erro
                    }
                }
            }
            else {
                $quantidadeComSucesso++

                Write-Host (
                    "Concluida " +
                    $quantidadeConcluida +
                    "/" +
                    $quantidadeTotal +
                    ": n=" +
                    $item.Tarefa.Quantidade +
                    " instancia=" +
                    $item.Tarefa.Instancia +
                    " h=" +
                    $item.Tarefa.TextoH
                )

                if(Test-Path $item.Log) {
                    Get-Content $item.Log
                }
            }

            Remove-Job $item.Trabalho -Force
            $trabalhosAtivos.RemoveAt($indice)
        }
    }

    $instanteAtual = Get-Date

    if(($instanteAtual - $instanteUltimoProgresso).TotalSeconds -ge 10) {
        $duracao = $instanteAtual - $tempoInicial

        Write-Host (
            "Progresso: " +
            $quantidadeConcluida +
            "/" +
            $quantidadeTotal +
            " | sucesso=" +
            $quantidadeComSucesso +
            " | falhas=" +
            $quantidadeComFalha +
            " | ativos=" +
            $trabalhosAtivos.Count +
            " | fila=" +
            $fila.Count +
            " | tempo=" +
            $duracao.ToString("hh\:mm\:ss")
        )

        $instanteUltimoProgresso = $instanteAtual
    }
}

$tempoFinal = Get-Date
$duracaoTotal = $tempoFinal - $tempoInicial

Write-Host ""
Write-Host "============================================================"
Write-Host "RESUMO DA EXECUCAO GERAL"
Write-Host "============================================================"
Write-Host ("Trabalhadores: " + $quantidadeMaximaDeTrabalhadores)
Write-Host ("Total: " + $quantidadeTotal)
Write-Host ("Sucesso: " + $quantidadeComSucesso)
Write-Host ("Falhas: " + $quantidadeComFalha)
Write-Host ("Duracao: " + $duracaoTotal.ToString("hh\:mm\:ss"))

if($houveFalha) {
    Write-Host ""
    Write-Host "Uma ou mais execucoes da busca local por reinsercao direcionada falharam."

    exit 1
}

$quantidadeDeResultados = @(
    Get-ChildItem -Path (Join-Path $diretorioProjeto "resultados") `
        -Filter "validacao_busca_local_reinsercao_direcionada_*.csv" `
        -File
).Count

if($quantidadeDeResultados -ne 280) {
    Write-Host ""
    Write-Host ("Quantidade inesperada de CSVs: " + $quantidadeDeResultados + ". Esperado: 280.")

    exit 1
}

Write-Host ""
Write-Host "Todas as execucoes da busca local por reinsercao direcionada terminaram com sucesso."

exit 0