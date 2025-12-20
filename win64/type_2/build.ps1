# User mode not supported
if ($km){
	return
}

$files_c = $(gci "$PSScriptRoot\src" -file -r -filter "*.c")
$files_h = @("$PSScriptRoot\src", "D:/ax_project/ax_virt_layer/win64/type_2/src/mte")
$files_lib = @("C:\msys64\ucrt64\x86_64-w64-mingw32\lib\")

$build_path = "$PSScriptRoot\build\"
$output_exe = "$PSScriptRoot\build\ax_virt_core.exe"

$files_o = @()

foreach ($src in $files_c){
	$out = $build_path+$($src.Name -replace "\.[^.]+$")+".obj"
	MSG -msg "Compiling file: $($src.FullName)" -color Yellow -opt

	& $CC `
		$PREF_SRC $($src.FullName) `
		($files_h | foreach {$PREF_INC+$_}) `
		($GLOB_INC | foreach {$PREF_INC+$_}) `
		($GLOB_DEF | foreach {$PREF_DEF+$_}) `
		($GLOB_FLAG | foreach {$PREF_BASE+$_}) `
		($PREF_INC+$LIB_HEADERS) `
		$PREF_OUT $out

	if ($lastexitcode -ne 0){
		MSG -msg "Compilation failed with code: $lastexitcode" -color Red
		return 1
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
	-lax_utility_lib `
	-lpthread `
	$PREF_OUT $output_exe

if ($lastexitcode -ne 0){
	MSG -msg "Virtualizer core build failed with code: $lastexitcode" -color Red
	return 1
}
MSG -msg "Virtualizer core created at: ${output_sys}" -color Blue

