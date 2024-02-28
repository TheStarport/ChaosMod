[Code]

// Gets the path of a file in the My Games\Freelancer(HD) directory
function GetOptionsPath(FileName: string): string;
var
  OptionsFolder: string;
  MyGamesFolder: string;
begin
  MyGamesFolder := ExpandConstant('{userdocs}\My Games\')
  OptionsFolder := 'FLChaosMod'

  CreateDirIfNotExists(MyGamesFolder);
  CreateDirIfNotExists(MyGamesFolder + OptionsFolder);

  Result := MyGamesFolder + OptionsFolder + '\' + FileName + '.ini'
end;
