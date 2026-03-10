param (
    [Parameter(Mandatory=$true)]
    [string]$InputFile
)

# 1. Run our custom compiler
Write-Host "--- 1. Running CCompiler ---" -ForegroundColor Cyan
.\build\ccompiler.exe $InputFile

if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation failed." -ForegroundColor Red
    exit $LASTEXITCODE
}

# 2. Figure out file names
$BaseName = [System.IO.Path]::GetFileNameWithoutExtension($InputFile)
$Directory = [System.IO.Path]::GetDirectoryName($InputFile)
if (-not $Directory) { $Directory = "." }

$AssemblyFile = "$Directory\$BaseName.s"
$ExeFile = "$Directory\$BaseName.exe"

# 3. Assemble with GCC
Write-Host "`n--- 2. Assembling with GCC ---" -ForegroundColor Cyan
gcc -masm=intel -o $ExeFile $AssemblyFile

if ($LASTEXITCODE -ne 0) {
    Write-Host "GCC Assembly failed." -ForegroundColor Red
    exit $LASTEXITCODE
}

# 4. Run the final executable and capture the exit code
Write-Host "`n--- 3. Running Executable ---" -ForegroundColor Cyan
& ".\$ExeFile"
$AppExitCode = $LASTEXITCODE

Write-Host "`n=============================="
Write-Host " Final Exit Code: $AppExitCode" -ForegroundColor Green
Write-Host "=============================="
