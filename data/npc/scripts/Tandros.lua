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

keywordHandler:addKeyword({'job'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am selling items of magic power such as runes, wands, rods, life fluids and mana fluids."})
keywordHandler:addKeyword({'name'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am Tandros the magnificent."})
keywordHandler:addKeyword({'time'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "It is a crime against the order of things to try measuring time. The very thought of squeezing the majesty of centuries and centuries into a puny mechanical device is blasphemy."})
keywordHandler:addKeyword({'king'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Our king is a worldly one. But if you buy enough of my fluids and runes you might become the king of magic one day."})
keywordHandler:addKeyword({'venore'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Technically I am an employee of those trade barons of Venore but of course no one can control my magnificent mind."})
keywordHandler:addKeyword({'thais'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "It is so crowded and people there are always busy. I dare to say that this is a city that has lost its magic at some point."})
keywordHandler:addKeyword({'carlin'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I heard there are many druids that are quite influential. They should know how to keep the magic of a place alive. I am looking forward to travel there one day."})
keywordHandler:addKeyword({'edron'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Edron is rumoured to be a place of ancient mysteries and powerful magic."})
keywordHandler:addKeyword({'jungle'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The magic is out there somewhere."})
keywordHandler:addKeyword({'tibia'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The world is full of magic that is waiting to be used ... perhaps by you! Take the first step by buying my wares to gather even more magic power for yourself."})
keywordHandler:addKeyword({'kazordoon'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Dwarves have little love for magic. That makes them quite suspicious, doesn't it?"})
keywordHandler:addKeyword({'dwarf'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Dwarves have little love for magic. That makes them quite suspicious, doesn't it?"})
keywordHandler:addKeyword({'dwarves'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Dwarves have little love for magic. That makes them quite suspicious, doesn't it?"})
keywordHandler:addKeyword({"ab'dendriel"}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Elves are such marvelous, mythic creatures. They are full of magic."})
keywordHandler:addKeyword({'elf'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Elves are such marvelous, mythic creatures. They are full of magic."})
keywordHandler:addKeyword({'elves'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Elves are such marvelous, mythic creatures. They are full of magic."})
keywordHandler:addKeyword({'darashia'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "An unremarkable little town, but riding there by carpet is pure magic."})
keywordHandler:addKeyword({'ankrahmun'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "A city that breathes evil and dark magic. Stay away or be at least well prepared if you intend to visit the city of the dead."})
keywordHandler:addKeyword({'ferumbras'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "He might be evil, but his powers are unimaginable! To stand a chance against evil overlords like him, you have to buy loads of my runes and fluids."})
keywordHandler:addKeyword({'excalibug'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "A weapon of unparalleled magic. Don't listen to people that tell you that this is only a myth. It might be a dream but remember, dreams can come true."})
keywordHandler:addKeyword({'ape'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "They are attacking travellers and even our settlement now and then. What can be a better way for you to survive than by preparing yourself well and to buy enough fluids and runes?"})
keywordHandler:addKeyword({'lizard'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The lizards live far away on the other side of the dangerous jungle. If you want to go there to learn more about their secrets, I strongly advise you to supply yourself with runes and fluids."})
keywordHandler:addKeyword({'dworc'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The dworcs are fierce enemies and the poison they use is lethal. If you don't have some fluids and runes with you, you are easy prey to them."})
keywordHandler:addKeyword({'offer'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I'm selling life and mana fluids, runes, wands, rods and spellbooks."})
keywordHandler:addKeyword({'good'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I'm selling life and mana fluids, runes, wands, rods and spellbooks."})
keywordHandler:addKeyword({'sell'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I'm selling life and mana fluids, runes, wands, rods and spellbooks."})
keywordHandler:addKeyword({'have'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I'm selling life and mana fluids, runes, wands, rods and spellbooks."})

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())