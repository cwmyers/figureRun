#include <LiquidCrystal.h>


//LCD pin to Arduino
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;

const int pin_BL = 10;

const int LEFT = 1;
const int RIGHT = 2;
const int UP = 3;
const int DOWN = 4;
const int SELECT = 5;
const int UNKNOWN_KEY = -1;

const int rows = 2;
const int columns = 16;
const int cellHeight = 8;
const int cellWidth = 5;
const int roadSize = 17;
const int roadLength = 33;

LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);



byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte smiley[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte frownie[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001
};

byte armsDown[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010
};

byte armsUp[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};

byte run1[8] = {
  0b00100,
  0b00110,
  0b00100,
  0b00101,
  0b01110,
  0b10100,
  0b00110,
  0b01001
};

byte run2[8] = {
  0b00100,
  0b00110,
  0b00100,
  0b10100,
  0b01110,
  0b00101,
  0b01100,
  0b10010
};

class CollisionResult {
  public:
    virtual bool gameOver() {
      return false;
    }
    virtual int bonus() {
      return 0;
    }
    virtual String cType() = 0;
    virtual bool levelComplete() {
      return false;
    }
    virtual bool isCollision() {
      return true;
    }
    virtual bool isCollected() {
      return false;
    }
};

class EndGame : public CollisionResult {
  public:
    bool gameOver() {
      return true;
    }
    String cType() {
      return String("EndGame");
    }

};
class Reward : public CollisionResult {
  public:

    int bonus() {
      return 10;
    }

    String cType() {
      return String("Reward");
    }

};

class NoCollision : public CollisionResult {
  public:
    String cType() {
      return String("NoCollision");
    }
    bool isCollision() {
      return false;
    }

};

class Collected: public CollisionResult {
  public:
    String cType() {
      return String("Collected");
    }

    bool isCollected() {
      return true;
    }
};

class LevelComplete : public CollisionResult {
  public:
    String cType() {
      return String("Level Complete");
    }
    bool levelComplete() {
      return true;
    }

};

NoCollision *nc = new NoCollision();
EndGame *eg = new EndGame();
Reward *reward = new Reward();
LevelComplete *levelComplete = new LevelComplete();
Collected *collected = new Collected();

///////////////////////////
// RoadObjects
////////////////////////////


class RoadObject {
  protected:
    int track;
    int position;

  public:
    RoadObject(int track, int position) {
      this->track = track;
      this->position = position;
    }
    int getTrack() {
      return this->track;
    }
    int getPosition() {
      return this->position;
    }
    virtual void update(int roadLength) {
      this->position = this->position - 1;
      if (this->position < 0 ) {
        this->position = roadLength;
      }
    }
    String toString() {
      return String("Spike position:") + this->position + " track: " + this->track;
    }
    virtual char displayChar() = 0;
    virtual CollisionResult* getCollisionResult();
    CollisionResult* collision(int track , int position) {
      if (this->getTrack() == track && this->position == position) {
        this->position = roadLength;
        return this->getCollisionResult();
      } else {
        return nc;
      }
    }
    virtual bool isFinishLine() {
      return false;
    }

    virtual bool isBullet() {
      return false;
    }
};

class Spike : public RoadObject {
  public:
    Spike(int track, int position): RoadObject(track, position)
    {};
    char displayChar() {
      return '*';
    }

    CollisionResult* getCollisionResult() {
      return eg;
    }
};

class DeathSpike : public RoadObject {
  public:
    DeathSpike(int track, int position): RoadObject(track, position)
    {};
    char displayChar() {
      return '<';
    }

    CollisionResult* getCollisionResult() {
      return eg;
    }

    void update(int roadLength) {
      this->position = this->position - 1;
      if (this->position % 3 == 0) {
        this->track = (this->track + 1 ) % 2;
      }
      if (this->position < 0 ) {
        this->position = roadLength;
      }
    }
};

class Wall : public RoadObject {
  public:
    Wall(int track, int position): RoadObject(track, position) {};

    char displayChar() {
      return '}';
    }

    CollisionResult* getCollisionResult() {
      return eg;
    }

};

class Bullet: public RoadObject {
  public:
    Bullet(int track, int position): RoadObject(track, position) {};

    char displayChar() {
      return '-';
    }

    CollisionResult* getCollisionResult() {
      return eg;
    }

    void update(int roadLength) {
      if (this->position < columns)
        this->position = this->position - 2;
      else
        this->position = this->position - 1;
      if (this->position < 0) {
        this->position = 102;
      }
    }

    bool isBullet() {
      return true;
    }
};



class Coin : public RoadObject {
  public:
    Coin(int track, int position): RoadObject(track, position)
    {};
    char displayChar() {
      return 'o';
    }

    CollisionResult* getCollisionResult() {
      return reward;
    }
};

class FinishLine: public RoadObject {
  public:
    FinishLine(int track, int position): RoadObject(track, position)
    {};
    char displayChar() {
      return '|';
    }

    CollisionResult* getCollisionResult() {
      return levelComplete;
    }

    bool isFinishLine() {
      return true;
    }
};

class Collectable: public RoadObject {
  public:
    Collectable(int track, int position): RoadObject(track, position) {};
    char displayChar() {
      return '#';
    }

    CollisionResult* getCollisionResult() {
      return collected;
    }
};


/////////////////
// Road
/////////////////

class Road {
  private:
    RoadObject **road;

  public:
    Road(RoadObject **road) {
      this->road = road;
    }

    RoadObject* getObject(int i) {
      return road[i];
    }

    int getHighestPosition() {
      int pos = 0;
      for (int i = 0; i < roadSize; i++) {
        RoadObject* s = this->road[i];
        if (!s->isFinishLine() && !s->isBullet())
          pos = max(pos, s->getPosition());
      }
      return pos;
    }

    void update() {
      for (int i = 0; i < roadSize; i++) {
        RoadObject* s = this->road[i];
        if (s->getPosition() <= 0) {
          s->update(this->getHighestPosition() + 2);
        } else {
          s->update(roadLength);
        }
      }
    }

};

/////////////////////////
// Levels
////////////////////////

class Level {
  protected:
    Road* road;
    String name;
    int scoreThreshold;
    int delayTime;

  public:
    Level( Road* road, String name, int delayTime) {
      this->road = road;
      this->name  = name;
      this->delayTime = delayTime;
    }

    int getDelay() {
      return delayTime;
    }

    Road* getRoad() {
      return this->road;
    }

    String getName() {
      return this->name;
    }

    virtual bool levelComplete(int score, CollisionResult* cr) {
      return cr->levelComplete();
    }

    virtual void updateRoad() {
      this->road->update();
    }

    virtual CollisionResult* detectCollision(int playerTrack, int playerColumnPosition) {
      CollisionResult* collision = nc;
      for (int i = 0; i < roadSize; i++) {

        RoadObject *s = this->getRoad()->getObject(i);
        CollisionResult *result = s->collision(playerTrack, playerColumnPosition);

        if (result->isCollision()) {
          collision = result;
        }

      }
      return collision;
    }


};

class ScoreLevel: public Level {
};

class FinishLineLevel: public Level {
};

class CollectionLevel: public Level {
  private:
    int itemsCollected = 0;
  public:
    CollectionLevel(Road* road, String name, int delayTime) : Level(road, name, delayTime) {
    }

    bool levelComplete(int score, CollisionResult* cr) {
      if (cr->isCollected()) {
        this->itemsCollected++;
        //        Serial.println("Item collected");
      }

      return this->itemsCollected > 10;
    }

};


class GameOverLevel: public Level {
  public:
    GameOverLevel(Road* road, String name, int delayTime) : Level(road, name, delayTime) {
    }

    CollisionResult* detectCollision(int playerTrack, int playerColumnPosition) {
      //      Serial.println("Let's end it!");
      return eg;
    }
};


RoadObject *levelOneRoad[roadSize] = {
  new Spike(0, 3),
  new Spike(1, 7),
  new Spike(0, 9),
  new Spike(1, 12),
  new Spike(1, 13),
  new Spike(0, 15),
  new Spike(1, 18),
  new Spike(0, 20),
  new Spike(0, 25),
  new Spike(0, 27),
  new Spike(1, 29),
  new Spike(1, 32),
  new Coin(0, 4),
  new Coin(1, 14),
  new Coin(1, 25),
  new FinishLine(0, 80),
  new FinishLine(1, 80),
};

RoadObject *levelTwoRoad[roadSize] = {
  new Coin(0, 3),
  new Spike(1, 7),
  new Spike(0, 9),
  new Spike(1, 12),
  new Spike(1, 13),
  new Spike(0, 15),
  new Spike(1, 18),
  new Spike(1, 19),
  new Spike(1, 20),
  new Spike(0, 25),
  new Spike(0, 27),
  new Spike(1, 29),
  new Spike(1, 32),
  new Coin(1, 14),
  new Coin(1, 25),
  new FinishLine(0, 100),
  new FinishLine(1, 100),
};
RoadObject *levelThreeRoad[roadSize] = {
  new Coin(0, 3),
  new DeathSpike(1, 7),
  new DeathSpike(0, 9),
  new DeathSpike(1, 12),
  new DeathSpike(1, 16),
  new DeathSpike(0, 18),
  new DeathSpike(1, 20),
  new DeathSpike(1, 22),
  new DeathSpike(0, 26),
  new DeathSpike(0, 29),
  new DeathSpike(0, 33),
  new DeathSpike(1, 36),
  new DeathSpike(1, 40),
  new DeathSpike(1, 44),
  new Coin(1, 25),
  new FinishLine(0, 100),
  new FinishLine(1, 100),
};

RoadObject *levelFourRoad[roadSize] = {
  new Coin(0, 3),
  new DeathSpike(1, 7),
  new DeathSpike(0, 9),
  new Collectable(1, 12),
  new DeathSpike(1, 16),
  new Collectable(0, 18),
  new DeathSpike(1, 20),
  new Collectable(1, 22),
  new DeathSpike(0, 26),
  new Spike(0, 29),
  new DeathSpike(0, 33),
  new Collectable(1, 36),
  new Spike(1, 40),
  new DeathSpike(1, 44),
  new Coin(1, 25),
  new Collectable(0, 50),
  new Coin(1, 52),
};

RoadObject *levelFiveRoad[roadSize] = {
  new Bullet(0, 14),
  new Wall(0, 14),
  new Bullet(1, 11),
  new Wall(1, 11),
  new Bullet(0, 20),
  new Wall(0, 20),
  new Bullet(1, 24),
  new Wall(1, 24),
  new Bullet(0, 33),
  new Wall(0, 33),
  new Bullet(1, 35),
  new Wall(1, 35),
  new Bullet(0, 47),
  new Wall(0, 47),
  new Coin(1, 25),
  new FinishLine(0, 100),
  new FinishLine(1, 100),
};

Level* levels[6] = {
  new Level(new Road(levelOneRoad), "L1 Easy Town", 900) ,
  new Level(new Road(levelTwoRoad), "L2 Speed Town",  500) ,
  new Level(new Road(levelThreeRoad), "L3 Death Valley",  900),
  new CollectionLevel(new Road(levelFourRoad), "L4 CollectValley",  700),
  new Level(new Road(levelFiveRoad), "L5 The Wall", 900),
  new GameOverLevel(new Road(levelFourRoad), "GameOver You Win",  700),
};

int playerTrack = 1;
int playerColumnPosition = 1;
int delayTime = 900;
int delayTimeCountdown = delayTime;
int levelDelayCountdown = 1000;
int oldKeyPress = DOWN;
int keyPressDelay = delayTime;
int score = 0;
CollisionResult* collision = nc;
int currentLevelIndex = 0;
bool newLevel = true;


void setup() {
  //  Serial.begin(115200);
  // initialize LCD and set up the number of columns and rows:
  lcd.begin(16, 2);

  // create a new character
  lcd.createChar(0, heart);
  // create a new character
  lcd.createChar(1, armsDown);
  // create a new character
  lcd.createChar(2, armsUp);
  // create a new character
  lcd.createChar(3, run1);
  // create a new character
  lcd.createChar(4, run2);

  // set the cursor to the top left
  lcd.setCursor(0, 0);

}


void loop() {
  Level *level = levels[currentLevelIndex];
  if (newLevel) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(level->getName());
    lcd.setCursor(0, 1);
    lcd.print(String("Score:") + score);
    newLevel = false;
    delay(2000);
  }

  if (collision->gameOver()) return;

  int keyPress = translateKeyPress(analogRead(0));
  //  Serial.println(keyPress);
  delayTimeCountdown--;
  levelDelayCountdown--;

  drawPlayer(delayTime, keyPress);


  if (levelDelayCountdown <= 0) {
    score++;
    level->updateRoad();
    drawBlocks(level);
    levelDelayCountdown = level->getDelay();

  }
  collision = level->detectCollision(playerTrack, playerColumnPosition);
  score += collision->bonus();
  if (collision->gameOver()) {
    runGameOver();
  }

  if (level->levelComplete(score, collision)) {
    currentLevelIndex++;
    newLevel = true;
  }

  if (delayTimeCountdown <= 0)
    delayTimeCountdown = delayTime;

}


