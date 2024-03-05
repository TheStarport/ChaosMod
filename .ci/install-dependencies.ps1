# Explicitly install dependencies as their own step so we can cache the results
$ErrorActionPreference = "Stop"
$VCPKG_DIR = "${HOME}\vcpkg"
$DX9_SDK = "${HOME}/DirectXSDK"
$INNO = "${HOME}/InnoSetup"

# either install Vcpkg or update an existing installation
if (Test-Path "$VCPKG_DIR" -PathType Container)
{
    $UPDATE_VCPKG = 1
}
else
{
    $UPDATE_VCPKG = 0
}

Write-Host "---- install-vcpkg.ps1 ----"
Write-Host "Vcpkg install directory: $VCPKG_DIR"
Write-Host "Update existing Vcpkg? $UPDATE_VCPKG"
Write-Host "---------------------------"

if ($UPDATE_VCPKG)
{
    # update existing Vcpkg
    Push-Location "$VCPKG_DIR"
    git --quiet pull
    .\bootstrap-vcpkg.bat -disableMetrics
    .\vcpkg.exe integrate install
    Pop-Location
}
else
{
    # install Vcpkg
    Push-Location "$HOME"
    git clone --quiet https://github.com/Microsoft/vcpkg.git
    Set-Location vcpkg
    .\bootstrap-vcpkg.bat -disableMetrics
    .\vcpkg.exe integrate install
    Pop-Location
    
}

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

# Only download and install dx9 sdk if not cached
if (-not (Test-Path $DX9_SDK)) {
    mkdir TEMP; cd TEMP
    & "C:\windows\system32\curl.exe" -L -o "_DX2010_.exe" "https://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe"
    7z x _DX2010_.exe DXSDK/Include -o_DX2010_
    

    7z x _DX2010_.exe DXSDK/Lib/x86 -o_DX2010_
    mv _DX2010_/DXSDK $DX9_SDK
    cd ..; rm TEMP -Force -Recurse
    [System.Environment]::SetEnvironmentVariable('DX9_SDK', $DX9_SDK, 'User')
}

# Install InnoSetup
& "C:\windows\system32\curl.exe" -L -o "${HOME}/InnoSetup.exe" "https://jrsoftware.org/download.php/is.exe"

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Start-Process ${HOME}/InnoSetup.exe -NoNewWindow -Wait -ArgumentList /VERYSILENT,/PORTABLE=1,/DIR="${INNO}",/SUPPRESSMSGBOXES,/NORESTART,/NOICONS,/CURRENTUSER

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

rm ${HOME}/InnoSetup.exe

