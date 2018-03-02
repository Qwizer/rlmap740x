local function doTargetCorpse(cid, pos)
	local getPos = pos
	getPos.stackpos = 255
	corpse = getThingfromPos(getPos)
	if(corpse.uid > 0 and isCreature(corpse.uid) == false and isInArray(CORPSES, corpse.itemid) ) then
		doRemoveItem(corpse.uid)
		doPlayerSummonCreature(cid, "Skeleton", pos)
		doSendMagicEffect(pos, CONST_ME_MAGIC_BLUE)
		return LUA_NO_ERROR
	end

	doSendMagicEffect(getCreaturePosition(cid), CONST_ME_POFF)
	doPlayerSendDefaultCancel(cid, RETURNVALUE_NOTPOSSIBLE)
	return LUA_ERROR
end

function onCastSpell(cid, var)
	local pos = variantToPosition(var)
	if(pos.x ~= 0 and pos.y ~= 0 and pos.z ~= 0) then
		return doTargetCorpse(cid, pos)
	end

	doSendMagicEffect(getCreaturePosition(cid), CONST_ME_POFF)
	doPlayerSendDefaultCancel(cid, RETURNVALUE_NOTPOSSIBLE)
	return LUA_ERROR
end