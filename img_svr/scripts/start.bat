%~d0
cd %~p0

start start_dispatcher.bat
timeout /t 1

start start_server.bat
timeout /t 1

start start_server2.bat
timeout /t 1

exit
