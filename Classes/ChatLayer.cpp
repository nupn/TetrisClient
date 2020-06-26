#include "ChatLayer.h"
#include "CServerMgr.h"
#include "msg.pb.h"
#include "CPlayer.h"
#include "protocol.h"


CChatLayer::CChatLayer(const stChatLayerSetting& setting)
{
	m_stSetting = setting;
}

 
CChatLayer::~CChatLayer()
{

}


bool CChatLayer::init()
{
	const int nWidth = m_stSetting.nWidth;
	const int nHeight = m_stSetting.nHeight;
	const int nInputTextHeight = m_stSetting.nFontSize + 6;

	const int RedDotImgSize = 12;

	//텍스트바 가이드 이미지
	auto pRedDot = Sprite::create("PosPoint.png");
	pRedDot->setScaleX(nWidth / RedDotImgSize);
	pRedDot->setScaleY(nInputTextHeight / RedDotImgSize);
	pRedDot->setAnchorPoint(Vec2::ZERO);
	this->addChild(pRedDot);


	pRedDot = Sprite::create("PosPoint.png");
	//pRedDot->setPosition(100, 200);
	pRedDot->setScaleX(nWidth / RedDotImgSize);
	pRedDot->setScaleY(nHeight / RedDotImgSize);
	pRedDot->setOpacity(0.5);
	pRedDot->setAnchorPoint(Vec2::ZERO);

	this->addChild(pRedDot);

	auto pTextField = TextFieldKR::textFieldWithPlaceHolder("<click here for input>",
		Size(nWidth, nInputTextHeight),
		TextHAlignment::LEFT,
		"fonts/malgun.ttf",
		m_stSetting.nFontSize);
	pTextField->setAnchorPoint(Vec2::ZERO);
	pTextField->setTag(20);
	pTextField->setLineBreakWithoutSpace(true);
	addChild(pTextField);
	
	// For Test
	for (int i = 0; i < 50; ++i)
	{
		std::string strTemp = StringUtils::format("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa%d", i);
		m_vecChatMsg.push_back(strTemp);
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(InputText::onTouchBegan, static_cast<InputText*>(this));
	listener->onTouchEnded = CC_CALLBACK_2(InputText::onTouchEnded, static_cast<InputText*>(this));
	//listener->onTouchBegan = CC_CALLBACK_2(InputText::onTouchBegan, this);
	//listener->onTouchEnded = CC_CALLBACK_2(InputText::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	AddTrackNode(pTextField);

	TableView* tableView = TableView::create(this, Size(nWidth, nHeight - nInputTextHeight));
	tableView->setDirection(ScrollView::Direction::VERTICAL);
	tableView->setPosition(Vec2(0, nInputTextHeight + 3));
	tableView->setDelegate(this);
	tableView->setBounceable(false);
	tableView->setVerticalFillOrder(TableView::VerticalFillOrder::BOTTOM_UP);
	this->addChild(tableView);
	tableView->reloadData();
	m_ptableView = tableView;

	bar = ScrollBar::creatScrollbar(tableView);

	listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(ScrollBar::onTouchBegan, bar);
	listener->onTouchMoved = CC_CALLBACK_2(ScrollBar::onTouchMoved, bar);
	listener->onTouchEnded = CC_CALLBACK_2(ScrollBar::onTouchEnded, bar);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = CC_CALLBACK_2(CChatLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	AddTrackNode(pTextField);




	return Layer::init();
}


void CChatLayer::scrollViewDidScroll(ScrollView* view) {
	if (bar != nullptr && view != nullptr)
	{
		bar->OffsetRefresh(view->getContentOffset().y);
	}
}


void CChatLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
	CCLOG("cell touched at index: %ld", static_cast<long>(cell->getIdx()));
}

Size CChatLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(m_stSetting.nWidth, m_stSetting.nFontSize *2 + 3);
}

