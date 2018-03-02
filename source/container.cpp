//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////
#include "otpch.h"

#include "container.h"
#include "iomapotbm.h"
#include "game.h"
#include "player.h"
#include "configmanager.h"

extern Game g_game;
extern ConfigManager g_config;


Container::Container(uint16_t _type) : Item(_type)
{
	//std::cout << "Container constructor " << this << std::endl;
	maxSize = items[_type].maxItems;
	total_weight = 0.0;
	amountOfItems = 1;
	deepness = 0;
	serializationCount = 0;
}

Container::~Container()
{
	//std::cout << "Container destructor " << this << std::endl;
	for(ItemList::iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit){
		(*cit)->setParent(NULL);
		(*cit)->releaseThing2();
	}

	itemlist.clear();
}

Item* Container::clone() const
{
	Container* _item = static_cast<Container*>(Item::clone());
	for(ItemList::const_iterator it = itemlist.begin(); it != itemlist.end(); ++it){
		_item->addItem((*it)->clone());
	}
	return _item;
}

Container* Container::getParentContainer()
{
	if(Thing* thing = getParent()) {
		if(Item* item = thing->getItem()) {
			return item->getContainer();
		}
	}

	return NULL;
}

const Container* Container::getParentContainer() const
{
	if(const Thing* thing = getParent()) {
		if(const Item* item = thing->getItem()) {
			return item->getContainer();
		}
	}

	return NULL;
}


void Container::addItem(Item* item)
{
	itemlist.push_back(item);
	item->setParent(this);
}

Attr_ReadValue Container::readAttr(AttrTypes_t attr, PropStream& propStream)
{
	switch(attr){
		case ATTR_CONTAINER_ITEMS:
		{
			uint32_t count;
			if(!propStream.GET_UINT32(count)){
				return ATTR_READ_ERROR;
			}
			serializationCount = count;
			return ATTR_READ_END;
		}

		default:
			break;
	}

	return Item::readAttr(attr, propStream);
}

bool Container::unserializeItemNode(FileLoader& f, NODE node, PropStream& propStream)
{
	bool ret = Item::unserializeItemNode(f, node, propStream);

	if(ret){
		unsigned long type;
		NODE nodeItem = f.getChildNode(node, type);
		while(nodeItem){
			//load container items
			if(type == OTBM_ITEM){
				PropStream itemPropStream;
				f.getProps(nodeItem, itemPropStream);

				Item* item = Item::CreateItem(itemPropStream);
				if(!item){
					return false;
				}

				if(!item->unserializeItemNode(f, nodeItem, itemPropStream)){
					return false;
				}

				addItem(item);

				//deepness
				if (item->getContainer()){
					if (getParentContainer())
						item->getContainer()->setDeepness(getDeepness() + 1);
					else{ //we only update deepness when a container get inside of another, so it means that deepness is not updated
						setDeepness(1);
						item->getContainer()->setDeepness(2);
					}
				}

				updateAmountOfItems(item->getTotalAmountOfItemsInside());
				total_weight += item->getWeight();
				if(Container* parent_container = getParentContainer()) {
					parent_container->updateItemWeight(item->getWeight());

				}
			}
			else /*unknown type*/
				return false;

			nodeItem = f.getNextNode(nodeItem, type);
		}

		return true;
	}

	return false;
}

void Container::updateAmountOfItems(int32_t diff)
{
	amountOfItems += diff;
	if(Container* parent_container = getParentContainer()){
		parent_container->updateAmountOfItems(diff);
	}
}

void Container::updateItemWeight(double diff)
{
	total_weight += diff;
	if(Container* parent_container = getParentContainer()){
		parent_container->updateItemWeight(diff);
	}
}

double Container::getWeight() const
{
	return Item::getWeight() + total_weight;
}

std::string Container::getContentDescription() const
{
	std::ostringstream os;
	return getContentDescription(os).str();
}

std::ostringstream& Container::getContentDescription(std::ostringstream& os) const
{
	bool firstitem = true;
	Container* evil = const_cast<Container*>(this);
	for(ContainerIterator cit = evil->begin(); cit != evil->end(); ++cit)
	{
		Item* i = *cit;

		if(firstitem)
			firstitem = false;
		else
			os << ", ";

		os << i->getLongName();
	}

	if(firstitem)
		os << "nothing";

	return os;
}

