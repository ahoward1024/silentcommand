@echo off

set project=silentcommand

call vcvarsall.bat x64

IF EXIST bin\%project%.exe (
	devenv bin\%project%.exe
	echo devenv bin\%project%.exe
) else (
	echo Devenv not started, executable bin\%project%.exe does not exist.
)

IF EXIST %project%.sublime-project (
	start sublime_text.exe --project %project%.sublime-project
	echo sublime %project%
) else (
	echo Sublime not started, project %project%.sublime-project does not exist.
)