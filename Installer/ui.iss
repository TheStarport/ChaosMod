[Code]
var
  // Custom Pages
  DataDirPage: TInputDirWizardPage;
  PageEnglishImprovements: TWizardPage;
  StartupRes: TInputOptionWizardPage;
  SmallText: TInputOptionWizardPage;
  PagePlanetScape: TWizardPage;
  PageSkips: TWizardPage;

  // Russian fonts
  RussianFonts: TCheckBox;
  descRussianFonts: TNewStaticText;

  // Fix clipping with 16:9 resolution planetscapes
  PlanetScape: TCheckBox;
  descPlanetScape: TNewStaticText;

  // Skip intros
  SkipIntros: TCheckBox;
  descSkipIntros: TNewStaticText;

  // Jump tunnel duration
  JumpTunnel10Sec: TRadioButton;
  JumpTunnel5Sec: TRadioButton;
  descJumpTunnelDuration: TNewStaticText;

// Update progress of installer bar
procedure UpdateProgress(Position: Longint);
begin
  WizardForm.ProgressGauge.Position :=
    Position * WizardForm.ProgressGauge.Max div 100;
end;

// Handles key presses for an integer field
procedure DigitFieldKeyPress(Sender: TObject; var Key: Char);
begin
  if not ((Key = #8) or { Tab key }
          (Key = #3) or (Key = #22) or (Key = #24) or { Ctrl+C, Ctrl+V, Ctrl+X }
          IsDigit(Key)) then
  begin
    Key := #0;
  end;
end;       

procedure InitializeUi();
var 
  dir : string;
begin
  dir := 'C:\Program Files (x86)\Microsoft Games\Freelancer'

  // Initialize DataDirPage and add content
  DataDirPage := CreateInputDirPage(wpInfoBefore,
  'Select Freelancer installation', 'Where is Freelancer installed?',
  'Select the folder in which a fresh and completely unmodded copy of Freelancer is installed. This is usually ' + dir + '.' + #13#10 +
  'The folder you select here will be copied without modification.',
  False, '');
  DataDirPage.Add('');
  
  // If the Reg key exists, use its content to populate the folder location box. Use the default path if otherwise.
  RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Microsoft Games\Freelancer\1.0', 'AppPath', dir)
  DataDirPage.Values[0] := dir

  // Initialize English Improvements page and add content
  PageEnglishImprovements := CreateCustomPage(DataDirPage.ID,
  'Localization', 'Apply Russian Font Support.');
  
  descRussianFonts := TNewStaticText.Create(PageEnglishImprovements);
  descRussianFonts.Parent := PageEnglishImprovements.Surface;
  descRussianFonts.Top := ScaleY(80);
  descRussianFonts.WordWrap := True;
  descRussianFonts.Width := PageEnglishImprovements.SurfaceWidth;
  descRussianFonts.Caption := 'This option will use a Cyrillic version of the Agency FB font for Freelancer. Users with a Russian Freelancer installation may want to enable this.';

  RussianFonts := TCheckBox.Create(PageEnglishImprovements);
  RussianFonts.Parent := PageEnglishImprovements.Surface;
  // Only check the Russian fonts option if the user's system language is set to Russian
  RussianFonts.Checked := SystemLanguage = S_Russian;
  RussianFonts.Caption := 'Use Russian fonts';
  RussianFonts.Width := PageEnglishImprovements.SurfaceWidth - ScaleX(8);
  RussianFonts.Top := ScaleY(60);

  // Initialize StartupRes page and add content
  StartupRes := CreateInputOptionPage(PageEnglishImprovements.ID,
  'Startup Screen Resolution', 'Choose your native resolution.',
  'By default, the "Freelancer" splash screen you see when you start the game has a resolution of 1280x960. This makes it appear stretched and a bit blurry on HD 16:9 resolutions. ' +
  'We recommend setting this option to your monitor''s native resolution. ' +
  'Please note that a higher resolution option may negatively impact the game''s start-up speed.',
  True, False);
  StartupRes.Add('Remove Startup Screen');
  StartupRes.Add('720p 16:9 - 1280x720');
  StartupRes.Add('960p 4:3 - 1280x960 (Vanilla)');
  StartupRes.Add('1080p 4:3 - 1440x1080');
  StartupRes.Add('1080p 16:9 - 1920x1080');
  StartupRes.Add('1440p 4:3 - 1920x1440');
  StartupRes.Add('1440p 16:9 - 2560x1440');
  StartupRes.Add('4K 4:3 - 2880x2160');
  StartupRes.Add('4K 16:9 - 3840x2160');

  // Determine best default startup resolution based on user's screen size
  if DesktopRes.Height >= 2160 then
    StartupRes.Values[8] := True
  else if DesktopRes.Height >= 1440 then
    StartupRes.Values[6] := True
  else
    StartupRes.Values[4] := True;
  
  // Fix Small Text on 1440p/4K resolutions
  SmallText := CreateInputOptionPage(StartupRes.ID,
  'Fix small text on 1440p/4K resolutions', 'Check to install.',
  'Many high-resolution Freelancer players have reported missing HUD text and misaligned buttons in menus. In 4K, the nav map text is too small and there are many missing text elements in the HUD. For 1440p screens, the only apparent issue is the small nav map text.' + #13#10 + #13#10 +
  'Select the option corresponding to the resolution you''re going to play Freelancer in. If you play in 1920x1080 or lower, the "No" option is fine as the elements are configured correctly already.',
  True, False);
  SmallText.Add('No');
  SmallText.Add('Yes, apply fix for 2560x1440 and 1920x1200 screens');
  SmallText.Add('Yes, apply fix for 3840x2160 screens');

  // Determine best small text fix based on user's screen size
  if DesktopRes.Height >= 2160 then
    SmallText.Values[2] := True
  else if DesktopRes.Height >= 1200 then
    SmallText.Values[1] := True
  else
    SmallText.Values[0] := True;
  
  // Fix clipping with 16:9 resolution planetscapes
  PagePlanetScape := CreateCustomPage(
    SmallText.ID,
    'Fix clipping with 16:9 resolution planetscapes',
    'Check to install.'
  );
  
  descPlanetScape := TNewStaticText.Create(PagePlanetScape);
  descPlanetScape.Parent := PagePlanetScape.Surface;
  descPlanetScape.WordWrap := True;
  descPlanetScape.Top := ScaleY(20);
  descPlanetScape.Width := PagePlanetScape.SurfaceWidth;
  descPlanetScape.Caption := 'Since Freelancer was never optimized for 16:9 resolutions, there are several inconsistencies with planetscapes that occur while viewing them in 16:9, such as clipping and geometry issues.' + #13#10 + #13#10 +
  'This mod gives you the option of fixing this, as it adjusts the camera values in the planetscapes so the issues are no longer visible in 16:9 resolutions.' + #13#10 + #13#10 +
  'Disable this option if you play in 4:3. Also please note that the planetscape views may look zoomed in when using this option with an ultrawide resolution.'
  
  PlanetScape := TCheckBox.Create(PagePlanetScape);
  PlanetScape.Parent := PagePlanetScape.Surface;
  PlanetScape.Caption := 'Fix clipping with 16:9 resolution planetscapes';
  PlanetScape.Width := PagePlanetScape.SurfaceWidth - ScaleX(8);

  // Only check the planetscapes fix option if the user's aspect ratio is 16:9
  PlanetScape.Checked := true;

  // Skips
  PageSkips := CreateCustomPage(
    PagePlanetScape.ID,
    'Skippable options',
    'Want to save time?'
  );

  // Skip intros  
  JumpTunnel10Sec := TRadioButton.Create(PageSkips);
  JumpTunnel10Sec.Parent := PageSkips.Surface;
  JumpTunnel10Sec.Caption := '10 second jump tunnels (Vanilla)';
  JumpTunnel10Sec.Width := PageSkips.SurfaceWidth - ScaleX(8);
  JumpTunnel10Sec.Checked := True;

  JumpTunnel5Sec := TRadioButton.Create(PageSkips);
  JumpTunnel5Sec.Parent := PageSkips.Surface;
  JumpTunnel5Sec.Top := ScaleY(20);
  JumpTunnel5Sec.Caption := '5 second jump tunnels';
  JumpTunnel5Sec.Width := PageSkips.SurfaceWidth - ScaleX(8);
  
  descJumpTunnelDuration := TNewStaticText.Create(PageSkips);
  descJumpTunnelDuration.Parent := PageSkips.Surface;
  descJumpTunnelDuration.WordWrap := True;
  descJumpTunnelDuration.Top := JumpTunnel5Sec.Top + ScaleY(40);
  descJumpTunnelDuration.Width := PageSkips.SurfaceWidth;
  descJumpTunnelDuration.Caption := 'This option allows you to change the duration of the jump tunnels which you go through when using any jump hole or jump gate.';
  
  // Jump tunnel duration  
  descSkipIntros := TNewStaticText.Create(PageSkips);
  descSkipIntros.Parent := PageSkips.Surface;
  descSkipIntros.WordWrap := True;
  descSkipIntros.Top := descJumpTunnelDuration.Top + ScaleY(100);
  descSkipIntros.Width := PageSkips.SurfaceWidth;
  descSkipIntros.Caption := 'This option skips the 3 movies that play when the game starts, which include the Microsoft logo, Digital Anvil logo, and Freelancer intro.';
  
  SkipIntros := TCheckBox.Create(PageSkips);
  SkipIntros.Parent := PageSkips.Surface;
  SkipIntros.Top := descJumpTunnelDuration.Top + ScaleY(80);
  SkipIntros.Checked := True;
  SkipIntros.Caption := 'Skip startup intros';
  SkipIntros.Width := PageSkips.SurfaceWidth - ScaleX(8);
end;
