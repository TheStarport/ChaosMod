; Forked from HD Edition then stripped down

#define MyAppVersion "0.1"
#define MyAppName "Freelancer: Chaos Mod v" + MyAppVersion
; Name without the colon to prevent file/explorer-related issues
#define MyAppFileName "Freelancer Chaos Mod"
#define MyAppPublisher ""
#define MyAppURL "https://github.com/TheStarport/ChaosMod"
#define MyAppExeName "Freelancer.exe"
#define SizeZip 0
#define SizeExtracted 4646719488
#define SizeVanilla 985624576
#define SizeBuffer 100000
#define SizeAll SizeZip + SizeExtracted + SizeVanilla + SizeBuffer

[Setup]
AllowNoIcons=yes
AppId={{30DACAFD-F4DC-4EF0-BA6B-5D12CF92B0CF}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppCopyright={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
ChangesAssociations=yes
Compression=lzma2
DefaultDirName={sd}\Games\{#MyAppFileName}
DefaultGroupName={#MyAppFileName}
DisableWelcomePage=False
DisableDirPage=False
DiskSpanning=True
ExtraDiskSpaceRequired = {#SizeAll}
InfoBeforeFile={#SourcePath}\Assets\Text\installinfo.txt
OutputBaseFilename=FreelancerChaosMod
SetupIconFile={#SourcePath}\Assets\Images\icon.ico
SolidCompression=yes
UninstallDisplayIcon={#SourcePath}\Assets\Images\icon.ico
UninstallDisplayName={#MyAppName}
WizardImageFile={#SourcePath}\Assets\Images\backgroundpattern.bmp
WizardSmallImageFile={#SourcePath}\Assets\Images\icon*.bmp
WizardSizePercent=105
VersionInfoVersion=1.0.0.0

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Icons]
Name: "{group}\{#MyAppFileName}"; Filename: "{app}\EXE\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppFileName}"; Filename: "{app}\EXE\{#MyAppExeName}"; Tasks: desktopicon

[Files]
Source: "Assets\Text\installinfo.txt"; DestDir: "{app}"; Flags: ignoreversion deleteafterinstall
Source: "Assets\Text\PerfOptions.ini"; DestDir: "{app}"; Flags: ignoreversion deleteafterinstall
Source: "Assets\Fonts\AGENCYB.TTF"; DestDir: "{autofonts}"; FontInstall: "Agency FB Bold"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Assets\Fonts\AGENCYR.TTF"; DestDir: "{autofonts}"; FontInstall: "Agency FB"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Assets\Fonts\AGENCYR_CR.TTF"; DestDir: "{autofonts}"; FontInstall: "Agency FB Cyrillic"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Assets\Fonts\ARIALUNI.TTF"; DestDir: "{autofonts}"; FontInstall: "Arial Unicode MS"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Assets\External\7za.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall;
Source: "Assets\External\utf-8-bom-remover.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall;
Source: "Assets\External\FreelancerHDESetup_v06_silent_test.exe"; DestDir: "{tmp}"; Flags: nocompression deleteafterinstall;
Source: "Assets\External\HexToBinary.dll"; Flags: dontcopy;
Source: "Assets\Mod\chaosmod.7z"; DestDir: "{tmp}"; Flags: nocompression deleteafterinstall
Source: "Assets\Images\icon.ico"; DestDir: "{app}\EXE"; DestName: "{#MyAppExeName}"; Flags: ignoreversion

[Run]
Filename: "{app}\EXE\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Messages]
WelcomeLabel2=Hello Beagle. I would like to play a game. Install this mod and enjoy... well you probably wont, but I will.
FinishedLabel=Setup has finished installing [name] on your computer. The application may be launched by selecting the installed shortcut.%n%nNOTE: [name] has been installed as a separate application. Therefore, your vanilla Freelancer installation has not been modified and can still be played at any time.

[Code]
var
  ProgressListBox: TNewListBox;
  ProgressPage: TOutputProgressWizardPage;

// Imports from other .iss files
#include "types.iss"
#include "external.iss"
#include "globals.iss"
#include "utilities.iss"
#include "ui.iss"
#include "mod_options.iss"

(*function BufferToAnsi(const Buffer: string): AnsiString;
var
  W: Word;
  I: Integer;
begin
  SetLength(Result, Length(Buffer) * 2);
  for I := 1 to Length(Buffer) do
  begin
    W := Ord(Buffer[I]);
    Result[(I * 2)] := Chr(W shr 8); // high byte
    Result[(I * 2) - 1] := Chr(Byte(W)); // low byte
  end;
end;

procedure SubInstallerUpdate;
var
  S: AnsiString;
  I, L, Max: Integer;
  Buffer: string;
  Stream: TFileStream;
  Lines: TStringList;
begin
  try
    // Need shared read as the output file is locked for writing,
    // so we cannot use LoadStringFromFile
    Stream :=
      TFileStream.Create(ExpandConstant('{tmp}/hdlog.txt'), fmOpenRead or fmShareDenyNone);
    try
      L := Stream.Size;
      Max := 100*2014;
      if L > Max then
      begin
        Stream.Position := L - Max;
        L := Max;
      end;
      SetLength(Buffer, (L div 2) + (L mod 2));
      Stream.ReadBuffer(Buffer, L);
      S := BufferToAnsi(Buffer);
    finally
      Stream.Free;
    end;
  except
    Log(Format('Failed to read progress from file %s - %s', [
                ExpandConstant('{tmp}/hdlog.txt'), GetExceptionMessage]));
  end;

  if S <> '' then
  begin
    Log('Progress len = ' + IntToStr(Length(S)));
    Lines := TStringList.Create();
    Lines.Text := S;
    for I := 0 to Lines.Count - 1 do
    begin
      if I < ProgressListBox.Items.Count then
      begin
        ProgressListBox.Items[I] := Lines[I];
      end
        else
      begin
        ProgressListBox.Items.Add(Lines[I]);
      end
    end;
    ProgressListBox.ItemIndex := ProgressListBox.Items.Count - 1;
    ProgressListBox.Selected[ProgressListBox.ItemIndex] := False;
    Lines.Free;
  end;

  // Just to pump a Windows message queue (maybe not be needed)
  ProgressPage.SetProgress(0, 1);
end;

procedure UpdateProgressProc(
  H: LongWord; Msg: LongWord; Event: LongWord; Time: LongWord);
begin
  SubInstallerUpdate;
end;*)

// Checks which step we are on when it changed. If its the postinstall step then start the actual installing
procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode, i: Integer;
  // TODO next update: remove
  OptListFilePath : string;
  Timer: LongWord;
begin
    if CurStep = ssPostInstall then
    begin
        (*SaveStringToFile(ExpandConstant('{tmp}/hdlog.txt'), '', False);

        ProgressPage := CreateOutputProgressPage('Installing Freelancer HD Edition', 'Please wait until this finishes...');
        Timer := SetTimer(0, 0, 250, CreateCallback(@UpdateProgressProc));

        ProgressPage.Show();
        ProgressListBox := TNewListBox.Create(WizardForm);
        ProgressListBox.Parent := ProgressPage.Surface;
        ProgressListBox.Top := 0;
        ProgressListBox.Left := 0;
        ProgressListBox.Width := ProgressPage.SurfaceWidth;
        ProgressListBox.Height := ProgressPage.SurfaceHeight;

        // Fake SetProgress call in UpdateProgressProc will show it,
        // make sure that user won't see it
        ProgressPage.ProgressBar.Top := -100;

        SubInstallerUpdate;*)

        WizardForm.StatusLabel.Caption := 'Installing Freelancer HD Edition...';
        // Run HD Edition installer
        Exec(ExpandConstant('{tmp}\FreelancerHDESetup_v06_silent_test.exe'),
          '/VERYSILENT /VanillaFlDir="' + DataDirPage.Values[0] + '"' +
          ' /PlanetScape=' + IntToStr(integer(PlanetScape.Checked)) +
          ' /RussianFonts=' + IntToStr(integer(RussianFonts.Checked)) +
          ' /StartupRes=' + IntToStr(integer(StartupRes.SelectedValueIndex)) +
          ' /PlanetScape=' + IntToStr(integer(PlanetScape.Checked)) +
          ' /SkipIntros=' + IntToStr(integer(PlanetScape.Checked)) +
          ' /SmallText=' + IntToStr(integer(SmallText.SelectedValueIndex)) +
          ' /EnglishImrpovements=0' +
          ' /BestOptions=1' +
          ' /DisplayMode=0' +
          ' /GraphicsApi=0' +
          ' /WeaponGroups=0' +
          ' /WidescreenHud=0' +
          ' /DarkHud=0' +
          ' /SinglePlayerConsole=0' +
          ' /DoNotPauseOnAltTab=0' +
          ' /Reflections=1' +
          ' /LOG="' + ExpandConstant('{tmp}/hdlog.txt') + '"' + 
          ' /JumpTunnel=' + IntToStr(integer(JumpTunnel5Sec.Checked)) +
          ' /DIR="' + ExpandConstant('{app}/Installed') + '"', 
          '', SW_HIDE, ewWaitUntilTerminated, ResultCode);

        UpdateProgress(30);

        (* KillTimer(0, Timer);
        ProgressPage.Hide;
        DeleteFile(ExpandConstant('{tmp}/hdlog.txt'));
        ProgressPage.Free(); *)

        if ResultCode <> 0 then
        begin
          Abort();
          Exit;
        end;

        // Move all the files
        DirectoryCopy(ExpandConstant('{app}\Installed'), ExpandConstant('{app}'), True);
        DelTree(ExpandConstant('{app}\Installed'), True, True, True);

        // Cleaning
        WizardForm.StatusLabel.Caption := 'Cleaning up HD Edition...';

        DelTree(ExpandConstant('{app}\FLDocs'), True, True, True);
        DelTree(ExpandConstant('{app}\EXE\reshade-shaders'), True, True, True);
        DeleteFile(ExpandConstant('{app}\EXE\console_reademe.txt'));
        DeleteFile(ExpandConstant('{app}\DLLS\BIN\callsign.dll'));
        DeleteFile(ExpandConstant('{app}\DLLS\BIN\callsign_readme.txt'));
        DeleteFile(ExpandConstant('{app}\EXE\d3d8.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\d3d8_dxwrapper.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\d3d8_legacy.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\d3d9_reshade.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\dgVoodoo.conf'));
        DeleteFile(ExpandConstant('{app}\EXE\dgVoodooCpl.exe'));
        DeleteFile(ExpandConstant('{app}\EXE\dgVoodooReadme.url'));
        DeleteFile(ExpandConstant('{app}\EXE\dgVoodooReadmeDirectX.url'));
        DeleteFile(ExpandConstant('{app}\EXE\dxgi.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\dxwrapper.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\dxwrapper.ini'));
        DeleteFile(ExpandConstant('{app}\EXE\dxwrapper_license'));
        DeleteFile(ExpandConstant('{app}\EXE\flplusplus.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\equipresources_vanilla.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\flplusplus.ini'));
        DeleteFile(ExpandConstant('{app}\EXE\flplusplus_license.txt'));
        DeleteFile(ExpandConstant('{app}\EXE\HudFacility.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\HUDless.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\HudStatus.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\HudTarget.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\HudWeaponGroups.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\infocards_vanilla.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\jflp.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\misctext_vanilla.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\misctextinfo2_vanilla.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\nameresources_vanilla.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\numeric.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\offerbriberesources_vanilla.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\ReShade.ini'));
        DeleteFile(ExpandConstant('{app}\EXE\ReShade_LICENSE.txt'));
        DeleteFile(ExpandConstant('{app}\EXE\ReShadePreset.ini'));
        DeleteFile(ExpandConstant('{app}\EXE\resources_vanilla.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\storyfactions_readme.txt'));
        DeleteFile(ExpandConstant('{app}\EXE\storyfactions.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\TurretZoom_readme.txt'));
        DeleteFile(ExpandConstant('{app}\EXE\TurretZoom.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\Territory.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\Territory_readme.txt'));
        DeleteFile(ExpandConstant('{app}\EXE\text-strings-revision-license.txt'));
        DeleteFile(ExpandConstant('{app}\EXE\WheelScroll.dll'));
        DeleteFile(ExpandConstant('{app}\EXE\zlib_license.txt'));

        UpdateProgress(55);

        // Unzip
        WizardForm.StatusLabel.Caption := ExpandConstant('Unpacking ChaosMod Files...');
        Exec(ExpandConstant('{tmp}\7za.exe'), ExpandConstant(' x -y -aoa "{tmp}\chaosmod.7z"  -o"{app}"'), '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
        // -aoa Overwrite All existing files without prompt
        // -o Set output directory
        // -y Assume "Yes" on all Queries

        UpdateProgress(85);
        WizardForm.StatusLabel.Caption := 'Cleaning up...';

        for i := 0 to EditedConfigFiles.Count - 1 do
          RemoveBOM(EditedConfigFiles[i]);

        // Delete restart.fl to stop crashes
        DeleteFile(ExpandConstant('{userdocs}\My Games\Freelancer\Accts\SinglePlayer\Restart.fl'));
        DeleteFile(ExpandConstant('{userdocs}\My Games\FLChaosMod\Accts\SinglePlayer\Restart.fl'));

        // Remove additional junk files
        DeleteFile(ExpandConstant('{app}\UNINSTAL.EXE'));

        // Install Complete!
        UpdateProgress(100);
    end;
end;

// Various logic to be applied when the user clicks on the Next button.
function NextButtonClick(PageId: Integer): Boolean;
var
  i : Integer;
  RefreshRateError: String;
  DetectedFlLanguage: FlLanguage;
begin
    Result := True;

    // Validate vanilla Freelancer directory
    if PageId = DataDirPage.ID then begin
      // Check if Freelancer is installed in the folder they have specified
      if not FileExists(DataDirPage.Values[0] + '\EXE\Freelancer.exe') then begin
        MsgBox('Freelancer does not seem to be installed in that folder. Please select the correct folder.', mbError, MB_OK);
        Result := False;
        exit;
      end;

      // If the installer is being run from the same directory as the vanilla Freelancer directory, the installation will fail because the running installer cannot be copied.
      // This checks if the active installer has been ran from any directory inside the selected vanilla Freelancer folder.
      // No issues occur when Freelancer from the vanilla directory is running, so no need to check for that.
      if Pos(DataDirPage.Values[0], GetCurrentDir()) > 0 then begin
        MsgBox(ExpandConstant('The {#MyAppName} installer is located in the same directory as the vanilla Freelancer directory. This would cause the installation to fail because this file cannot be copied.' + #13#10 + #13#10
          + 'Please close the {#MyAppName} installer, move the installer .exe file to a directory outside your vanilla Freelancer installation and try again.'), mbError, MB_OK);
        Result := False;
        exit;
      end;

      DetectedFlLanguage := GetFreelancerLanguage(DataDirPage.Values[0]);

      if DetectedFlLanguage = FL_Russian then
        RussianFonts.Checked := true
      else if DetectedFlLanguage <> FL_Unknown then
        RussianFonts.Checked := false;
    end;

    // Validate install location
    if (PageId = 6) then begin
      // Needs to be in a seperate if statement since it tries to expand {app} even if not on PageID 6. Pascal what are you doing!
      if(Pos(AddBackslash(DataDirPage.Values[0]),ExpandConstant('{app}')) > 0) then begin
        MsgBox('Freelancer: Chaos Mod cannot be installed to the same location as your vanilla install. Please select a new location.', mbError, MB_OK);
        Result := False;
        exit;
      end;
      // Check the install directory is empty
      if(not isEmptyDir(ExpandConstant('{app}'))) then begin
        MsgBox('Freelancer: Chaos Mod cannot be installed to a directory that is not empty. Please empty this directory or choose another one.', mbError, MB_OK);
        Result := False;
        exit;
      end;
    end;
end;

// Run when the wizard is opened.
procedure InitializeWizard;
begin
    WizardForm.WizardSmallBitmapImage.Stretch := false;

    InitConstants();

    SystemLanguage := GetSystemLanguage();

    // Initialize EditedConfigFiles
    EditedConfigFiles := TStringList.Create;
    EditedConfigFiles.Sorted := true;

    // Initialize UI. This populates all our ui elements with text, size and other properties
    InitializeUi();
 end;
