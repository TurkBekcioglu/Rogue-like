#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP),
player(NULL),map(NULL),fovRadius(10),
screenWidth(screenWidth),screenHeight(screenHeight),level(1) {
	TCODConsole::initRoot(screenWidth,screenHeight,"libtcod C++ tutorial",false);
	player = new Actor(40,25,'@',"player",TCODColor::white);
	player->description= "you";
	player->destructible=new PlayerDestructible(30, 2, 5, "your cadaver");
	player->attacker=new Attacker(5, 5);
	player->container = new Container(26);
	player->ai = new PlayerAi();
	actors.push(player);

	stairs = new Actor(0,0,'>',"stairs",TCODColor::white);
	stairs->blocks=false;
	stairs->fovOnly=false;
	actors.push(stairs);

	// add boss
	boss = new Actor(0, 0, 'B', "burrito",
		TCODColor::red);
	boss->destructible = new MonsterDestructible(20 + level, 4 + level, 4 + level, "half eaten burrito");
	boss->attacker = new Attacker(5 + level, 4 + level);
	boss->ai = new MonsterAi();
	engine.actors.push(boss);



	map = new Map(80,43);
	gui = new Gui();
	gui->message(TCODColor::red,
		"Welcome stranger!\nI hope your hungry because breakfast is served!");
}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
	delete gui;
}

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
	gameStatus=IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
	player->update();
	if ( gameStatus == NEW_TURN ) {
		for (Actor **iterator=actors.begin(); iterator != actors.end();
			iterator++) {
			Actor *actor=*iterator;
		if ( actor != player ) {
			actor->update();
		}
	}
}
}

void Engine::render() {
	TCODConsole::root->clear();
	 // draw the map
	map->render();

	// draw the actors
	for (Actor **iterator=actors.begin(); iterator != actors.end();
		iterator++) {
		Actor *actor=*iterator;
	if ( actor != player && ((!actor->fovOnly && map->isExplored(actor->x,actor->y))
		|| map->isInFov(actor->x,actor->y)) ) {
		actor->render();
}
}

player->render();

	// show the player's stats
gui->render();
TCODConsole::root->print(1,screenHeight-3, "ATK:  %d",
	(int)player->attacker->power);
TCODConsole::root->print(1,screenHeight-2, "DEF:  %d",
	(int)player->destructible->defense);
TCODConsole::root->print(10,screenHeight-3, "DEX:  %d",
	(int)player->attacker->dexterity);
TCODConsole::root->print(10,screenHeight-2, "LUCK: %d",
	(int)player->destructible->luck);

}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor,0);
}

void Engine::nextLevel() {
	level++;
	gui->message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp/2);
	gui->message(TCODColor::red,"After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");
	delete map;
	 // delete all actors but player and stairs
	for (Actor **it=actors.begin(); it!=actors.end(); it++) {
		if ( *it != player && *it != stairs && *it != boss) {
			delete *it;
			it = actors.remove(it);
			boss->ch = 'B';
			boss->name = "burrito";
			boss->destructible->hp=boss->destructible->maxHp;
		}
	}
			// create a new map
	map = new Map(80,43);
	 //map->init(true);
	gameStatus=STARTUP;
}