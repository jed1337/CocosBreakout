#ifndef _MYSPRITE_HPP_
#define _MYSPRITE_HPP_

#include "cocos2d.h"

class Block: public cocos2d::Sprite
{
public:
	Block();
	~Block();
	//static MySprite* create();

	void initOptions();

	void addEvents();
	//void touchEvent(cocos2d::Touch* touch,cocos2d::Vector2 _p);

private:

};

#endif // _MYSPRITE_HPP_