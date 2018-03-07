class Attacker {
	public :
   float power; // hit points given
   float dexterity; //affects chance to hit
   Actor *weapon;
   bool isEquipped;

   Attacker(float power, float dexterity);
   void attack(Actor *owner, Actor *target);
   float buff(float amount, Stat stat);
   Actor* equip(Actor *owner);
};