param ($NoLaunch)

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
Copy-Item -Path "$PSScriptRoot\Assets\DATA\" -Destination "$env:FLHOOK_COPY_PATH\..\" -Recurse -Force
Copy-Item -Path "$PSScriptRoot\Assets\EXE\" -Destination "$env:FLHOOK_COPY_PATH\..\" -Recurse -Force

IF(!$NoLaunch)
{
    Write-Host "Starting Chaos Mod in windowed mode" -ForegroundColor Green
    Start-Process -FilePath "$env:FLHOOK_COPY_PATH\Freelancer.exe" -ArgumentList "-w"
}