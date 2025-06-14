param(
	[string[]]$machines,
	[bool]$build,
	[bool]$status,
	[bool]$reboot
)

$MACHINES_ALL = $env:AX_VIRT_LAYER_MACHINES -split ";"
$MACHINES_REQ = $MACHINES_ALL | where-object { $machines -contains $_ }

# Get login credentials
$CREDENTIAL = get-credential -erroraction stop

$SESSION = $null
$CLIENT = $null
$SYSTEM = $null
$CLIENT_PATH = $null
$CURRENT = $null

# Read machine string into local variables
function SetupMachine(){
	param(
		[string]$machineString
	)

	$global:CURRENT = ($machineString -split "/")	
	
	$global:CLIENT = $global:CURRENT[0]
	$global:SYSTEM = $global:CURRENT[1]
	$global:CLIENT_PATH = $global:CURRENT[2]
	echo "Setting up client at: $global:CLIENT with target: $global:SYSTEM"
	
	# Create a session with target machine
	try{
		$global:SESSION = new-pssession -computername $global:CLIENT -credential $CREDENTIAL -erroraction stop
	}
	catch{
		write-host "Unable to establish connection with client $global:CLIENT" -foregroundcolor red
	}

	return 
}

# Windows client driver injection function
function WindowsDriverInjection{
	param(
		[System.Management.Automation.Runspaces.PSSession]$client,
		[string]$path,
		[bool]$reboot
	)

	echo "Removing a driver service"
	# Try to delete the driver
	invoke-command -session $client -erroraction stop -scriptblock {
		param($serviceName) 

		sc.exe delete $serviceName
	} -argumentlist $env:AX_VIRT_LAYER_NAME

	echo "Creating a driver service"
	# Try to create the driver
	invoke-command -session $client -erroraction stop -scriptblock { 
		param($serviceName, $driverPath) 
		
		sc.exe create $serviceName binPath= $driverPath type= kernel start= auto
	} -argumentlist $env:AX_VIRT_LAYER_NAME, $path

	if ($reboot){
		invoke-command  -session $client -erroraction stop -scriptblock { restart-computer -force }
	}

	return
}
# Windows client driver status check function
function WindowsDriverStatus{
	param(
		[System.Management.Automation.Runspaces.PSSession]$client
	)

	invoke-command -session $client -erroraction stop -scriptblock {
		param(
			[string]$serviceName
		)
		sc.exe queryex $serviceName
	
	} -argumentlist $env:AX_VIRT_LAYER_NAME
	
	return
}

# Setup the driver on provided machines
function BuildMachines{
	param(
		[string[]]$machines
	)
	
	for ($i = 0; $i -lt $machines.Length; $i++){
		SetupMachine -machineString $machines[$i]
	
		# Copy driver to client destination path
		try{
			copy-item -path "$env:AX_VIRT_LAYER_BUILD_DIR\ax_virt_layer.sys" -destination "$global:CLIENT_PATH" -tosession $global:SESSION -erroraction stop
			write-host "Driver copied to $global:CLIENT_PATH\ax_virt_layer.sys" -foregroundcolor green
		}
		catch{
			write-host "Driver copying failed. Make sure the client has enabled remoting (enable-psremoting) and Powershell version is 5+." -foregroundcolor red
			remove-pssession $global:SESSION
			continue
		}
	
		if ($global:SYSTEM -in @("win11", "win10")){
			WindowsDriverInjection -client $global:SESSION -path "$global:CLIENT_PATH\ax_virt_layer.sys" -reboot $reboot
		}
		elseif ($global:SYSTEM -eq "linux" ){
			# TODO: Linux driver injection support	
		}
		elseif ($global:SYSTEM -eq "custom" -and $CustomDriverInjection -ne $null){
			CustomDriverInjection @CustomDriverInjectionArgs
		}
		else {
			write-host "Invalid system argument passed for $global:CLIENT machine, aborting." -foregroundcolor red
			remove-pssession $global:SESSION
			continue
		}
	
		# Delete the session with target machine
		remove-pssession $global:SESSION
	}
}
# Check the driver on provided machines
function CheckMachines{
	param(
		[string[]]$machines
	)

	for ($i = 0; $i -lt $machines.Length; $i++){
		SetupMachine -machineString $machines[$i]

		if ($global:SYSTEM -in @("win11", "win10")){
			WindowsDriverStatus -client $global:SESSION -path "$global:CLIENT_PATH\ax_virt_layer.sys":
		}
		elseif ($global:SYSTEM -eq "linux" ){
			# TODO: Linux driver injection support	
		}
		elseif ($global:SYSTEM -eq "custom" -and $CustomDriverStatus -ne $null){
			CustomDriverStatus @CustomDriverStatusArgs
		}
		else {
			write-host "Invalid system argument passed for $CLIENT machine, aborting." -foregroundcolor red
			remove-pssession $SESSION
			continue
		}
	}
}

# Setup the driver on each requested machine
if ($build){
	BuildMachines -machines $MACHINES_REQ
}
elseif ($status){
	CheckMachines -machines $MACHINES_REQ
}
else{
	write-host "Unknown action argument, aborting." -foregroundcolor red
}
