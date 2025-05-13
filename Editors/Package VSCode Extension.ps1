# Define root directory (current directory where the script is located)
$rootDir = Get-Location

# Define paths to the Shared folder, CaracalVSCodeExtension folder, and tmLanguage file
$sharedFolder = "$rootDir/Shared"  # Path to the Shared folder
$vsCodeExtensionFolder = "$rootDir/CaracalVSCodeExtension"  # Path to the CaracalVSCodeExtension folder
$tmLanguageFile = "caracal.tmLanguage.json"  # Name of the file to copy
$targetSyntaxesFolder = "$vsCodeExtensionFolder/syntaxes"  # Path where the file should go

# Ensure the syntaxes folder exists
if (-not (Test-Path -Path $targetSyntaxesFolder)) {
    Write-Host "Creating syntaxes folder..."
    New-Item -ItemType Directory -Path $targetSyntaxesFolder
}

# Copy the tmLanguage file to the syntaxes folder
Write-Host "Copying $tmLanguageFile from $sharedFolder to $targetSyntaxesFolder..."
Copy-Item -Path "$sharedFolder/$tmLanguageFile" -Destination $targetSyntaxesFolder -Force

# Check if vsce is installed and run vsce package
$vscePath = (Get-Command vsce -ErrorAction SilentlyContinue).Source
if (-not $vscePath) {
    Write-Host "vsce is not installed. Installing vsce..."
    npm install -g vsce
}

# Ensure we are in the correct directory (VSCodeExtension folder) before running vsce package
Write-Host "Changing to VSCodeExtension folder and running vsce package..."
Set-Location -Path $vsCodeExtensionFolder
vsce package

# After packaging, delete the previously copied tmLanguage file from the syntaxes folder
Write-Host "Deleting previously copied tmLanguage file from syntaxes folder..."
Remove-Item -Path "$targetSyntaxesFolder/$tmLanguageFile" -Force

# Move the packaged .vsix file to the root folder
$vsixFile = Get-ChildItem -Path $vsCodeExtensionFolder -Filter *.vsix | Sort-Object LastWriteTime -Descending | Select-Object -First 1
if ($vsixFile) {
    # Rename the .vsix file to include the current date (CaracalVSCodeExtension-YYYY-MM-dd)
    $dateString = (Get-Date).ToString("yyyy-MM-dd")
    $newFileName = "CaracalVSCodeExtension-$dateString.vsix"
    $newVsixPath = "$rootDir/$newFileName"

    # Check if the file exists at the destination and overwrite it
    if (Test-Path -Path $newVsixPath) {
        Write-Host "File already exists at $newVsixPath. Overwriting..."
        Remove-Item -Path $newVsixPath -Force  # Remove the existing file before moving
    }

    Write-Host "Moving the packaged extension to the root folder: $newVsixPath..."
    Move-Item -Path $vsixFile.FullName -Destination $newVsixPath
} else {
    Write-Host "No .vsix file found after packaging!"
}
