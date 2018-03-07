class Destructible {
	public :
   float maxHp; // maximum health points
   float hp; // current health points
   float defense; // hit points deflected
   float luck; //affects chance to dodge
   const char *corpseName; // the actor's name once dead/destroyed
   Actor * armor;
   bool isEquipped;

   Destructible(float maxHp, float defense, float luck, const char *corpseName);
   inline bool isDead() { return hp <= 0; }

   virtual ~Destructible() {};

   float takeDamage(Actor *owner, float damage);

   virtual void die(Actor *owner);

   float heal(float amount);

   float buff(float amount, Stat stat);

   Actor* equip(Actor *owner);
};

class MonsterDestructible : public Destructible {
	public :
	MonsterDestructible(float maxHp, float defense, float luck, const char *corpseName);
	void die(Actor *owner);
};

class PlayerDestructible : public Destructible {
	public :
	PlayerDestructible(float maxHp, float defense, float luck, const char *corpseName);
	void die(Actor *owner);
};