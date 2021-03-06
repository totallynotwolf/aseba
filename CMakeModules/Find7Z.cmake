INCLUDE(FindPackageHandleStandardArgs)

# 7zr(.exe) only supports 7z archives, while 7z(.exe) and 7za(.exe)
# additionally support many other formats (eg zip)
find_program(SEVENZIP_BIN
	NAMES 7z 7za
	HINTS "${MINGWDIR}" "${MINGWLIBS}/bin" "$ENV{ProgramFiles(x86)}/7-zip" "$ENV{ProgramFiles}/7-zip" "$ENV{ProgramW6432}/7-zip"
	PATH_SUFFIXES bin
	DOC "7zip executable"
	)
# handle the QUIETLY and REQUIRED arguments and set SEVENZIP_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SevenZip DEFAULT_MSG SEVENZIP_BIN)

MARK_AS_ADVANCED(SEVENZIP_BIN)
