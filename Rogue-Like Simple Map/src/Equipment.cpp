#include "main.hpp"

Equipment::Equipment(float attack, float defense, float dexterity, float luck, EType type) :
 attack(attack),defense(defense),dexterity(dexterity),luck(luck),type(type) {
}

bool Equipment::use(Actor *owner, Actor *wearer) {
	if (type == WEAPON && wearer->attacker) {
		Actor * tmp;
		tmp = wearer->attacker->equip(owner);
		wearer->container->remove(owner);
		wearer->container->add(tmp);
	} else if (type == ARMOR && wearer->destructible) {
		Actor * tmp;
		tmp = wearer->destructible->equip(owner);
		wearer->container->remove(owner);
		wearer->container->add(tmp);
	}
	return true;
}

float Equipment::getStat(Stat stat) {
	switch(stat) {
		case HP:
		return 0;

		case MHP:
		return 0;

		case ATTACK:
		return attack;

		case DEFENSE:
		return defense;

		case DEXTERITY:
		return dexterity;

		case LUCK:
		return luck;

		default:
		return 0;
	}
}