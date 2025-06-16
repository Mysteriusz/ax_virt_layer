param(
	[string[]]$machines,
	[bool]$build,
	[bool]$status,
	[bool]$reboot
)

$global:reboot = $reboot
$global:status = $status
$global:build = $build

$MACHINES_ALL = $env:AX_VIRT_LAYER_MACHINES -split ";"

if ($machines -eq $null){
	$MACHINES_REQ = $MACHINES_ALL
}
else{
	$MACHINES_REQ = $MACHINES_ALL | where-object { $machines -contains $_ }
}

# Get login credentials
$CREDENTIAL = get-credential -erroraction stop

class MACHINE{
	[System.Management.Automation.Runspaces.PSSession]$session
	[string]$name
	[string]$system
	[string]$path
}

$global:CURRENT = $null
$global:DRIVER_NAME = "ax_virt_layer.sys"
$global:CERT_NAME = "ax_virt_layer.pfx"

function Approve(){
	param(
		[string]$message,
		[ConsoleColor]$messageColor
	)	

	if ($message -ne $null){
		write-host $message -foregroundcolor $messageColor
	}

	write-host ""
	write-host "Press " -nonewline
	write-host "[Y]" -foregroundcolor yellow -nonewline
	write-host " to Accept or " -nonewline
	write-host "[N]"-foregroundcolor yellow -nonewline
	write-host " to Decline" -nonewline
	write-host ""

	do{
		$key = [System.Console]::ReadKey($true).KeyChar
	} while($key -ne 'y' -and $key -ne 'n')

	if ($key -eq 'y'){
		return 1
	}
	else{
		return 0
	}
}

# Read machine string into local variables
function SetupMachine(){
	param(
		[string]$machineString
	)

	if ($machineString -eq $null) {
		return
	}

	$global:CURRENT = [MACHINE]::new()

	$splitted = $machineString -split '\|'
	
	$global:CURRENT.name = $splitted[0]
	$global:CURRENT.system = $splitted[1]
	$global:CURRENT.path = $splitted[2]
	echo "Setting up client at: $($CURRENT.name) with target: $($CURRENT.system)"
	
	# Create a session with target machine
	try{
		$global:CURRENT.session = new-pssession -computername $CURRENT.name -credential $CREDENTIAL -erroraction stop
	}
	catch{
		write-host "Unable to establish connection with client $($CURRENT.name)" -foregroundcolor red
		$global:CURRENT = $null
	}

	return 
}

# Windows client reboot function
function WindowsReboot{
	param(
		[MACHINE]$machine
	)

	invoke-command -session $machine.session -erroraction stop -scriptblock { 
		restart-computer -force | out-null
	}
	write-host "Restarting $($machine.name)." -foregroundcolor yellow
}
# Windows client driver injection function
function WindowsDriverInjection{
	param(
		[MACHINE]$machine
	)

	echo "Removing a driver service"
	# Try to delete the driver
	invoke-command -session $machine.session -erroraction stop -scriptblock {
		param(
			$serviceName
		) 

		sc.exe delete $serviceName | out-null
	} -argumentlist $env:AX_VIRT_LAYER_NAME

	echo "Creating a driver service"
	# Try to create the driver
	invoke-command -session $machine.session -erroraction stop -scriptblock { 
		param(
			$serviceName, 
			$driverPath
		) 
		
		sc.exe create $serviceName binPath= $driverPath type= kernel start= auto | out-null
	} -argumentlist $env:AX_VIRT_LAYER_NAME, "$($machine.path)\$global:DRIVER_NAME"

	write-host "Installed driver from path: $($machine.path)\$global:DRIVER_NAME" -foregroundcolor green
}
# Windows client driver status check function
function WindowsDriverStatus{
	param(
		[MACHINE]$machine
	)

	invoke-command -session $machine.session -erroraction stop -scriptblock {
		param(
			[string]$serviceName
		)

		sc.exe queryex $serviceName 
	
	} -argumentlist $env:AX_VIRT_LAYER_NAME
}

