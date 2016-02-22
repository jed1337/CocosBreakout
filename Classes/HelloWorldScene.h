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
	void createHUD();

	void update(float);
	void move(float);
	void checkIfBallOutOfBounds();
	void checkWin();

	void remvoveBlock(Sprite*);
	void incrementScore(int);

	void win();
	void lose();


	//Listeners
	bool onContactBegin(cocos2d::PhysicsContact& contact);
	void onContactSeperate(cocos2d::PhysicsContact& contact);

	void onKeyPress(EventKeyboard::KeyCode,Event*);
	void onKeyRelease(EventKeyboard::KeyCode,Event*);

	// implement the "static create()" method manually
	CREATE_FUNC(HelloWorld);

private:
	enum Type { BALL,PADDLE,BLOCK,EDGE };
	enum Direction { NONE,RIGHT,LEFT };
	enum ZScore{HUD, OBJECT};
	Direction curDirection = Direction::NONE;

	float ballSpeed = 85000.0f;
	float paddleSpeed = 400;

	Sprite* ball;
	Sprite* paddle;
	Sprite* edgeSp;

	string scoreString = "Score: ";
	Label* scoreLabel;

	int score = 0;

	PhysicsWorld* m_world;
	void setPhyWorld(PhysicsWorld* world) { m_world = world; };

	PhysicsMaterial blockMaterial = PhysicsMaterial(20.0f,1.0f,0.0f);
	PhysicsMaterial edgeMaterial = PhysicsMaterial(20.0f,1.0f,0.0f);
	PhysicsMaterial ballMaterial = PhysicsMaterial(1.0f,1.0f,0.0f);
	PhysicsMaterial paddleMaterial = PhysicsMaterial(20.f,1.0f,0.0f);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

};

#endif // __HELLOWORLD_SCENE_H__
