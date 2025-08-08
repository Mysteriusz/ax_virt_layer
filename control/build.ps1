$FILE_C = gci "$(Get-Location)\src" -recurse -file -filter "*.c"
$FILE_H = gci "$(Get-Location)\src" -recurse -file -filter "*.h"

$INCLUDE = @(
	$("/I"+$env:AX_VIRT_LAYER_INCLUDE)
	$($FILE_H | select -expandproperty DirectoryName | foreach {"/I"+$_})
	$("/I"+$env:AX_VIRT_H)
)

$LIBS = @(
	$env:LIBS_UM -split ';'
	$env:AX_VIRT_LIB
)

$DEFINE = @("/DUNICODE", "/DAX_WINDOWS", "/DAX_UM")

$OUTPUT_DIR = "$(Get-Location)\build"
$OUTPUT_BIN = "ax_virt_control.exe"

$FILE_O = @()
foreach ($FILE in $FILE_C) {
    $O = Join-Path $OUTPUT_DIR ($FILE.BaseName + ".obj")	
    cl $DEFINE $INCLUDE /c($FILE.FullName) /Fo:$O
    $FILE_O += $O
}

cl $LIBS $INCLUDE $FILE_O /Fe:"$OUTPUT_DIR\$OUTPUT_BIN" /std:c17 /Od /link /subsystem:windows /entry:wmain

