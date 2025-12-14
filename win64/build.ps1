# User mode not supported
if ($um){
	return
}

$files_c = $(gci "$PSScriptRoot\src" -file -r -filter "*.c")
$files_h = $(gci "$PSScriptRoot\src" -file -r -filter "*.h" | foreach {"/I"+$_.Directory} | get-unique)

$deps = @($LIB_HEADERS, "C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\km\full\")
$deps_libs = @($("$LIB_BUILD"+"$LIB_BIN"), "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\km\x64\ntoskrnl.lib")
$build_path = "$PSScriptRoot\build\"
$output_sys = "$PSScriptRoot\build\ax_virt_layer.sys"

$files_o = @()

foreach ($src in $files_c){
	$out = $build_path+$($src.Name -replace "\.[^.]+$")+".obj"
	MSG -msg "Compiling file: $($src.FullName)" -color Yellow -opt

	cl `
		/c $src.FullName `
		($GLOB_DEF | foreach {$PREF_DEF+$_}) `
		$files_h `
		($deps | foreach {"/I"+$_}) `
		/Fo$out

	if ($lastexitcode -ne 0){
		MSG -msg "Compilation failed with code: $lastexitcode" -color Red
		return 1
	}

	$files_o += $out

	MSG -msg "Successfully compiled to: $out" -color Green -opt
}

cl `
	$files_o `
	/link `
	$deps_libs `
	/ENTRY:DriverEntry `
	/DRIVER `
	/SUBSYSTEM:NATIVE `
	/NODEFAULTLIB `
	/OUT:$output_sys
if ($lastexitcode -ne 0){
	MSG -msg "Driver build failed with code: $lastexitcode" -color Red
	return 1
}
MSG -msg "Driver binary created at: ${output_sys}" -color Blue

# TEMPORARY REMOTE FILE SHARING

rem -ip "192.168.0.241" -cmd "sc.exe stop AX_VIRT_LAYER" | out-null
rem -ip "192.168.0.241" -cmd "cpi -path '\\192.168.0.122\ax_build\ax_virt_layer.sys' -destination 'C:\ax_virt_layer.sys' -force" | out-null
rem -ip "192.168.0.241" -cmd "sc.exe start AX_VIRT_LAYER" | out-null
rem -ip "192.168.0.241" -cmd "powershell 'start-process C:\users\wixxx\desktop\ax_emulator.exe -verb runas'" | out-null

