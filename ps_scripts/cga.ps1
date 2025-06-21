# Run this script as Administrator

$guestUsername = "GuestUser"

# 1. Create local user
net user $guestUsername /add /active:yes /passwordchg:no /passwordreq:no

# 2. Remove from 'Users', add to 'Guests'
net localgroup Users $guestUsername /delete
net localgroup Guests $guestUsername /add

# 3. Disable password changes
wmic useraccount where name="$guestUsername" set PasswordChangeable=FALSE

# 4. Apply Group Policy: Restrict access to drives
# This sets "Prevent access to drives from My Computer"
# Registry key: HKCU\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer
# Value: NoDrives (DWORD) and NoViewOnDrive (DWORD)
# Value 4 = Hide C: only. You can combine letters using a bitmask:
# A=1, B=2, C=4, D=8, E=16, F=32, etc. (So A+C = 5, C+D = 12, etc.)

$regPath = "HKU"

# Load user hive to apply restriction to the guest user
$guestProfilePath = (Get-CimInstance Win32_UserProfile | Where-Object { $_.LocalPath -like "*\$guestUsername" }).LocalPath
if (-not $guestProfilePath) {
    Write-Host "⚠️ Log in with the '$guestUsername' account once so the profile is created, then re-run this script."
    return
}

# Get the SID of the user
$guestSID = (Get-LocalUser $guestUsername).SID.Value
$regHivePath = "$env:TEMP\${guestUsername}.dat"

# Load the guest's registry hive
reg load "HKU\$guestSID" "$guestProfilePath\NTUSER.DAT"

# Apply the drive restriction (e.g., hide C: = 4, D: = 8, C+D = 12, etc.)
$bitmask = 4 + 8  # C: + D:
New-Item -Path "Registry::HKU\$guestSID\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer" -Force | Out-Null
Set-ItemProperty -Path "Registry::HKU\$guestSID\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer" -Name "NoDrives" -Value $bitmask -Type DWord
Set-ItemProperty -Path "Registry::HKU\$guestSID\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer" -Name "NoViewOnDrive" -Value $bitmask -Type DWord

# Unload the guest user's hive
reg unload "HKU\$guestSID"

Write-Host "✅ Guest account '$guestUsername' created and drive access restricted."

