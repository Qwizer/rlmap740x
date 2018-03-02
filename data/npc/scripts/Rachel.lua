dofile(getDataDir() .. 'npc/scripts/lib/greeting.lua')


local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)



-- OTServ event handling functions
function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

function greetCallback(cid)
if getPlayerVocation(cid) == 1 or getPlayerVocation(cid) == 5 then
	if getPlayerSex(cid) == 1 then
	npcHandler:setMessage(MESSAGE_GREET, "Welcome back, brother ".. getPlayerName(cid) .."!")
	else
	npcHandler:setMessage(MESSAGE_GREET, "Welcome back, sister ".. getPlayerName(cid) .."!")
	end
	return true
else
npcHandler:setMessage(MESSAGE_GREET, "Welcome ".. getPlayerName(cid) .."! Whats your need?")
return true
	end	
end	
npcHandler:setCallback(CALLBACK_GREET, greetCallback)

local shopModule = ShopModule:new()
npcHandler:addModule(shopModule)


shopModule:addSellableItem({'talon'}, 2151, 320)

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

keywordHandler:addKeyword({'job'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am the head alchemist of Carlin. I keep the secret recipies of our ancestors. Besides, I am selling mana and life fluids, spellbooks, wands, rods and runes."})
keywordHandler:addKeyword({'name'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am the illusterous Rachel, of course."})
keywordHandler:addKeyword({'time'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Time is of no meaning to us sorcerers."})
keywordHandler:addKeyword({'wisdom'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Wisdom arises from patience."})
keywordHandler:addKeyword({'patience'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "You have to free yourself from unpatience to learn the deeper secrets of magic."})
keywordHandler:addKeyword({'ancestor'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "We are a guild of old traditions and even older secrets."})
keywordHandler:addKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Spells are the minor parts that make a sorcerer. To be one is a state of mind, not of a full spellbook."})
keywordHandler:addKeyword({'power'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Power is important, but it is just the way, not the ultimate goal."})
keywordHandler:addKeyword({'goal'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "This secrect will be taught you by life, not by me."})
keywordHandler:addKeyword({'vocation'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Your vocation is your profession. There are four vocations in Tibia: Sorcerers, paladins, knights, and druids."})
keywordHandler:addKeyword({'spells'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am too busy to teach you, ask in your guild about that."})



npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())