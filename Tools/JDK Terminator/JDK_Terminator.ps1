# Set JDK path
$JDKPath = "F:\DEV\_Runtime\AndroidStudio\jre"

# Process
$EXEPath = $JDKPath + "\bin\java.exe"
# Write-Output $EXEPath

# Select-Object Name, CommandLine, ExecutablePath, Path

$strComputer = "."
$colProcesses = Get-WmiObject -Class Win32_Process -ComputerName $strComputer | Where-Object { $_.name -eq 'java.exe' }

foreach ($objProcess in $colProcesses) {
    if ($objProcess.ExecutablePath -eq $EXEPath) {
        Write-Output $objProcess.ExecutablePath
        $objProcess.Terminate()        
        Exit
    }
}

Write-Output "No Process Found"
