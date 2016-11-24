#ifndef Guns_h__
#define Guns_h__

#include "cocos2d.h"


class Player;

class Gun : public cocos2d::Node
{
public:
	enum MISSILE
	{
		GUN_ROCKET,
		GUN_FIRE,
		GUN_LASER
	} m_missile;

	static Gun* create(int cd, cocos2d::Color4F color, Gun::MISSILE missile);
	Gun(int cd, cocos2d::Color4F color, Gun::MISSILE missile);
	virtual bool init();
	void setPlayer(Player* player);
	Player * getPlayer();

	void fire();

	bool isReady();

	Player* m_player;
	cocos2d::Color4F m_color;
	long long m_last;
	long long m_cd;
};


class Missile : public cocos2d::DrawNode
{
public:
	CREATE_FUNC(Missile);
	virtual void update(float delta);
	void setDmg(int dmg);
	int getDmg();
	std::function<void(void)> onUpdate;
protected:
	int m_dmg;
};





#endif // Guns_h__
