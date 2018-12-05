%~d0
cd %~p0

start ../../3rd/Windows/redis/redis-server.exe

cd ../../netbase

start ./McsfLoggerServer.exe
start ./McsfSystemDispatcher.exe

exit


