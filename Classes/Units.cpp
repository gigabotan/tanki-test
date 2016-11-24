#include "Units.h"

USING_NS_CC;

Unit::Unit():
	m_health(100), m_maxHealth(100), m_armor(1), m_maxSpeed(100), m_healthbar(nullptr)
{
}

bool Unit::isDead()
{
	return m_isDead;
}

int Unit::getHealth()
{
	return m_health;
}

float Unit::getArmor()
{
	return m_armor;
}

void Unit::hit(int dmg)
{	
	m_health -= int(dmg*m_armor);
	if (m_health <= 0)
	{
		m_isDead = true;
		m_health = 0;
	}
}

cocos2d::Vec2 Unit::getDirection()
{	
	auto direction = Vec2();
	direction.y = cosf(CC_DEGREES_TO_RADIANS(this->getRotation()));
	direction.x = sinf(CC_DEGREES_TO_RADIANS(this->getRotation()));
	direction.normalize();
	return direction;
}

void Unit::move(bool forward)
{
	auto mass = getPhysicsBody()->getMass();
	getPhysicsBody()->applyImpulse(forward ? Vec2(0.0, 1.0)*mass*10 : Vec2(0.0, -1.0)*mass*10);

}

void Unit::moveTo(const cocos2d::Vec2& target)
{	
	auto targetVect = target - getPosition();
	targetVect.normalize();
	setPosition(targetVect*m_maxSpeed);
}

void Unit::rotate(bool clockwise)
{	
	setRotation(clockwise? getRotation() + 5 : getRotation() - 5);
	m_healthbar->setRotation(-getRotation());
}

void Unit::rotateTo(const cocos2d::Vec2& target)
{
	auto targetVect = getPosition() - target;
	auto radan = targetVect.getAngle();
	auto angle = -CC_RADIANS_TO_DEGREES(radan);
	setRotation(angle-90);
	m_healthbar->setRotation(-getRotation());
}


bool Unit::init(int health, float armor, int maxSpeed, const std::string& spriteName)
{	
	m_health = health;
	m_maxHealth = health;
	m_armor = armor;
	m_maxSpeed = maxSpeed;
	m_spriteName = spriteName;
	m_isDead = false;

	auto sprite = Sprite::createWithSpriteFrameName(m_spriteName);
	addChild(sprite);
	sprite->getTexture()->setAliasTexParameters();
	addComponent(PhysicsBody::createBox(sprite->getContentSize()));
	getPhysicsBody()->setDynamic(true);
	getPhysicsBody()->setVelocityLimit(m_maxSpeed);
	getPhysicsBody()->setRotationEnable(false);
	setContentSize(sprite->getContentSize());


	m_healthbar = ui::LoadingBar::create("healthbar.png", ui::TextureResType::PLIST, 100);
	m_healthbar->setName("HealthBar");
	m_healthbar->setPosition(Vec2::ZERO);
	m_healthbar->setAnchorPoint(-Vec2(0, (getContentSize().height / 2)));
	addChild(m_healthbar);

	return true;
}

void Unit::update(float delta)
{
	if (m_health >= 0)
	{
		m_healthbar->setPercent((float(m_health) / m_maxHealth)*100);
	}
	
	if (getPhysicsBody()->getVelocity().getLength() > 1)
	{
		getPhysicsBody()->setVelocity(getPhysicsBody()->getVelocity()*0.9);
	}
	else
	{
		getPhysicsBody()->setVelocity(Vec2::ZERO);
	}
	
	
}






Player * Player::create(int health, float armor, int maxSpeed, const std::string & spriteName)
{
	Player * ret = new (std::nothrow) Player();
	if (ret && ret->init(health, armor, maxSpeed, spriteName))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

Player::Player(): Unit(), m_activeGun(0)
{
}

bool Player::init(int health, float armor, int maxSpeed, const std::string& spriteName)
{
	if (!Unit::init(health, armor, maxSpeed, spriteName))
	{
		return false;
	}

	auto gun1 = Gun::create(100, Color4F::BLACK, Gun::GUN_ROCKET);
	gun1->setPlayer(this);
	m_guns.pushBack(gun1);
	auto gun2 = Gun::create(10, Color4F::YELLOW, Gun::GUN_FIRE);
	gun2->setPlayer(this);
	m_guns.pushBack(gun2);
// 	auto gun3 = Gun::create(0, Color4F::RED, Gun::GUN_LASER);
// 	gun3->setPlayer(this);
// 	m_guns.pushBack(gun3);
	changeWeapon(true);

	return true;
}

void Player::fire()
{
	if (!m_guns.empty())
	{
		m_guns.at(m_activeGun)->fire();
	}
}

void Player::changeWeapon(bool next)
{
	auto guns = m_guns.size();
	m_guns.at(m_activeGun)->removeFromParent();
	if (guns > 1) 
	{
		m_activeGun = (m_activeGun + 1 == guns ? 0 : m_activeGun + 1);		
	}
	this->addChild(m_guns.at(m_activeGun));
}





Monster * Monster::create(int health, float armor, int maxSpeed, int dmg, const std::string & spriteName)
{
	Monster * ret = new (std::nothrow) Monster();
	if (ret && ret->init(health, armor, maxSpeed, dmg, spriteName))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

Monster * Monster::createRed()
{
	return create(20, 1, 50, 20, "monster_red.png");
}

Monster * Monster::createGreen()
{
	return create(30, 0.8, 30, 40, "monster_green.png");
}

Monster * Monster::createBlue()
{
	return create(150, 0.5, 20, 100, "monster_blue.png");
}

Monster * Monster::createRandomMonster()
{
	auto num = RandomHelper::random_int(1, 3);
	switch (num)
	{
	case 1:
		return createBlue();
	case 2:
		return createGreen();
	case 3:
		return createRed();
	}
}

Monster::Monster():
	Unit(), m_dmg(0), m_target(nullptr)
{
}

bool Monster::init(int health, float armor, int maxSpeed, int dmg, const std::string & spriteName)
{
	if (!Unit::init(health, armor, maxSpeed, spriteName))
	{
		return false;
	}
	m_dmg = dmg;

	return true;
}

void Monster::update(float dt)
{
	Unit::update(dt);
	if (m_target)
	{
		rotateTo(m_target->getPosition());
		move(true);
	}
}

void Monster::setTarget(Node * target)
{
	if (target) {
		m_target = target;
	}
}

int Monster::getDmg()
{
	return m_dmg;
}
