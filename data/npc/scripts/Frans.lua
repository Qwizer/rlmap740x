dofile(getDataDir() .. 'npc/scripts/lib/greeting.lua')

local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)



-- OTServ event handling functions
function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

local shopModule = ShopModule:new()
npcHandler:addModule(shopModule)

shopModule:addBuyableItem({'spellbook'}, 2175, 150)

shopModule:addBuyableItem({'blank rune'}, 2260, 10, 'blank rune')

function creatureSayCallback(cid, type, msg) msg = string.lower(msg)
	if(npcHandler.focus ~= cid) then
		return false
	end
	
	if msgcontains(msg, 'bp of lf') or msgcontains(msg, 'bp of life fluid') or msgcontains(msg, 'bp lf') then
	npcHandler:say('Do you want to buy a backpack of life fluid for 1200 gold coins?')
	talk_state = 5
	
		elseif msgcontains(msg, 'yes') and talk_state == 5 then
			if getPlayerMoney(cid) >= 1200 then
				purple2_bp = doPlayerAddItem(cid, 2000, 1)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doAddContainerItem(purple2_bp, 2006, 10)
				doPlayerRemoveMoney(cid, 1200)
				npcHandler:say('Thank you for buying.')
				talk_state = 0
			else
				npcHandler:say('You don\'t have enough money.')
				talk_state = 0
			end
	end

if msgcontains(msg, 'bp of mf') or msgcontains(msg, 'bp of manafluid') or msgcontains(msg, 'bp mf') then
	npcHandler:say('Do you want to buy a backpack of mana fluid for 2000 gold coins?')
	talk_state = 6
	
		elseif msgcontains(msg, 'yes') and talk_state == 6 then
			if getPlayerMoney(cid) >= 2000 then
				brown_bp = doPlayerAddItem(cid, 2001, 1)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doAddContainerItem(brown_bp, 2006, 7)
				doPlayerRemoveMoney(cid, 2000)
				npcHandler:say('Thank you for buying.')
				talk_state = 0
			else
				npcHandler:say('You don\'t have enough money.')
				talk_state = 0
			end
	end	
	
	if msgcontains(msg, 'bp of br') or msgcontains(msg, 'bp of blank rune') or msgcontains(msg, 'bp br') then
	npcHandler:say('Do you want to buy a backpack of blank rune for 200 gold coins?')
	talk_state = 7
	
		elseif msgcontains(msg, 'yes') and talk_state == 7 then
			if getPlayerMoney(cid) >= 200 then
				brown_bp = doPlayerAddItem(cid, 2003, 1)
				doAddContainerItem(brown_bp, 2260, 20)
				doPlayerRemoveMoney(cid, 200)
				npcHandler:say('Thank you for buying.')
				talk_state = 0
			else
				npcHandler:say('You don\'t have enough money.')
				talk_state = 0
			end
	end	
	
    return true
end

keywordHandler:addKeyword({'name'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am a FRANS."})
keywordHandler:addKeyword({'frans'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Floating ReeeeAnimated Necromantic Seeeervant ... FRANS."})
keywordHandler:addKeyword({'job'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am selliiiing ruuuunes, wands, roooods and spellbooooooks."})
keywordHandler:addKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Sorcerorssss, druidssss, they all come to ussss."})
keywordHandler:addKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Sorcerorssss, druidssss, they all come to ussss."})
keywordHandler:addKeyword({'magic'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Is aaaall about magic more or lesssss, isn't it?"})
keywordHandler:addKeyword({'vladruc'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Heeee is the bossss. Better don't messss with him!"})
keywordHandler:addKeyword({'urghain'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Heeee is the bossss. Better don't messss with him!"})
keywordHandler:addKeyword({'ferumbras'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Wouldn't he beeee the perfect FRANS?"})
keywordHandler:addKeyword({'market'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Yes, that's a market heeeere, smarty ... Nice to seeeee I am not the only one without a braiiiin here."})
keywordHandler:addKeyword({'excalibug'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "We FRANSes don't liiiike any bugssss."})
keywordHandler:addKeyword({'offer'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "What do youuuu think I am? A lousy barberrrr? I'm selliiiing ruuuunes and spellboooooks."})
keywordHandler:addKeyword({'good'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "What do youuuu think I am? A lousy barberrrr? I'm selliiiing ruuuunes and spellboooooks."})
keywordHandler:addKeyword({'sell'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "What do youuuu think I am? A lousy barberrrr? I'm selliiiing ruuuunes and spellboooooks."})
keywordHandler:addKeyword({'have'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "What do youuuu think I am? A lousy barberrrr? I'm selliiiing ruuuunes and spellboooooks."})


npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())