# Setup the driver on provided machines
function BuildMachines{
	param(
		[string[]]$machines
	)
	
	for ($i = 0; $i -lt $machines.Length; $i++){
		SetupMachine -machineString $machines[$i]
		
		if ($CURRENT -eq $null){
			continue
		}

		$cert = get-childitem "Cert:\CurrentUser\My" | where-object { $_.Subject -eq "CN=Test ax_virt_layer Cert for $($CURRENT.name)" }

		if ($cert -eq $null){
			# Create a certificate and sign the driver
			$cert = new-selfsignedcertificate -type CodeSigningCert -subject "CN=Test ax_virt_layer Cert for $($CURRENT.name)" -keyexportpolicy Exportable -keyspec Signature -certstorelocation "Cert:\CurrentUser\My"
		}

		# Export the certificate
		export-pfxcertificate -cert $cert -filepath "$env:AX_VIRT_LAYER_BUILD_DIR\$global:CERT_NAME" -password $CREDENTIAL.Password | out-null
		
		# Sign the driver
		signtool sign /fd SHA256 /a /f "$env:AX_VIRT_LAYER_BUILD_DIR\$global:CERT_NAME" /p $(read-host "certificate password (same as credential)") "$env:AX_VIRT_LAYER_BUILD_DIR\$global:DRIVER_NAME" | out-null

		# Copy driver to client destination path
		try{
			copy-item -path "$env:AX_VIRT_LAYER_BUILD_DIR\$global:DRIVER_NAME" -destination "$($CURRENT.path)" -tosession $CURRENT.session -erroraction stop
			write-host "Driver copied to $($CURRENT.path)\$global:DRIVER_NAME" -foregroundcolor green
		}
		catch{
			# If exception is ERROR_SHARING_VIOLATION
			if ($_.Exception.HResult -eq -2146233087){
				$rr = Approve -message "The driver is in use. Restart machine to delete the driver service?" -messageColor yellow
				
				if ($rr){
					# Mark driver as to-remove
					invoke-command -session $CURRENT.session -erroraction stop -scriptblock {
						param(
							$serviceName
						) 

						sc.exe delete $serviceName | out-null
					} -argumentlist $env:AX_VIRT_LAYER_NAME
					WindowsReboot -machine $CURRENT

					write-host "After the machine restarts re-run the build command." -foregroundcolor yellow
					continue
				}
			}
			
			write-host "Driver copying failed. Make sure the client has enabled remoting (enable-psremoting) and Powershell version is 5+. Also check the AX_VIRT_LAYER_BUILD_DIR environment variable." -foregroundcolor red
			remove-pssession $CURRENT.session
			continue
		}

		# Copy certificate to client destination path
		try{
			copy-item -path "$env:AX_VIRT_LAYER_BUILD_DIR\$global:CERT_NAME" -destination "$($CURRENT.path)" -tosession $CURRENT.session -erroraction stop
			write-host "Certificate copied to $($CURRENT.path)\$global:CERT_NAME" -foregroundcolor green
		}
		catch{
			write-host "Certificate copying failed. Make sure the client has enabled remoting (enable-psremoting) and Powershell version is 5+. Also check the AX_VIRT_LAYER_BUILD_DIR environment variable." -foregroundcolor red
			remove-pssession $CURRENT.session
			continue
		}

		# Import the certificate on target device
		invoke-command -session $CURRENT.session -erroraction stop -scriptblock {
			param(
				[string]$certPath,
				[System.Security.SecureString]$certPassword
			)

			$cert = get-childitem "Cert:\LocalMachine\My" | where-object { $_.Subject -eq "CN=Test ax_virt_layer Cert for $($CURRENT.name)" }
			
			if ($cert -eq $null){
				import-pfxcertificate -filepath $certPath -certstorelocation "Cert:\LocalMachine\My" -password $certPassword | out-null
			}
		} -argumentlist "$($CURRENT.path)\$global:CERT_NAME", $CREDENTIAL.Password

		write-host "Certificate successfully imported on client." -foregroundcolor green


		if ($CURRENT.system -in @("win11", "win10", "win7")){
			WindowsDriverInjection -machine $CURRENT
		}
		elseif ($CURRENT.system -eq "linux" ){
			# TODO: Linux driver injection support	
		}
		elseif ($CURRENT.system -eq "custom" -and $CustomDriverInjection -ne $null){
			CustomDriverInjection @CustomDriverInjectionArgs
		}
		else {
			write-host "Invalid system argument passed for $($CURRENT.name) machine, aborting." -foregroundcolor red
			remove-pssession $CURRENT.session
			continue
		}
	
		# Reboot if requested
		if ($global:reboot){
			WindowsReboot -machine $CURRENT
		}

		# Delete the session with target machine
		remove-pssession $CURRENT.session
	}
}
# Check the driver on provided machines
function CheckMachines{
	param(
		[string[]]$machines
	)

	for ($i = 0; $i -lt $machines.Length; $i++){
		SetupMachine -machineString $machines[$i]
			
		if ($CURRENT -eq $null){
			continue
		}

		if ($CURRENT.system -in @("win11", "win10", "win7")){
			WindowsDriverStatus -machine $CURRENT
		}
		elseif ($CURRENT.system -eq "linux" ){
			# TODO: Linux driver injection support	
		}
		elseif ($CURRENT.system -eq "custom" -and $CustomDriverStatus -ne $null){
			CustomDriverStatus @CustomDriverStatusArgs
		}
		else {
			write-host "Invalid system argument passed for $($CURRENT.name) machine, aborting." -foregroundcolor red
			remove-pssession $CURRENT.session
			continue
		}

		# Reboot if requested
		if ($global:reboot){
			WindowsReboot -machine $CURRENT
		}

		# Delete the session with target machine
		remove-pssession $CURRENT.session
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
