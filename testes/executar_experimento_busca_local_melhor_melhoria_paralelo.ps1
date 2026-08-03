$raiz = Split-Path -Parent $PSScriptRoot
Set-Location $raiz

$quantidadeMaximaDeTrabalhadores = 4

$tarefas = @(
    @{ Arquivo = "sch1000.txt"; H = "4"; Nome = "1000_04" },
    @{ Arquivo = "sch1000.txt"; H = "2"; Nome = "1000_02" },
    @{ Arquivo = "sch500.txt"; H = "4"; Nome = "500_04" },
    @{ Arquivo = "sch500.txt"; H = "2"; Nome = "500_02" },
    @{ Arquivo = "sch200.txt"; H = "4"; Nome = "200_04" },
    @{ Arquivo = "sch200.txt"; H = "2"; Nome = "200_02" },
    @{ Arquivo = "sch100.txt"; H = "4"; Nome = "100_04" },
    @{ Arquivo = "sch100.txt"; H = "2"; Nome = "100_02" },
    @{ Arquivo = "sch50.txt"; H = "4"; Nome = "50_04" },
    @{ Arquivo = "sch50.txt"; H = "2"; Nome = "50_02" }
)

$fila = [System.Collections.Queue]::new()

foreach($tarefa in $tarefas) {
    $fila.Enqueue($tarefa)
}

$processosAtivos = @()
$quantidadeConcluida = 0
$houveErro = $false

function Iniciar-Tarefa {
    param($tarefa)

    $saida = "resultados\validacao_busca_local_melhor_melhoria_$($tarefa.Nome).csv"
    $log = "resultados\validacao_busca_local_melhor_melhoria_$($tarefa.Nome).log"
    $erro = $log + ".erro"

    Remove-Item $saida -ErrorAction SilentlyContinue
    Remove-Item $log -ErrorAction SilentlyContinue
    Remove-Item $erro -ErrorAction SilentlyContinue

    $processo = Start-Process -FilePath ".\testes\experimento_busca_local_melhor_melhoria.exe" -ArgumentList @($tarefa.Arquivo,$tarefa.H,$saida) -WorkingDirectory $raiz -RedirectStandardOutput $log -RedirectStandardError $erro -WindowStyle Hidden -PassThru

    Write-Host "Iniciada: arquivo=$($tarefa.Arquivo) h=0.$($tarefa.H) PID=$($processo.Id)"

    return @{
        Processo = $processo
        Tarefa = $tarefa
        Saida = $saida
        Log = $log
        Erro = $erro
    }
}

while($fila.Count -gt 0 -or $processosAtivos.Count -gt 0) {
    while($fila.Count -gt 0 -and $processosAtivos.Count -lt $quantidadeMaximaDeTrabalhadores) {
        $proximaTarefa = $fila.Dequeue()
        $processosAtivos += Iniciar-Tarefa $proximaTarefa
    }

    Start-Sleep -Milliseconds 500

    $processosRestantes = @()

    foreach($item in $processosAtivos) {
        if($item.Processo.HasExited) {
            $item.Processo.WaitForExit()
            $item.Processo.Refresh()
            $codigoDeSaida = $item.Processo.ExitCode
            $quantidadeConcluida++

            if($codigoDeSaida -ne 0) {
                Write-Host "Falha: arquivo=$($item.Tarefa.Arquivo) h=0.$($item.Tarefa.H) codigo=$codigoDeSaida"
                $houveErro = $true
            }
            else {
                Write-Host "Concluida: arquivo=$($item.Tarefa.Arquivo) h=0.$($item.Tarefa.H) progresso=$quantidadeConcluida/$($tarefas.Count)"
            }

            if(Test-Path $item.Log) {
                Get-Content $item.Log
            }

            if(Test-Path $item.Erro) {
                $conteudoDeErro = Get-Content $item.Erro

                if($conteudoDeErro) {
                    Write-Host "Saida de erro:"
                    $conteudoDeErro
                    $houveErro = $true
                }
            }

            if(Test-Path $item.Saida) {
                $quantidadeDeLinhas = (Get-Content $item.Saida | Measure-Object -Line).Lines

                if($quantidadeDeLinhas -ne 2) {
                    Write-Host "Arquivo incompleto: $($item.Saida)"
                    $houveErro = $true
                }
            }
            else {
                Write-Host "Arquivo nao gerado: $($item.Saida)"
                $houveErro = $true
            }
        }
        else {
            $processosRestantes += $item
        }
    }

    $processosAtivos = $processosRestantes
}

if($houveErro) {
    exit 1
}

Write-Host ""
Write-Host "Todas as tarefas da melhor melhoria terminaram com sucesso."

exit 0