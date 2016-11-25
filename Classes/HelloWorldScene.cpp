#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;

enum NODE_TAG {
	NODE_TAG_WALL,
	NODE_TAG_PLAYER,
	NODE_TAG_MONSTER,
	NODE_TAG_MISSILE
};



Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2::ZERO);
	scene->getPhysicsWorld()->setSubsteps(2);
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

HelloWorld::HelloWorld(): m_monsters(0), m_player(nullptr)
{
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }


	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("images.plist");
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

	MenuItemFont::setFontSize(18);
	auto item = MenuItemFont::create("Toggle debug", CC_CALLBACK_1(HelloWorld::toggleDebugCallback, this));

	auto menu = Menu::create(item, nullptr);
	this->addChild(menu);
	menu->setPosition(Vec2(visibleSize.width - item->getContentSize().width / 2 - 10, visibleSize.height - item->getContentSize().height / 2 - 10));


    /////////////////////////////
    // 3. add your codes below...


    
	auto background = LayerColor::create(Color4B(9, 73, 26, 255), visibleSize.width, visibleSize.height);
	background->setPosition(origin);
	addChild(background, -1);


	// Wall around screen

	auto wall = Node::create();
	wall->addComponent(PhysicsBody::createEdgeBox(Size(visibleSize.width-10, visibleSize.height-10)));
	wall->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	wall->getPhysicsBody()->setCategoryBitmask(0xF000);
	wall->getPhysicsBody()->setContactTestBitmask(0x000F);
	wall->getPhysicsBody()->setCollisionBitmask(0x0F00);
	this->addChild(wall);
	wall->setTag(NODE_TAG_WALL);

	
	//Add player
	m_player = Player::create(100, 0.5, 300, "tank.png");
	m_player->setScale(3.0);
	m_player->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	m_player->getPhysicsBody()->setCategoryBitmask(0x0F00);
	m_player->getPhysicsBody()->setContactTestBitmask(0x00F0);
	m_player->getPhysicsBody()->setCollisionBitmask(0xF000);
	addChild(m_player);
	m_player->setTag(NODE_TAG_PLAYER);



	

	//Look for pressed keys
	auto eventListener = EventListenerKeyboard::create();
	Director::getInstance()->getOpenGLView()->setIMEKeyboardState(true);
	eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		keys.insert(keyCode);
		switch (keyCode)
		{
			case EventKeyboard::KeyCode::KEY_Q:
				m_player->changeWeapon(true);
			break;
			case EventKeyboard::KeyCode::KEY_W:
				m_player->changeWeapon(false);
			break;
		}
	};
	eventListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		keys.erase(keyCode);
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);




	//adds contact event listener
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
	contactListener->onContactSeparate = CC_CALLBACK_1(HelloWorld::onContactSeparate, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);


	this->scheduleUpdate();

    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}


void HelloWorld::update(float delta)
{
	// Register an update function that checks to see if the CTRL key is pressed
	// and if it is displays how long, otherwise tell the user to press it

	if (m_player->isDead())
	{
		Director::getInstance()->replaceScene(HelloWorld::createScene());
	}
	
	processInput();
	updateMonsters();


}

void HelloWorld::toggleDebugCallback(cocos2d::Ref * sender)
{
	toggleDebug();
}

void HelloWorld::toggleDebug()
{
	_debugDraw = !_debugDraw;
	getScene()->getPhysicsWorld()->setDebugDrawMask(_debugDraw ? PhysicsWorld::DEBUGDRAW_ALL : PhysicsWorld::DEBUGDRAW_NONE);
}