TableViewCell* CChatLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
	const int nWidth = m_stSetting.nWidth;
	const int nInputTextHeight = m_stSetting.nFontSize + 3;

	//auto string = StringUtils::format("aaaa:aaaaaaaaaaaaaaaasdsda%ld", static_cast<long>(idx));
	string msg;
	if (idx < m_vecChatMsg.size())
	{
		msg = m_vecChatMsg.at(idx);
	}

	TableViewCell *cell = table->dequeueCell();
	if (!cell) {
		cell = new (std::nothrow) TableViewCell();
		cell->autorelease();
		cell->setContentSize(Size(nWidth, nInputTextHeight * 2));

		auto label = Label::createWithTTF(msg, "fonts/malgun.ttf", m_stSetting.nFontSize);
		label->setLineBreakWithoutSpace(true);
		label->setPosition(Vec2::ZERO);
		label->setAnchorPoint(Vec2::ZERO);
		label->setTag(123);
		cell->addChild(label);
	}
	else
	{
		auto label = (Label*)cell->getChildByTag(123);
		label->setString(msg);
	}


	return cell;
}

ssize_t CChatLayer::numberOfCellsInTableView(TableView *table)
{
	return m_vecChatMsg.size();
}

void CChatLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{

	if (keyCode == EventKeyboard::KeyCode::KEY_ENTER)
	{
		TextFieldKR* pTextField = static_cast<TextFieldKR*>(this->getChildByTag(20));

		if (m_vecChatMsg.size() >= m_nMaxChatMsg)
		{
			m_vecChatMsg.pop_front();
		}


		CPlayer& player = boost::serialization::singleton <CPlayer> ::get_mutable_instance();
		if (!player.IsLogin()) {
			return;
		}

		game::chat_message chatMsg;
		chatMsg.set_id(player.Id());
		chatMsg.set_name(player.Name().c_str());
		chatMsg.set_msg(pTextField->GetString().c_str());

		int size = chatMsg.ByteSizeLong() + 8;
		
		char sendbuf[1024];
		google::protobuf::io::ArrayOutputStream aos(sendbuf, size);
		CodedOutputStream* coded_output = new CodedOutputStream(&aos);

		coded_output->WriteVarint32(C_CHAT_MSG);
		coded_output->WriteVarint32(chatMsg.ByteSizeLong());
		chatMsg.SerializeToCodedStream(coded_output);

		CServerMgr& svrs = boost::serialization::singleton<CServerMgr>::get_mutable_instance();
		svrs.SendPacket(ServerType::CHAT_SERVER, sendbuf, size);

		pTextField->Clear();
	}
}


void CChatLayer::Handle(int packetType, CodedInputStream* meesage)
{
	if (m_vecChatMsg.size() >= m_nMaxChatMsg)
	{
		m_vecChatMsg.pop_front();
	}

	if (packetType != S_CHAT_MSG) {//chat packet Type
		return;
	}

	game::chat_message payload;
	google::protobuf::io::CodedInputStream::Limit msgLimit = meesage->PushLimit(1024);
	if (!payload.ParseFromCodedStream(meesage)) {
		printf("        parseFromCodedStream Fail\n");
	}
	meesage->PopLimit(1024);
	printf("        payload: %d\n", payload.id());
	printf("        payload: %s\n", payload.name().c_str());
	printf("        payload: %s\n", payload.msg().c_str());

	char buf[128];
	snprintf(buf, sizeof(buf), "%s : %s", payload.name().c_str(), payload.msg().c_str());


	m_vecChatMsg.push_back(buf);

	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]()->void
		{

			m_ptableView->reloadData();

			bar->ContentRefresh(m_ptableView->getContentSize().height);
			bar->OffsetRefresh(m_ptableView->getContentOffset().y);
		});

}

void CChatLayer::PushMessage(string message)
{
	if (m_vecChatMsg.size() >= m_nMaxChatMsg)
	{
		m_vecChatMsg.pop_front();
	}

	m_vecChatMsg.push_back(message);
	m_ptableView->reloadData();

	bar->ContentRefresh(m_ptableView->getContentSize().height);
	bar->OffsetRefresh(m_ptableView->getContentOffset().y);
}

void ChatScrollView::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	TableViewCell::draw(renderer, transform, flags);
}
