@echo off
setlocal

set BUILD_DIR=%~dp0build

start "gate_server"    cmd /k "%BUILD_DIR%\gate_server\gate_server.exe"
start "status_server"  cmd /k "%BUILD_DIR%\status_server\status_server.exe"
start "chat_server1"   cmd /k "%BUILD_DIR%\chat_server1\chat_server1.exe"
start "chat_server2"   cmd /k "%BUILD_DIR%\chat_server2\chat_server2.exe"
start "client"         cmd /k "%BUILD_DIR%\client\client.exe"

echo All services started.
