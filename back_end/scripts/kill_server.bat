%~d0
cd %~p0

@taskkill /f /IM redis-server.exe
@taskkill /f /IM McsfLoggerServer.exe
@taskkill /f /IM McsfSystemDispatcher.exe

exit


