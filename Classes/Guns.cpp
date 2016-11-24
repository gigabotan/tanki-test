#include "Guns.h"
#include "Units.h"
USING_NS_CC;


Gun * Gun::create(int cd, cocos2d::Color4F color, Gun::MISSILE missile)
{	
	Gun * ret = new (std::nothrow) Gun(cd, color, missile);
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

Gun::Gun(int cd, Color4F color, Gun::MISSILE missile):
	m_last(utils::getTimeInMilliseconds()),
	m_cd(cd),
	m_color(color),
	m_missile(missile)
{
}

bool Gun::init()
{	
	auto marker = DrawNode::create();
	marker->drawSolidRect(Vec2(-1, -1), Vec2(2, 2), m_color);
	addChild(marker);

	return true;
}

void Gun::setPlayer(Player* player)
{
	m_player = player;
}

Player * Gun::getPlayer()
{
	if (m_player)
	{
		return m_player;
	}
	return nullptr;
}


void Gun::fire()
{
	if (isReady())
	{
		m_last = utils::getTimeInMilliseconds();
		switch (m_missile)
		{
			case GUN_ROCKET:
			{
				auto missile = Missile::create();
				missile->setDmg(20);
				missile->drawDot(Vec2::ZERO, 3.0, Color4F::YELLOW);
				missile->addComponent(PhysicsBody::createCircle(1));
				auto pos = m_player->convertToWorldSpace(Vec2(0, m_player->getContentSize().height/2));
				missile->setPosition(pos);
				missile->getPhysicsBody()->applyImpulse(m_player->getDirection()*100);
				missile->getPhysicsBody()->setCategoryBitmask(0x000F);
				missile->getPhysicsBody()->setContactTestBitmask(0xF0F0);
				missile->getPhysicsBody()->setCollisionBitmask(0x0000);
				getScene()->addChild(missile);
				missile->setTag(3);
			}
				break;
			case GUN_FIRE:
			{
				auto missile = Missile::create();
				missile->setDmg(10);
				missile->drawDot(Vec2::ZERO, 2.0, Color4F::ORANGE);
				missile->addComponent(PhysicsBody::createCircle(2));
				auto pos = m_player->convertToWorldSpace(Vec2(0, m_player->getContentSize().height / 2));
				missile->setPosition(pos);
				missile->getPhysicsBody()->applyImpulse(m_player->getDirection() * 100);
				missile->getPhysicsBody()->setCategoryBitmask(0x000F);
				missile->getPhysicsBody()->setContactTestBitmask(0xF0F0);
				missile->getPhysicsBody()->setCollisionBitmask(0x0000);
				getScene()->addChild(missile);


				missile->onUpdate = [=]() {
					if (missile->getScale() < 3)
					{
						missile->setScale(missile->getScale()*1.01);
					}
					else 
					{
						missile->removeFromParent();
					}
				};

				missile->setTag(3);

			}
				
				break;

			case GUN_LASER:
			{
				// not implemented yet
				// yes, the game is not ideal
			}

				break;
		}
	}
}

bool Gun::isReady()
{
	if (utils::getTimeInMilliseconds() - m_last >= m_cd)
	{
		return true;
	}
	return false;
}



void Missile::update(float delta)
{
	if (onUpdate)
	{
		this->onUpdate();
	}
}

void Missile::setDmg(int dmg)
{
	m_dmg = dmg;
}

int Missile::getDmg()
{
	return m_dmg;
}
