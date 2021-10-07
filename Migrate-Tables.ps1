[System.Reflection.Assembly]::LoadWithPartialName("System.windows.forms") | Out-Null

    $foldername = New-Object System.Windows.Forms.FolderBrowserDialog
    $foldername.Description = "Select a folder"
    $foldername.rootfolder = "MyComputer"
    $foldername.SelectedPath = $initialDirectory
    if ($foldername.ShowDialog() -eq "OK") {
        $folder += $foldername.SelectedPath
        Set-Location "\\sitsrv061\WinFrame\Transfer\cir.al\StandaloneDevTools\Migrator"
        .\Migrator.exe $folder
    }