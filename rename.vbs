Set objFSO = CreateObject("Scripting.FileSystemObject")

objFSO.MoveFile "C:\FSO\*.txt" , "C:\FSO\BackupLog.txt"