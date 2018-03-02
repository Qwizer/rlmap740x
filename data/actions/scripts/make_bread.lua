-- by Nottinghster

local ITEM_WHEAT = 2694
local ITEM_FLOUR = 2692
local ITEM_DOUGH = 2693
local ITEM_BREAD = 2689
local ITEM_MILL = {1381, 1382, 1383, 1384}
local TYPE_EMPTY = 0
local TYPE_WATER = 1
	
function onUse(cid, item, fromPosition, itemEx, toPosition)
	if(item.itemid == ITEM_WHEAT and isInArray(ITEM_MILL, itemEx.itemid) == true) then
		doPlayerAddItem(cid, ITEM_FLOUR)
		doRemoveItem(item.uid, 1)
		return true
		
	elseif(item.itemid == ITEM_FLOUR and isItemFluidContainer(itemEx.itemid) == true and itemEx.type == TYPE_WATER) then
		doPlayerAddItem(cid, ITEM_DOUGH)
		doChangeTypeItem(itemEx.uid, TYPE_EMPTY)
		doRemoveItem(item.uid, 1)
		return true
		
	elseif(item.itemid == ITEM_DOUGH and isInArray(OVEN_ON, itemEx.itemid) == true) then
		doPlayerAddItem(cid, ITEM_BREAD)
		doRemoveItem(item.uid, 1)
		return true
	end
	
	return false
end 