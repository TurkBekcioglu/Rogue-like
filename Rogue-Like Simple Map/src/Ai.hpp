class Ai {
  public :
  virtual void update(Actor *owner)=0;

  virtual ~Ai() {};

};

class PlayerAi : public Ai {
  public :
  void update(Actor *owner);

  void handleActionKey(Actor *owner, int ascii);  

  protected :
  bool moveOrAttack(Actor *owner, int targetx, int targety);

  Actor *choseFromInventory(Actor *owner);
  Actor *showEquipment(Actor *owner);
};

class MonsterAi : public Ai {
  public :
  void update(Actor *owner);

  protected :
  int moveCount;
  void moveOrAttack(Actor *owner, int targetx, int targety);
};