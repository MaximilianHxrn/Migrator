[System.Reflection.Assembly]::LoadWithPartialName("System.windows.forms") | Out-Null

    $foldername = New-Object System.Windows.Forms.FolderBrowserDialog
    $foldername.Description = "Select a folder"
    $foldername.rootfolder = "MyComputer"
    $foldername.SelectedPath = $initialDirectory
    if ($foldername.ShowDialog() -eq "OK") {
        $folder += $foldername.SelectedPath
        Set-Location "C:\\Users\\Shabib\\Desktop\\Repository\\Migrator"
        .\Migrator.exe $folder
    }
    else {
        .\Migrate-Tables.ps1
    }