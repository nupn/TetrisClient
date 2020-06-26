#include "LobbyScene.h"
#include "SimpleAudioEngine.h"
#include "TextFieldKR.h"
#include "CServerMgr.h"
#include "msg.pb.h"
#include "CPlayer.h"


using namespace google::protobuf::io;

CLobbyScene::~CLobbyScene()
{
}

Scene* CLobbyScene::createScene()
{
	CLobbyScene* scene = CLobbyScene::create();
	return scene;
}

// on "init" you need to initialize your instance
bool CLobbyScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}


	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(CLobbyScene::menuCloseCallback, this));

	if (closeItem == nullptr ||
		closeItem->getContentSize().width <= 0 ||
		closeItem->getContentSize().height <= 0)
	{

	}
	else
	{
		float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
		float y = origin.y + closeItem->getContentSize().height / 2;
		closeItem->setPosition(Vec2(x, y));
	}

	auto nextButton = MenuItemImage::create(
		"ArrowRight01.png",
		"ArrowRight02.png",
		CC_CALLBACK_1(CLobbyScene::onNextClick, this));
	nextButton->setPosition(Vec2::Vec2(origin.x + visibleSize.width/2  - nextButton->getContentSize().width / 2 + 30,  300));

	auto prevButton = MenuItemImage::create(
		"ArrowLeft01.png",
		"ArrowLeft02.png",
		CC_CALLBACK_1(CLobbyScene::onPrevClick, this));
	prevButton->setPosition(Vec2::Vec2(origin.x + visibleSize.width / 2 - prevButton->getContentSize().width / 2 - 30, 300));


	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, prevButton, nextButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);


	stChatLayerSetting setting;
	setting.nWidth = 900;
	setting.nHeight = 300;
	setting.nFontSize = 24;

	m_pChatLayer = CChatLayer::create(setting);
	m_pChatLayer->setPosition(Vec2::Vec2(70, 70));
	addChild(m_pChatLayer);

	/*
	auto pTextField = TextFieldTTF::textFieldWithPlaceHolder("<click here for input>",
	"fonts/arial.ttf",
	36);
	pTextField->setPosition(500, 100);
	addChild(pTextField);
	//*/


	//auto pInputText = new InputText;
	//pInputText->setPosition(100, 100);
	//this->addChild(pInputText);
	//pInputText->_trackNode = pTextField;

	/*
	CRoomListLayer* pRoomList = CRoomListLayer::create();
	pRoomList->setPosition(30, 350);
	pRoomList->setAnchorPoint(Vec2::ZERO);
	addChild(pRoomList);

	m_pRoomListLayer = pRoomList;
	*/
	this->schedule(schedule_selector(CLobbyScene::CheckLogin), 1.0f);

	CServerMgr& svrs = boost::serialization::singleton<CServerMgr>::get_mutable_instance();
	if (!svrs.IsConnect(ServerType::CHAT_SERVER)){
		svrs.Connect(ServerType::CHAT_SERVER);
	}
	return true;
}

void CLobbyScene::CheckLogin(float f)
{
	CServerMgr& svrs = boost::serialization::singleton<CServerMgr>::get_mutable_instance();
	if (svrs.IsConnect(ServerType::CHAT_SERVER)) {
		CPlayer& player = boost::serialization::singleton <CPlayer> ::get_mutable_instance();
		if (!player.IsLogin()) {
			player.Login();
		}
	}
}


void CLobbyScene::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	//Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void CLobbyScene::Handle(int packetType, CodedInputStream* meesage)
{


	if (!m_pChatLayer)
	{
		return;
	}
}


void CLobbyScene::onPrevClick(cocos2d::Ref* pSender)
{
	
}

void CLobbyScene::onNextClick(cocos2d::Ref* pSender)
{

}

void CLobbyScene::onEnter()
{
	Scene::onEnter();

}