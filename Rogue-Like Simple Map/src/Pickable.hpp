class Pickable {
	public :
	virtual ~Pickable() {};
	bool pick(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
	virtual float getStat(Stat stat);

	void drop(Actor *owner, Actor *wearer);
};