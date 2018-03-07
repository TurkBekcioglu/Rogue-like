#include "main.hpp"
#include <algorithm>
#include <iostream>

static const int ROOM_MAX_SIZE = 10;
static const int ROOM_MIN_SIZE = 5;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;
static const int MAX_ROOM_EQUIPS = 1;

class BspListener : public ITCODBspCallback {
  private :
   Map &map; // a map to dig
   int roomNum; // room number
   int lastx,lasty; // center of the last room

   public :
   BspListener(Map &map) : map(map), roomNum(0) {}

   bool visitNode(TCODBsp *node, void *userData) {
     if ( node->isLeaf() ) {
      int x,y,w,h;
// dig a room
      TCODRandom *rng=TCODRandom::getInstance();
      w=rng->getInt(ROOM_MIN_SIZE, node->w-2);
      h=rng->getInt(ROOM_MIN_SIZE, node->h-2);
      x=rng->getInt(node->x+1, node->x+node->w-w-1);
      y=rng->getInt(node->y+1, node->y+node->h-h-1);
      map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1);

      if ( roomNum != 0 ) {
   // dig a corridor from last room
       //map.dig(lastx,lasty,x+w/2,lasty);
       //map.dig(x+w/2,lasty,x+w/2,y+h/2);
     }

     lastx=x+w/2;
     lasty=y+h/2;
     roomNum++;
   }

   return true;
 }
};

Map::Map(int width, int height) : width(width),height(height) {
 tiles=new Tile[width*height];
 map=new TCODMap(width,height);
 TCODBsp bsp(0,0,width,height);
 bsp.splitRecursive(NULL,8,ROOM_MAX_SIZE,ROOM_MAX_SIZE,1.5f,1.5f);
 BspListener listener(*this);
 bsp.traverseInvertedLevelOrder(&listener,NULL);

    // Fill in all of the empty space with mazes.
    for (int y = 1; y < height; y += 2) {
      for (int x = 1; x < width; x += 2) {
        if (map->isWalkable(x, y)) continue;
        createMaze(x, y);
      }


}

 createMaze(1, 1);
}

Map::~Map() {
 delete [] tiles;
 delete map;
}

bool Map::isWall(int x, int y) const {
 return !map->isWalkable(x,y);
}

bool Map::canWalk(int x, int y) const {
 if (isWall(x,y)) {
       // this is a wall
   return false;
 }
 for (Actor **iterator=engine.actors.begin();
   iterator!=engine.actors.end();iterator++) {
   Actor *actor = *iterator;
 if ( actor->blocks && actor->x == x && actor->y == y ) {
       // there is a blocking actor here. cannot walk
  return false;
}
}
return true;
}


bool Map::isInFov(int x, int y) const {
 if ( x < 0 || x >= width || y < 0 || y >= height ) {
   return false;
 }
 if ( map->isInFov(x,y) ) {
   tiles[x+y*width].explored=true;
   return true;
 }
 return false;
}

void Map::computeFov() {
 map->computeFov(engine.player->x,engine.player->y,
   engine.fovRadius);
}

bool Map::isExplored(int x, int y) const {
 return tiles[x+y*width].explored;
}

void Map::render() const {
 static const TCODColor darkWall(0,0,100);
 static const TCODColor darkGround(50,50,150);
 static const TCODColor lightWall(130,110,50);
 static const TCODColor lightGround(200,180,50);

 for (int x=0; x < width; x++) {
   for (int y=0; y < height; y++) {
     if ( isInFov(x,y) ) {
       TCODConsole::root->setCharBackground(x,y,
         isWall(x,y) ? lightWall :lightGround );
     } else if ( isExplored(x,y) ) {
       TCODConsole::root->setCharBackground(x,y,
         isWall(x,y) ? darkWall : darkGround );
     }
   }
 }
}

