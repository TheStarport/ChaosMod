if (-not (Test-Path -Path "$PSScriptRoot/vendor/discord_game_sdk"))
{
    Write-Output "Discord Game SDK not found. Downloading."
    $url = "https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip"
    Invoke-WebRequest -Uri $url -OutFile $PSScriptRoot/discord_game_sdk.zip

    Write-Output "Unzipping Discord SDK"
    unzip $PSScriptRoot/discord_game_sdk.zip -d $PSScriptRoot/vendor/discord_game_sdk

    Write-Output "Cleaning Up"
    Remove-Item $PSScriptRoot/discord_game_sdk.zip
}
