$ErrorActionPreference = "Stop"

$diretorioProjeto = Split-Path -Parent $PSScriptRoot
Set-Location $diretorioProjeto

$quantidadeMaximaDeTrabalhadores = 8

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

function Obter-Peso {
    param(
        [int]$Quantidade,
        [string]$FatorH
    )

    if($Quantidade -eq 1000) {
        if($FatorH -eq "2") {
            return 160
        }

        if($FatorH -eq "4") {
            return 150
        }

        if($FatorH -eq "8") {
            return 100
        }

        return 90
    }

    if($Quantidade -eq 500) {
        if($FatorH -eq "2") {
            return 90
        }

        if($FatorH -eq "4") {
            return 80
        }

        if($FatorH -eq "8") {
            return 50
        }

        return 45
    }

    if($Quantidade -eq 200) {
        if($FatorH -eq "2") {
            return 35
        }

        if($FatorH -eq "4") {
            return 30
        }

        if($FatorH -eq "8") {
            return 20
        }

        return 18
    }

    if($Quantidade -eq 100) {
        if($FatorH -eq "2" -or $FatorH -eq "4") {
            return 12
        }

        return 8
    }

    if($Quantidade -eq 50) {
        return 4
    }

    if($Quantidade -eq 20) {
        return 2
    }

    return 1
}

$tarefas = [System.Collections.Generic.List[object]]::new()

foreach($arquivo in $arquivos) {
    foreach($fatorH in $fatoresH) {
        for($identificadorDaInstancia = 1;$identificadorDaInstancia -le 10;$identificadorDaInstancia++) {
            $sufixo = $arquivo.Quantidade.ToString() +
                "_" +
                $identificadorDaInstancia.ToString().PadLeft(2,"0") +
                "_" +
                $fatorH.Sufixo

            $peso = Obter-Peso -Quantidade $arquivo.Quantidade -FatorH $fatorH.Argumento
            $peso = $peso + ((11 - $identificadorDaInstancia) / 100.0)

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
        "resultados\validacao_busca_local_particao_estrutural_" +
        $Tarefa.Sufixo +
        ".log"
    )

    $caminhoErro = Join-Path $diretorioProjeto (
        "resultados\validacao_busca_local_particao_estrutural_" +
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

        & ".\testes\experimento_busca_local_particao_estrutural.exe" `
            $arquivo `
            $fatorH `
            $instancia `
            $sufixo `
            1> $caminhoLog `
            2> $caminhoErro

        [PSCustomObject]@{
            CodigoDeSaida=$LASTEXITCODE
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

    Start-Sleep -Milliseconds 400

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
    Write-Host "Uma ou mais execucoes da busca local por particao estrutural falharam."

    exit 1
}

$quantidadeDeResultados = @(
    Get-ChildItem -Path (Join-Path $diretorioProjeto "resultados") `
        -Filter "validacao_busca_local_particao_estrutural_*.csv" `
        -File
).Count

if($quantidadeDeResultados -ne 280) {
    Write-Host ""
    Write-Host ("Quantidade inesperada de CSVs: " + $quantidadeDeResultados + ". Esperado: 280.")

    exit 1
}

Write-Host ""
Write-Host "Todas as execucoes da busca local por particao estrutural terminaram com sucesso."

exit 0
