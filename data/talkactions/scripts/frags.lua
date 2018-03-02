function onSay(cid, words, param)
	local table = getPlayerUnjustKills(cid)
	local timestamp = getPlayerSkullEndTime(cid)
	local skulltype = getPlayerSkullType(cid)
	local dateTable = os.date("*t", timestamp)
	
	doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Day: " .. table.day .. " (red skull:" .. table.dayRedSkull .. ")")
	doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Week: " .. table.week .. " (red skull:" .. table.weekRedSkull .. ")")
	doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Month: " .. table.month .. " (red skull:" .. table.monthRedSkull .. ")")

	if (skulltype == 4) then
	doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "You will loose your Red Skull at date : "..dateTable.day.."."..dateTable.month.."."..dateTable.year.." and approximately : "..dateTable.hour..":00 CEST ")
	end

	return false
end
