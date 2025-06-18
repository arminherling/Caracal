$scriptPath = Join-Path $PSScriptRoot "GenerateOutFiles.ps1"
$testsDataPath = Join-Path $PSScriptRoot "..\Tests\Data"

& $scriptPath -Directory $testsDataPath -Extension ".out_parse"
& $scriptPath -Directory $testsDataPath -Extension ".out_cpp"