Item* Container::getItem(uint32_t index)
{
	size_t n = 0;
	for (ItemList::const_iterator cit = getItems(); cit != getEnd(); ++cit) {
		if(n == index)
			return *cit;
		else
			++n;
	}

	return NULL;
}

uint32_t Container::getItemHoldingCount() const
{
	uint32_t counter = 0;

	for(ContainerIterator iter = begin(); iter != end(); ++iter){
		++counter;
	}

	return counter;
}

bool Container::isHoldingItem(const Item* item) const
{
	for(ContainerIterator cit = begin(); cit != end(); ++cit){
		if(*cit == item)
			return true;
	}
	return false;
}

void Container::onAddContainerItem(Item* item)
{
	const Position& cylinderMapPos = getPosition();

	SpectatorVec list;
	SpectatorVec::iterator it;
	g_game.getSpectators(list, cylinderMapPos, false, false, 2, 2, 2, 2);

	//send to client
	Player* player = NULL;
	for(it = list.begin(); it != list.end(); ++it) {
		if((player = (*it)->getPlayer())){
			player->sendAddContainerItem(this, item);
		}
	}

	//event methods
	for(it = list.begin(); it != list.end(); ++it) {
		if((player = (*it)->getPlayer())){
			player->onAddContainerItem(this, item);
		}
	}
}

void Container::onUpdateContainerItem(uint32_t index, Item* oldItem, const ItemType& oldType,
	Item* newItem, const ItemType& newType)
{
	const Position& cylinderMapPos = getPosition();

	SpectatorVec list;
	SpectatorVec::iterator it;
	g_game.getSpectators(list, cylinderMapPos, false, false, 2, 2, 2, 2);

	//send to client
	Player* player = NULL;
	for(it = list.begin(); it != list.end(); ++it) {
		if((player = (*it)->getPlayer())){
			player->sendUpdateContainerItem(this, index, oldItem, newItem);
		}
	}

	//event methods
	for(it = list.begin(); it != list.end(); ++it) {
		if((player = (*it)->getPlayer())){
			player->onUpdateContainerItem(this, index, oldItem, oldType, newItem, newType);
		}
	}
}

void Container::onRemoveContainerItem(uint32_t index, Item* item)
{
	const Position& cylinderMapPos = getPosition();

	SpectatorVec list;
	SpectatorVec::iterator it;
	g_game.getSpectators(list, cylinderMapPos, false, false, 2, 2, 2, 2);

	//send change to client
	Player* player = NULL;
	for(it = list.begin(); it != list.end(); ++it) {
		if((player = (*it)->getPlayer())){
			player->sendRemoveContainerItem(this, index, item);
		}
	}

	//event methods
	for(it = list.begin(); it != list.end(); ++it) {
		if((player = (*it)->getPlayer())){
			player->onRemoveContainerItem(this, index, item);
		}
	}
}

ReturnValue Container::__queryAdd(int32_t index, const Thing* thing, uint32_t count,
	uint32_t flags) const
{
	bool childIsOwner = ((flags & FLAG_CHILDISOWNER) == FLAG_CHILDISOWNER);
	if(childIsOwner){
		//a child container is querying, since we are the top container (not carried by a player)
		//just return with no error.
		return RET_NOERROR;
	}

	const Item* item = thing->getItem();
	if(item == NULL){
		return RET_NOTPOSSIBLE;
	}

	if(!item->isPickupable()){
		return RET_CANNOTPICKUP;
	}

	if(item == this){
		return RET_THISISIMPOSSIBLE;
	}

	const Cylinder* cylinder = getParent();
	while(cylinder){
		if(cylinder == thing){
			return RET_THISISIMPOSSIBLE;
		}
		cylinder = cylinder->getParent();
	}

	bool skipLimit = ((flags & FLAG_NOLIMIT) == FLAG_NOLIMIT);

	if(index == INDEX_WHEREEVER && !skipLimit){
		if(size() >= capacity())
			return RET_CONTAINERNOTENOUGHROOM;
	}

	const Cylinder* topParent = getTopParent();
	if(topParent != this){
		ReturnValue ret = topParent->__queryAdd(INDEX_WHEREEVER, item, count, flags | FLAG_CHILDISOWNER);
		if (ret != RET_NOERROR){
			return ret;
		}
	}

	int32_t maxDeepness = g_config.getNumber(ConfigManager::MAX_DEEPNESS_OF_CHAIN_OF_CONTAINERS);
	if (maxDeepness > 0){
		if (item->getContainer() && getParentContainer()){ //only containers inside of other containers have a deepness updated
			if (getDeepness() + 1 > maxDeepness){
				return RET_CONTAINERHASTOMANYCONTAINERS;
			}
		}
	}

	/* the return value of RET_CONTAINERHASTOMANYITEMS and RET_CONTAINERHASTOMANYCONTAINERS
	   should be sent ONLY if there weren't any other error */
	const Container* c = this;
	int32_t max_amount_inside = g_config.getNumber(ConfigManager::MAX_AMOUNT_ITEMS_INSIDE_CONTAINERS);
	if (max_amount_inside > 0){
		do {
			if ((c->getTotalAmountOfItemsInside() + thing->getTotalAmountOfItemsInside()) > (uint32_t) max_amount_inside + 1)
				return RET_CONTAINERHASTOMANYITEMS;
			c = c->getParentContainer();
		}
		while(c);
	}

	return RET_NOERROR;
}

