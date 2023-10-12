<#
 .DESCRIPTION: Asset Copy and Run Script for Freelancer Chaos Mod
 .SYNOPSIS: This script checks for running instances of Freelancer.exe, kills them, and then copies files from the 'Assets' folder of the script root over to the pre-defined FLHOOK_COPY_PATH directory. It it's run without the $NoLaunch parameter, it will then launch the applciation from the same directory and pipe FlSpew.txt into the console with log highlighting for warnings and errors until the application is terminated. If the application terminates unexpectedly, it will fetch the application crash event log from the Windows Event Logs for quick debugging.
 .AUTHOR: IrateRedKite, Lazrius 
 .REVISION HISTORY: 
 v1.0 2023-10-12: Initial release
 v1.2 2023-10-12: Script now compiles infocards from an frc file using Adoxa's tool on launch
#>
Param ($noLaunch)
$init = { function Get-LogColor {
        Param([Parameter(Position = 0)]
            [String]$logEntry)
    
        process {
            if ($logEntry.Contains("DEBUG")) { Return "Green" }
            elseif ($logEntry.Contains("WARNING")) { Return "Yellow" }
            elseif ($logEntry.Contains("fail")) { Return "DarkYellow" }
            elseif ($logEntry.Contains("ERROR")) { Return "Red" }
            else { Return "White" }
        }
    }
}
function TailFileUntilProcessStops {
    Param ($processID, $filePath)
    $loopBlock = {
        Param($filePath) Get-Content $filePath -Wait | ForEach-Object { Write-Host -ForegroundColor (Get-LogColor $_) $_ }
    }
    $tailLoopJob = start-job -scriptBlock $loopBlock -ArgumentList $filePath -InitializationScript $init
    try {
        do {
            $tailLoopJob | Receive-Job
            try {
                Wait-Process -id $processID -ErrorAction Stop -Timeout 1
                $waitMore = $false
            }
            catch {
                $waitMore = $true
            }
        } while ($waitMore)
    }
    finally {
        Stop-Job $tailLoopJob
        Remove-Job $tailLoopJob
    }
}
$infocardXMLPath = "$PSScriptRoot\Assets\InfocardImports.frc" 
$frcPath = "$PSScriptRoot\frc.exe"
Write-Host "Compiling infocards from $infocardXMLPath to $PSScriptRoot\Assets\DATA\CHAOS\ChaosInfocards.dll..." -ForegroundColor Blue
Start-Process $frcPath -ArgumentList "$infocardXMLPath", "$PSScriptRoot\Assets\DATA\CHAOS\ChaosInfocards.dll"

Write-Host "Looking for current instances of Freelancer.exe" -ForegroundColor Blue
$freelancer = Get-Process freelancer -ErrorAction SilentlyContinue
if ($freelancer) {
    $freelancerName = $freelancer.Path
    $freelancerId = $freelancer.Id
    Write-Host "Found $freelancerName ($freelancerId)" -ForegroundColor Yellow 
    Write-Host "Stopping $freelancerName ($freelancerId)" -ForegroundColor Yellow
    Get-Process "freelancer" | Stop-Process
    $freelancer.WaitForExit()
}
if ($env:FLHOOK_COPY_PATH) {
    $fullCopyDestination = Resolve-Path $env:FLHOOK_COPY_PATH\..\
    Write-Host "Copying asset files from $PSScriptRoot to $fullCopyDestination" -ForegroundColor Blue
    Copy-Item -Path "$PSScriptRoot\Assets\DATA\" -Destination "$fullCopyDestination" -Recurse -Force
    Copy-Item -Path "$PSScriptRoot\Assets\EXE\" -Destination "$fullCopyDestination" -Recurse -Force
}
else {
    Write-Host "The FLHOOK_COPY_PATH environment variable is not set! Aborting..." -ForegroundColor Red
    exit
}
if (!$noLaunch) {
    $startTime = Get-Date -Format "yyyy-MM-dd-HH:mm:ss"
    $spewLocation = "$env:LOCALAPPDATA\Freelancer\FLSpew.txt"
    $freelancerJob = Start-Process -PassThru -FilePath "$env:FLHOOK_COPY_PATH\Freelancer.exe" -ArgumentList "-w"
    $freelancerJobId = $freelancerJob.Id
    Write-Host "Starting Chaos Mod in windowed mode at $startTime with PID $freelancerJobId" -ForegroundColor Green
    Write-Host "Logging $spewLocation to console" -ForegroundColor Blue
    TailFileUntilProcessStops -processID $freelancerJob.id -filepath $spewLocation
    $endTime = Get-Date -Format "yyyy-MM-dd-HH:mm:ss"  
    $hexJobId = ('0x{0:x}' -f $freelancerJobId)
    $err = Get-WinEvent -FilterHashtable @{LogName = 'Application'; ID = 1000 } -MaxEvents 1 | Where-Object -Property Message -Match 'Faulting application name: Freelancer.exe' | Where-Object -Property Message -Match "$hexJobId"
    if ($err) {
        Write-Host "Chaos Mod Process with PID $freelancerJobId has terminated unexpectedly at $endTime. Fetching crash event from Application logs... " -ForegroundColor Red
        Write-Host $err.Message
        Write-Host "For debugging, please reference the Fault offset and Faulting module name against https://wiki.the-starport.net/wiki/fl-binaries/crash-offsets" -ForegroundColor Red
    }
    else {
        Write-Host "Chaos Mod Process with PID $freelancerJobId has terminated at $endTime" -ForegroundColor Green
    }
}