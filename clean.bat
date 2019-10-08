@echo Cleans out all the tempory files that are created when running the environment.
del *.log
del error.*
del *log.txt
del luac.out
del last_fbx_scene.txt
del assets\recent

REM Can be regenerated.
rd bin /q /s

REM Keeps getting re-generated, but shouldn't ship.
rd Solutions /q /s
rd Editor /q /s
rd logbackups /q /s
rd Backup /q /s
rd Backups /q /s
rd assets\assets /q /s
rd assets\_tmp /q /s
rd user /q /s
rd user(1) /q /s
rd user(2) /q /s

