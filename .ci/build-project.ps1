$ErrorActionPreference = "Stop"

msbuild ${HOME}/project/ChaosMod.sln /t:Build /p:Configuration=Release /p:Platform=x86