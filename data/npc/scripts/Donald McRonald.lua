local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

-- OTServ event handling functions
function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end

	function FocusModule:init(handler)
	FOCUS_GREETSWORDS = {'hi', 'hello'}
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

function greetCallback(cid)
	if getPlayerVocation(cid) == 2 or getPlayerVocation(cid) == 6 then
	npcHandler:setMessage(MESSAGE_GREET, "Hello, Druid ".. getPlayerName(cid) .."!")
	return true
	else
	npcHandler:setMessage(MESSAGE_GREET, "Hmmm, well, hello ".. getPlayerName(cid) .."!")
	return true
	end	
end	
npcHandler:setCallback(CALLBACK_GREET, greetCallback)

local shopModule = ShopModule:new()
npcHandler:addModule(shopModule)

shopModule:addBuyableItem({'wheat'}, 2694, 1)
shopModule:addBuyableItem({'cheese'}, 2696, 5)
shopModule:addBuyableItem({'carrot'}, 2684, 3)
shopModule:addBuyableItem({'corncob'}, 2686, 3)

keywordHandler:addKeyword({'weather'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Weather is good enough to work on the fields."})
keywordHandler:addKeyword({'crops'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I take care of our crops"})
keywordHandler:addKeyword({'crop'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "It is hard to grow but worth the effort."})
keywordHandler:addKeyword({'field'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "My fields are enchanted by the druids and the wheat grows very quickly."})
keywordHandler:addKeyword({'city'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "The city is to the north."})
keywordHandler:addKeyword({'mill'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I somtimes have to bring the wheat there."})
keywordHandler:addKeyword({'spooked'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I dont know."})
keywordHandler:addKeyword({'king'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "King Tibianus is our king."})
keywordHandler:addKeyword({'frodo'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Frodo? He is a friend of mine."})
keywordHandler:addKeyword({'oswald'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "He ignores us and we ignore him."})
keywordHandler:addKeyword({'bloodblade'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "A general in the army."})
keywordHandler:addKeyword({'elane'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Too noble to care about us."})
keywordHandler:addKeyword({'gorn'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Hardly know him."})
keywordHandler:addKeyword({'sam'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "A blacksmith, eh?"})
keywordHandler:addKeyword({'quentin'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "A generous person."})
keywordHandler:addKeyword({'lynda'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "She has a good soul."})
keywordHandler:addKeyword({'buy'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I can offer you wheat, cheese, carrots, and corncobs."})
keywordHandler:addKeyword({'sell'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I can offer you wheat, cheese, carrots, and corncobs."})
keywordHandler:addKeyword({'have'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "I can offer you wheat, cheese, carrots, and corncobs."})
keywordHandler:addKeyword({'food'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "Are you looking for food? I have wheat, cheese, carrots, and corn to sell. If you want to sell bread, talk to my wife, Sherry."})
keywordHandler:addKeyword({'bread'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = "If you want to sell bread, talk to my wife, Sherry."})

function creatureSayCallback(cid, type, msg)
	if(npcHandler.focus ~= cid) then
		return false
	end
	
local tasks = {
["rats"] =            {killsRequired = 100,  storage = 9001,  money = 1500},
["minotaurs"] =       {killsRequired = 500, storage = 9002, exp = 120000},
["ghouls"] =          {killsRequired = 200, storage = 9003, exp = 70000},
["cyclops"] =         {killsRequired = 200, storage = 9004, exp = 150000},
["ancient scarabs"] = {killsRequired = 300, storage = 9006, exp = 250000},
["dragons"] =         {killsRequired = 400, storage = 9007, exp = 300000},
["necromancers"] =    {killsRequired = 80, storage = 9008, item = {2195}},
["giant spiders"] =   {killsRequired = 300, storage = 9009, exp = 400000},
["warlocks"] =        {killsRequired = 150, storage = 9012, exp = 500000},
["demons"] =          {killsRequired = 1666, storage = 9013, item = {2495}},
["goblins"] =         {killsRequired = 55, storage = 9014, exp = 5000},
["larvas"] =          {killsRequired = 400, storage = 9015, exp = 95000},
["rotworms"] =        {killsRequired = 100, storage = 9016, exp = 28500},
["orcs"] =            {killsRequired = 100, storage = 9017, exp = 33500},
["scarabs"] =         {killsRequired = 200, storage = 9018, exp = 130000},
["trolls"] =          {killsRequired = 50, storage = 9019, exp = 4500},
["black knights"] =   {killsRequired = 50, storage = 9020, exp = 300000, item = {2195}},
["demon skeletons"] = {killsRequired = 200, storage = 9021, exp = 200000},
["dwarf guards"] =    {killsRequired = 300, storage = 9022, exp = 320000},
["fire elementals"] = {killsRequired = 50, storage = 9023, exp = 80000},
["heros"] =           {killsRequired = 100, storage = 9024, exp = 260000},
["vampires"] =        {killsRequired = 250, storage = 9025, exp = 260000},
["snakes"] =          {killsRequired = 50, storage = 9026, money = 3000},
["wasps"] =           {killsRequired = 100, storage = 9027, exp = 25000},
["wolfs"] =           {killsRequired = 100, storage = 9028, money = 4000},
["dwarfs"] =          {killsRequired = 150, storage = 9029, exp = 75000},
["dwarf soldiers"] =  {killsRequired = 200, storage = 9030, exp = 110000},
["cave rats"] =       {killsRequired = 50, storage = 9031, money = 2500},
["amazons"] =         {killsRequired = 120, storage = 9032, money = 8000},
["valkyries"] =       {killsRequired = 120, storage = 9033, money = 10000},
["scorpions"] =       {killsRequired = 50, storage = 9034, money = 6000},
["lions"] =           {killsRequired = 50, storage = 9035, money = 5000},
["dragon lords"] =    {killsRequired = 400, storage = 9036, exp = 800000},
["poison Spiders"] =  {killsRequired = 130, storage = 9037, exp = 20000},
["wild Warriors"] =   {killsRequired = 150, storage = 9038, exp = 39000},
["slimes"] =          {killsRequired = 150, storage = 9039, exp = 91000},
["hunters"] =         {killsRequired = 110, storage = 9040, exp = 125000},
["bonebeast"] =       {killsRequired = 200, storage = 9041, exp = 330000},
["elfs"] =            {killsRequired = 100, storage = 9042, exp = 25000},
["elf scouts"] =      {killsRequired = 150, storage = 9043, exp = 70000},
["elf arcanists"] =   {killsRequired = 300, storage = 9044, exp = 355000},
["swamp trolls"] =    {killsRequired = 50, storage = 9045, exp = 5250},
["frost trolls"] =    {killsRequired = 100, storage = 9046, exp = 12000},
["bugs"] =            {killsRequired = 50, storage = 9047, exp = 4000},
["hyaenas"] =         {killsRequired = 50, storage = 9048, exp = 7800},
["stone golems"] =    {killsRequired = 60, storage = 9049, exp = 22500},
["stalkers"] =        {killsRequired = 150, storage = 9050, exp = 83000},
["ghosts"] =          {killsRequired = 140, storage = 9051, exp = 90000},
["gargoyles"] =       {killsRequired = 150, storage = 9052, exp = 99000},
["crypt shamblers"] = {killsRequired = 160, storage = 9053, exp = 160000},
["beholders"] =       {killsRequired = 180, storage = 9054, exp = 110000},
["minotaur mages"] =  {killsRequired = 100, storage = 9055, exp = 100000},
["monks"] =           {killsRequired = 110, storage = 9056, exp = 115000},
["witches"] =         {killsRequired = 50, storage = 9057, exp = 50000},
["polar bears"] =     {killsRequired = 50, storage = 9058, exp = 42000},
["skeletons"] =       {killsRequired = 150, storage = 9059, exp = 37000},
}

local monster = tasks[msg]
local tblMonsters, kills, totalKills, exp, money, item = {},{},{},{},{},{}

     if (msgcontains(msg,"task")) then
         table.foreach(tasks, function(k, v)
           if getPlayerStorageValue(cid, v.storage) >= 0 and getPlayerStorageValue(cid, v.storage) <= (v.killsRequired)+1 then
              table.insert(tblMonsters, k) table.insert(kills, v.storage) table.insert(totalKills, v.killsRequired)
              table.insert(exp, v.exp) table.insert(money, v.money) table.insert(item, v.item)
           end
         end)
         if (#kills ~= 0) then
           if (getPlayerStorageValue(cid, kills[1]) >= 0) and (getPlayerStorageValue(cid, kills[1]) < totalKills[1]) then
               npcHandler:say("You haven't killed enough "..table.concat(tblMonsters, ", ").." yet. You need ["..totalKills[1]-getPlayerStorageValue(cid, kills[1]).."] kills left.", cid)
           elseif (getPlayerStorageValue(cid, kills[1]) == totalKills[1]+1) then
                  npcHandler:say("Congratulations so you've got killed "..table.concat(tblMonsters, ", ").."! and passed the task. Here is your reward.", cid)
                  setPlayerStorageValue(cid, kills[1], getPlayerStorageValue(cid, kills[1]) + 1)
				  setPlayerStorageValue(cid, kills[1], getPlayerStorageValue(cid, kills[1]) + 1)
                  doPlayerAddExp(cid, exp[1])
                  doPlayerAddMoney(cid, money[1])
                  if (#item ~= 0) then
                     for i, v in ipairs(item) do
                        local container = doPlayerAddItem(cid, 1987, 1)
                        for i = 1, #v do
                           doAddContainerItem(container, v[i], 1)
                        end
                     end
                  end
              talk_state = 0
           end
         else
            table.foreach(tasks, function(k, v) table.insert(tblMonsters, k) return nil end)
            npcHandler:say("Ahh... you looking for a new mission. I'll reward you for "..table.concat(tblMonsters, ", ").." which one we are talking about?", cid)
            talk_state = 1
         end

     elseif (msgcontains(msg,"cancel")) then
             npcHandler:say("Don't forget that you'll lost previous killing, which task do you want to cancel?", cid)
             talk_state = 2
     elseif monster and (talk_state == 2) then
         if (getPlayerStorageValue(cid, monster.storage) >= 0) and (getPlayerStorageValue(cid, monster.storage) < monster.killsRequired) then
             npcHandler:say("Let it be! "..msg.." task has been canceled. ", cid)
             setPlayerStorageValue(cid, monster.storage, -1)
             talk_state = 0
         else
             npcHandler:say("You can't cancel completed tasks.", cid)
         end
     elseif (monster and talk_state == 1) then
            npcHandler:say("Do you really want "..msg.." task?", cid)
            killsRequired = monster.killsRequired storage = monster.storage premium = monster.premium
            talk_state = 3

     elseif (msgcontains(msg, "yes")) and (talk_state == 3) then
         if (premium ~= nil and premium == true) then
            if (isPremium(cid) == FALSE) then
               npcHandler:say("You need premium account in order to do "..msg.." task.", cid)
               return false
            end
         end
         if (getPlayerStorageValue(cid, storage) < 0) then 
             npcHandler:say("Alright then.. you have to kill ["..killsRequired.."] "..msg..". Good luck!", cid)
             setPlayerStorageValue(cid, storage, 0)
             talk_state = 0  
         else
             npcHandler:say("Sorry but you've got killed "..msg.." already and passed the task. Ask me for new tasks.", cid)
         end		 
     elseif (talk_state == 3)  then
             npcHandler:say("Maybe later.", cid)
             talk_state = 0 
     end
   return true
end

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())