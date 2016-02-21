#include "HelloWorldScene.h"
#include "GameOverScene.h"
#include "mysprite.h"

using namespace cocos2d;
using namespace std;

Scene* HelloWorld::createScene() {
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2::ZERO);
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	auto layer = HelloWorld::create();
	layer->setPhyWorld(scene->getPhysicsWorld());

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool HelloWorld::init() {
	// 1. super init first
	if (!Layer::init()) {
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	createBall();
	createEdge();
	createPaddle();
	createBricks();

	this->scheduleUpdate();

	//Add listeners
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener,this);

	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPress,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener,paddle);

/*
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan,this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved,this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener,this);*/

	return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

Sprite* HelloWorld::createSprite(string path,float x,float y,int tag) {
	Sprite* temp;
	if (path.empty())
		temp = Sprite::create();
	else
		temp = Sprite::create(path);

	temp->setPosition(x,y);
	temp->setTag(tag);
	return temp;
}

void HelloWorld::createBricks() {
	int padding = 10;
	int bWidth = 32;

	for (int i = 0; i<20; i++) {
		//int bWidth = block->getContentSize().width;
		//int xOffset = padding+bWidth/2+((bWidth+padding)*i);

		Sprite* block = createSprite("Block.png", (bWidth)*i, 200, Type::BLOCK);

		PhysicsBody* blockBody = PhysicsBody::createBox(block->getContentSize()-Size(10,10) ,blockMaterial);
		blockBody->setContactTestBitmask(0x000001);

		block->setPhysicsBody(blockBody);
		this->addChild(block);
	}
}

void HelloWorld::createEdge() {
	edgeSp = createSprite("",visibleSize.width/2,visibleSize.height/2, Type::EDGE);

	PhysicsBody* boundBody = PhysicsBody::createEdgeBox(visibleSize,edgeMaterial);
	boundBody->setContactTestBitmask(0x000001); // This is the important command, if not available, there is nothing happening when colliding

	edgeSp->setPhysicsBody(boundBody); // Set physics Body
	this->addChild(edgeSp); // Add into Layer
}

void HelloWorld::createBall() {
	//ball = Sprite::create("Ball.png",Rect(0,0,52,52));
	int a = 0;
	//ball = Sprite::create("Ball.png",Rect(0,0,a, a));
	//ball = Sprite::create("Ball.png");
	ball = createSprite("Ball.png",100,100,Type::BALL);

	PhysicsBody* ballBody = PhysicsBody::createCircle(ball->getContentSize().width/2,ballMaterial); // The physics body circle shape
	ballBody->setGravityEnable(false); // Not set acceleration
	ballBody->applyImpulse(Vect(ballSpeed,ballSpeed)); // Create a force Vector to act with the direction of 45 degree, because x = y  Push a force into the ball edge
	ballBody->setContactTestBitmask(0x000001); //

	ball->setPhysicsBody(ballBody); // Set Physics body
	this->addChild(ball);
}

void HelloWorld::createPaddle() {
	paddle = Sprite::create("Paddle.png");
	PhysicsBody* paddleBody = PhysicsBody::createBox(paddle->getContentSize(),paddleMaterial);

	paddleBody->setGravityEnable(false);
	paddleBody->setDynamic(false); // Set static when reacting, no restitution, no changing position
	paddleBody->setContactTestBitmask(0x000001); // With reaction 

	paddle->setPosition(visibleSize.width/2,25);
	paddle->setPhysicsBody(paddleBody);

	ball->setTag(Type::PADDLE);
	this->addChild(paddle);
}

void HelloWorld::checkWin() {
	bool isWin = true;

	Vector<PhysicsBody*>bodies = m_world->getAllBodies();

	for each (PhysicsBody* body in bodies) {
		if (body->getNode()->getTag()==Type::BLOCK) {
			isWin = false;
		}
	}
	if (isWin==true) {
		//win();
	}
}

void HelloWorld::win() {
	auto gameOverScene = GameOverScene::create();
	gameOverScene->getLayer()->getLabel()->setString("You Win!");
	Director::getInstance()->replaceScene(gameOverScene);
}

void HelloWorld::lose() {
	auto gameOverScene = GameOverScene::create();
	gameOverScene->getLayer()->getLabel()->setString("You Lose!");
	Director::getInstance()->replaceScene(gameOverScene);
}

void HelloWorld::update(float dt) {
	checkWin();
}

bool HelloWorld::onContactBegin(PhysicsContact & contact) {
	Sprite* spriteA = (Sprite*) contact.getShapeA()->getBody()->getNode();
	Sprite* spriteB = (Sprite*) contact.getShapeB()->getBody()->getNode();

	int tagA = spriteA->getTag();
	int tagB = spriteB->getTag();

	if (tagA==Type::BLOCK) {
		this->removeChild(spriteA,true);
	}

	if (tagB==Type::BLOCK) {
		this->removeChild(spriteB,true);
	}

	if ((tagA==Type::EDGE||tagB==Type::EDGE)& (ball->getPositionY()<=paddle->getPositionY())) {
		//lose();
		
	}

	return true;
}

void HelloWorld::onTouchMoved(Touch* touch,Event * event) {
	Point touchLocation = this->convertToWorldSpace(this->convertTouchToNodeSpace(touch));

	paddle->setPositionX(touchLocation.x);
}

void HelloWorld::onTouchEnded(Touch *,Event *) {}

bool HelloWorld::onTouchBegan(Touch *,Event *) {
	return true;
}

void HelloWorld::onKeyPress(EventKeyboard::KeyCode keycode,Event * event) {
	Node* paddle = event->getCurrentTarget();
	Vec2 position = paddle->getPosition();
	Vec2 move = Vec2(5,0);

	switch (keycode) {
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_KP_LEFT:
		paddle->setPosition(position-move);
		break;
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_KP_RIGHT:
		paddle->setPosition(position+move);
		break;
	}
}

//void HelloWorld::onKeyPress(EventKeyboard::KeyCode keycode,Event * event) {
//	Vec2 position = paddle->getPosition();
//	Vec2 move = Vec2(5,0);
//
//	switch (keycode) {
//	case EventKeyboard::KeyCode::KEY_A:
//	case EventKeyboard::KeyCode::KEY_KP_LEFT:
//		paddle->setPosition(position-move);
//		break;
//	case EventKeyboard::KeyCode::KEY_D:
//	case EventKeyboard::KeyCode::KEY_KP_RIGHT:
//		paddle->setPosition(position+move);
//		break;
//	}
//}

