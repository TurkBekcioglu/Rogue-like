class Buffer : public Pickable {
	public :
   float amount; // how many of the stat
   Stat stat;

   Buffer(float amount, Stat stat);
   ~Buffer() {};
   bool use(Actor *owner, Actor *wearer);
   float getStat(Stat stat);
};