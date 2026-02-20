@echo off
echo ===============================
echo Raspberry Pi Pico W Connection Test
echo ===============================

REM 1. Test if we can ping the Pico W
echo.
echo [1] Pinging Pico W at 192.168.4.1...
ping 192.168.4.1 -n 4
if errorlevel 1 (
    echo Ping failed. Check Wi-Fi connection or Pico W AP.
) else (
    echo Ping successful.
)

REM 2. Test if port 80 is open using PowerShell
echo.
echo [2] Checking if port 80 on 192.168.4.1 is open...
powershell -Command "try { $tcp = New-Object System.Net.Sockets.TcpClient('192.168.4.1', 80); Write-Host 'Port 80 is open'; $tcp.Close() } catch { Write-Host 'Port 80 is closed or unreachable' }"

REM 3. Show local IP configuration
echo.
echo [3] Your IP address and network info:
ipconfig

REM 4. Test default gateway
echo.
echo [4] Default gateway:
for /f "tokens=3" %%G in ('ipconfig ^| findstr /i "Gateway"') do echo %%G

REM 5. Test DNS resolution
echo.
echo [5] Testing DNS (just as extra, Pico W might not need DNS):
nslookup google.com

REM 6. Traceroute to Pico W
echo.
echo [6] Traceroute to Pico W:
tracert 192.168.4.1

REM 7. Test HTTP GET (requires PowerShell)
echo.
echo [7] Attempting HTTP GET from Pico W web server:
powershell -Command "try { $response = Invoke-WebRequest -Uri http://192.168.4.1 -UseBasicParsing; Write-Host 'HTTP GET successful, status code:' $response.StatusCode } catch { Write-Host 'HTTP GET failed' }"

echo.
echo ===============================
echo Debug Complete
pause
