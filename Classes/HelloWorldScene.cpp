#include "HelloWorldScene.h"
#include "GameOverScene.h"
#include "mysprite.h"

using namespace cocos2d;
using namespace std;

Scene* HelloWorld::createScene() {
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2::ZERO);
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	auto layer = HelloWorld::create();
	layer->setPhyWorld(scene->getPhysicsWorld());

	scene->addChild(layer);
	return scene;
}

bool HelloWorld::init() {
	if (!Layer::init()) {
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	createBricks();
	createEdge();
	createHUD();
	createBall();
	createBallParticles();
	createPaddle();
	loadAudio();
	
	this->scheduleUpdate();

	//Add listeners
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin,this);
	contactListener->onContactSeparate = CC_CALLBACK_1(HelloWorld::onContactSeperate,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener,this);

	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPress,this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyRelease,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener,paddle);

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
	temp->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	temp->setTag(tag);
	return temp;
}

void HelloWorld::createBricks() {
	int padding = 5;
	int xWidth = 25;
	int yWidth = 30;

	for (int i = 0; i<5; i++) {
		for (int j = 0; j<16; j++) {
			if(j%2==(i%2)){//Create checkered box pattern
				Sprite* block = createSprite("Sprites/"+to_string(i)+".png",(xWidth+padding)*(j+1),330-(yWidth*i),Type::BLOCK);

				PhysicsBody* blockBody = PhysicsBody::createBox(block->getContentSize()-Size(padding,padding),blockMaterial);
				blockBody->setContactTestBitmask(0x000001);

				block->setPhysicsBody(blockBody);
				this->addChild(block, ZScore::OBJECT);
			}
		}
	}
}

void HelloWorld::createEdge() {
	edgeSp = createSprite("",visibleSize.width/2,visibleSize.height/2, Type::EDGE);

	PhysicsBody* boundBody = PhysicsBody::createEdgeBox(visibleSize,edgeMaterial);
	boundBody->setContactTestBitmask(0x000001);

	edgeSp->setPhysicsBody(boundBody);
	this->addChild(edgeSp,ZScore::OBJECT);
}

void HelloWorld::createBall() {
	ball = createSprite("Sprites/Ball.png",100,100,Type::BALL);

	PhysicsBody* ballBody = PhysicsBody::createCircle(ball->getContentSize().width/2,ballMaterial);
	ballBody->setGravityEnable(false); 
	ballBody->applyImpulse(Vec2(ballSpeed,ballSpeed)); 
	ballBody->setContactTestBitmask(0x000001);

	ball->setPhysicsBody(ballBody);
	this->addChild(ball,ZScore::OBJECT);
}

void HelloWorld::createBallParticles() {
	ballParticle = new ParticleSystemQuad();
	ballParticle = ParticleFireworks::create();

	customizeParticle(ballParticle,ballParticleColour,Color4F::BLACK,0.5,100,400,5,0.001, ball->getPosition());
	ballParticle->setDuration(-1);

	this->addChild(ballParticle);
}

void HelloWorld::customizeParticle(ParticleSystemQuad* particle , Color4F startColour,Color4F endColour,
	float opacity,int speed,int totalParticles,float startSize,float endSize, Vec2 position) {
	
	particle->setStartColor(startColour);
	particle->setEndColor(endColour);

	particle->setOpacity(opacity);
	particle->setSpeed(speed);
	particle->setTotalParticles(totalParticles);

	particle->setStartSize(startSize);
	particle->setEndSize(endSize);
	particle->setPosition(position);
}

void HelloWorld::createPaddle() {
	paddle = Sprite::create("Paddle.png");
	PhysicsBody* paddleBody = PhysicsBody::createBox(paddle->getContentSize(),paddleMaterial);

	paddleBody->setGravityEnable(false);
	paddleBody->setDynamic(false);
	paddleBody->setContactTestBitmask(0x000001);

	paddle->setPosition(visibleSize.width/2,15);
	paddle->setPhysicsBody(paddleBody);

	ball->setTag(Type::PADDLE);
	this->addChild(paddle,ZScore::OBJECT);
}

void HelloWorld::loadAudio() {
	audio->preloadBackgroundMusic(audioBGMPath);
	audio->playBackgroundMusic(audioBGMPath, true);

	audio->preloadEffect(audioCollidePath);
	audio->preloadEffect(audioLifeLostPath);
}

