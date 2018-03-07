#include "main.hpp"

Buffer::Buffer(float amount, Stat stat) : amount(amount),stat(stat) {
}

bool Buffer::use(Actor *owner, Actor *wearer) {
	float amountBuffed = 0;
	if ( wearer->attacker ) {
		amountBuffed = wearer->attacker->buff(amount, stat);
	}
	if ( amountBuffed != 0 ) {
		
		return Pickable::use(owner,wearer);
	} else if ( wearer->destructible ) {
		amountBuffed = wearer->destructible->buff(amount, stat);
		if ( amountBuffed != 0 ) {
			return Pickable::use(owner,wearer);
		}
	}
	return false;
}

float Buffer::getStat(Stat stat1) {
	if(stat1 == stat) {
		return amount;
	} else {
		return 0;
	}
}