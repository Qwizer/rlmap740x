local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)
local talkState = {}
function onCreatureAppear(cid) npcHandler:onCreatureAppear(cid) end
function onCreatureDisappear(cid) npcHandler:onCreatureDisappear(cid) end
function onCreatureSay(cid, type, msg) npcHandler:onCreatureSay(cid, type, msg) end
function onThink() npcHandler:onThink() end
function creatureSayCallback(cid, type, msg)
	if(npcHandler.focus ~= cid) then
		return false
	end

local talkUser = NPCHANDLER_CONVbehavior == CONVERSATION_DEFAULT and 0 or cid
local msg = string.lower(msg)
if isInArray({"task"}, msg) then
npcHandler:say("Easy, Medium and Hard!", cid)
talkState[talkUser] = 1
if getPlayerStorageValue(cid, 100157) <= 0 then
setPlayerStorageValue(cid, 100157, 1)
end
elseif isInArray({"easy"}, msg) then
npcHandler:say("Tell me the name of the monster you want to make the task! Dwarfs, Goblins, Larvas, Minotaurs, Rotworms, Orcs, Pandas, Scarabs, Tarantulas and Trolls", cid)
talkState[talkUser] = 1
elseif isInArray({"medium"}, msg) then
npcHandler:say("Tell me the name of the monster you want to make the task! Ancient Scarabs, Apes, Black Knights, Cyclops, Demon Skeletons, Dragons, Dwarf Guards, Fire Elementals, Giant Spiders, Heros, Necromancers,Terror Birds And Vampires", cid)
talkState[talkUser] = 1
elseif isInArray({"hard"}, msg) then
npcHandler:say("Tell me the name of the monster you want to make the task! Behemoths, Dragon Lords, Demons, Hydras, Serpent Spawns, warlocks!", cid)
talkState[talkUser] = 1
elseif talkState[talkUser] == 1 then
if tasktabble[msg] then
if CheckTask(cid) ~= true then
local contagem = getPlayerStorageValue(cid, tasktabble[msg].storage)
if (contagem == -1) then contagem = 1 end
if not tonumber(contagem) then npcHandler:say('Sorry, but you have already finished the task of '..msg, cid) return true end
setPlayerStorageValue(cid, tasktabble[msg].storage_start, 1)
npcHandler:say("Congratulations, you are now participating in the mission of the task "..msg..", lack kill "..string.sub(((contagem)-1)-tasktabble[msg].count, 2).." "..msg, cid)
talkState[talkUser] = 0
else
npcHandler:say('Sorry, but you are already part of a task!', cid)
talkState[talkUser] = 0
end
else
npcHandler:say('enter the correct name of the task mission!', cid)
talkState[talkUser] = 1
end
elseif isInArray({"receber","reward","recompensa","report","reportar","receiver"}, msg) then
if CheckTask(cid) then
for k, v in pairs(tasktabble) do
racetype = k
if getPlayerStorageValue(cid,v.storage_start) >= 1 then
local contagem = getPlayerStorageValue(cid, v.storage)
if (contagem == -1) then contagem = 1 end
if not tonumber(contagem) then npcHandler:say('you can only get items once!', cid) return true end
if (((contagem)-1) >= v.count) then
for _, check in pairs(configbosses_task) do
if string.lower(check.race) == string.lower(racetype) then
local on = getPlayersOnline()
for i=1, #on do
if HavePlayerPosition(on[i], check.FromPosToPos[1],check.FromPosToPos[2]) then
selfSay('wait a moment, there are people doing the task.', cid) return true
end
end
doTeleportThing(cid, check.Playerpos)
local function checkArea(cid)
if not isCreature(cid) then return LUA_ERROR end
if HavePlayerPosition(cid, check.FromPosToPos[1],check.FromPosToPos[2]) then
doTeleportThing(cid, getTownTemplePosition(getPlayerTown(cid)))
end
end
addEvent(checkArea, check.time*60*1000, cid)
end
end
local str = ""
if v.exp ~= nil then doPlayerAddExp(cid, v.exp ) str = str.."".. (str == "" and "" or ",") .." "..v.exp.." of experience" end
if v.money ~= nil then doPlayerAddMoney(cid, v.money) str = str.."".. (str == "" and "" or ",") ..""..v.money.." Golds" end
if v.reward ~= nil then doAddItemsFromList(cid,v.reward) str = str.."".. (str == "" and "" or ",") ..""..getItemsFromList(v.reward) end
npcHandler:say("Thanks for your help Rewards: "..(str == "" and "any" or ""..str.."").." for completing the task of "..k, cid)
setPlayerStorageValue(cid, v.storage, "Finished")
setPlayerStorageValue(cid, v.storage_start, 0)
setPlayerStorageValue(cid, 521456, getPlayerStorageValue(cid, 521456) == -1 and 1 or getPlayerStorageValue(cid, 521456)+1)
else
npcHandler:say('Sorry, but you just killed '..((contagem)-1)..' of '..v.count..' '..k, cid)
end
end
end
else
npcHandler:say("you're not on any mission task", cid)
end
elseif isInArray({"sair","leave","exit"}, msg) then
if CheckTask(cid) then
talkState[talkUser] = 2
for k, v in pairs(tasktabble) do
if getPlayerStorageValue(cid,v.storage_start) >= 1 then
storagesair = v.storage_start
storagezerar = v.storage
local contagem = getPlayerStorageValue(cid, v.storage)
if (contagem == -1) then contagem = 1 end
npcHandler:say('you are participating in a task mission '..k..' and has already '..((contagem)-1)..' '..k..' dead, want to quit?', cid)
end
end
else
npcHandler:say("you're not on any mission task", cid)
end
elseif msgcontains(msg, 'yes') and talkState[talkUser] == 2 then
setPlayerStorageValue(cid, storagesair, 0)
setPlayerStorageValue(cid, storagezerar, 1)
npcHandler:say("You have been removed from the task successfully!", cid)
elseif msg == "no" then  
selfSay("then alright", cid)  
talkState[talkUser] = 0  
npcHandler:releaseFocus(cid)  
end
return TRUE
end
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())