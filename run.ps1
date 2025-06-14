$MACHINES = $env:AX_VIRT_LAYER_MACHINES -split ";"

# Get login credentials
$CREDENTIAL = get-credential -ErrorAction Stop

# Setup the driver on each machine
for ($i = 0; $i -lt $MACHINES.Length; $i++){
	$CURRENT = ($MACHINES[$i] -split "/")	

	$CLIENT = $CURRENT[0]
	$TARGET = $CURRENT[1]
	echo "Setting up client at: $CLIENT with target: $TARGET"

	try{
		invoke-command -computername $CLIENT -scriptblock {ipconfig} -credential $CREDENTIAL -ErrorAction Stop
	}
	catch{
		write-host "Command failed to run on client machine. Make sure the client has enabled remoting (enable-psremoting) as well as correct winrm config and try again." -foregroundcolor red
	}
}
