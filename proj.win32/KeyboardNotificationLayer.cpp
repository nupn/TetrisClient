#include "KeyboardNotificationLayer.h"

USING_NS_CC;

#define FONT_NAME                       "fonts/Thonburi.ttf"
#define FONT_SIZE                       36

static Rect getRect(Node * node)
{
	Rect rc;
	rc.origin = node->getPosition();
	rc.size = node->getContentSize();
	rc.origin.x -= rc.size.width / 2;
	rc.origin.y -= rc.size.height / 2;
	return rc;
}

//////////////////////////////////////////////////////////////////////////
// implement KeyboardNotificationLayer
//////////////////////////////////////////////////////////////////////////

InputText::InputText()
{
	// Register Touch Event

}

void InputText::keyboardWillShow(IMEKeyboardNotificationInfo& info)
{
	/*
	CCLOG("TextInputTest:keyboardWillShowAt(origin:%f,%f, size:%f,%f)",
		info.end.origin.x, info.end.origin.y, info.end.size.width, info.end.size.height);

	if (!_trackNode)
	{
		return;
	}

	auto rectTracked = getRect(_trackNode);
	CCLOG("TextInputTest:trackingNodeAt(origin:%f,%f, size:%f,%f)",
		rectTracked.origin.x, rectTracked.origin.y, rectTracked.size.width, rectTracked.size.height);

	// if the keyboard area doesn't intersect with the tracking node area, nothing need to do.
	if (!rectTracked.intersectsRect(info.end))
	{
		return;
	}

	// assume keyboard at the bottom of screen, calculate the vertical adjustment.
	float adjustVert = info.end.getMaxY() - rectTracked.getMinY();
	CCLOG("TextInputTest:needAdjustVerticalPosition(%f)", adjustVert);

	*/

	// move all the children node of KeyboardNotificationLayer
	/*
	auto& children = getChildren();
	Node * node = 0;
	ssize_t count = children.size();
	Vec2 pos;
	for (int i = 0; i < count; ++i)
	{
		node = children.at(i);
		pos = node->getPosition();
		pos.y += adjustVert;
		node->setPosition(pos);
	}
	*/
}

// Layer function

void InputText::AddTrackNode(TextFieldKR* pNode)
{
	auto iter = std::find(std::begin(_trackNodes), std::end(_trackNodes), pNode);
	if (iter == std::end(_trackNodes))
	{
		_trackNodes.emplace_back(pNode);
	}
}


bool InputText::onTouchBegan(Touch  *touch, Event  *event)
{
	_beginPos = touch->getLocation();
	return true;
}


void InputText::onTouchEnded(Touch  *touch, Event  *event)
{
	for (auto node : _trackNodes)
	{
		auto endPos = touch->getLocation();

		float delta = 5.0f;
		if (std::abs(endPos.x - _beginPos.x) > delta
			|| std::abs(endPos.y - _beginPos.y) > delta)
		{
			// not click
			_beginPos.x = _beginPos.y = -1;
			return;
		}

		// decide the trackNode is clicked.
		Rect rect;
		rect.size = node->getContentSize();
		auto clicked = isScreenPointInRect(endPos, Camera::getVisitingCamera(), node->getWorldToNodeTransform(), rect, nullptr);
		if (clicked)
		{
			// TextFieldTTFTest be clicked
			node->attachWithIME();
			node->setCursorFromPoint(endPos, Camera::getVisitingCamera());

		}
		else
		{
			// TextFieldTTFTest not be clicked
			node->detachWithIME();
		}
	}

}