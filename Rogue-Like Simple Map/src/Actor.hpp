class Actor {
	public :
   int x,y; // position on map
   int ch; // ascii code
   TCODColor col; // color
   const char *name; // the actor's name
   const char *description; // the actor's description
   bool blocks; // can we walk on this actor?
   bool fovOnly; // only display when in fov
   Attacker *attacker; // something that deals damage
   Destructible *destructible; // something that can be damaged
   Ai *ai; // something self-updating
   Pickable *pickable; // something that can be picked and used
   Container *container; // something that can contain actors

   Actor(int x, int y, int ch, const char *name,
   	const TCODColor &col);
   Actor();
   ~Actor();
   void update();
   void render() const;
   Actor operator=(Actor actor);
};
