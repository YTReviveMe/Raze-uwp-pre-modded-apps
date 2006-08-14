; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "EDuke32"
!define PRODUCT_VERSION "1.4.0 beta 2"
!define PRODUCT_PUBLISHER "EDuke32 Team"
!define PRODUCT_WEB_SITE "http://www.eduke32.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

SetCompressor /SOLID lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\box-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\box-uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"

; Welcome page
!define MUI_WELCOMEPAGE_TEXT "$(MUI_WELCOMEPAGE_TEXT)"
LangString MUI_WELCOMEPAGE_TEXT {LANG_ENGLSH} "This wizard will guide you through the installation of ${PRODUCT_NAME} ${PRODUCT_VERSION}\n\nClick next to continue."
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "..\GNU.TXT"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!define MUI_DIRECTORYPAGE_TEXT_TOP "$(MUI_DIRECTORYPAGE_TEXT_TOP)"
LangString MUI_DIRECTORYPAGE_TEXT_TOP {LANG_ENGLSH} "Please select your Duke Nukem 3D directory."
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "EDuke32"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\ChangeLog.html"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "eduke32-${PRODUCT_VERSION}.exe"
InstallDir ""
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "!Game" SEC_GAME
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\eduke32.exe"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\EDuke32.lnk" "$INSTDIR\eduke32.exe"
  File "..\GNU.TXT"
  File "..\buildlic.txt"
  File "..\ChangeLog.html"
  File "..\ChangeLog"
  File "..\setup.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Setup.lnk" "$INSTDIR\setup.exe"
SectionEnd

Section "Level editor" SEC_EDITOR
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\mapster32.exe"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Mapster32.lnk" "$INSTDIR\mapster32.exe"
SectionEnd

Section /o "Dukester X 1.5 support" SEC_DX
  SetOutPath "$INSTDIR"
  SetOverwrite ifdiff
  File "..\duke3d_w32.exe"
SectionEnd

Section /o "Samples" SEC_SAMPLES
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\duke3d.def.sample"
  File "..\enhance.con.sample"
  SetOutPath "$INSTDIR"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\EDuke32 Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\eduke32.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\eduke32.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "${PRODUCT_STARTMENU_REGVAL}" "$ICONS_GROUP"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_GAME} "The main EDuke32 game components (required to play EDuke enhanced mods)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_EDITOR} "The enhanced Mapster32 editor (optional)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_DX} "Support for Dukester X 1.5 (optional)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_SAMPLES} "Sample enhancement definition files (optional)"
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "EDuke32 was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove EDuke32 and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  ReadRegStr $ICONS_GROUP ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "${PRODUCT_STARTMENU_REGVAL}"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\duke3d.def.sample"
  Delete "$INSTDIR\enhance.con.sample"
  Delete "$INSTDIR\mapster32.exe"
  Delete "$INSTDIR\setup.exe"
  Delete "$INSTDIR\ChangeLog.html"
  Delete "$INSTDIR\ChangeLog"
  Delete "$INSTDIR\GNU.TXT"
  Delete "$INSTDIR\buildlic.txt"
  Delete "$INSTDIR\eduke32.exe"
  Delete "$INSTDIR\duke3d_w32.exe"
;  Delete "$INSTDIR\datainst.exe"
  
;  Delete "$INSTDIR\duke3d.grp"
;  Delete "$INSTDIR\duke3d.cfg"

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\EDuke32 Website.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Mapster32.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Setup.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\EDuke32.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Game Data Installer.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