ReturnValue Container::__queryMaxCount(int32_t index, const Thing* thing, uint32_t count,
	uint32_t& maxQueryCount, uint32_t flags) const
{
	const Item* item = thing->getItem();
	if(item == NULL){
		maxQueryCount = 0;
		return RET_NOTPOSSIBLE;
	}

	if( ((flags & FLAG_NOLIMIT) == FLAG_NOLIMIT) ){
		maxQueryCount = std::max((uint32_t)1, count);
		return RET_NOERROR;
	}

	int32_t freeSlots = std::max((int32_t)(capacity() - size()), (int32_t)0);

	if(item->isStackable()){
		uint32_t n = 0;
		
		if(index != INDEX_WHEREEVER){
			const Thing* destThing = __getThing(index);
			const Item* destItem = NULL;
			if(destThing)
				destItem = destThing->getItem();

			if(destItem && destItem->getID() == item->getID()){
				n = 100 - destItem->getItemCount();
			}
		}

		maxQueryCount = freeSlots * 100 + n;

		if(maxQueryCount < count){
			return RET_CONTAINERNOTENOUGHROOM;
		}
	}
	else{
		maxQueryCount = freeSlots;

		if(maxQueryCount == 0){
			return RET_CONTAINERNOTENOUGHROOM;
		}
	}

	return RET_NOERROR;
}

ReturnValue Container::__queryRemove(const Thing* thing, uint32_t count, uint32_t flags) const
{
	int32_t index = __getIndexOfThing(thing);

	if(index == -1){
		return RET_NOTPOSSIBLE;
	}

	const Item* item = thing->getItem();
	if(item == NULL){
		return RET_NOTPOSSIBLE;
	}

	if(count == 0 || (item->isStackable() && count > item->getItemCount())){
		return RET_NOTPOSSIBLE;
	}

	if(item->isNotMoveable() && !hasBitSet(FLAG_IGNORENOTMOVEABLE, flags)){
		return RET_NOTMOVEABLE;
	}

	return RET_NOERROR;
}

Cylinder* Container::__queryDestination(int32_t& index, const Thing* thing, Item** destItem,
	uint32_t& flags)
{
	if(index == 254 /*move up*/){
		index = INDEX_WHEREEVER;
		*destItem = NULL;

		Container* parentContainer = dynamic_cast<Container*>(getParent());
		if(parentContainer)
			return parentContainer;

		return this;
	}

	if(index == 255 /*add wherever*/){
		index = INDEX_WHEREEVER;
		*destItem = NULL;
	}
	else if(index >= (int32_t)capacity()){
		/*
		if you have a container, maximize it to show all 20 slots
		then you open a bag that is inside the container you will have a bag with 8 slots
		and a "grey" area where the other 12 slots where from the container
		if you drop the item on that grey area
		the client calculates the slot position as if the bag has 20 slots
		*/

		index = INDEX_WHEREEVER;
		*destItem = NULL;
	}

	const Item* item = thing->getItem();
	if(item == NULL){
		return this;
	}

	bool autoStack = g_config.getBoolean(ConfigManager::CONTAINER_ITEMS_AUTO_STACK);
	if(autoStack){
		if(item->isStackable()){
			if(item->getParent() != this){
				//try find a suitable item to stack with
				uint32_t n = 0;
				for(ItemList::iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit){
					if((*cit) != item && (*cit)->getID() == item->getID() && (*cit)->getItemCount() < 100){
						*destItem = (*cit);
						index = n;
						return this;
					}

					++n;
				}
			}
		}
	}

	if(index != INDEX_WHEREEVER){
		Thing* destThing = __getThing(index);
		if(destThing)
			*destItem = destThing->getItem();

		Cylinder* subCylinder = dynamic_cast<Cylinder*>(*destItem);

		if(subCylinder){
			index = INDEX_WHEREEVER;
			*destItem = NULL;
			return subCylinder;
		}
	}

	return this;
}

