local M = {}


local function readTemperature()
  pin = 4
  ow.setup(pin)
  count = 0
  repeat
    count = count + 1
    addr = ow.reset_search(pin)
    addr = ow.search(pin)
    tmr.wdclr()
  until((addr ~= nil) or (count > 100))
  if (addr == nil) then
    print("Error: no DS18S20 has been found. Is it connected?")
  else
    crc = ow.crc8(string.sub(addr,1,7))
    if (crc == addr:byte(8)) then
      if ((addr:byte(1) == 0x10) or (addr:byte(1) == 0x28)) then
          repeat
            ow.reset(pin)
            ow.select(pin, addr)
            ow.write(pin, 0x44, 1)
            tmr.delay(1000000)
            present = ow.reset(pin)
            ow.select(pin, addr)
            ow.write(pin,0xBE, 1)
            data = nil
            data = string.char(ow.read(pin))
            for i = 1, 8 do
              data = data .. string.char(ow.read(pin))
            end

            crc = ow.crc8(string.sub(data,1,8))
            if (crc == data:byte(9)) then
               t = (data:byte(1))
               tCalc = t / 2
               return tCalc
            end                   
            tmr.wdclr()
          until false
      else
        print("Device family is not recognized.")
      end
    else
      print("CRC is not valid!")
    end
  end
end
M.readTemperature = readTemperature

return M