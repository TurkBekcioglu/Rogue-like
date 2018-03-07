class Equipment : public Pickable {
	public :
   float attack;
   float defense;
   float dexterity;
   float luck;
   EType type;

   Equipment(float attack, float defense, float dexterity, float luck, EType type);
   ~Equipment(){};
   bool use(Actor *owner, Actor *wearer);
   float getStat(Stat stat);
};