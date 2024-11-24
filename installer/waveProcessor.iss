[Setup]
AppName=waveProcessor
AppContact=spam@spam.com
AppCopyright=Copyright (C) 2019 MANUFACTURER
AppPublisher=MANUFACTURER
AppPublisherURL=http://www.spam.com
AppSupportURL=http://www.spam.com
AppVersion=1.0.0
VersionInfoVersion=1.0.0
DefaultDirName={pf}\waveProcessor
DefaultGroupName=waveProcessor
Compression=lzma2
SolidCompression=yes
OutputDir=.\
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=waveProcessor Installer
LicenseFile=license.rtf
SetupLogging=yes
ShowComponentSizes=no
; WizardImageFile=installer_bg-win.bmp
; WizardSmallImageFile=installer_icon-win.bmp

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Messages]
WelcomeLabel1=Welcome to the waveProcessor installer
SetupWindowTitle=waveProcessor installer
SelectDirLabel3=The standalone application and supporting files will be installed in the following folder.
SelectDirBrowseLabel=To continue, click Next. If you would like to select a different folder (not recommended), click Browse.

[Components]
;Name: "app"; Description: "Standalone application (.exe)"; Types: full custom;
;Name: "vst2_32"; Description: "32-bit VST2 Plugin (.dll)"; Types: full custom;
;Name: "vst2_64"; Description: "64-bit VST2 Plugin (.dll)"; Types: full custom; Check: Is64BitInstallMode;
Name: "vst3_32"; Description: "32-bit VST3 Plugin (.vst3)"; Types: full custom;
Name: "vst3_64"; Description: "64-bit VST3 Plugin (.vst3)"; Types: full custom; Check: Is64BitInstallMode;
;Name: "aax_32"; Description: "32-bit AAX Plugin (.aaxplugin)"; Types: full custom;
;Name: "aax_64"; Description: "64-bit AAX Plugin (.aaxplugin)"; Types: full custom; Check: Is64BitInstallMode;
Name: "manual"; Description: "User guide"; Types: full custom; Flags: fixed

[Dirs] 
;Name: "{cf32}\Avid\Audio\Plug-Ins\waveProcessor.aaxplugin\"; Attribs: readonly; Components:aax_32; 
;Name: "{cf64}\Avid\Audio\Plug-Ins\waveProcessor.aaxplugin\"; Attribs: readonly; Check: Is64BitInstallMode; Components:aax_64; 
Name: "{cf32}\VST3\waveProcessor.vst3\"; Attribs: readonly; Components:vst3_32; 
Name: "{cf64}\VST3\waveProcessor.vst3\"; Attribs: readonly; Check: Is64BitInstallMode; Components:vst3_64; 

[Files]
;Source: "..\build-win\waveProcessor_Win32.exe"; DestDir: "{app}"; Check: not Is64BitInstallMode; Components:app; Flags: ignoreversion;
;Source: "..\build-win\waveProcessor_x64.exe"; DestDir: "{app}"; Check: Is64BitInstallMode; Components:app; Flags: ignoreversion;

;Source: "..\build-win\waveProcessor_Win32.dll"; DestDir: {code:GetVST2Dir_32}; Check: not Is64BitInstallMode; Components:vst2_32; Flags: ignoreversion;
;Source: "..\build-win\waveProcessor_Win32.dll"; DestDir: {code:GetVST2Dir_32}; Check: Is64BitInstallMode; Components:vst2_32; Flags: ignoreversion;
;Source: "..\build-win\waveProcessor_x64.dll"; DestDir: {code:GetVST2Dir_64}; Check: Is64BitInstallMode; Components:vst2_64; Flags: ignoreversion;

Source: "..\build-win\waveProcessor.vst3\*.*"; Excludes: "\Contents\x86_64\*,*.pdb,*.exp,*.lib,*.ilk,*.ico,*.ini"; DestDir: "{cf32}\VST3\waveProcessor.vst3\"; Components:vst3_32; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\waveProcessor.vst3\Desktop.ini"; DestDir: "{cf32}\VST3\waveProcessor.vst3\"; Components:vst3_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\waveProcessor.vst3\PlugIn.ico"; DestDir: "{cf32}\VST3\waveProcessor.vst3\"; Components:vst3_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\build-win\waveProcessor.vst3\*.*"; Excludes: "\Contents\x86\*,*.pdb,*.exp,*.lib,*.ilk,*.ico,*.ini"; DestDir: "{cf64}\VST3\waveProcessor.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\waveProcessor.vst3\Desktop.ini"; DestDir: "{cf64}\VST3\waveProcessor.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\waveProcessor.vst3\PlugIn.ico"; DestDir: "{cf64}\VST3\waveProcessor.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

; Source: "..\build-win\aax\bin\waveProcessor.aaxplugin\*.*"; Excludes: "\Contents\x64\*,*.pdb,*.exp,*.lib,*.ilk,*.ico,*.ini"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\waveProcessor.aaxplugin\"; Components:aax_32; Flags: ignoreversion recursesubdirs;
; Source: "..\build-win\aax\bin\waveProcessor.aaxplugin\Desktop.ini"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\waveProcessor.aaxplugin\"; Components:aax_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
; Source: "..\build-win\aax\bin\waveProcessor.aaxplugin\PlugIn.ico"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\waveProcessor.aaxplugin\"; Components:aax_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

;Source: "..\build-win\waveProcessor.aaxplugin\*.*"; Excludes: "\Contents\Win32\*,*.pdb,*.exp,*.lib,*.ilk,*.ico,*.ini"; DestDir: "{cf64}\Avid\Audio\Plug-Ins\waveProcessor.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: ignoreversion recursesubdirs;
;Source: "..\build-win\waveProcessor.aaxplugin\Desktop.ini"; DestDir: "{cf64}\Avid\Audio\Plug-Ins\waveProcessor.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
;Source: "..\build-win\waveProcessor.aaxplugin\PlugIn.ico"; DestDir: "{cf64}\Avid\Audio\Plug-Ins\waveProcessor.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

;Source: "..\manual\waveProcessor manual.pdf"; DestDir: "{app}"
;Source: "changelog.txt"; DestDir: "{app}"
;Source: "readme-win.rtf"; DestDir: "{app}"; DestName: "readme.rtf"; Flags: isreadme

[Icons]
;Name: "{group}\waveProcessor"; Filename: "{app}\waveProcessor.exe"
;Name: "{group}\User guide"; Filename: "{app}\waveProcessor manual.pdf"
;Name: "{group}\Changelog"; Filename: "{app}\changelog.txt"
;Name: "{group}\readme"; Filename: "{app}\readme.rtf"
;Name: "{group}\Uninstall waveProcessor"; Filename: "{app}\unins000.exe"

[Code]
var
  OkToCopyLog : Boolean;



procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssDone then
    OkToCopyLog := True;
end;

procedure DeinitializeSetup();
begin
  if OkToCopyLog then
    FileCopy (ExpandConstant ('{log}'), ExpandConstant ('{app}\InstallationLogFile.log'), FALSE);
  RestartReplace (ExpandConstant ('{log}'), '');
end;

[UninstallDelete]
Type: files; Name: "{app}\InstallationLogFile.log"