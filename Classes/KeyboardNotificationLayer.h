#pragma once
#include "cocos2d.h"
#include "TextFieldKR.h"


class InputText : cocos2d::IMEDelegate
{
public:
	InputText();

	virtual void keyboardWillShow(cocos2d::IMEKeyboardNotificationInfo& info) override;

	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

	void AddTrackNode(TextFieldKR* pNode);

private:
	std::vector<TextFieldKR*>  _trackNodes;
	cocos2d::Vec2  _beginPos;
};