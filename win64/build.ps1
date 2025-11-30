$files_c = $(gci "$PSScriptRoot\src" -file -r -filter "*.c")
$files_h = $(gci "$PSScriptRoot\src" -file -r -filter "*.h")

$deps = @("$PSScriptRoot\deps\", "C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\km\full\")
$deps_libs = @("$PSScriptRoot\deps\ax_utility_lib.lib", "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\km\x64\ntoskrnl.lib")
$build_path = "$PSScriptRoot\build\"
$output_sys = "$PSScriptRoot\build\ax_virt_layer.sys"

$files_o = @()

foreach ($file in $files_c){
	$out = $build_path+$($file.Name -replace "\.[^.]+$")+".obj"

	cl `
	/c $file.FullName `
	/DAX_WIN64=1 `
	/DAX_KM=1 `
	($files_h | foreach {"/I"+$_}) `
	($deps | foreach {"/I"+$_}) `
	/Fo$out

	$files_o += $out
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

