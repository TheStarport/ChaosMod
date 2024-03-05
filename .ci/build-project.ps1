$ErrorActionPreference = "Stop"

msbuild ${HOME}/project/FreelancerChaosMod.sln /t:Build /p:Configuration=Release /p:Platform=x86

exit $LASTEXITCODE