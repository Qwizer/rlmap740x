function onSay(cid, words, param)
	doSetGameState(GAME_STATE_SHUTDOWN)
	doSaveServer(true)
	return false
end
