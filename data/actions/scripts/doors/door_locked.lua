function onUse(cid, item, fromPosition, itemEx, toPosition)
	if(item.actionid == 0) then
		-- This is impossible to happen, but whatever.
		doTransformItem(item.uid, item.itemid+2)
		return true
	end

	doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "It is locked.")
	return true
end