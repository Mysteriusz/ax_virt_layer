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

$MACHINES = $env:AX_VIRT_LAYER_MACHINES -split ";"

# Get login credentials
$CREDENTIAL = get-credential -erroraction stop

# Setup the driver on each machine
for ($i = 0; $i -lt $MACHINES.Length; $i++){
	$CURRENT = ($MACHINES[$i] -split "/")	

	$CLIENT = $CURRENT[0]
	$SYSTEM = $CURRENT[1]
	$CLIENT_PATH = $CURRENT[2]
	echo "Setting up client at: $CLIENT with target: $SYSTEM"

	# Create a session with target machine
	try{
		$SESSION = new-pssession -computername $CLIENT -credential $CREDENTIAL -erroraction stop
	}
	catch{
		write-host "Unable to establish connection with client $CLIENT" -foregroundcolor red
		continue
	}

	# Copy driver to client destination path
	try{
		copy-item -path "$env:AX_VIRT_LAYER_BUILD_DIR\ax_virt_layer.sys" -destination "$CLIENT_PATH" -tosession $SESSION -erroraction stop
		write-host "Driver copied to $CLIENT_PATH\ax_virt_layer.sys" -foregroundcolor green
	}
	catch{
		write-host "Driver copying failed. Make sure the client has enabled remoting (enable-psremoting) and Powershell version is 5+." -foregroundcolor red
	}

	if ($SYSTEM -eq "win11"){
		WindowsDriverInjection -client $SESSION -path "$CLIENT_PATH\ax_virt_layer.sys" -reboot 1
	}

	# Delete the session with target machine
	remove-pssession $SESSION
}
