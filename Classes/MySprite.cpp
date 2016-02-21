#include "MySprite.hpp"

using namespace cocos2d;

MySprite::MySprite() {}

MySprite::~MySprite() {}
//
//MySprite* MySprite::create() {
//	MySprite* pSprite = new MySprite();
//
//	if (pSprite->initWithSpriteFrameName("MySprite.png")) {
//		pSprite->autorelease();
//
//		pSprite->initOptions();
//
//		pSprite->addEvents();
//
//		return pSprite;
//	}
//
//	CC_SAFE_DELETE(pSprite);
//	return NULL;
//}

void MySprite::initOptions() {
	// do things here like setTag(), setPosition(), any custom logic.
}

void MySprite::addEvents() {
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [&](cocos2d::Touch* touch,cocos2d::Event* event) {
		cocos2d::Vector2 p = touch->getLocation();
		cocos2d::Rect rect = this->getBoundingBox();

		if (rect.containsPoint(p)) {
			return true; // to indicate that we have consumed it.
		}

		return false; // we did not consume this event, pass thru.
	};

	listener->onTouchEnded = [=](cocos2d::Touch* touch,cocos2d::Event* event) {
		MySprite::touchEvent(touch);
	};

	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener,30);
}

//void MySprite::touchEvent(cocos2d::Touch* touch,cocos2d::Vector2 _point) {
//	CCLOG("touched MySprite");
//}