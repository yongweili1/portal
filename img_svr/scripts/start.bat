%~d0
cd %~p0

start kill.bat
timeout /t 2

start start_dispatcher.bat
timeout /t 3

start start_server.bat
timeout /t 1

exit
