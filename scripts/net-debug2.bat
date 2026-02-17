@echo off
set PICO_IP=192.168.4.1
set PICO_PORT=80

echo ========================================
echo PICO 2W CONNECTIVITY DEBUGGER
echo ========================================

:: 1. Check Local IP
echo [1/4] Checking your local IP address...
ipconfig | findstr /i "IPv4"
echo.

:: 2. Ping the Pico
echo [2/4] Pinging Pico at %PICO_IP%...
ping -n 3 %PICO_IP% | find "TTL"
if errorlevel 1 (
    echo [!] ERROR: Cannot reach Pico. Check if you are connected to its Wi-Fi SSID.
) else (
    echo [OK] Pico is responding to ICMP (Ping).
)
echo.

:: 3. Test Port 80 (Web Server)
echo [3/4] Testing Web Server at %PICO_IP%:%PICO_PORT%...
:: Using PowerShell to test the specific TCP port
powershell -Command "Test-NetConnection -ComputerName %PICO_IP% -Port %PICO_PORT%" | findstr "TcpTestSucceeded"
if errorlevel 1 (
    echo [!] ERROR: Port %PICO_PORT% is closed. Is your web server script running?
) else (
    echo [OK] Web server is accepting connections.
)
echo.

:: 4. Check ARP Table (Hardware Check)
echo [4/4] Checking ARP cache for Pico MAC address...
arp -a | findstr "%PICO_IP%"
echo.

echo ========================================
echo Debugging Complete.
pause