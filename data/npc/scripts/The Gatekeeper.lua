local LEVEL = 8

local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

-- OTServ event handling functions
function onCreatureAppear(cid)			npcHandler:onCreatureAppear(cid)			end
function onCreatureDisappear(cid)		npcHandler:onCreatureDisappear(cid)			end
function onCreatureSay(cid, type, msg)		npcHandler:onCreatureSay(cid, type, msg)		end
function onThink()				npcHandler:onThink()					end
-- OTServ event handling functions end


function oracle(cid, message, keywords, parameters, node)
	if(cid ~= npcHandler.focus) then
		return false
	end
	
	local cityNode = node:getParent():getParent()
	local vocNode = node:getParent()
	local destination = cityNode:getParameters().destination
	local townid = cityNode:getParameters().townid
	local voc = vocNode:getParameters().voc
	
	if(destination ~= nil and voc ~= nil and townid ~= nil) then
		if(getPlayerLevel(cid) < parameters.level) then
			npcHandler:say('CHILD! COME BACK WHEN YOU HAVE GROWN UP!')
		else
			npcHandler:say('SO BE IT!')
            doPlayerSetVocation(cid,voc)
			doPlayerSetTown(cid,townid)
        	doTeleportThing(cid,destination)
			doSendMagicEffect(destination, CONST_ME_MAGIC_BLUE)
        end
	else
		error('Destination:', destination, 'Vocation:', vocation, 'Townid:', townid)
	end
	npcHandler:resetNpc()
	return true
end


function greetCallback(cid)
	if(getPlayerLevel(cid) < LEVEL) then
		npcHandler:say('CHILD! COME BACK WHEN YOU HAVE GROWN UP!')
		return false
	else
		return true
	end
end

-- Set the greeting callback function
npcHandler:setCallback(CALLBACK_GREET, greetCallback)

-- Set the greeting message.
npcHandler:setMessage(MESSAGE_GREET, '|PLAYERNAME|, ARE YOU PREPARED TO FACE YOUR DESTINY?')

-- Pre-create the yes/no nodes.
local yesNode = KeywordNode:new({'yes'}, oracle, {level = LEVEL})
local noNode = KeywordNode:new({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, moveup = 1, text = 'AND WHAT PROFESSION HAVE YOU CHOSEN: KNIGHT, PALADIN, SORCERER, OR DRUID?'})

