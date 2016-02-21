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

	//auto wall = Node::create();
	//wall->setPhysicsBody(PhysicsBody::createEdgeBox(visibleSize,PhysicsMaterial(0.1f,1,0.0f)));
	//wall->setPosition(Vec2(visibleSize/2));
	//this->addChild(wall);
	//// create five balls with random velocity and direction
	//for (int i = 0; i < 15; ++i) {
	//	Size size(10+CCRANDOM_0_1()*10,10+CCRANDOM_0_1()*10);
	//	Size winSize = visibleSize;
	//	Vec2 position = Vec2(winSize.width,winSize.height)-Vec2(size.width,size.height);
	//	position.x = position.x * CCRANDOM_0_1();
	//	position.y = position.y * CCRANDOM_0_1();
	//	position = origin+position+Vec2(size.width/2,size.height/2);
	//	Vect velocity((CCRANDOM_0_1()-0.5)*400,(CCRANDOM_0_1()-0.5)*400);
	//	auto ball = Sprite::create("Ball.png");
	//	ball->setPosition(position);
	//	ball->setPhysicsBody(PhysicsBody::createCircle(ball->getContentSize().width/2,PhysicsMaterial(0.1f,1,0.0f)));
	//	ball->getPhysicsBody()->setContactTestBitmask(UINT_MAX);
	//	ball->getPhysicsBody()->setVelocity(velocity);
	//	this->addChild(ball);
	//}


	//Add listeners
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin,this);
	contactListener->onContactSeparate = CC_CALLBACK_1(HelloWorld::onContactSeperate,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener,this);

	/*auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener,this);*/

	/*auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPress,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener,paddle);*/

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan,this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved,this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener,this);

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

	for (int i = 0; i<3; i++) {
		for (int j = 0; j<11; j++) {
			Sprite* block = createSprite("Block.png",(bWidth+padding)*(j+1),300-(50*i),Type::BLOCK);

			PhysicsBody* blockBody = PhysicsBody::createBox(block->getContentSize()-Size(10,10),blockMaterial);
			blockBody->setContactTestBitmask(0x000001);

			block->setPhysicsBody(blockBody);
			this->addChild(block);
		}
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

bool HelloWorld::onContactBegin(PhysicsContact& contact) {
	auto a = contact.getShapeA()->getBody();
	auto b = contact.getShapeB()->getBody();

	// save the velocity, ignore the direction of velocity, only save the length
	float* v = new float[2];
	v[0] = a->getVelocity().length();
	v[1] = b->getVelocity().length();

	contact.setData(v);

	return true;
}

void HelloWorld::onContactSeperate(PhysicsContact& contact) {
	PhysicsBody* bodyA = contact.getShapeA()->getBody();
	PhysicsBody* bodyB = contact.getShapeB()->getBody();

	// restore the velocity, keep the direction of the velocity.
	float* v = (float*) contact.getData();
	Vec2 va = bodyA->getVelocity();
	Vec2 vb = bodyB->getVelocity();
	va.normalize();
	vb.normalize();
	bodyA->setVelocity(va * v[0]);
	bodyB->setVelocity(vb * v[1]);

	delete v;

	Sprite* spriteA = (Sprite*) bodyA->getNode();
	Sprite* spriteB = (Sprite*) bodyB->getNode();

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
}

//bool HelloWorld::onContactBegin(PhysicsContact & contact) {
//
//
//	return true;
//}


void HelloWorld::onTouchMoved(Touch* touch,Event * event) {
	Point touchLocation = this->convertToWorldSpace(this->convertTouchToNodeSpace(touch));

	paddle->setPositionX(touchLocation.x);
}

void HelloWorld::onTouchEnded(Touch *,Event *) {}

bool HelloWorld::onTouchBegan(Touch *,Event *) {
	return true;
}

void HelloWorld::onKeyPress(EventKeyboard::KeyCode keycode,Event * event)
{
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

