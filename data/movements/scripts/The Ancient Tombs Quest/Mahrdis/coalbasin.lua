-- by Nottinghster
function onStepIn(cid, item, pos)

	coin = {x=33240, y=32855, z=13, stackpos=1}
	newpos = {x=33246, y=32850, z=13}

	getcoin = getThingfromPos(coin)
	
	if item.actionid == 6007 and getcoin.itemid == 2159 then
		doTeleportThing(cid,newpos)
		doRemoveItem(getcoin.uid,1)
		doSendMagicEffect(coin, CONST_ME_MAGIC_RED)
		doSendMagicEffect(getCreaturePosition(cid), CONST_ME_MAGIC_BLUE)
		else
	end
	
	if item.actionid == 6008 then
	doTeleportThing(cid, {x=33239, y=32856, z=13})
	doSendMagicEffect(getCreaturePosition(cid), CONST_ME_MAGIC_BLUE)	
	end
	return true
end