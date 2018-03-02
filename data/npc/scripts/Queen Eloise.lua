
local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)



-- OTServ event handling functions
function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

function FocusModule:init(handler)
	FOCUS_GREETSWORDS = {'hi queen', 'hello queen', 'hail queen', 'hail the queen'}
	FOCUS_FAREWELLSWORDS = {'bye', 'farewell'}
	self.npcHandler = handler
	for i, word in pairs(FOCUS_GREETSWORDS) do
		local obj = {}
		table.insert(obj, word)
		obj.callback = FOCUS_GREETSWORDS.callback or FocusModule.messageMatcher
		handler.keywordHandler:addKeyword(obj, FocusModule.onGreet, {module = self})
	end
		
	for i, word in pairs(FOCUS_FAREWELLSWORDS) do
		local obj = {}
		table.insert(obj, word)
		obj.callback = FOCUS_FAREWELLSWORDS.callback or FocusModule.messageMatcher
		handler.keywordHandler:addKeyword(obj, FocusModule.onFarewell, {module = self})
	end
		
	return true
end

keywordHandler:addKeyword({'job'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am Queen Eloise. It is my duty to reign over this marvellous city and the lands of the north."})
keywordHandler:addKeyword({'justice'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "We women try to bring justice and wisdom to all, even to males."})
keywordHandler:addKeyword({'name'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am Queen Eloise. For you it's 'My Queen' or 'Your Majesty', of course."})
keywordHandler:addKeyword({'news'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I don't care about gossip like a simpleminded male would do."})
keywordHandler:addKeyword({'tibia'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Soon the whole land will be ruled by women at last!"})
keywordHandler:addKeyword({'land'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Soon the whole land will be ruled by women at last!"})
keywordHandler:addKeyword({'you'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Thank you, I'm fine."})
keywordHandler:addKeyword({'castle'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "It's my humble domain."})
keywordHandler:addKeyword({'sell'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Sell? Your question shows that you are a typical member of your gender!"})
keywordHandler:addKeyword({'god'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "We honor the gods of good in our fair city, especially Crunor, of course."})
keywordHandler:addKeyword({'citizen'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "All citizens of Carlin are my subjects. I see them more as my childs, though, epecially the male population."})
keywordHandler:addKeyword({'thais'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "This beast scared my cat away on my last diplomatic mission in this filthy town."})
keywordHandler:addKeyword({'ferumbras'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "He is the scourge of the whole continent!"})
keywordHandler:addKeyword({'treasure'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The royal treasure is hidden beyond the grasps of any thieves by magical means."})
keywordHandler:addKeyword({'monster'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Go and hunt them! For queen and country!"})
keywordHandler:addKeyword({'help'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Visit the church or the townguards for help."})
keywordHandler:addKeyword({'quest'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I will call for heroes as soon as the need arises again."})
keywordHandler:addKeyword({'mission'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I will call for heroes as soon as the need arises again."})
keywordHandler:addKeyword({'gold'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Our city is rich and prospering."})
keywordHandler:addKeyword({'money'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Our city is rich and prospering."})
keywordHandler:addKeyword({'tax'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Our city is rich and prospering."})
keywordHandler:addKeyword({'sewer'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I don't want to talk about 'sewers'."})
keywordHandler:addKeyword({'dungeon'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Dungeons are places where males crawl around and look for trouble."})
keywordHandler:addKeyword({'equipment'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Feel free to visit our town's magnificent shops."})
keywordHandler:addKeyword({'food'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Feel free to visit our town's magnificent shops."})
keywordHandler:addKeyword({'time'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Don't worry about time in the presence of your Queen."})
keywordHandler:addKeyword({'hero'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "We need the assistance of heroes now and then. Even males prove useful now and then."})
keywordHandler:addKeyword({'adventure'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "We need the assistance of heroes now and then. Even males prove useful now and then."})
keywordHandler:addKeyword({'collector'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The taxes in Carlin are not high, more a symbol than a sacrifice."})
keywordHandler:addKeyword({'queen'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I am the Queen, the only rightful ruler on the continent!"})
keywordHandler:addKeyword({'army'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Ask one of the soldiers about that."})
keywordHandler:addKeyword({'enemy'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Our enemies are numerous. We have to fight vile monsters and have to watch this silly king in the south carefully."})
keywordHandler:addKeyword({'enemi'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Our enemies are numerous. We have to fight vile monsters and have to watch this silly king in the south carefully."})
keywordHandler:addKeyword({'thais'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "They dare to reject my reign over them!"})
keywordHandler:addKeyword({'south'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "They dare to reject my reign over them!"})
keywordHandler:addKeyword({'carlin'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Isn't our city marvellous? Have you noticed the lovely gardens on the roofs?"})
keywordHandler:addKeyword({'city'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Isn't our city marvellous? Have you noticed the lovely gardens on the roofs?"})
keywordHandler:addKeyword({'shop'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "My subjects maintain many fine shops. Go and have a look at their wares."})
keywordHandler:addKeyword({'merchant'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Ask around about them."})
keywordHandler:addKeyword({'craftsmen'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Ask around about them."})
keywordHandler:addKeyword({'guild'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The four major guilds are the Knights, the Paladins, the Druids, and the Sorcerers."})
keywordHandler:addKeyword({'minotaur'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "They havn't troubled our city lately. I guess, they fear the wrath of our druids."})
keywordHandler:addKeyword({'paladin'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The paladins are great hunters."})
keywordHandler:addKeyword({'legola'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The paladins are great hunters."})
keywordHandler:addKeyword({'elane'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "It's a shame that the High Paladin does not reside in Carlin."})
keywordHandler:addKeyword({'knight'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The knights of Carlin are the bravest."})
keywordHandler:addKeyword({'trisha'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The knights of Carlin are the bravest."})
keywordHandler:addKeyword({'sorc'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The sorcerers have a small isle for their guild. So if they blow something up it does not burn the whole city to ruins."})
keywordHandler:addKeyword({'lea'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The sorcerers have a small isle for their guild. So if they blow something up it does not burn the whole city to ruins."})
keywordHandler:addKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The druids of Carlin are our protectors and advisors. Their powers provide us with wealth and food."})
keywordHandler:addKeyword({'padreia'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The druids of Carlin are our protectors and advisors. Their powers provide us with wealth and food."})
keywordHandler:addKeyword({'good'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Carlin is a center of the forces of good, of course."})
keywordHandler:addKeyword({'evil'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The forces of evil have a firm grip on this puny city to the south."})
keywordHandler:addKeyword({'order'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The order, Crunor gives the world, is essential for survival."})
keywordHandler:addKeyword({'chaos'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Chaos is common in the southern regions, where they allow a man to reign over a realm."})
keywordHandler:addKeyword({'excalibug'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "A mans tale ... that means 'nonsense', of course."})
keywordHandler:addKeyword({'reward'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "If you want a reward, go and bring me something this silly King Tibianus wants dearly!"})
keywordHandler:addKeyword({'tbi'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "A dusgusting organisation, which could be only created by men."})
keywordHandler:addKeyword({'eremo'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "It is said that he lives on a small island near Edron. Maybe the people there know more about him."})

function creatureSayCallback(cid, type, msg)
	if(npcHandler.focus ~= cid) then
		return false
	end

	-- The Postman Missions Quest
	if msgcontains(msg, 'uniform') and getPlayerStorageValue(cid,233) == 5 then
	npcHandler:say('I remember about those uniforms, they had a camouflage inlay so they could be worn the inside out too. I will send some colorsamples via mail to Mr. Postner.')
	setPlayerStorageValue(cid,233,6)
	talk_state = 0
	
	elseif msgcontains(msg, 'uniform') and getPlayerStorageValue(cid,233) == 0 then
	npcHandler:say('The uniforms of our guards and soldiers are of unparraleled quality of course.')
	talk_state = 0
	end
	
	return true
end

local node1 = keywordHandler:addKeyword({'promot'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Do you want to be promoted in your vocation for 20000 gold?'})
	node1:addChildKeyword({'yes'}, StdModule.promotePlayer, {npcHandler = npcHandler, promotions = {[1] = 5, [2] = 6, [3] = 7, [4] = 8}, cost = 20000, level = 20, text = 'Congratulations! You are now promoted.'})
	node1:addChildKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'Ok, then not.', reset = true})

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())