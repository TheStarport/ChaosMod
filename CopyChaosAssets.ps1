<#
 .DESCRIPTION: Asset Copy and Run Script for Freelancer Chaos Mod
 .SYNOPSIS: This script checks for running instances of Freelancer.exe, kills them, and then copies files from the 'Assets' folder of the script root over to the pre-defined FL_CHAOS_MOD directory. It it's run without the $NoLaunch parameter, it will then launch the applciation from the same directory and pipe FlSpew.txt into the console with log highlighting for warnings and errors until the application is terminated. If the application terminates unexpectedly, it will fetch the application crash event log from the Windows Event Logs for quick debugging.
 .AUTHOR: IrateRedKite, Lazrius 
 .REVISION HISTORY: 
 v1.0 2023-10-12: Initial release
 v1.2 2023-10-12: Script now compiles infocards from an frc file using Adoxa's tool on launch
 v1.3 2023-10-19: Include XML -> ALE pipeline utilizing the Freelancer XML project
 v1.4 2023-10-20: Fixed the XML -> ALE pipeline, up to 500 jobs now run in parallel when unpacking and packing files. Add timers and some additional checks.
#>

Param ($noLaunch, $noXml)
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

$func = {
    param(     
        [Parameter(Mandatory)]   
        [string]$proc,
        [Parameter(Mandatory)]
        [string]$params
    )
    Start-Process -FilePath "$proc" -Wait -ArgumentList $params
}

function Convert {
    param(     
        [Parameter(Mandatory)]   
        [boolean]$toXml
    )

    $source = if ($toXml -eq $true) { $effectPackedPath } else { $effectXMLPath }
    $destination = if ($toXml -eq $true) { $effectXMLPath } else { $effectALEOutputPath }
    $exePath = if ($toXml -eq $true) { "UTFXML.exe" } else { "XMLUTF.exe" }
    $filter = if ($toXml -eq $true) { ".ale" } else { ".ale.xml" }

    $files = Get-ChildItem "$source" -Filter "*$filter"

    $counter = [pscustomobject] @{ Value = 0 }
    $groupSize = 100

    $groups = $files | Group-Object -Property { [math]::Floor($counter.Value++ / $groupSize) }
    
    foreach ($group in $groups) {
        $jobs = foreach ($file in $group.Group) {
            Start-Job -ScriptBlock $func -Arg @("$PSScriptRoot\$exePath", "-o $destination $($file.FullName)")
            $trimmedDestination = $destination.TrimStart("$PSScriptRoot")
            Write-Host "Converting $($file) and writing it to $trimmedDestination"
        }
        Receive-Job $jobs -Wait -AutoRemoveJob
    }
}

#Define a timer
$watch = New-Object System.Diagnostics.Stopwatch

#Set infocard directories and compile them from the .frc file.
$infocardXMLPath = "$PSScriptRoot\Assets\InfocardImports.frc" 
$frcPath = "$PSScriptRoot\frc.exe"
$trimmedinfocardXMLPath = $infocardXMLPath.TrimStart("$PSScriptRoot")
$infocardDestinationPath = "$PSScriptRoot\Assets\DATA\CHAOS\ChaosInfocards.dll"
$trimmedinfocardDestinationPath = $infocardDestinationPath.TrimStart("$PSScriptRoot")

Write-Host "Compiling infocards from $trimmedinfocardXMLPath to $trimmedinfocardDestinationPath..." -ForegroundColor Blue
$watch.Start() 
Start-Process -Wait $frcPath -ArgumentList "$infocardXMLPath", "$infocardDestinationPath"
$watch.Stop()
$time = Write-Output $watch.Elapsed.TotalSeconds
$watch.reset()
Write-Host "Infocards compiled in $time seconds!" -ForegroundColor Green

if ($noXml -eq $false)
{
    #Set ALE and XML directories
    $effectXMLPath = "$PSScriptRoot\Assets\DATA\CHAOS\VFX\XML"
    $effectPackedPath = "$PSScriptRoot\Assets\DATA\CHAOS\VFX"
    $effectALEOutputPath = "$PSScriptRoot\Assets\DATA"

    #Unpack the ALE files to XML for source control
    Write-Host "Unpacking ALE assets to XML for source control..."  -ForegroundColor Blue
    $watch.Start() 
    Convert -toXml $true
    $watch.Stop()
    $time = Write-Output $watch.Elapsed.TotalSeconds
    Write-Host "ALE assets unpacked in $time seconds!" -ForegroundColor Green
    $watch.reset()

    #Repack the XML files to ALE for build
    Write-Host "Packing ALE assets for build..." -ForegroundColor Blue
    $watch.Start() 
    Convert -toXml $false
    $watch.Stop()
    $time = Write-Output $watch.Elapsed.TotalSeconds
    Write-Host "ALE assets packed in $time seconds!" -ForegroundColor Green
    $watch.reset()
}

#Look for Freelancer.exe and terminate the process if it exists.
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

#Checks if FL_CHAOS_MOD doesn't exist and then prompts the user to enter it for this session.
if (!$env:FL_CHAOS_MOD) {
    $env:FL_CHAOS_MOD = Read-Host -Prompt "Enter the full path for the EXE folder of the Freelancer instance you wish to run. This environment variable will only persist for the duration of this PowerShell session and should be set permanently using the System.Environment class."
    $checkCopyPath = Test-Path -Path $env:FL_CHAOS_MOD
    if($checkCopyPath -eq $false){
        Write-Host "The FL_CHAOS_MOD environment variable has been set to a path that does not exist. Asset copy may fail!" -ForegroundColor Red
    }
    Write-Host "FL_CHAOS_MOD has been set to $env:FL_CHAOS_MOD" -ForegroundColor Blue
}

#Check the FL_CHAOS_MOD environment variable exists and if so, copy the files over to the application's directory.
if ($env:FL_CHAOS_MOD) {
    $fullCopyDestination = Resolve-Path $env:FL_CHAOS_MOD\..\
    Write-Host "Copying asset files from $PSScriptRoot\ to $fullCopyDestination" -ForegroundColor Blue
    $watch.Start() 
    Copy-Item -Path "$PSScriptRoot\Assets\DATA\" -Destination "$fullCopyDestination" -Recurse -Force
    Copy-Item -Path "$PSScriptRoot\Assets\EXE\" -Destination "$fullCopyDestination" -Recurse -Force
    $watch.Stop()
    $time = Write-Output $watch.Elapsed.TotalSeconds
    Write-Host "Asset files copied over in $time seconds!" -ForegroundColor Green
    $watch.reset()
}
else {
    Write-Host "The FL_CHAOS_MOD environment variable is not set! Aborting..." -ForegroundColor Red
    exit
}

#Launch the application and filter the logs into the PowerShell console.
if (!$noLaunch) {
    $startTime = Get-Date -Format "yyyy-MM-dd-HH:mm:ss"
    $spewLocation = "$env:LOCALAPPDATA\Freelancer\FLSpew.txt"
    $freelancerJob = Start-Process -PassThru -FilePath "$env:FL_CHAOS_MOD\Freelancer.exe" -ArgumentList "-w"
    $freelancerJobId = $freelancerJob.Id
    Write-Host "Starting Chaos Mod in windowed mode at $startTime with PID $freelancerJobId" -ForegroundColor Green
    Write-Host "Logging $spewLocation to console" -ForegroundColor Magenta
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