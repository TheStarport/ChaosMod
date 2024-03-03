$ErrorActionPreference = "Stop"

msbuild ./ChaosMod.sln /t:Build /p:Configuration=Release /p:Platform=x86