void Map::dig(int x1, int y1, int x2, int y2) {
 if ( x2 < x1 ) {
   int tmp=x2;
   x2=x1;
   x1=tmp;
 }
 if ( y2 < y1 ) {
   int tmp=y2;
   y2=y1;
   y1=tmp;
 }
 for (int tilex=x1; tilex <= x2; tilex++) {
  for (int tiley=y1; tiley <= y2; tiley++) {
    map->setProperties(tilex,tiley,true,true);
  }
}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
 dig (x1,y1,x2,y2);
 TCODRandom *rng=TCODRandom::getInstance();
 if ( first ) {
       // put the player in the first room
   engine.player->x=(x1+x2)/2;
   engine.player->y=(y1+y2)/2;
 } else {
  int nbMonsters=rng->getInt(0,MAX_ROOM_MONSTERS);
  while (nbMonsters > 0) {
    int x=rng->getInt(x1,x2);
    int y=rng->getInt(y1,y2);
    if ( canWalk(x,y) ) {
      addMonster(x,y);
    }
    nbMonsters--;
  }
}
  // add items
int nbItems=rng->getInt(0,MAX_ROOM_ITEMS);
while (nbItems > 0) {
  int x=rng->getInt(x1,x2);
  int y=rng->getInt(y1,y2);
  if ( canWalk(x,y) ) {
    addItem(x,y);
  }
  nbItems--;
}
// add equipment
int nbEquips=rng->getInt(0,MAX_ROOM_EQUIPS);
while (nbEquips > 0) {
  int x=rng->getInt(x1,x2);
  int y=rng->getInt(y1,y2);
  if ( canWalk(x,y) ) {
    addEquip(x,y);
  }
  nbEquips--;
}
// set stairs position
engine.stairs->x=(x1+x2)/2;
engine.stairs->y=(y1+y2)/2;

//set boss position
engine.boss->x=(x1+x2)/2 + 1;
engine.boss->y=(y1+y2)/2 + 1;
}

void Map::createMaze(int x, int y) {

  std::vector<std::vector<int>> cells;
  std::vector<int> cCell;
  std::vector<int> prevDir;

  map->setProperties(x,y,true,true);
  cells.push_back((const std::vector<int>){x, y});

  while (!cells.empty()) {
    cCell = cells.back();

    std::vector<std::vector<int>> dirs = {(const std::vector<int>){1, 0}, (const std::vector<int>){-1, 0}, (const std::vector<int>){0, 1}, (const std::vector<int>){0, -1}};
    std::vector<std::vector<int>> adjacentCells;

    for (std::vector<int> dir : dirs) {
      if (mazeCheck(cCell, dir)) {
        adjacentCells.push_back(dir);
      }
    }

    if (!adjacentCells.empty()) {

      std::vector<int> dir;

      TCODRandom *rng=TCODRandom::getInstance();

      if ((std::find(adjacentCells.begin(), adjacentCells.end(), prevDir) != adjacentCells.end()) && rng->getInt(0, 100) > 40) {
        dir = prevDir;
      }
      else {
        int randomIndex = rng->getInt(0, adjacentCells.size() - 1);
        dir = adjacentCells.at(randomIndex);
      }

      dig(cCell.at(0) + dir.at(0), cCell.at(1) + dir.at(1), cCell.at(0) + dir.at(0) * 2, cCell.at(1) + dir.at(1) * 2);

      cells.push_back((const std::vector<int>){cCell.at(0) + dir.at(0) * 2, cCell.at(1) + dir.at(1) * 2});
      prevDir = dir;
    }
    else {
      cells.pop_back();
      //dummy vector
      prevDir = (const std::vector<int>){0, 0};
    }

    std::cout << cCell[0] << " " << cCell[1] << std::endl;
  }
}

bool Map::mazeCheck(std::vector<int> vec, std::vector<int> direction) {
  if ((vec[0] + direction[0] * 3 < this->width && vec[1] + direction[1] * 3 < this->height) &&
    (vec[0] + direction[0] * 3 >= 0 && vec[1] + direction[1] * 3 >= 0)) {
    return isWall(vec[0] + direction[0] * 2, vec[1] + direction[1] * 2);
  }
  return false;
}

