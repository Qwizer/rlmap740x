function onSay(cid, words, param)
	local town_pos = getPlayerMasterPos(cid)
	local old_pos = getPlayerPosition(cid)
	if(doTeleportThing(cid, town_pos)) then
		if(isGmInvisible(cid) == false) then
			doSendMagicEffect(old_pos, CONST_ME_POFF)
			doSendMagicEffect(town_pos, CONST_ME_TELEPORT)
		end
	else
		doPlayerSendCancel(cid, "Can not teleport to that position. Check your master position.")
	end

	return false
end
