# Generates Tests/TestData/Input/oneMilLines.cara.
param(
    [int]$TargetLines = 1000000
)

$repoRoot = Split-Path -Parent $PSScriptRoot
$outputPath = Join-Path $repoRoot "Tests\TestData\Input\oneMilLines.cara"

$preamble = @'
// generated benchmark input, one uniquely named chunk per index
#extern(symbol = "clock")
def benchClock() i32 {}

def main() i32
{
    return 42;
}

'@ -replace "`r`n", "`n"

$chunkTemplate = @'

/* chunk {i}
   block comment form */
enum Color{i}
{
    Red :: {red}
    Green :: {green}
    Blue :: {blue}
}

type Point{i}(x: i32, y: i32)
{
    _x : i32 : x
    _y : i32 : y

    def sum() i32
    {
        return ._x %+ ._y;
    }

    def Point{i}.scaled(value: i32, factor: i32) i32
    {
        return value %* factor;
    }
}

def helper{i}(count: i32, flag: bool) i32
{
    total := 0;
    index := 0;
    while index < count
    {
        total = total %+ index %- 1;
        index = index %+ 1;
        if total > 100 and flag
        {
            break;
        }
        if total == 7 or total >= 90
        {
            skip;
        }
    }
    return total;
}

def compute{i}() i32
{
    p := Point{i}.new({x}, {y});
    bytes : [u8; _] = [{b1}'u8, {b2}'u8, ...];
    view := bytes.slice();
    _ := view.length;
    message := "chunk {i} says \"hi\"\n";
    combined := message.concat("end");
    ratio := {f}'f32;
    half := ratio / 2.0'f32;
    counter := 0;
    r := ref counter;
    r = {c};
    negative := -{c};
    flag := true;
    other := false;
    if !other and half >= 0.5'f32 and combined.length() != 3 and counter <= 100
    {
        flag = false;
    }
    else
    {
        flag = true;
    }
    value := helper{i}(p.sum(), flag);
    value = value %+ Point{i}.scaled(negative, 2);
    return value %- 2;
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
