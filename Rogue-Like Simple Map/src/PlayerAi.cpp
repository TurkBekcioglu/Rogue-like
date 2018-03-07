#include "stdio.h"
#include "main.hpp"

void PlayerAi::update(Actor *owner) {
	if ( owner->destructible && owner->destructible->isDead() ) {
		return;
	}
	int dx=0,dy=0;
	switch(engine.lastKey.vk) {
		case TCODK_UP : dy=-1; break;
		case TCODK_DOWN : dy=1; break;
		case TCODK_LEFT : dx=-1; break;
		case TCODK_RIGHT : dx=1; break;
		case TCODK_CHAR : handleActionKey(owner, engine.lastKey.c); break;
		default:break;
	}
	if (dx != 0 || dy != 0) {
		engine.gameStatus=Engine::NEW_TURN;
		if (moveOrAttack(owner, owner->x+dx,owner->y+dy)) {
			engine.map->computeFov();
		}
	}
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx,int targety) {
	if ( engine.map->isWall(targetx,targety) ) return false;
   // look for living actors to attack
	for (Actor **iterator=engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor=*iterator;
	if ( actor->destructible && !actor->destructible->isDead()
		&& actor->x == targetx && actor->y == targety ) {
		owner->attacker->attack(owner, actor);
	return false;
}
}
  // look for corpses or items
for (Actor **iterator=engine.actors.begin();
	iterator != engine.actors.end(); iterator++) {
	Actor *actor=*iterator;
bool corpseOrItem=(actor->destructible && actor->destructible->isDead())
|| actor->pickable;
if ( corpseOrItem
	&& actor->x == targetx && actor->y == targety ) {
	engine.gui->message(TCODColor::lightGrey,"There's a %s here.",actor->name);
}
}
owner->x=targetx;
owner->y=targety;
return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
	switch(ascii) {
		case 'e' :
		{
			Actor *actor=showEquipment(owner);
			if (actor) {
				engine.gameStatus=Engine::NEW_TURN;
			}
		}
		break;

		case '>' :
		if ( engine.stairs->x == owner->x && engine.stairs->y == owner->y ) {
			engine.nextLevel();
		} else {
			engine.gui->message(TCODColor::lightGrey,"There are no stairs here.");
		}
		break;
   case 'i' : // display inventory
   {
   	Actor *actor=choseFromInventory(owner);
   	if ( actor ) {
   		actor->pickable->use(actor,owner);
   		engine.gameStatus=Engine::NEW_TURN;
   	}
   }
   break;
   case 'd' : // drop item 
   {
   	Actor *actor=choseFromInventory(owner);
   	if ( actor ) {
   		actor->pickable->drop(actor,owner);
   		engine.gameStatus=Engine::NEW_TURN;
   	}           
   }
   break;
   case 'g' : // pickup item
   {
   	bool found=false;
   	for (Actor **iterator=engine.actors.begin();
   		iterator != engine.actors.end(); iterator++) {
   		Actor *actor=*iterator;
   	if ( actor->pickable && actor->x == owner->x && actor->y == owner->y ) {
   		if (actor->pickable->pick(actor,owner)) {
   			found=true;
   			engine.gui->message(TCODColor::lightGrey,"You pick up the %s.",
   				actor->name);
   			break;
   		} else if (! found) {
   			found=true;
   			engine.gui->message(TCODColor::red,"Your inventory is full.");
   		}
   	}
   }
   if (!found) {
   	engine.gui->message(TCODColor::lightGrey,"There's nothing here that you can pick up.");
   }
   engine.gameStatus=Engine::NEW_TURN;
}
break;
}
}

Actor *PlayerAi::choseFromInventory(Actor *owner) {
	static const int INVENTORY_WIDTH=50;
	static const int INVENTORY_HEIGHT=28;
	static TCODConsole con(INVENTORY_WIDTH,INVENTORY_HEIGHT);
   // display the inventory frame
	con.setDefaultForeground(TCODColor(200,180,50));
	con.printFrame(0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,true,
		TCOD_BKGND_DEFAULT,"inventory");
	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut='a';
	int y=1;
	for (Actor **it=owner->container->inventory.begin();
		it != owner->container->inventory.end(); it++) {
		Actor *actor=*it;
	con.print(2,y,"(%c) %s: %s", shortcut, actor->name, actor->description);
	y++;
	shortcut++;
}
	// blit the inventory console on the root console
TCODConsole::blit(&con, 0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,
	TCODConsole::root, engine.screenWidth/2 - INVENTORY_WIDTH/2,
	engine.screenHeight/2-INVENTORY_HEIGHT/2);
TCODConsole::flush();
	// wait for a key press
TCOD_key_t key;
TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
if ( key.vk == TCODK_CHAR ) {
	int actorIndex=key.c - 'a';
	if ( actorIndex >= 0 && actorIndex < owner->container->inventory.size() ) {
		return owner->container->inventory.get(actorIndex);
	}
}
return NULL;
}


Actor *PlayerAi::showEquipment(Actor *owner) {
	static const int INVENTORY_WIDTH=50;
	static const int INVENTORY_HEIGHT=28;
	static TCODConsole con(INVENTORY_WIDTH,INVENTORY_HEIGHT);

	con.setDefaultForeground(TCODColor(200,180,50));
	con.printFrame(0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,true,
		TCOD_BKGND_DEFAULT,"equipment");
	// display the items
	con.setDefaultForeground(TCODColor::white);

	if (owner->attacker->isEquipped) {
		Actor *equip = owner->attacker->weapon;
		con.print(2,2,"WEAPON: %s\n\nATK: %i DEX: %i",
			equip->name, (int)equip->pickable->getStat(ATTACK),(int)equip->pickable->getStat(DEXTERITY));
	} else {
		con.print(2,2,"WEAPON: none");
	}

	if (owner->destructible->isEquipped) {
		Actor *equip = owner->destructible->armor;
		con.print(2,7,"ARMOR: %s\n\nDEF: %i LUCK: %i",
			equip->name, (int)equip->pickable->getStat(DEFENSE),(int)equip->pickable->getStat(LUCK));
	}
	else {
		con.print(2,7,"ARMOR: none");
	}

	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,
		TCODConsole::root, engine.screenWidth/2 - INVENTORY_WIDTH/2,
		engine.screenHeight/2-INVENTORY_HEIGHT/2);
	TCODConsole::flush();
	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
	if ( key.vk == TCODK_CHAR ) {
		int actorIndex=key.c - 'a';
		if ( actorIndex >= 0 && actorIndex < owner->container->inventory.size() ) {
			return owner->container->inventory.get(actorIndex);
		}
	}
	return NULL;
}