void Map::addMonster(int x, int y) {
	int scale = engine.level;
	TCODRandom *rng=TCODRandom::getInstance();
	int randVal = rng->getInt(0,100); 
	if ( randVal < 50 ) {
   // create a egg
		Actor *egg = new Actor(x,y,'&',"Egg",
			TCODColor::yellow);
		egg->destructible = new MonsterDestructible(7 + scale, 0 + scale, 3 + scale, "dead egg");
		egg->attacker = new Attacker(2 + scale, 4 + scale);
		egg->ai = new MonsterAi();
		engine.actors.push(egg);
	} else if (randVal < 60) {
   // create a waffle
		Actor *waffle = new Actor(x,y,'#',"Waffle",
			TCODColor::darkerYellow);
		waffle->destructible = new MonsterDestructible(15 + scale, 0 + scale, 4 + scale, "waffle carcass");
		waffle->attacker = new Attacker(3 + scale, 4 + scale);
		waffle->ai = new MonsterAi();
		engine.actors.push(waffle);
	} else if (randVal < 90) {
		Actor *bacon = new Actor(x,y,'$',"Bacon",
			TCODColor::darkerRed);
		bacon->destructible = new MonsterDestructible(17 + scale, 1 + scale, 5 + scale, "slaughtered bacon");
		bacon->attacker = new Attacker(3 + scale, 4 + scale);
		bacon->ai = new MonsterAi();
		engine.actors.push(bacon);
	} else {
		Actor *pancake = new Actor(x,y,'O',"Pancake",
			TCODColor::darkerOrange);
		pancake->destructible = new MonsterDestructible(15 + scale, 1 + scale, 5 + scale, "pancake corpse");
		pancake->attacker = new Attacker(3 + scale, 3 + scale);
		pancake->ai = new MonsterAi();
		engine.actors.push(pancake);

	}
}

void Map::addItem(int x, int y) {
  TCODRandom *rng=TCODRandom::getInstance();
  int randVal =  rng->getInt(0,100);
  if(randVal < 55) {
    Actor *healthPotion=new Actor(x,y,'!',"health potion",
    	TCODColor::violet);
    float amount = engine.player->destructible->maxHp*0.15;
    healthPotion->description= "recover 15 percent of HP";
    healthPotion->blocks=false;
    healthPotion->pickable=new Buffer(amount, HP);
    engine.actors.push(healthPotion);
  } else if(randVal <= 60) {
  	Actor *defensePotion=new Actor(x,y,'!',"defense potion",
  		TCODColor::darkerOrange);
  	defensePotion->description= "increase DEF by 1";
    defensePotion->blocks=false;
    defensePotion->pickable=new Buffer(1, DEFENSE);
    engine.actors.push(defensePotion);
  } else if(randVal < 70) {
  	Actor *attackPotion=new Actor(x,y,'!',"attack potion",
  		TCODColor::red);
  	attackPotion->description= "increase ATK by 1";
    attackPotion->blocks=false;
    attackPotion->pickable=new Buffer(1, ATTACK);
    engine.actors.push(attackPotion);
  } else if(randVal < 80) {
  	Actor *dexterityPotion=new Actor(x,y,'!',"dexterity potion",
  		TCODColor::blue);
  	dexterityPotion->description= "increase DEX by 1";
    dexterityPotion->blocks=false;
    dexterityPotion->pickable=new Buffer(1, DEXTERITY);
    engine.actors.push(dexterityPotion);
  } else if(randVal < 90) {
  	Actor *vigorPotion=new Actor(x,y,'!',"vigor potion",
  		TCODColor::green);
  	vigorPotion->description= "increase MAX HP by 5";
    vigorPotion->blocks=false;
    vigorPotion->pickable=new Buffer(5, MHP);
    engine.actors.push(vigorPotion);
  } else if(randVal <= 95) {
  	Actor *luckPotion=new Actor(x,y,'!',"luck potion",
  		TCODColor::white);
  	luckPotion->description= "increase LUCK by by 1";
    luckPotion->blocks=false;
    luckPotion->pickable=new Buffer(1, LUCK);
    engine.actors.push(luckPotion);
  }	else {
  	int scale = engine.level;
  	int amount = rng->getInt(-scale, scale);
  	if(amount == 0) {
  		amount = 1;
  	}
  	Actor *mysteryPotion=new Actor(x,y,'!',"mystery potion",
  		TCODColor::black);
  	mysteryPotion->description= "increase/decrease a random stat";
    mysteryPotion->blocks=false;
    int type = rng->getInt(0,60);
    if(type >= 50) {
    	mysteryPotion->pickable=new Buffer(amount, HP);
    } else if(type >= 40) {
    	mysteryPotion->pickable=new Buffer(amount, MHP);
    } else if(type >= 30) {
    	mysteryPotion->pickable=new Buffer(amount, ATTACK);
    } else if(type >= 20) {
    	mysteryPotion->pickable=new Buffer(amount, DEFENSE);
    } else if(type >= 10) {
    	mysteryPotion->pickable=new Buffer(amount, DEXTERITY);
    } else if (type < 10) {
    	mysteryPotion->pickable=new Buffer(amount, LUCK);
    }
    engine.actors.push(mysteryPotion);
  }
}

