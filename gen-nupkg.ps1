param([string]$current_ref)

[string]$version
$version = (git describe --tags HEAD) | Out-String
$version = $version.Trim().Remove(0, 1)

[string]$revision
$revision = (git rev-parse HEAD) | Out-String
$revision = $revision.Trim()

$content = Get-Content -path libjieba-c.nuspec.template -Raw
$filledContent = $content.Replace('##version##', $version)
$filledContent = $filledContent.Replace('##revision##', $revision)
Set-Content -Path libjieba-c.nuspec -Value $filledContent