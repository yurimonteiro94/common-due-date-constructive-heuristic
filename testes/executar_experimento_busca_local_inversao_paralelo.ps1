$ErrorActionPreference = "Stop"

$diretorioProjeto = Split-Path -Parent $PSScriptRoot
Set-Location $diretorioProjeto

$quantidadeMaximaDeTrabalhadores = 4

$tarefas = @(
    [PSCustomObject]@{Arquivo="sch1000.txt"; H="4"; Instancia="1"; Sufixo="1000_04"; Peso=100},
    [PSCustomObject]@{Arquivo="sch1000.txt"; H="2"; Instancia="1"; Sufixo="1000_02"; Peso=95},
    [PSCustomObject]@{Arquivo="sch500.txt"; H="4"; Instancia="1"; Sufixo="500_04"; Peso=65},
    [PSCustomObject]@{Arquivo="sch500.txt"; H="2"; Instancia="1"; Sufixo="500_02"; Peso=60},
    [PSCustomObject]@{Arquivo="sch200.txt"; H="4"; Instancia="1"; Sufixo="200_04"; Peso=35},
    [PSCustomObject]@{Arquivo="sch200.txt"; H="2"; Instancia="1"; Sufixo="200_02"; Peso=32},
    [PSCustomObject]@{Arquivo="sch100.txt"; H="4"; Instancia="1"; Sufixo="100_04"; Peso=20},
    [PSCustomObject]@{Arquivo="sch100.txt"; H="2"; Instancia="1"; Sufixo="100_02"; Peso=18},
    [PSCustomObject]@{Arquivo="sch50.txt"; H="4"; Instancia="1"; Sufixo="50_04"; Peso=10},
    [PSCustomObject]@{Arquivo="sch50.txt"; H="2"; Instancia="1"; Sufixo="50_02"; Peso=9}
)

$fila = [System.Collections.Generic.Queue[object]]::new()

foreach($tarefa in ($tarefas | Sort-Object Peso -Descending)) {
    $fila.Enqueue($tarefa)
}

$trabalhosAtivos = [System.Collections.Generic.List[object]]::new()
$houveFalha = $false
$quantidadeConcluida = 0
$quantidadeTotal = $tarefas.Count

function Iniciar-Tarefa {
    param(
        [Parameter(Mandatory=$true)]
        [object]$Tarefa
    )

    $caminhoLog = Join-Path $diretorioProjeto ("resultados\validacao_busca_local_inversao_" + $Tarefa.Sufixo + ".log")
    $caminhoErro = Join-Path $diretorioProjeto ("resultados\validacao_busca_local_inversao_" + $Tarefa.Sufixo + ".log.erro")

    Remove-Item $caminhoLog -Force -ErrorAction SilentlyContinue
    Remove-Item $caminhoErro -Force -ErrorAction SilentlyContinue

    $trabalho = Start-Job -ScriptBlock {
        param(
            [string]$DiretorioProjeto,
            [string]$Arquivo,
            [string]$H,
            [string]$Instancia,
            [string]$Sufixo,
            [string]$CaminhoLog,
            [string]$CaminhoErro
        )

        Set-Location $DiretorioProjeto

        & ".\testes\experimento_busca_local_inversao.exe" `
            $Arquivo `
            $H `
            $Instancia `
            $Sufixo `
            1> $CaminhoLog `
            2> $CaminhoErro

        return $LASTEXITCODE
    } -ArgumentList `
        $diretorioProjeto,`
        $Tarefa.Arquivo,`
        $Tarefa.H,`
        $Tarefa.Instancia,`
        $Tarefa.Sufixo,`
        $caminhoLog,`
        $caminhoErro

    Write-Host ("Iniciada: arquivo=" + $Tarefa.Arquivo + " h=0." + $Tarefa.H + " Job=" + $trabalho.Id)

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

    Start-Sleep -Milliseconds 250

    for($indice = $trabalhosAtivos.Count - 1;$indice -ge 0;$indice--) {
        $item = $trabalhosAtivos[$indice]
        $estado = $item.Trabalho.State

        if($estado -eq "Completed" -or $estado -eq "Failed" -or $estado -eq "Stopped") {
            $quantidadeConcluida++
            $codigoDeSaida = 1

            if($estado -eq "Completed") {
                $resultadoDoTrabalho = @(Receive-Job -Job $item.Trabalho)

                if($resultadoDoTrabalho.Count -gt 0) {
                    $codigoDeSaida = [int]$resultadoDoTrabalho[$resultadoDoTrabalho.Count - 1]
                }
            }
            else {
                Receive-Job -Job $item.Trabalho -ErrorAction SilentlyContinue | Out-Null
            }

            if($codigoDeSaida -ne 0) {
                $houveFalha = $true

                Write-Host (
                    "Falha: arquivo=" +
                    $item.Tarefa.Arquivo +
                    " h=0." +
                    $item.Tarefa.H +
                    " codigo=" +
                    $codigoDeSaida
                )

                if(Test-Path $item.Log) {
                    Get-Content $item.Log
                }

                if(Test-Path $item.Erro) {
                    Get-Content $item.Erro
                }
            }
            else {
                Write-Host (
                    "Concluida " +
                    $quantidadeConcluida +
                    "/" +
                    $quantidadeTotal +
                    ": arquivo=" +
                    $item.Tarefa.Arquivo +
                    " h=0." +
                    $item.Tarefa.H
                )

                if(Test-Path $item.Log) {
                    Get-Content $item.Log
                }
            }

            Remove-Job -Job $item.Trabalho -Force
            $trabalhosAtivos.RemoveAt($indice)
        }
    }
}

if($houveFalha) {
    Write-Host ""
    Write-Host "Uma ou mais execucoes da busca por inversao falharam."

    exit 1
}

Write-Host ""
Write-Host "Todas as execucoes da busca por inversao terminaram com sucesso."

exit 0