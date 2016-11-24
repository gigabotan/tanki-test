#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Units.h"


class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

	HelloWorld();
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);


	virtual void update(float delta) override;

	void toggleDebugCallback(cocos2d::Ref* sender);
	void toggleDebug();
	bool _debugDraw = false;

	std::set<cocos2d::EventKeyboard::KeyCode> keys;
	

	Player* m_player;
	int m_monsters;

	void processInput();
	bool onContactBegin(cocos2d::PhysicsContact& contact);
	bool onContactSeparate(cocos2d::PhysicsContact& contact);
	void onContactMonsterMissile(Node* monster, Node* missile);
	void onContactBeginMonsterPlayer(Node* monsterNode);
	void onContactSeparateMonsterPlayer(Node* monsterNode);
	void updateMonsters();
	

};

#endif // __HELLOWORLD_SCENE_H__
