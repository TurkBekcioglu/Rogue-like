#include <stdio.h>
#include "main.hpp"

Destructible::Destructible(float maxHp, float defense, float luck, const char *corpseName) :
maxHp(maxHp),hp(maxHp),defense(defense),luck(luck),corpseName(corpseName),isEquipped(false) {
}

float Destructible::takeDamage(Actor *owner, float damage) {
	damage -= defense;
	if ( damage > 0 ) {
		hp -= damage;
		if ( hp <= 0 ) {
			hp = 0;
			die(owner);
		}
	} else {
		damage=0;
	}
	return damage;
}

void Destructible::die(Actor *owner) {
   // transform the actor into a corpse!
	owner->ch='%';
	owner->col=TCODColor::darkRed;   
	owner->name=corpseName;
	owner->blocks=false;
   // make sure corpses are drawn before living actors
	engine.sendToBack(owner);
}



MonsterDestructible::MonsterDestructible(float maxHp, float defense, float luck, const char *corpseName) :
Destructible(maxHp,defense,luck,corpseName) {
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, float luck, const char *corpseName) :
Destructible(maxHp,defense,luck,corpseName) {
}

void MonsterDestructible::die(Actor *owner) {
   // transform it into a nasty corpse! it doesn't block, can't be
   // attacked and doesn't move
	engine.gui->message(TCODColor::lightGrey,"%s is dead",owner->name);
	Destructible::die(owner);
}

void PlayerDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::red,"You died!");
	Destructible::die(owner);
	engine.gameStatus=Engine::DEFEAT;
}

float Destructible::heal(float amount) {
	hp += (int)amount;
	if ( hp > maxHp ) {
		amount -= hp-maxHp;
		hp=maxHp;
	}
	if ( hp <= 0) {
		hp=1;
	}
	if (amount < 0) {
		return -amount;
	}
	return amount;
}

float Destructible::buff(float amount, Stat stat) {
	switch(stat) {
		case 0:
		return heal(amount);

		case 1:
		maxHp += amount;
		return amount;

		case 2:
		return 1;

		case 3:
		defense += amount;
		return amount;

		case 4:
		return 1;

		case 5:
		luck += amount;
		return amount;

		default:
		return 1;
	}
}

Actor* Destructible::equip(Actor *owner) {
	Actor * tmp;
	if(isEquipped) {
		defense -= armor->pickable->getStat(DEFENSE);
		luck -= armor->pickable->getStat(LUCK);
		tmp = armor;
	  	armor = owner;
		defense += armor->pickable->getStat(DEFENSE);
		luck += armor->pickable->getStat(LUCK);
	} else {
		tmp = nullptr;
		armor = owner;
		defense += armor->pickable->getStat(DEFENSE);
		luck += armor->pickable->getStat(LUCK);
	}
	isEquipped = true;
	return tmp;
}