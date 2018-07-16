#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "../proj.win32/KeyboardNotificationLayer.h"
#include "extensions/cocos-ext.h"
#include "ScrollBar.h"
#include "../proj.win32/PacketHandler.h"

#include <deque>
#include <string>

using namespace std;

class CLobbyScene : public cocos2d::Scene, InputText,
	public cocos2d::extension::TableViewDataSource,
	public cocos2d::extension::TableViewDelegate,
	public PacketHandler
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually


	//For Scoll View
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view)override;
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view)override {};

	virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell)override;
	virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx)override;
	virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx)override;
	virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table)override;


	ScrollBar* bar;


	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);

	CREATE_FUNC(CLobbyScene);

	virtual void Handle(const ServerMessage::Chat& message) override;
private:

	deque<string> m_vecChatMsg;
	int			m_nMaxChatMsg = 100;
	TableView* m_ptableView;
};

class ChatScrollView : public cocos2d::extension::TableViewCell
{
public:
	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
};

#endif // __HELLOWORLD_SCENE_H__