void Container::__addThing(Thing* thing)
{
	return __addThing(0, thing);
}

void Container::__addThing(int32_t index, Thing* thing)
{
	if(index >= (int32_t)capacity()){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__addThing], index:" << index << ", index >= capacity()" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}
	Item* item = thing->getItem();

	if(item == NULL){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__addThing] item == NULL" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}

#ifdef __DEBUG__MOVESYS__
	if(index != INDEX_WHEREEVER){
		if(size() >= capacity()){
			std::cout << "Failure: [Container::__addThing] size() >= capacity()" << std::endl;
			DEBUG_REPORT
			return /*RET_CONTAINERNOTENOUGHROOM*/;
		}
	}
#endif

	item->setParent(this);
	itemlist.push_front(item);
	updateAmountOfItems(item->getTotalAmountOfItemsInside());

	total_weight += item->getWeight();
	Container* parent_container = getParentContainer();
	if(parent_container) {
		parent_container->updateItemWeight(item->getWeight());
	}

	if (item->getContainer()){
		if (parent_container)
			item->getContainer()->setDeepness(getDeepness() + 1);
		else{ //we only update deepness when a container get inside of another, so it means that deepness is not updated
			setDeepness(1);
			item->getContainer()->setDeepness(2);
		}
	}

	//send change to client
	if(getParent() && (getParent() != VirtualCylinder::virtualCylinder)){
		onAddContainerItem(item);
	}
}

void Container::__updateThing(Thing* thing, uint16_t itemId, uint32_t count)
{
	int32_t index = __getIndexOfThing(thing);
	if(index == -1){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__updateThing] index == -1" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}

	Item* item = thing->getItem();
	if(item == NULL){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__updateThing] item == NULL" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}

	const ItemType& oldType = Item::items[item->getID()];
	const ItemType& newType = Item::items[itemId];

	const double old_weight = item->getWeight();

	item->setID(itemId);
	item->setSubType(count);

	const double diff_weight = -old_weight + item->getWeight();
	total_weight += diff_weight;
	if(Container* parent_container = getParentContainer()) {
		parent_container->updateItemWeight(diff_weight);
	}

	//deepness
	if (item->getContainer()){
		if (getParentContainer()){
			item->getContainer()->setDeepness(getDeepness() + 1);
		}
		else{ //we only update deepness when a container get inside of another, so it means that deepness is not updated
			setDeepness(1);
			item->getContainer()->setDeepness(2);
		}
	}

	//send change to client
	if(getParent()){
		onUpdateContainerItem(index, item, oldType, item, newType);
	}
}

void Container::__replaceThing(uint32_t index, Thing* thing)
{
	Item* item = thing->getItem();
	if(item == NULL){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__replaceThing] item == NULL" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}

	uint32_t count = 0;
	ItemList::iterator cit = itemlist.end();
	for(cit = itemlist.begin(); cit != itemlist.end(); ++cit){
		if(count == index)
			break;
		else
			++count;
	}

	if(cit == itemlist.end()){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__updateThing] item not found" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}

	updateAmountOfItems(int32_t(int64_t(item->getTotalAmountOfItemsInside()) - (*cit)->getTotalAmountOfItemsInside()));

	if ((!(*cit)->getContainer()) && item->getContainer()){
		if (getParentContainer()){
			item->getContainer()->setDeepness(getDeepness() + 1);
		}
		else{ //we only update deepness when a container get inside of another, so it means that deepness is not updated
			setDeepness(1);
			item->getContainer()->setDeepness(2);
		}
	}

	total_weight -= (*cit)->getWeight();
	total_weight += item->getWeight();

	if(Container* parent_container = getParentContainer()) {
		parent_container->updateItemWeight(-(*cit)->getWeight() + item->getWeight());
	}

	itemlist.insert(cit, item);
	item->setParent(this);

	//send change to client
	if(getParent()){
		const ItemType& oldType = Item::items[(*cit)->getID()];
		const ItemType& newType = Item::items[item->getID()];
		onUpdateContainerItem(index, *cit, oldType, item, newType);
	}

	(*cit)->setParent(NULL);
	itemlist.erase(cit);
}

