# Set JDK path
# $JDKPath = "F:\DEV\_Runtime\AndroidStudio\jre"

# https://docs.microsoft.com/zh-cn/powershell/scripting/samples/working-with-registry-entries?view=powershell-7.2
$CFReg = Get-ItemProperty -Path Registry::"HKEY_CURRENT_USER\Software\Clickteam\Fusion Developer 2.5\General"

$JDKPath = $CFReg.JDKDir + "\bin\java.exe"
$H5JDKPath = $CFReg.Html5JavaDir + "\bin\java.exe"

# Process
# https://docs.microsoft.com/zh-cn/powershell/module/microsoft.powershell.core/about/about_arrays?view=powershell-7.2
# https://docs.microsoft.com/zh-cn/powershell/scripting/learn/deep-dives/everything-about-arrays?view=powershell-7.2
$EXEPath = $JDKPath , $H5JDKPath

# Write-Output $EXEPath

# Select-Object Name, CommandLine, ExecutablePath, Path

$strComputer = "."
$colProcesses = Get-WmiObject -Class Win32_Process -ComputerName $strComputer | Where-Object { $_.name -eq 'java.exe' }

$foundProcess = 0

foreach ($Path in $EXEPath) {
    foreach ($objProcess in $colProcesses) {
        if ($objProcess.ExecutablePath -eq $Path) {
            Write-Output $objProcess.ExecutablePath
            $objProcess.Terminate()

            $foundProcess = 1

            break
        }
    }
}

if ($foundProcess -eq 0) {
    Write-Output "No Process Found"
}
