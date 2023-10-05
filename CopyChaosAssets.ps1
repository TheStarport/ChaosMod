Write-Host "Looking for current instances of Freelancer.exe" -ForegroundColor Yellow
$freelancer = Get-Process freelancer -ErrorAction SilentlyContinue
if ($freelancer){
Write-Host "Found $freelancer" -ForegroundColor Cyan
Write-Host "Stopping $freelancer" -ForegroundColor Yellow
Get-Process "freelancer" | Stop-Process
$freelancer.WaitForExit()
}
Remove-Variable freelancer

Write-Host "Copying asset files" -ForegroundColor Yellow
Copy-Item -Path ".\Assets\DATA\" -Destination "$env:LOCALAPPDATA\Freelancer HD Edition\" -Recurse -Force
Copy-Item -Path ".\Assets\EXE\" -Destination "$env:LOCALAPPDATA\Freelancer HD Edition\" -Recurse -Force

Write-Host "Starting Freelancer HD Edition in windowed mode" -ForegroundColor Green
Start-Process -FilePath "$env:LOCALAPPDATA\Freelancer HD Edition\EXE\Freelancer.exe" -ArgumentList "-w"