void runGameOver() {
  lcd.setCursor(playerColumnPosition, playerTrack);
  lcd.write(2);
  delay(500);
  lcd.setCursor(playerColumnPosition, playerTrack);
  lcd.write(1);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(2));
  lcd.setCursor(0, 0);
  lcd.print(String("Score:") + score);
  lcd.setCursor(0, 1);
  lcd.print("Hit RST to play");

  lcd.setCursor(15, 0);
  lcd.write(byte(0));
  delay(10000);
}

void drawBlocks(Level *level) {
  String displayBuffer[2] = {"                ",
                             "                "
                            };
  for (int i = 0; i < roadSize; i++) {
    RoadObject *o = level->getRoad()->getObject(i);

    if (o->getPosition() < columns) {
      displayBuffer[o->getTrack()].setCharAt(o->getPosition(), o->displayChar());
    }
  }
  //Serial.print(displayBuffer[0]);
  //Serial.println("#");
  //Serial.print(displayBuffer[1]);
  //Serial.println("#");
  lcd.setCursor(0, 0);
  lcd.print(displayBuffer[0]);
  lcd.setCursor(0, 1);
  lcd.print(displayBuffer[1]);
}

void drawPlayer(int delayTime, int keyPress) {
  if (keyPress != oldKeyPress ) {
    if (keyPress == UP) {
      clearPlayer(playerColumnPosition, 1);
      playerTrack = 0;
    }
    if (keyPress == DOWN) {
      clearPlayer(playerColumnPosition, 0);
      playerTrack = 1;
    }
    //    if (keyPress == RIGHT) {
    //      clearPlayer(playerColumnPosition, playerTrack);
    //      playerColumnPosition = min(playerColumnPosition + 1, columns - 1);
    //    }
    //    if (keyPress == LEFT) {
    //      clearPlayer(playerColumnPosition, playerTrack);
    //      playerColumnPosition = max(0, playerColumnPosition - 1);
    //    }
    oldKeyPress = keyPress;
    keyPressDelay = delayTime;
  }
  keyPressDelay--;
  if (delayTimeCountdown >= delayTime / 2) {
    lcd.setCursor(playerColumnPosition, playerTrack);
    lcd.write(3);
  } else {
    lcd.setCursor(playerColumnPosition, playerTrack);
    lcd.write(4);
  }
}

void clearPlayer(int column, int row) {
  lcd.setCursor(column, row);
  lcd.print(" ");
}


int translateKeyPress(int x) {
  if (x < 60) {
    return RIGHT;
  }
  else if (x < 200) {
    return UP;
  }
  else if (x < 400) {
    return DOWN;
  }
  else if (x < 600) {
    return LEFT;
  }
  else if (x < 800) {
    return SELECT;
  }
  else {
    return UNKNOWN_KEY;
  }
}
