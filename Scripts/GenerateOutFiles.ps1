param(
    [string]$Directory = ".",
    [string]$Extension = ".out_parse"
)

Get-ChildItem -Path $Directory -Filter *.cara -Recurse | ForEach-Object {
    $outFile = [System.IO.Path]::ChangeExtension($_.FullName, $Extension)
    if (-not (Test-Path $outFile)) {
        New-Item -Path $outFile -ItemType File | Out-Null
        Write-Host "Created: $outFile"
    }
}
