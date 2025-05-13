# Define root directory (current directory where the script is located)
$rootDir = Get-Location

# Define paths to the Shared folder, CaracalVSExtension folder, and tmLanguage file
$sharedFolder = "$rootDir/Shared"  # Path to the Shared folder
$vs2022ExtensionFolder = "$rootDir/CaracalVSExtension"  # Path to the CaracalVSExtension folder
$tmLanguageFile = "caracal.tmLanguage.json"  # Name of the file to copy
$targetSyntaxesFolder = "$vs2022ExtensionFolder/Syntaxes"  # Path where the file should go

# Ensure the syntaxes folder exists
if (-not (Test-Path -Path $targetSyntaxesFolder)) {
    Write-Host "Creating Syntaxes folder..."
    New-Item -ItemType Directory -Path $targetSyntaxesFolder
}

# Copy the tmLanguage file to the syntaxes folder
Write-Host "Copying $tmLanguageFile from $sharedFolder to $targetSyntaxesFolder..."
Copy-Item -Path "$sharedFolder/$tmLanguageFile" -Destination $targetSyntaxesFolder -Force

# Ensure MSBuild is available in the system (via Visual Studio build tools)
$msbuildPath = (Get-Command msbuild -ErrorAction SilentlyContinue).Source
if (-not $msbuildPath) {
    Write-Host "MSBuild is not installed. Please ensure Visual Studio Build Tools are installed."
    exit 1
}

# Set the path to the CaracalVSExtension project file (usually has a .sln or .vsixproj extension)
$vsixProjectFile = "$vs2022ExtensionFolder/CaracalVSExtension.sln"

# Check if the CaracalVSExtension project file exists
if (-not (Test-Path -Path $vsixProjectFile)) {
    Write-Host "CaracalVSExtension project file not found at $vsixProjectFile"
    exit 1
}

# Build the extension using MSBuild
Write-Host "Building the VS2022 extension using MSBuild..."
& "$msbuildPath" "$vsixProjectFile" /p:Configuration=Release /p:Platform="Any CPU"

# After building, ensure the VSIX file is generated
$vsixOutputFolder = "$vs2022ExtensionFolder/bin/Release"
$vsixFile = Get-ChildItem -Path $vsixOutputFolder -Filter *.vsix | Sort-Object LastWriteTime -Descending | Select-Object -First 1

if ($vsixFile) {
    # Rename the .vsix file to include the current date (CaracalVSExtension-YYYY-MM-dd)
    $dateString = (Get-Date).ToString("yyyy-MM-dd")
    $newFileName = "CaracalVSExtension-$dateString.vsix"
    $newVsixPath = "$rootDir/$newFileName"

    # Check if the file exists at the destination and overwrite it
    if (Test-Path -Path $newVsixPath) {
        Write-Host "File already exists at $newVsixPath. Overwriting..."
        Remove-Item -Path $newVsixPath -Force  # Remove the existing file before moving
    }

    Write-Host "Moving the packaged extension from CaracalVSExtension to the root folder: $newVsixPath..."
    Move-Item -Path $vsixFile.FullName -Destination $newVsixPath
} else {
    Write-Host "No .vsix file found after building CaracalVSExtension!"
}

# After packaging, delete the previously copied tmLanguage file from the syntaxes folder
Write-Host "Deleting previously copied tmLanguage file from syntaxes folder in CaracalVSExtension..."
Remove-Item -Path "$targetSyntaxesFolder/$tmLanguageFile" -Force

Write-Host "Deleting obj and bin folder in CaracalVSExtension..."
Remove-Item -Path "$vs2022ExtensionFolder/obj" -Force -Recurse
Remove-Item -Path "$vs2022ExtensionFolder/bin" -Force -Recurse