void HelloWorld::createHUD() {
	Size screenSize = this->getContentSize();

	scoreLabel = Label::createWithTTF(scoreString+to_string(score),hudFontPath,hudFontSize);
	scoreLabel->setAnchorPoint(Vec2::ZERO);
	scoreLabel->setPosition(Vec2(10,35));

	livesLabel = Label::createWithTTF(livesString+to_string(lives),hudFontPath,hudFontSize);
	livesLabel->setAnchorPoint(Vec2::ZERO);
	livesLabel->setPosition(Vec2(10,10));

	this->addChild(scoreLabel,1);
	this->addChild(livesLabel,1);
}

void HelloWorld::update(float delta) {
	ballParticle->setPosition(ball->getPosition());

	move(delta);
	checkIfBallOutOfBounds();
	checkWin();
}

void HelloWorld::move(float delta) {
	Vec2 position = paddle->getPosition();
	Vec2 move = Vec2(paddleSpeed*delta,0);

	float maxX = this->getBoundingBox().getMaxX();
	float widthLimit = paddle->getBoundingBox().size.width/2;

	switch (curDirection) {
	case Direction::LEFT:
		paddle->setPosition(position-move);
		break;
	case Direction::RIGHT:
		paddle->setPosition(position+move);
		break;
	}

	if (position.x<0-widthLimit) {
		paddle->setPosition(Vec2(maxX+widthLimit, position.y));
	}
	else if (position.x>maxX+widthLimit+1) {
		paddle->setPosition(Vec2(0-widthLimit, position.y));
	}
}

void HelloWorld::checkIfBallOutOfBounds() {
	Vec2 oldPosition = ball->getPosition();
	Vec2 newPosition = oldPosition;
	int padding = 15;

	if (oldPosition.x<-padding) {
		newPosition.x = padding;
	}
	else if (oldPosition.x>visibleSize.width+padding) {
		newPosition.x = visibleSize.width-padding;
	}
	else if(oldPosition.y<-padding){
		newPosition.y = padding;
	}
	else if (oldPosition.y>visibleSize.height+padding) {
		newPosition.y = visibleSize.height-padding;
	}
	ball->setPosition(newPosition);
}

void HelloWorld::checkWin() {
	bool isWin = true;

	Vector<PhysicsBody*>bodies = physicsWorld->getAllBodies();

	for each (PhysicsBody* body in bodies) {
		if (body->getNode()->getTag()==Type::BLOCK) {
			isWin = false;
		}
	}
	if (isWin==true) {
		win();
	}
}

void HelloWorld::win() {
	auto gameOverScene = GameOverScene::create();
	gameOverScene->getLayer()->getLabel()->setString("You Win!");
	Director::getInstance()->replaceScene(gameOverScene);
	Director::getInstance()->pause();
}

void HelloWorld::lose() {
	decrementLives();

	if(lives<0){
		auto gameOverScene = GameOverScene::create();
		gameOverScene->getLayer()->getLabel()->setString("You Lose!");
		Director::getInstance()->replaceScene(gameOverScene);
		//Director::getInstance()->pause();
	}
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
		remvoveBlock(spriteA);
	}

	if (tagB==Type::BLOCK) {
		remvoveBlock(spriteB);
	}

	if ((tagA==Type::EDGE||tagB==Type::EDGE)& (ball->getPositionY()<=paddle->getPositionY())) {
		lose();
	}
}

void HelloWorld::remvoveBlock(Sprite * block) {
	ParticleSystemQuad* b = new ParticleSystemQuad();
	b= ParticleExplosion::create();

	customizeParticle(b,Color4F::WHITE,Color4F::WHITE,0.5,300,400,7.0, 3.0, block->getPosition());
	b->setEndColorVar(Color4F::BLACK);

	this->addChild(b);

	this->removeChild(block,true);

	audio->playEffect(audioCollidePath);
	incrementScore(10);
}

void HelloWorld::incrementScore(int add) {
	score += add;
	scoreLabel->setString(scoreString+to_string(score));
}

void HelloWorld::decrementLives() {
	lives--;
	livesLabel->setString(livesString+to_string(lives<0?0:lives));

	ParticleSystemQuad* p = new ParticleSystemQuad();
	p = ParticleExplosion::create();

	customizeParticle(p,Color4F::YELLOW,Color4F::BLACK,0.35,500,750,20.0,40.0,ball->getPosition());
	p->setEndColorVar(Color4F::WHITE);

	audio->playEffect(audioLifeLostPath);
	this->addChild(p);
}

void HelloWorld::onKeyPress(EventKeyboard::KeyCode keycode,Event * event) {
	switch (keycode) {
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		curDirection = Direction::LEFT;
		break;
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		curDirection = Direction::RIGHT;
		break;
	}
}

void HelloWorld::onKeyRelease(EventKeyboard::KeyCode,Event *) {
	curDirection = Direction::NONE;
}