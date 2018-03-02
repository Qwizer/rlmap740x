-- Loot Message
MessageSent = {}

function getContentDescription(uid, comma)
	local ret, i, containers = '', 0, {}

	while i < getContainerSize(uid) do
		local v, s = getContainerItem(uid, i), ''
		local k = getItemDescriptions(v.itemid)
		if k.name ~= '' then
			if isContainer(v.uid) and getContainerSize(v.uid) > 0 then
				table.insert(containers, v.uid)
			else
				if v.type > 1 and isItemStackable(v.itemid) and k.showCount then
					s = v.type .. ' ' .. getItemDescriptions(v.itemid).plural
				else
					local article = k.article
					s = (article == '' and '' or article .. ' ') .. k.name
				end
				ret = ret .. (i == 0 and not comma and '' or ', ') .. s
			end
		else
			ret = ret .. (i == 0 and not comma and '' or ', ') .. 'an item of type ' .. v.itemid .. ', please report it to gamemaster'
		end
		i = i + 1
	end

	for i = 1, #containers do
		ret = ret .. getContentDescription(containers[i], true)
	end

	return ret
end

local function send(cid, lastHit, pos, name, party, target)
	local corpse = getTileItemByType(pos, ITEM_TYPE_CONTAINER).uid
	local ret = isContainer(corpse) and getContentDescription(corpse)

	if party then
		local leaderid
		for _, pid in ipairs(getPartyMembers(cid)) do
			if isPartyLeader(pid) then
				leaderid = pid
			end
		end
		local hash = ("%d:%d"):format(leaderid, target)
		if not MessageSent[hash] then
			for _, pid in ipairs(getPartyMembers(cid)) do
				local send = getPlayerStorageValue(pid,STORAGE_LOOTMESSAGE)
				if send == 1 then
					doPlayerSendTextMessage(pid, MESSAGE_INFO_DESCR, 'Loot of '.. getArticleByWord(name) .. ' ' .. string.lower(name) .. ': ' .. (ret ~= '' and ret or 'nothing'))
				end
			end
			MessageSent[hash] = true
			addEvent(function() MessageSent[hash] = nil end, 1000)
		end
	else
		local send = getPlayerStorageValue(cid,STORAGE_LOOTMESSAGE)
		if send == 1 then
			doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, 'Loot of '.. getArticleByWord(name) .. ' ' .. string.lower(name) .. ': ' .. (ret ~= '' and ret or 'nothing'))
		end
	end
	
end

function onKill(cid, target, lastHit)
	if not isPlayer(target) then
		addEvent(send, 0, cid, lastHit, getThingPos(target), getCreatureName(target), getPartyMembers(cid), target)
	end
	return true
end
