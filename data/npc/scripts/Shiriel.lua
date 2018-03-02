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

shopModule:addBuyableItem({'life'}, 2006, 60, 10, 'life fluid')
shopModule:addBuyableItem({'mana'}, 2006, 100, 7, 'mana fluid')
shopModule:addBuyableItem({'spellbook'}, 2175, 150)
shopModule:addBuyableItem({'blank rune'}, 2260, 10, 'blank rune')

function creatureSayCallback(cid, type, msg) msg = string.lower(msg)
	if(npcHandler.focus ~= cid) then
		return false
	end

	if msgcontains(msg, 'vial') or msgcontains(msg, 'deposit') or msgcontains(msg, 'flask') then
		npcHandler:say("I will pay you 5 gold for every empty vial. Ok?", 1)
		talk_state = 857
	elseif talk_state == 857 and msgcontains(msg, 'yes') then
		if sellPlayerEmptyVials(cid) == true then
			npcHandler:say("Here's your money!", 1)
			talk_state = 0
		else
			npcHandler:say("You don't have any empty vials!", 1)
			talk_state = 0
		end
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

keywordHandler:addKeyword({'job'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I sell mystic runes, spellbooks, wands, rods and fluids of life or mana."})
keywordHandler:addKeyword({'name'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am Shiriel Sharaziel."})
keywordHandler:addKeyword({'time'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Time was mastered by my people long ago."})
keywordHandler:addKeyword({'elves'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Our noble race has knowledge of secrets beyond your comprehension."})
keywordHandler:addKeyword({'dwarf'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Not worth to bother about."})
keywordHandler:addKeyword({'human'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Cursed with a short livespan and not worth to be remembered."})
keywordHandler:addKeyword({'troll'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "We should eradicate them all."})
keywordHandler:addKeyword({'cenath'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "We are the teachers to the other castes."})
keywordHandler:addKeyword({'kuridai'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Their knowledge is limited."})
keywordHandler:addKeyword({'deraisim'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "They lack the patience that suits a race with our lifespan."})
keywordHandler:addKeyword({'abdaisim'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I think they are lost forever."})
keywordHandler:addKeyword({'teshial'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "They were not prepared for what they encountered in their quest for knowledge. WE will be prepared."})
keywordHandler:addKeyword({'ferumbras'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "A humanbreed abnomination."})
keywordHandler:addKeyword({'crunor'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I have no time for superstition."})
keywordHandler:addKeyword({'excalibug'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I would love to analyse it one day."})
keywordHandler:addKeyword({'new'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "News are secrets and you are not worthy of my secrets."})
keywordHandler:addKeyword({'magic'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I could teach you some spells ... but I won't."})



npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())