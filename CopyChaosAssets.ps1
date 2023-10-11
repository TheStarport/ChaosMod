param ($NoLaunch)

Write-Host "Looking for current instances of Freelancer.exe" -ForegroundColor Blue
$freelancer = Get-Process freelancer -ErrorAction SilentlyContinue
if ($freelancer) {
    Write-Host "Found $freelancer" -ForegroundColor Yellow
    Write-Host "Stopping $freelancer" -ForegroundColor Yellow
    Get-Process "freelancer" | Stop-Process
    $freelancer.WaitForExit()
}
Remove-Variable freelancer

if ($env:FLHOOK_COPY_PATH) {
    $FullCopyDestination = Resolve-Path $env:FLHOOK_COPY_PATH\..\
    Write-Host "Copying asset files from $PSScriptRoot to $FullCopyDestination" -ForegroundColor Blue
    Copy-Item -Path "$PSScriptRoot\Assets\DATA\" -Destination "$FullCopyDestination" -Recurse -Force
    Copy-Item -Path "$PSScriptRoot\Assets\EXE\" -Destination "$FullCopyDestination" -Recurse -Force
    Remove-Variable FullCopyDestination
}
else {
    Write-Host "The FLHOOK_COPY_PATH environment variable is not set! Aborting..." -ForegroundColor Red
    exit
}
if (!$NoLaunch) {
    Write-Host "Starting Chaos Mod in windowed mode" -ForegroundColor Green
    Start-Process -FilePath "$env:FLHOOK_COPY_PATH\Freelancer.exe" -ArgumentList "-w"
}