class Engine {
	public :

	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;


	TCODList<Actor *> actors;
	Actor *player;
	Actor *stairs;
	Actor *boss;
	Map *map;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	Gui *gui;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	int level;
    void nextLevel();

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void update();
	void render();
	void sendToBack(Actor *actor);

};

extern Engine engine;
