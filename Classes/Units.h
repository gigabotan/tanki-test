#ifndef Units_h__
#define Units_h__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "Guns.h"

class Unit : public cocos2d::Node
{
public:
	Unit();

	bool isDead();
	int getHealth();
	float getArmor();
	void hit(int dmg);


	cocos2d::Vec2 getDirection();
	void move(bool forward);
	void moveTo(const cocos2d::Vec2& target);
	void rotate(bool clockwise);
	void rotateTo(const cocos2d::Vec2& target);
	
	virtual bool init(int health, float armor, int maxSpeed, const std::string& spriteName);
	virtual void update(float delta);

protected:
	bool m_isDead;
	int m_health;
	int m_maxHealth;
	float m_armor;
	int m_maxSpeed;

	std::string m_spriteName;
	cocos2d::ui::LoadingBar * m_healthbar;
	

};




class Player : public Unit
{
public:
	static Player* create(int health, float armor, int maxSpeed, const std::string& spriteName);
	Player();

	virtual bool init(int health, float armor, int maxSpeed, const std::string& spriteName);
	void fire();
	void changeWeapon(bool next);
	


protected:
	
	cocos2d::Vector<Gun*> m_guns;
	int m_activeGun;
};




class Monster : public Unit
{
public:
	static Monster* create(int health, float armor, int maxSpeed, int dmg, const std::string& spriteName);
	static Monster* createRed();
	static Monster* createGreen();
	static Monster* createBlue();
	static Monster* createRandomMonster();
	Monster();
	virtual bool init(int health, float armor, int maxSpeed, int dmg, const std::string& spriteName);
	virtual void update(float dt);

	void setTarget(Unit* target);

	int getDmg();
	void startAttack();
	void stopAttack();

protected:
	long long m_attackCd;
	long long m_lastAttack;
	bool m_attacking;
	int m_dmg;
	Unit* m_target;
};




#endif // Units_h__
