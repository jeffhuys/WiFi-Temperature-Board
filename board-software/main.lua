local ds18s20 = require('readTemp')


-- Define variables
--- Remote server
SERVER_ADDR 	= "172.20.10.2"				-- Server address to send data to
SERVER_PORT 	= "33333"					-- Server port

---
TEMP_PIN		= 3 -- GPIO 0
LATEST_TEMP		= -1
LATEST_IP		= 'No IP yet'
SEND_COUNT		= 0
SEND_INTERVAL 	= 5000 -- Every 5 seconds





print("Current IP: (could be nil)")
ip = wifi.sta.getip()
print(ip)

print("Connecting to network...")

-- Wait 10 seconds for WiFi to connect
-- tmr.delay(WIFI_TIMEOUT)

ip = wifi.sta.getip()


CONNECTION_ESTABLISHED  = false
CONNECTION_CREATED		= false

tmr.alarm(1, SEND_INTERVAL, 1, function() 
		sendTemperature()
	end )

function sendTemperature()
	local temp = ds18s20.readTemperature()
	LATEST_TEMP = temp

	ip = wifi.sta.getip()
	LATEST_IP = ip
	if ip ~= nil then
		if CONNECTION_ESTABLISHED then
			--print("IP Address: " .. ip)

			
			cu:send(temp)

			SEND_COUNT = SEND_COUNT + 1
		else
			if CONNECTION_CREATED == false then
				cu = net.createConnection(net.UDP)
				CONNECTION_CREATED = true
			end
			print("Got IP! Connecting to " .. SERVER_ADDR .. ":" .. SERVER_PORT)
			cu:connect(SERVER_PORT,SERVER_ADDR)
			CONNECTION_ESTABLISHED = true
		end
	else
		CONNECTION_ESTABLISHED = false
		print("No IP address assigned (yet)")
	end

	-- Always send to pin. 8 bits is enough.
	pulsePin(LATEST_TEMP)
end 

-- Pulse to pin 3 (GPIO 0)
function pulsePin(temperature)
	print("Starting the pulse")
	binaryTemperature = bin(temperature)
	delayTime = 100000

	-- First, set pin to LOW
	gpio.write(TEMP_PIN, gpio.LOW)
	-- Wait 100 ms
	tmr.delay(delayTime)

	gpio.write(TEMP_PIN, gpio.HIGH)
	tmr.delay(delayTime)

	-- local toggle = 0
	-- for x = 1, 4 do
	-- 	if toggle == 0 then
	-- 		gpio.write(TEMP_PIN, gpio.HIGH)
	-- 		toggle = 1
	-- 		print('HIGH')
	-- 	else
	-- 		gpio.write(TEMP_PIN, gpio.LOW)
	-- 		toggle = 0
	-- 		print('LOW')
	-- 	end
	-- 	tmr.delay(delayTime)
	-- 	tmr.wdclr()
	-- end

	local finalArr = ''

	for i = 1, #binaryTemperature do
	    local c = binaryTemperature:sub(i,i)
	    
	    if c == '1' then
	    	gpio.write(TEMP_PIN, gpio.HIGH)
	    	finalArr = finalArr .. '1'
	    	-- print(c .. " HIGH")
	    else
	    	gpio.write(TEMP_PIN, gpio.LOW)
	    	finalArr = finalArr .. '0'
	    	-- print(c .. " LOW")
	    end

	    -- 100000 us = 100ms
		tmr.delay(delayTime)
		tmr.wdclr()
	end

	gpio.write(TEMP_PIN, gpio.LOW)
	print(finalArr)

end