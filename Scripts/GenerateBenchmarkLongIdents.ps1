# Generates Tests/TestData/Input/oneMilLinesLongIdents.cara.
# Same chunk structure as GenerateBenchmark.ps1, but with descriptive multi-word
# identifiers so the file matches a long-identifier coding style.
param(
    [int]$TargetLines = 1000000
)

$repoRoot = Split-Path -Parent $PSScriptRoot
$outputPath = Join-Path $repoRoot "Tests\TestData\Input\oneMilLinesLongIdents.cara"

$preamble = @'
// generated benchmark input, one uniquely named chunk per index, long identifier style
#extern(symbol = "clock")
def benchmarkClockProbe() i32 {}

def main() i32
{
    return 42;
}

'@ -replace "`r`n", "`n"

$chunkTemplate = @'

/* chunk {i}
   block comment form */
enum TerminalColorPalette{i}
{
    Red :: {red}
    Green :: {green}
    Blue :: {blue}
}

type CoordinatePoint{i}(horizontalCoordinate: i32, verticalCoordinate: i32)
{
    _horizontalCoordinate : i32 : horizontalCoordinate
    _verticalCoordinate : i32 : verticalCoordinate

    def computeCoordinateSum() i32
    {
        return ._horizontalCoordinate %+ ._verticalCoordinate;
    }

    def CoordinatePoint{i}.computeScaledMagnitude(inputMagnitudeValue: i32, scalingFactorAmount: i32) i32
    {
        return inputMagnitudeValue %* scalingFactorAmount;
    }
}

def computeIterativeTotal{i}(iterationLimitCount: i32, earlyBreakRequested: bool) i32
{
    accumulatedTotalValue := 0;
    currentIterationIndex := 0;
    while currentIterationIndex < iterationLimitCount
    {
        accumulatedTotalValue = accumulatedTotalValue %+ currentIterationIndex %- 1;
        currentIterationIndex = currentIterationIndex %+ 1;
        if accumulatedTotalValue > 100 and earlyBreakRequested
        {
            break;
        }
        if accumulatedTotalValue == 7 or accumulatedTotalValue >= 90
        {
            skip;
        }
    }
    return accumulatedTotalValue;
}

def evaluateGeneratedChunk{i}() i32
{
    coordinatePointInstance := CoordinatePoint{i}.new({x}, {y});
    collectedPayloadBytes : [u8; _] = [{b1}'u8, {b2}'u8, ...];
    payloadByteView := collectedPayloadBytes.slice();
    _ := payloadByteView.length;
    formattedChunkMessage := "chunk {i} says \"hi\"\n";
    combinedMessageText := formattedChunkMessage.concat("end");
    computedAspectRatio := {f}'f32;
    halvedAspectRatio := computedAspectRatio / 2.0'f32;
    referencedCounterValue := 0;
    counterReference := ref referencedCounterValue;
    counterReference = {c};
    negatedCounterValue := -{c};
    primaryConditionFlag := true;
    secondaryConditionFlag := false;
    if !secondaryConditionFlag and halvedAspectRatio >= 0.5'f32 and combinedMessageText.length() != 3 and referencedCounterValue <= 100
    {
        primaryConditionFlag = false;
    }
    else
    {
        primaryConditionFlag = true;
    }
    aggregatedResultValue := computeIterativeTotal{i}(coordinatePointInstance.computeCoordinateSum(), primaryConditionFlag);
    aggregatedResultValue = aggregatedResultValue %+ CoordinatePoint{i}.computeScaledMagnitude(negatedCounterValue, 2);
    return aggregatedResultValue %- 2;
}

'@ -replace "`r`n", "`n"

$builder = [System.Text.StringBuilder]::new()
[void]$builder.Append($preamble)
$lineCount = ($preamble -split "`n").Length - 1
$index = 0

while ($lineCount -lt $TargetLines) {
    $fraction = ($index % 10) + 0.5
    $chunk = $chunkTemplate `
        -replace '\{i\}', $index `
        -replace '\{red\}', ($index % 7) `
        -replace '\{green\}', ($index % 7 + 10) `
        -replace '\{blue\}', ($index % 7 + 20) `
        -replace '\{x\}', ($index % 50) `
        -replace '\{y\}', ($index % 31 + 1) `
        -replace '\{b1\}', ($index % 200) `
        -replace '\{b2\}', (($index * 3) % 200) `
        -replace '\{f\}', $fraction.ToString("0.0", [System.Globalization.CultureInfo]::InvariantCulture) `
        -replace '\{c\}', ($index % 90 + 1)
    [void]$builder.Append($chunk)
    $lineCount += ($chunk -split "`n").Length - 1
    $index++
}

$content = $builder.ToString()
$utf8NoBom = [System.Text.UTF8Encoding]::new($false)
[System.IO.File]::WriteAllText($outputPath, $content, $utf8NoBom)
Write-Host "wrote $outputPath - $lineCount lines, $($content.Length) bytes, $index chunks"
