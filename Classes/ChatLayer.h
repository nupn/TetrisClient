#pragma once
#include "cocos2d.h"
#include "ScrollBar.h"
#include <deque>
#include "extensions/cocos-ext.h"
#include "KeyboardNotificationLayer.h"
#include "PacketHandler.h"
using namespace google::protobuf::io;
using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;


//여러줄 입력처리
struct stChatLayerSetting
{
	int nWidth = 0;
	int nHeight = 0;
	int nFontSize = 0;
};


class CChatLayer : public Layer,
	public InputText,
	public cocos2d::extension::TableViewDataSource,
	public cocos2d::extension::TableViewDelegate,
	public PacketHandler
{
public:
	static CChatLayer* create(const stChatLayerSetting& setting)
	{ 
		CChatLayer *pRet = new(std::nothrow) CChatLayer(setting);
		if (pRet && pRet->init()) 
		{
			pRet->autorelease(); 
			return pRet; 
		} 
		else 
		{ 
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	};

	CChatLayer(const stChatLayerSetting& setting);
	virtual ~CChatLayer();

	virtual bool init() override;


	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view)override;
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view)override {};

	virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell)override;
	virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx)override;
	virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx)override;
	virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table)override;

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void PushMessage(string message);
	
	virtual void Handle(int packetType, CodedInputStream* meesage);

private:
	ScrollBar* bar = nullptr;
	deque<string> m_vecChatMsg;
	int			m_nMaxChatMsg = 100;
	TableView* m_ptableView;
	stChatLayerSetting m_stSetting;
};


class ChatScrollView : public cocos2d::extension::TableViewCell
{
public:
	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
};
