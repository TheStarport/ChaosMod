Write-Output "Looking for current instances of Freelancer.exe"
$freelancer = Get-Process freelancer -ErrorAction SilentlyContinue
if ($freelancer){
Write-Output "Found $freelancer"
Write-Output "Stopping $freelancer"
Get-Process "freelancer" | Stop-Process
$freelancer.WaitForExit()
}
Remove-Variable freelancer

Write-Output "Copying asset files"
Copy-Item -Path ".\Assets\DATA\" -Destination "$env:LOCALAPPDATA\Freelancer HD Edition\" -Recurse -Force
Copy-Item -Path ".\Assets\EXE\" -Destination "$env:LOCALAPPDATA\Freelancer HD Edition\" -Recurse -Force

Write-Output "Starting Freelancer HD Edition in windowed mode"
Start-Process -FilePath "$env:LOCALAPPDATA\Freelancer HD Edition\EXE\Freelancer.exe" -ArgumentList "-w"