void Container::__removeThing(Thing* thing, uint32_t count)
{
	Item* item = thing->getItem();
	if(item == NULL){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__removeThing] item == NULL" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}

	int32_t index = __getIndexOfThing(thing);
	if(index == -1){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__removeThing] index == -1" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}

	ItemList::iterator cit = std::find(itemlist.begin(), itemlist.end(), thing);
	if(cit == itemlist.end()){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__removeThing] item not found" << std::endl;
		DEBUG_REPORT
#endif
		return /*RET_NOTPOSSIBLE*/;
	}

	if(item->isStackable() && count != item->getItemCount()){
		uint8_t newCount = (uint8_t)std::max((int32_t)0, (int32_t)(item->getItemCount() - count));
		if(newCount == 0){
			updateAmountOfItems(-(int32_t)item->getTotalAmountOfItemsInside());
		}
		const double old_weight = -item->getWeight();
		item->setItemCount(newCount);
		const double diff_weight = old_weight + item->getWeight();
		total_weight += diff_weight;

		//send change to client
		if(getParent()){
			if(Container* parent_container = getParentContainer()) {
				parent_container->updateItemWeight(diff_weight);
			}
			const ItemType& it = Item::items[item->getID()];
			onUpdateContainerItem(index, item, it, item, it);
		}
	}
	else{
		//send change to client
		if(getParent()){
			if(Container* parent_container = getParentContainer()) {
				parent_container->updateItemWeight(-item->getWeight());
			}
			onRemoveContainerItem(index, item);
		}
		updateAmountOfItems(-(int32_t)item->getTotalAmountOfItemsInside());
		total_weight -= item->getWeight();
		item->setParent(NULL);
		itemlist.erase(cit);
	}
}

int32_t Container::__getIndexOfThing(const Thing* thing) const
{
	uint32_t index = 0;
	for(ItemList::const_iterator cit = getItems(); cit != getEnd(); ++cit){
		if(*cit == thing)
			return index;
		else
			++index;
	}

	return -1;
}

int32_t Container::__getFirstIndex() const
{
	return 0;
}

int32_t Container::__getLastIndex() const
{
	return size();
}

uint32_t Container::__getItemTypeCount(uint16_t itemId, int32_t subType /*= -1*/) const
{
	uint32_t count = 0;

	for(ItemList::const_iterator it = itemlist.begin(); it != itemlist.end(); ++it){
		if((*it)->getID() == itemId){
			count+= countByType(*it, subType);
		}
	}

	return count;
}

std::map<uint32_t, uint32_t>& Container::__getAllItemTypeCount(std::map<uint32_t, uint32_t>& countMap) const
{
	Item* item = NULL;
	for(ItemList::const_iterator it = itemlist.begin(); it != itemlist.end(); ++it){
		item = (*it);
		countMap[item->getID()] += item->getItemCount();
	}

	return countMap;
}

Thing* Container::__getThing(uint32_t index) const
{
	if(index < 0 || index > size())
		return NULL;

	uint32_t count = 0;
	for(ItemList::const_iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit){
		if(count == index)
			return *cit;
		else
			++count;
	}

	return NULL;
}

void Container::postAddNotification(Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link /*= LINK_OWNER*/, bool isNewItem /*=true*/)
{
	Cylinder* topParent = getTopParent();

	if(topParent->getCreature()){
		topParent->postAddNotification(thing, oldParent, index, LINK_TOPPARENT, isNewItem);
	}
	else{
		if(topParent == this){
			//let the tile class notify surrounding players
			if(topParent->getParent()){
				topParent->getParent()->postAddNotification(thing, oldParent, index, LINK_NEAR, isNewItem);
			}
		}
		else{
			topParent->postAddNotification(thing, oldParent, index, LINK_PARENT, isNewItem);
		}
	}
}

