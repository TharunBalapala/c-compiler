# g++ compiler script
if (!(Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}
Write-Host "Compiling src/*.cpp -> build/ccompiler.exe..."
g++ -std=c++17 -Wall -Wextra src/*.cpp -o build/ccompiler.exe
if ($?) {
    Write-Host "Build succeeded!" -ForegroundColor Green
} else {
    Write-Host "Build failed." -ForegroundColor Red
}
