#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

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
	void createHUD();
	void createBall();
	void createBallParticles();
	void customizeParticle(ParticleSystemQuad*, Color4F, Color4F, float, int, int, float, float, Vec2);
	void createPaddle();
	void loadAudio();

	void update(float);
	void move(float);
	void checkIfBallOutOfBounds();
	void checkWin();

	void remvoveBlock(Sprite*);
	void incrementScore(int);
	void decrementLives();

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
	//Constants
	enum Type { BALL,PADDLE,BLOCK,EDGE };
	enum Direction { NONE,RIGHT,LEFT };
	enum ZScore{HUD, OBJECT};
	Direction curDirection = Direction::NONE;

	//Objects
	Sprite* ball;
	Sprite* paddle;
	Sprite* edgeSp;

	ParticleSystemQuad* ballParticle;
	Color4F ballParticleColour = Color4F(0.25,0.25,0.75,0.75);

	float ballSpeed = 55000.0f;
	float paddleSpeed = 450;

	//HUD
	Label* scoreLabel;
	string scoreString = "Score: ";
	Label* livesLabel;
	string livesString = "Lives: ";
	int score = 0;
	int lives = 5;
	const char* hudFontPath = "fonts/pixelmix.ttf";
	int hudFontSize = 20;

	//Audio and audio paths
	//const char* audioBGMPath = "Audio/Bgm/Neon.mp3";
	const char* audioBGMPath = "Audio/Bgm/Substance.mp3";
	const char* audioCollidePath = "Audio/Effects/Bassdrum.wav";
	//const char* audioCollidePath = "Audio/Effects/Wowpulse.wav";
	const char* audioLifeLostPath = "Audio/Effects/Glitch.wav";
	CocosDenshion::SimpleAudioEngine* audio = CocosDenshion::SimpleAudioEngine::getInstance();

	//Physics world
	PhysicsWorld* physicsWorld;
	void setPhyWorld(PhysicsWorld* world) { physicsWorld = world; };

	//Physics material
	PhysicsMaterial blockMaterial = PhysicsMaterial(20.0f,1.0f,0.0f);
	PhysicsMaterial edgeMaterial = PhysicsMaterial(20.0f,1.0f,0.0f);
	PhysicsMaterial ballMaterial = PhysicsMaterial(1.0f,1.0f,0.0f);
	PhysicsMaterial paddleMaterial = PhysicsMaterial(20.f,1.0f,0.0f);

	//Size constants
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

};

#endif // __HELLOWORLD_SCENE_H__
