#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

using namespace cocos2d;
using namespace std;

class HelloWorld: public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	Sprite* createSprite(string,float,float,int);
	void createBricks();
	void createEdge();
	void createBall();
	void createPaddle();

	void checkWin();
	void win();
	void lose();

	void update(float dt);

	//Listeners
	bool onContactBegin(PhysicsContact& contact);
	void onTouchMoved(Touch*,Event*);
	void onTouchEnded(Touch*,Event*);
	bool onTouchBegan(Touch*,Event*);
	void onKeyPress(EventKeyboard::KeyCode,Event*);

	// implement the "static create()" method manually
	CREATE_FUNC(HelloWorld);

private:
	enum Type { BALL,PADDLE,BLOCK,EDGE };

	float ballSpeed = 30000.0f;

	Sprite* ball;
	Sprite* paddle;
	Sprite* edgeSp;

	PhysicsWorld* m_world;
	void setPhyWorld(PhysicsWorld* world) { m_world = world; };

	PhysicsMaterial blockMaterial = PhysicsMaterial(5.0f,1.0f,0.0f);
	PhysicsMaterial edgeMaterial = PhysicsMaterial(1.0f,1.0f,0.0f);
	PhysicsMaterial ballMaterial = PhysicsMaterial(1.0f,1.0f,0.0f);
	PhysicsMaterial paddleMaterial = PhysicsMaterial(10.f,1.0f,0.0f);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

};

#endif // __HELLOWORLD_SCENE_H__