void Map::addEquip(int x, int y) {
	int scale = engine.level;
	TCODRandom *rng=TCODRandom::getInstance();
	if ( rng->getInt(0,100) < 30) {

		TCODRandom *rng2=TCODRandom::getInstance();
		if (rng2->getInt(0,100) < 65) {
		Actor *armor = new Actor(x,y,'T',"Plate mail",
			TCODColor::darkViolet);
		armor->description= "Equip to increase ATK/DEX";
		armor->blocks=false;
    armor->pickable=new Equipment(0, 1+scale, 0, 1+scale, ARMOR);
    engine.actors.push(armor);
  } else if ( rng2->getInt(0,100) < 85) {
		Actor *armor = new Actor(x,y,'T',"Super mail",
			TCODColor::violet);
		armor->description= "Equip to increase ATK/DEX";
		armor->blocks=false;
    armor->pickable=new Equipment(0, 2+scale, 0, 2+scale, ARMOR);
    engine.actors.push(armor);
  } else {
  	Actor *armor = new Actor(x,y,'T',"Mega mail",
			TCODColor::pink);
		armor->description= "Equip to increase ATK/DEX";
		armor->blocks=false;
    armor->pickable=new Equipment(0, 3+scale, 0, 3+scale, ARMOR);
    engine.actors.push(armor);

  }

	} else if (rng->getInt(0,100) < 50 ) {

		TCODRandom *rng2=TCODRandom::getInstance();
		if (rng2->getInt(0,100) < 65) {
		Actor *weapon = new Actor(x,y,'/',"Spatula",
			TCODColor::darkViolet);
		weapon->description= "Equip to increase DEF/LUCK";
		weapon->blocks=false;
    weapon->pickable=new Equipment(1+scale, 0, 1+scale, 0, WEAPON);
    engine.actors.push(weapon);
  } else if ( rng2->getInt(0,100) < 85) {
  	Actor *weapon = new Actor(x,y,'/',"Spork",
			TCODColor::violet);
		weapon->description= "Equip to increase DEF/LUCK";
		weapon->blocks=false;
    weapon->pickable=new Equipment(2+scale, 0, 2+scale, 0, WEAPON);
    } else {
    	Actor *weapon = new Actor(x,y,'/',"Butcher Knife",
			TCODColor::pink);
		weapon->description= "Equip to increase DEF/LUCK";
		weapon->blocks=false;
    weapon->pickable=new Equipment(3+scale, 0, 3+scale, 0, WEAPON);
  }
}

}