[Code]

// Removes a read only attribute from a file
procedure RemoveReadOnly(FileName : String);
var
 Attr : DWord;
begin
  Attr := GetFileAttributes(FileName);

  if (Attr and 1) = 1 then          
  begin
    Attr := Attr -1;
    SetFileAttributes(FileName, Attr);
  end;
end;

// Used to copy the vanilla install to {app} also the extracted .zip file back to {app}
procedure DirectoryCopy(SourcePath, DestPath: string; Move: Boolean);
var
  FindRec: TFindRec;
  SourceFilePath: string;
  DestFilePath: string;
  Size: Int64;
begin
  if FindFirst(SourcePath + '\*', FindRec) then
  begin
    try
      repeat
        if (FindRec.Name <> '.') and (FindRec.Name <> '..') then
        begin
          SourceFilePath := SourcePath + '\' + FindRec.Name;
          DestFilePath := DestPath + '\' + FindRec.Name;
          WizardForm.StatusLabel.Caption := 'Copying from ' + SourceFilePath + ' ' + DestFilePath;
          
          if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
          begin
            Size := Int64(FindRec.SizeHigh) shl 32 + FindRec.SizeLow;
            if not FileCopy(SourceFilePath, DestFilePath, False) then
            begin
              RaiseException(Format('Failed to copy %s to %s', [
                SourceFilePath, DestFilePath]));
            end;

            // Delete the source file if we just want to move instead of copy
            if Move then
              DeleteFile(SourceFilePath);

            // We want to ensure every file has write access so we can properly overwrite them later.
            // Presumably these permissions aren't an issue on Wine.
            RemoveReadOnly(DestFilePath);
          end
            else
          begin
            Size := 0
            if DirExists(DestFilePath) or CreateDir(DestFilePath) then
              DirectoryCopy(SourceFilePath, DestFilePath, Move)
              else
              RaiseException(Format('Failed to create %s', [DestFilePath]));
          end;
        end;

        Size := Size / 1024;
        WizardForm.ProgressGauge.Position := WizardForm.ProgressGauge.Position + Longint(Size) * WizardForm.ProgressGauge.Max div 100;
      until not FindNext(FindRec);
    finally
      FindClose(FindRec);
    end;
  end
    else
  begin
    RaiseException(Format('Failed to list %s', [SourcePath]));
  end;
end;

// Used to replace strings in files. This replaces FLMM functions
function FileReplaceString(const FileName, SearchString, ReplaceString: string):boolean;
var
  MyFile : TStrings;
  MyText : string;
  Index : Integer;
begin
  MyFile := TStringList.Create;

  try
    result := true;

    try
      MyFile.LoadFromFile(FileName);
      MyText := MyFile.Text;

      // Save the file in which the text has been changed
      if StringChangeEx(MyText, SearchString, ReplaceString, True) > 0 then
      begin;
        MyFile.Text := MyText;
        MyFile.SaveToFile(FileName);

        // Keep track of all config files that have been edited. We only want to store each file name once.
        if not EditedConfigFiles.Find(FileName, Index) then
          EditedConfigFiles.Add(FileName);
      end;
    except
      result := false;
    end;
  finally
    MyFile.Free;
  end;
end;

// Returns true if the directory is empty
function isEmptyDir(dirName: String): Boolean;
var
  FindRec: TFindRec;
  FileCount: Integer;
begin
  Result := True;
  if FindFirst(dirName+'\*', FindRec) then begin
    try
      repeat
        if (FindRec.Name <> '.') and (FindRec.Name <> '..') then begin
          FileCount := 1
          Result := False
          break;
        end;
      until not FindNext(FindRec);
    finally
      FindClose(FindRec);
      if FileCount = 0 then Result := True;
    end;
  end;
end;

// Used to remove an unwanted byte order mark in a file. 
// Calls an external program to take care of that.
function RemoveBOM(const FileName: String): Integer;
var
  ResultCode: Integer;
begin
  Exec(ExpandConstant('{tmp}\utf-8-bom-remover.exe'), ExpandConstant('"' + FileName + '"'), '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  result := ResultCode;
end;

// Whether the given char is a digit
function IsDigit(C: Char): Boolean;
begin
  Result := (C >= '0') and (C <= '9')
end;

// Creates a directory only if it doesn't exist yet
procedure CreateDirIfNotExists(const Dir: String);
begin
  if not DirExists(Dir) then
    CreateDir(Dir);
end; 

// Convert a char array to string (both Ansi)
function CharArrayToStringAnsi(charArray: array of AnsiChar): AnsiString;
var
  i: integer;
begin
  for i := Low(charArray) to High(charArray) do
  begin
    // If we've reached a null-character, that means we're at the end of the string
    if charArray[i] = #0 then
      break;

    Result := Result + charArray[i];
  end;
end;

// Determine what language the user's system is
function GetSystemLanguage(): TSystemLanguage;
var
  UILanguage: Integer;
begin
  UILanguage := GetUILanguage

  // $3FF is used to extract the primary language identifier
  if UILanguage and $3FF = $07 then // $07 = German primary language identifier
    Result := S_German
  else if UILanguage and $3FF = $0C then // $07 = French primary language identifier
    Result := S_French
  else if UILanguage and $3FF = $19 then // $19 = Russian primary language identifier
    Result := S_Russian
  else
    Result := S_EnglishOrOther;
end;

// Determine what language the user's Freelancer installation is based on the OfferBribeResourecs.dll file
// The reason we use OfferBribeResourecs.dll is because this file is rarely touched by different variations of a translation, also the file size is smaller
function GetFreelancerLanguage(FreelancerPath: string): FlLanguage;
var
  OfferBribeResourcesFile: string;
  OfferBribeResourcesMD5: string;
begin
  OfferBribeResourcesFile := + FreelancerPath + '\EXE\offerbriberesources.dll';

  // Check if the OfferBribeResourecses file exists
  if not FileExists(OfferBribeResourcesFile) then
  begin
    Result := FL_Unknown;
    exit;
  end;

  // GetMD5OfFile throws an exception if it fails, so we put it in a try statement just to be sure
  try
    OfferBribeResourcesMD5 := GetMD5OfFile(OfferBribeResourcesFile);
  except
    Result := FL_Unknown;
    exit;
  end;

  // Compare the MD5 hash to a list of known MD5 hashes from different language files
  case OfferBribeResourcesMD5 of
    '9fb0c85a1f88e516762d71cbfbb69fa7', '801c5c314887e43de8f04dbeee921a31', 'f002ba64816723cb96d82af2c7af988a': Result := FL_English; // Vanilla English (official), JFLP v1.27 English, TSR v1.2 English
    '403c420f051dc3ce14fcd2f7b63cf0c8': Result := FL_German; // German (official)
    '78a283161a7aa6c91756733a4b456ab1': Result := FL_French; // French (official)
    '6ed61e8db874b5b8bae72d3689ac1f43', '1c5736b9c808538ff77174c29a2ffa08': Result := FL_Russian; // Russian translation by Elite-Games
    '17933dcced8a8faa0c1f2316f8289c35': Result := FL_Spanish; // Spanish translation by Clan DLAN
    'eaeab5c42d6d6a4d54dd1927a1351a6d': Result := FL_Mandarin; // Mandarin/Taiwanese translation
    'fad76d9880579e841b98d018e8dbde6c': Result := FL_Czech; // Czech translation by Spider
  else
    Result := FL_Unknown;
  end;
end;
