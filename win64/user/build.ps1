# User mode not supported
if ($km){
	return
}

$files_c = @(gci "$PSScriptRoot\src" -file -r -filter "*.c")

# Directory 'hooks'
$files_h = @(
	"$PSScriptRoot\src",
	"D:/ax_project/ax_virt_layer/win64/user/src",
	"D:/ax_project/ax_virt_layer/win64/user/src/mte/asm"
)

$files_lib = @("C:\msys64\ucrt64\x86_64-w64-mingw32\lib\")

$build_path = "$PSScriptRoot\build\"
$timestamp_txt = "$build_path\timestamp.txt"
$output_exe = "$build_path\ax_virt_core.exe"

if (test-path $timestamp_txt -pathtype Leaf){
	$last_date = $(get-item $timestamp_txt).LastWriteTime
}else{
	$last_date = [DateTime]::MinValue
}

$files_o = @(gci $build_path -file -r -filter "*.obj")

foreach ($src in $files_c){
	$out = $build_path+$($src.Name -replace "\.[^.]+$")+".obj"
	MSG -msg "Compiling file: $($src.FullName)" -color DarkYellow -opt

	$exists = $(test-path $out -pathtype Leaf)
	$src_date = $src.LastWriteTime

	if ($exists -and $last_date -ge $src_date){
		MSG -msg "Skipping file: $($src.FullName)" -color DarkGray -opt
		continue
	}

	& $CC `
		$PREF_SRC $($src.FullName) `
		($files_h | foreach {$PREF_INC+$_}) `
		($GLOB_INC | foreach {$PREF_INC+$_}) `
		($GLOB_DEF | foreach {$PREF_DEF+$_}) `
		($GLOB_FLAG | foreach {$PREF_BASE+$_}) `
		($PREF_INC+$LIB_HEADERS) `
		-save-temps `
		-O3 `
		$PREF_OUT $out

	if ($lastexitcode -ne 0){
		MSG -msg "Compilation failed with code: $lastexitcode" -color Red
		return 1
	}

	if ($exists){
		$files_o = $files_o | where {"$_.DirectoryName\$_.BaseName" -ne "$out.DirectoryName\$out.BaseName"}
	}
	$files_o += $out

	MSG -msg "Successfully compiled to: $out" -color Green -opt
}

& $CC `
	($files_h | foreach {$PREF_INC+$_}) `
	($GLOB_INC | foreach {$PREF_INC+$_}) `
	($GLOB_DEF | foreach {$PREF_DEF+$_}) `
	($GLOB_FLAG | foreach {$PREF_BASE+$_}) `
	$files_o `
	$PREF_LIB $LIB_BUILD `
	$PREF_LIB $files_lib `
	"-lax_utility_lib" `
	"-lpthread" `
	"-Wl,-pdb=$output_exe.pdb" `
	$PREF_OUT $output_exe

if ($lastexitcode -ne 0){
	MSG -msg "Virtualizer core build failed with code: $lastexitcode" -color Red
	return 1
}
MSG -msg "Virtualizer core created at: ${output_sys}" -color Blue

new-item `
	-path $build_path `
	-name "timestamp.txt" `
	-itemtype "File" `
	-value "$(get-date -uformat "%d/%m/%Y:%T")" `
	-force | out-null

