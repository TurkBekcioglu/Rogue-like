#include <stdio.h>
#include "main.hpp"

Attacker::Attacker(float power, float dexterity) : power(power),dexterity(dexterity),isEquipped(false) {
}

void Attacker::attack(Actor *owner, Actor *target) {
	TCODRandom *rng=TCODRandom::getInstance();
	int dodgeChance = (owner->attacker->dexterity - target->destructible->luck) * 5;
	int dodge =  rng->getInt(0,100);
	if (dodge <= 10 + dodgeChance) {
		engine.gui->message(owner==engine.player ? TCODColor::red : TCODColor::lightGrey,
			"%s attacks %s, but misses.", owner->name, target->name);
	} else {
		if ( target->destructible && ! target->destructible->isDead() ) {
			int damage = rng->getInt(power - power*0.25, power + power*0.25);
			if ( damage - target->destructible->defense > 0 ) {
				engine.gui->message(owner==engine.player ? TCODColor::red : TCODColor::lightGrey,
					"%s attacks %s for %g hit points.", owner->name, target->name,
					damage - target->destructible->defense);
			} else {
				engine.gui->message(TCODColor::lightGrey,
					"%s attacks %s but it has no effect!", owner->name, target->name);            
			}
			target->destructible->takeDamage(target, (float)damage);
		} else {
			engine.gui->message(TCODColor::lightGrey,
				"%s attacks %s in vain.",owner->name,target->name);
		}
	}
}

float Attacker::buff(float amount, Stat stat) {
  switch(stat) {
    case 0:
    return 0;

    case 1:
    return 0;

    case 2:
    power += amount;
    return amount;

    case 3:
    return 0;

    case 4:
    dexterity += amount;
    return amount;

    case 5:
    return 0;

    default:
    return 0;
  }
}

Actor* Attacker::equip(Actor *owner) {
	Actor * tmp;
	if(isEquipped) {
		power -= weapon->pickable->getStat(ATTACK);
		dexterity -= weapon->pickable->getStat(DEXTERITY);
		tmp = weapon;
	  weapon = owner;
		power += weapon->pickable->getStat(ATTACK);
		dexterity += weapon->pickable->getStat(DEXTERITY);
	} else {
		tmp = nullptr;
		weapon = owner;
		power += weapon->pickable->getStat(ATTACK);
		dexterity += weapon->pickable->getStat(DEXTERITY);
	}
	isEquipped = true;
	return tmp;
}
