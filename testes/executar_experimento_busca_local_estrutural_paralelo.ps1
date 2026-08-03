$raiz = Split-Path -Parent $PSScriptRoot
Set-Location $raiz

$execucoes = @(
    @{ Arquivo = "sch50.txt"; Saida = "resultados\validacao_busca_local_estrutural_50.csv"; Log = "resultados\validacao_busca_local_estrutural_50.log" },
    @{ Arquivo = "sch100.txt"; Saida = "resultados\validacao_busca_local_estrutural_100.csv"; Log = "resultados\validacao_busca_local_estrutural_100.log" },
    @{ Arquivo = "sch200.txt"; Saida = "resultados\validacao_busca_local_estrutural_200.csv"; Log = "resultados\validacao_busca_local_estrutural_200.log" },
    @{ Arquivo = "sch500.txt"; Saida = "resultados\validacao_busca_local_estrutural_500.csv"; Log = "resultados\validacao_busca_local_estrutural_500.log" },
    @{ Arquivo = "sch1000.txt"; Saida = "resultados\validacao_busca_local_estrutural_1000.csv"; Log = "resultados\validacao_busca_local_estrutural_1000.log" }
)

$processos = @()

foreach($execucao in $execucoes) {
    Remove-Item $execucao.Saida -ErrorAction SilentlyContinue
    Remove-Item $execucao.Log -ErrorAction SilentlyContinue
    Remove-Item ($execucao.Log + ".erro") -ErrorAction SilentlyContinue

    $processo = Start-Process -FilePath ".\testes\experimento_busca_local_estrutural.exe" -ArgumentList @($execucao.Arquivo,$execucao.Saida) -WorkingDirectory $raiz -RedirectStandardOutput $execucao.Log -RedirectStandardError ($execucao.Log + ".erro") -PassThru

    $processos += @{
        Processo = $processo
        Execucao = $execucao
    }
}

$houveErro = $false

foreach($item in $processos) {
    $item.Processo.WaitForExit()
    $item.Processo.Refresh()
    $codigoDeSaida = $item.Processo.ExitCode

    if($codigoDeSaida -ne 0) {
        Write-Host "Falha na execucao de $($item.Execucao.Arquivo). Codigo de saida: $codigoDeSaida"
        $houveErro = $true
    }
}

foreach($execucao in $execucoes) {
    Write-Host ""
    Write-Host "============================================================"
    Write-Host $execucao.Arquivo
    Write-Host "============================================================"

    if(Test-Path $execucao.Log) {
        Get-Content $execucao.Log
    }

    if(Test-Path ($execucao.Log + ".erro")) {
        $conteudoDeErro = Get-Content ($execucao.Log + ".erro")

        if($conteudoDeErro) {
            Write-Host ""
            Write-Host "Saida de erro:"
            $conteudoDeErro
        }
    }
}

if($houveErro) {
    exit 1
}

Write-Host ""
Write-Host "Todas as execucoes terminaram com sucesso."

exit 0