-- Create the actual keyword structure...
local node1 = keywordHandler:addKeyword({'yes'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'IN WHICH TOWN DO YOU WANT TO LIVE: AB\'DENDRIEL, KAZORDOON, ANKRAHMUN, PORT HOPE OR DARASHIA?'})
	local node2 = node1:addChildKeyword({'ab\'dendriel'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, townid = 1, destination = {x=32732, y=31634, z=7}, text = 'IN AB\'DENDRIEL! AND WHAT PROFESSION HAVE YOU CHOSEN: KNIGHT, PALADIN, SORCERER, OR DRUID?'})
		local node3 = node2:addChildKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, voc = 1, onlyFocus = true, text = 'A SORCERER! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, voc = 2, onlyFocus = true, text = 'A DRUID! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'paladin'}, StdModule.say, {npcHandler = npcHandler, voc = 3, onlyFocus = true, text = 'A PALADIN! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'knight'}, StdModule.say, {npcHandler = npcHandler, voc = 4, onlyFocus = true, text = 'A KNIGHT! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
			
	local node2 = node1:addChildKeyword({'kazordoon'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, townid = 2, destination = {x=32649, y=31925, z=11}, text = 'IN KAZORDOON! AND WHAT PROFESSION HAVE YOU CHOSEN: KNIGHT, PALADIN, SORCERER, OR DRUID?'})
		local node3 = node2:addChildKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, voc = 1, onlyFocus = true, text = 'A SORCERER! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, voc = 2, onlyFocus = true, text = 'A DRUID! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'paladin'}, StdModule.say, {npcHandler = npcHandler, voc = 3, onlyFocus = true, text = 'A PALADIN! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'knight'}, StdModule.say, {npcHandler = npcHandler, voc = 4, onlyFocus = true, text = 'A KNIGHT! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
			
	local node2 = node1:addChildKeyword({'ankrahmun'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, townid = 6, destination = {x=33194, y=32853, z=8}, text = 'IN ANKRAHMUN! AND WHAT PROFESSION HAVE YOU CHOSEN: KNIGHT, PALADIN, SORCERER, OR DRUID?'})
		local node3 = node2:addChildKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, voc = 1, onlyFocus = true, text = 'A SORCERER! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, voc = 2, onlyFocus = true, text = 'A DRUID! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'paladin'}, StdModule.say, {npcHandler = npcHandler, voc = 3, onlyFocus = true, text = 'A PALADIN! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'knight'}, StdModule.say, {npcHandler = npcHandler, voc = 4, onlyFocus = true, text = 'A KNIGHT! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
			
	local node2 = node1:addChildKeyword({'port'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, townid = 8, destination = {x=32595, y=32745, z=7}, text = 'IN PORT HOPE! AND WHAT PROFESSION HAVE YOU CHOSEN: KNIGHT, PALADIN, SORCERER, OR DRUID?'})
		local node3 = node2:addChildKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, voc = 1, onlyFocus = true, text = 'A SORCERER! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, voc = 2, onlyFocus = true, text = 'A DRUID! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'paladin'}, StdModule.say, {npcHandler = npcHandler, voc = 3, onlyFocus = true, text = 'A PALADIN! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'knight'}, StdModule.say, {npcHandler = npcHandler, voc = 4, onlyFocus = true, text = 'A KNIGHT! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)

	local node2 = node1:addChildKeyword({'hope'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, townid = 8, destination = {x=32595, y=32745, z=7}, text = 'IN PORT HOPE! AND WHAT PROFESSION HAVE YOU CHOSEN: KNIGHT, PALADIN, SORCERER, OR DRUID?'})
		local node3 = node2:addChildKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, voc = 1, onlyFocus = true, text = 'A SORCERER! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, voc = 2, onlyFocus = true, text = 'A DRUID! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'paladin'}, StdModule.say, {npcHandler = npcHandler, voc = 3, onlyFocus = true, text = 'A PALADIN! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'knight'}, StdModule.say, {npcHandler = npcHandler, voc = 4, onlyFocus = true, text = 'A KNIGHT! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
			
	local node2 = node1:addChildKeyword({'darashia'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, townid = 7, destination = {x=33213, y=32454, z=1}, text = 'IN DARASHIA! AND WHAT PROFESSION HAVE YOU CHOSEN: KNIGHT, PALADIN, SORCERER, OR DRUID?'})
		local node3 = node2:addChildKeyword({'sorcerer'}, StdModule.say, {npcHandler = npcHandler, voc = 1, onlyFocus = true, text = 'A SORCERER! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'druid'}, StdModule.say, {npcHandler = npcHandler, voc = 2, onlyFocus = true, text = 'A DRUID! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'paladin'}, StdModule.say, {npcHandler = npcHandler, voc = 3, onlyFocus = true, text = 'A PALADIN! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)
		local node3 = node2:addChildKeyword({'knight'}, StdModule.say, {npcHandler = npcHandler, voc = 4, onlyFocus = true, text = 'A KNIGHT! ARE YOU SURE? THIS DECISION IS IRREVERSIBLE!'})
			node3:addChildKeywordNode(yesNode)
			node3:addChildKeywordNode(noNode)

keywordHandler:addKeyword({'no'}, StdModule.say, {npcHandler = npcHandler, onlyFocus = true, text = 'COME BACK WHEN YOU ARE PREPARED TO FACE YOUR DESTINY!'})

-- Make it react to hi/bye etc.
npcHandler:addModule(FocusModule:new())