void Container::postRemoveNotification(Thing* thing,  const Cylinder* newParent, int32_t index, bool isCompleteRemoval, cylinderlink_t link /*= LINK_OWNER*/)
{
	Cylinder* topParent = getTopParent();

	if(topParent->getCreature()){
		topParent->postRemoveNotification(thing, newParent, index, isCompleteRemoval, LINK_TOPPARENT);
	}
	else{
		if(topParent == this){
			//let the tile class notify surrounding players
			if(topParent->getParent()){
				topParent->getParent()->postRemoveNotification(thing, newParent, index, isCompleteRemoval, LINK_NEAR);
			}
		}
		else{
			topParent->postRemoveNotification(thing, newParent, index, isCompleteRemoval, LINK_PARENT);
		}
	}
}

void Container::__internalAddThing(Thing* thing)
{
	__internalAddThing(0, thing);
}

void Container::__internalAddThing(uint32_t index, Thing* thing)
{
#ifdef __DEBUG__MOVESYS__NOTICE
	std::cout << "[Container::__internalAddThing] index: " << index << std::endl;
#endif

	Item* item = thing->getItem();
	if(item == NULL){
#ifdef __DEBUG__MOVESYS__
		std::cout << "Failure: [Container::__internalAddThing] item == NULL" << std::endl;
#endif
		return;
	}

	/*
	if(index < 0 || index >= capacity()){
#ifdef __DEBUG__
		std::cout << "Failure: [Container::__internalAddThing] - index is out of range" << std::endl;
#endif
		return;
	}
	*/

	item->setParent(this);
	itemlist.push_front(item);

	updateAmountOfItems(item->getTotalAmountOfItemsInside());

	total_weight += item->getWeight();
	Container* parent_container = getParentContainer();
	if(parent_container){
		parent_container->updateItemWeight(item->getWeight());
	}

	if (item->getContainer()){
		if (parent_container)
			item->getContainer()->setDeepness(getDeepness() + 1);
		else{ //we only update deepness when a container gets inside of another, so it means that deepness is not updated
			setDeepness(1);
			item->getContainer()->setDeepness(2);
		}
	}
}

void Container::__startDecaying()
{
	for(ItemList::const_iterator it = itemlist.begin(); it != itemlist.end(); ++it){
		(*it)->__startDecaying();
	}
}


ContainerIterator Container::begin()
{
	ContainerIterator cit(this);
	if(!itemlist.empty()){
		cit.over.push(this);
		cit.cur = itemlist.begin();
	}

	return cit;
}

ContainerIterator Container::end()
{
	ContainerIterator cit(this);
	return cit;
}

// Very evil constructors, look away if you are sensitive!
ContainerIterator Container::begin() const
{
	Container* evil = const_cast<Container*>(this);
	return evil->begin();
}

ContainerIterator Container::end() const
{
	Container* evil = const_cast<Container*>(this);
	return evil->end();
}

ContainerIterator::ContainerIterator():
	super(NULL) {}

ContainerIterator::ContainerIterator(Container* super):
	super(super) {}

ContainerIterator::~ContainerIterator() {}

ContainerIterator::ContainerIterator(const ContainerIterator& rhs):
	super(rhs.super), over(rhs.over), cur(rhs.cur) {}

bool ContainerIterator::operator==(const ContainerIterator& rhs)
{
	return !(*this != rhs);
}

bool ContainerIterator::operator!=(const ContainerIterator& rhs)
{
	assert(super);
	if(super != rhs.super)
		return true;

	if(over.empty() && rhs.over.empty())
		return false;

	if(over.empty())
		return true;

	if(rhs.over.empty())
		return true;

	if(over.front() != rhs.over.front())
		return true;

	return cur != rhs.cur;
}

ContainerIterator& ContainerIterator::operator=(const ContainerIterator& rhs)
{
	this->super = rhs.super;
	this->cur = rhs.cur;
	this->over = rhs.over;
	return *this;
}

Item* ContainerIterator::operator*()
{
	assert(super);
	return *cur;
}

Item* ContainerIterator::operator->()
{
	return *(*this);
}

ContainerIterator& ContainerIterator::operator++()
{
	assert(super);
	if(Item* i = *cur){
		Container* c = i->getContainer();
		if(c && !c->empty()){
			over.push(c);
		}
	}

	++cur;
	if(cur == over.front()->itemlist.end()){
		over.pop();
		if(over.empty()){
			return *this;
		}

		cur = over.front()->itemlist.begin();
	}

	return *this;
}

ContainerIterator ContainerIterator::operator++(int)
{
	ContainerIterator tmp(*this);
	++*this;
	return tmp;
}
