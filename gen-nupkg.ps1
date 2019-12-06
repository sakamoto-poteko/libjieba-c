param([string]$current_ref)

[string]$version
$version = (git describe --tags HEAD) | Out-String
$version = $version.Trim().Remove(0, 1)

((Get-Content -path libjieba-c.nuspec.template -Raw) -replace '##version##', $version) | Set-Content -Path libjieba-c.nuspec