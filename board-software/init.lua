-- init.lua
-- This file will be run on startup.
-- ------------------------------------------
-- The purpose of this file is to boot the
-- temperature broadcasting program.
-- To allow for overwriting of this and other
-- files, booting will continue after 30 sec
BOOT_TIMEOUT = 10 * 1000

--- Wireless
WIFI_SSID 		= "iPhone VLC"			-- SSID of WiFi to connect to
WIFI_PASS 		= "vlcvlcvlc"			-- Password of WiFi network

-- Print a message to minimize confusion
print("Waiting " .. (BOOT_TIMEOUT / 1000) .. "s before continuing.")

wifi.setmode(wifi.STATION)
wifi.sta.config(WIFI_SSID, WIFI_PASS)

TEMP_PIN = 3

-- Set pinmode to output and LOW
gpio.mode(TEMP_PIN, gpio.OUTPUT)
gpio.write(TEMP_PIN, gpio.LOW)

-- Setup a timer that will fire once
tmr.alarm(0, BOOT_TIMEOUT, 0, function()
		print("Booting.")
		dofile('main.lua')
	end )

function stopBoot()
	tmr.stop(0)
	print('Booting aborted.')
end

function boot()
	print('Booting.')
	dofile('main.lua')
end

function toModifiedBinary(number)
	while number>0.1 do
		local remainder = number % 2
		number = number / 2
		print(remainder)
	end
end

function bin(num)
	local binary = ""
	for i = 0, 6 do
		local bitToSave = '0'
		if bit.isset(num, i) then bitToSave = '1' end
		binary = binary .. bitToSave
	end
	if num % 1 > 0 then 
		binary = '1' .. binary
	else
		binary = '0' .. binary
	end
	return binary
end