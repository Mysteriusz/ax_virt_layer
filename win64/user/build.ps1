param(
	[DateTime]$last_date
)

# User mode not supported
if ($km){
	return
}

$files_c = @(gci "$PSScriptRoot\src" -file -r -filter "*.c")

# Directory 'hooks'
$dirs_h = @(
	"D:/ax_project/ax_virt_layer/win64/user/src",
	"D:/ax_project/ax_virt_layer/win64/user/src/mte/asm"
)

$files_lib = @("C:\msys64\ucrt64\x86_64-w64-mingw32\lib\")

$build_path = "$PSScriptRoot\build\"
$output_exe = "$build_path\ax_virt_core.exe"

$files_o = @(gci $build_path -file -r -filter "*.o")

$files_h_update = @()
foreach ($hdr in $(gci $dirs_h -r -file -filter "*.h" | sort Name -unique)){
	$out = [IO.FileInfo]$($build_path+"headers\$($hdr.BaseName)$($hdr.Extension)")

	if ($last_date -gt $hdr.LastWriteTime -and $(test-path $out.FullName -pathtype Leaf)){
		MSG -msg "Skipping header: $($hdr.FullName)" -color DarkGray -opt
		continue
	}
	MSG -msg "Header updated: $($hdr.FullName)" -color Yellow -opt
	$files_h_update += $hdr
}

foreach ($src in $files_c){
	$out = $build_path+$($src.Name -replace "\.[^.]+$")+".o"
	MSG -msg "Compiling file: $($src.FullName)" -color DarkYellow -opt

	if ($files_h_update.Count -gt 0){
		$src_h_deps = @()
		# Format includes from $src into array of files
		$src_h_deps = [IO.FileInfo[]]@($(& $CC -MM `
			$($src.FullName) `
			($dirs_h | foreach {$PREF_INC+$_}) `
			($GLOB_INC | foreach {$PREF_INC+$_}) `
			($GLOB_DEF | foreach {$PREF_DEF+$_}) `
			($GLOB_FLAG | foreach {$PREF_BASE+$_}) `
			2>&1) | select -skip 2 | foreach {$_.Trim(" \")})
	
		$src_h_deps = @($src_h_deps | sort Name -unique)

		$src_h_deps = [IO.FileInfo[]]@($src_h_deps + $files_h_update) | group -property Name | where {$_.Count -gt 1}
	}

	if ($src_h_deps.Count -gt 0){
		#echo $src_h_update
		MSG -msg "Headers updated: $($src_h_deps | select -expandproperty Name)" -color Yellow -opt
		#MSG -msg "Headers updated: $($src_h_update.Count)" -color Yellow -opt
	}
	elseif (TIMESTAMP_CHECK -file $src -stamp $last_date -out $out){
		MSG -msg "Skipping file: $($src.FullName)" -color DarkGray -opt
		continue
	}

	# Compile either cause included headers were updated or source file was updated
	& $CC `
		$PREF_SRC $($src.FullName) `
		($dirs_h | foreach {$PREF_INC+$_}) `
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

	if (test-path $out -pathtype Leaf){
		$files_o = @($files_o | where {"$_.DirectoryName\$_.BaseName" -ne "$out.DirectoryName\$out.BaseName"})
	}
	$files_o += $out

	MSG -msg "Successfully compiled to: $out" -color Green -opt
}

& $CC `
	($dirs_h | foreach {$PREF_INC+$_}) `
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

if ($files_h_update.Count -gt 0){
	cpi -path $files_h_update -destination "${build_path}headers"
}