void HelloWorld::processInput()
{
	if (keys.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
	{
		m_player->rotate(false);
	}
	if (keys.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
	{
		m_player->rotate(true);
	}
	if (keys.count(EventKeyboard::KeyCode::KEY_UP_ARROW))
	{
		m_player->move(true);
	}
	if (keys.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
	{
		m_player->move(false);
	}
	//if (keys.count(EventKeyboard::KeyCode::KEY_Q))
	//{
	//	m_player->changeWeapon(true);
	//}
	//if (keys.count(EventKeyboard::KeyCode::KEY_E))
	//{
	//	m_player->changeWeapon(false);
	//}
	if (keys.count(EventKeyboard::KeyCode::KEY_X))
	{
		m_player->fire();
	}
}

bool HelloWorld::onContactBegin(cocos2d::PhysicsContact & contact)
{

	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();


	if (nodeA && nodeB)
	{
		if (nodeA->getTag() == NODE_TAG_MONSTER )
		{
			if (nodeB->getTag() == NODE_TAG_PLAYER)
			{
				onContactBeginMonsterPlayer(nodeA);
			}
			else if (nodeB->getTag() == NODE_TAG_MISSILE)
			{
				onContactMonsterMissile(nodeA, nodeB);
				nodeB->removeFromParentAndCleanup(true);
			}
			
		}
		else if (nodeB->getTag() == NODE_TAG_MONSTER)
		{
			if (nodeA->getTag() == NODE_TAG_PLAYER)
			{
				onContactBeginMonsterPlayer(nodeB);
			}
			else if (nodeA->getTag() == NODE_TAG_MISSILE)
			{
				onContactMonsterMissile(nodeB, nodeA);
				nodeA->removeFromParentAndCleanup(true);
			}
		}
		else if (nodeA->getTag() == NODE_TAG_WALL)
		{
			nodeB->removeFromParentAndCleanup(true);
		}
		else if (nodeB->getTag() == NODE_TAG_WALL)
		{
			nodeA->removeFromParentAndCleanup(true);
		}
	}

	//bodies can collide
	return true;
}

bool HelloWorld::onContactSeparate(PhysicsContact & contact)
{
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();


	if (nodeA && nodeB)
	{
		if (nodeA->getTag() == NODE_TAG_MONSTER)
		{
			if (nodeB->getTag() == NODE_TAG_PLAYER)
			{
				onContactSeparateMonsterPlayer(nodeA);
			}

		}
		else if (nodeB->getTag() == NODE_TAG_MONSTER)
		{
			if (nodeA->getTag() == NODE_TAG_PLAYER)
			{
				onContactSeparateMonsterPlayer(nodeB);
			}
		}
	}

	return true;
}

void HelloWorld::onContactMonsterMissile(Node * monsterNode, Node * missileNode)
{
	Monster *monster = dynamic_cast<Monster *>(monsterNode);
	Missile *missile = dynamic_cast<Missile *>(missileNode);
	if (monster && missile)
	{
		monster->hit(missile->getDmg());

		if (monster->isDead())
		{
			m_monsters -= 1;
			monster->removeFromParent();
		}
	}
}

void HelloWorld::onContactBeginMonsterPlayer(Node * monsterNode)
{
	Monster *monster = dynamic_cast<Monster *>(monsterNode);
	if (monster && m_player)
	{
		monster->startAttack();
	}
}


void HelloWorld::onContactSeparateMonsterPlayer(Node * monsterNode)
{
	Monster *monster = dynamic_cast<Monster *>(monsterNode);
	if (monster && m_player)
	{
		monster->stopAttack();
	}
}

void HelloWorld::updateMonsters()
{

	if (m_monsters < 10)
	{
		auto angle = RandomHelper::random_int(0, 360);
		const static auto diagonal = [](){
			auto width = Director::getInstance()->getVisibleSize().width;
			auto height = Director::getInstance()->getVisibleSize().height;
			return int(sqrt(width*width + height*height));
		}();
		auto radius = RandomHelper::random_int(diagonal / 2, diagonal);


		auto monster = Monster::createRandomMonster();
		monster->setScale(2.0);
		monster->setPosition(Vec2(Director::getInstance()->getVisibleSize())/2 + Vec2(cosf(angle)*radius, sinf(angle)*radius));
		monster->getPhysicsBody()->setContactTestBitmask(0x0F0F);
		monster->getPhysicsBody()->setCollisionBitmask(0x00F0);
		monster->getPhysicsBody()->setCategoryBitmask(0x00F0);
		monster->setTarget(m_player);
		addChild(monster);
		monster->setTag(NODE_TAG_MONSTER);
		m_monsters += 1;
	}
}


