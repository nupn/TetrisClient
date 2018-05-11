#include "ScrollBar.h"

ScrollBar::ScrollBar(ScrollView* sv) {
    //bar 영역
    this->setAnchorPoint(ccp(0,0));
    if ( !LayerColor::initWithColor(ccc4(0, 255, 0, 0)) )
    {
        return;
    }


	int nXOffset = 2;
    
    Point svPosition = sv->getPosition();
    //CCLog("svPosition (%3.2f, %3.2f)", svPosition.x, svPosition.y);
    //type 따른 변화 적용
    float offset;
	MenuItemImage* button;
	MenuItemImage* button2;

    //bar 생성
    bar = cocos2d::extension::Scale9Sprite::create("Bar.png",
                                    CCRectMake(0, 0, 22, 22),           //이미지에서 사용할 영역
                                    CCRectMake(0, 3, 22, 16));         //이미지에서 늘어날 영역

    bar->setAnchorPoint(ccp(0.5,0));
    bar->setPosition(ccp(bar->getContentSize().width/2, 0));
        

	selectBar = Sprite::create();
	selectBar->setTextureRect(cocos2d::Rect(0, 0, 22, 22));
	selectBar->setColor(ccc3(255, 0, 0));
	selectBar->setOpacity(200);
	selectBar->setAnchorPoint(ccp(0.5f, 0.0f));
	selectBar->setPosition(ccp(bar->getContentSize().width / 2, 0));
		
	CCSize svSize = sv->cocos2d::CCNode::getContentSize();
	m_fScrollViewSize = svSize.height;
	m_nBarMinSize = 22;

	button = MenuItemImage::create("ArrowUp01.png", "ArrowUp02.png");
	button->setPosition(Vec2(0, svSize.height));

	button->setAnchorPoint(Vec2(0, 1));
	button->setOpacity(100);
	button->setTag(0);


	button2 = MenuItemImage::create("DownArrow01.png", "DownArrow02.png");
	button2->setPosition(Vec2(0, 0));

	button2->setAnchorPoint(Vec2(0, 0));
	button2->setOpacity(100);
	button2->setTag(1);

	m_fbarMoveableHeigth = svSize.height - button2->getContentSize().height - button->getContentSize().height;
	m_nBarFloatingHeight = button2->getContentSize().height;

	m_fScrollViewContentSize = sv->getContentSize().height;
    
	//this->setPosition((svPosition.x + svSize.width) + nXOffset, svPosition.y + button2->getContentSize().height);
	this->setPosition((svPosition.x + svSize.width) + nXOffset, svPosition.y);

    
	if (m_fScrollViewContentSize <= m_fScrollViewSize)
	{
		m_bEnable = true;
		this->setVisible(false);
	}
	else
	{
		m_bEnable = false;
		this->setVisible(true);
	}

    this->addChild(bar);
    //바 크기 설정
    __setBarScale();
    //바 시작 위치 설정
	offset = sv->getContentOffset().y;
    __setBarPosition(offset);

	this->addChild(button);
	this->addChild(button2);
	this->addChild(selectBar);


    CCNode* parent = sv->getParent();
    parent->addChild(this, 1000, 1000);
	
	m_scrollView = sv;
}

ScrollBar::~ScrollBar() {
//    if (bar) {
//        bar = NULL;
//    }
}

ScrollBar* ScrollBar::creatScrollbar(ScrollView* sv) {
	ScrollBar* obj = new ScrollBar(sv);
    if(obj) {
        obj->autorelease();
        return obj;
    } else {
        delete obj;
        return NULL;
    }
//    return obj;
}

/**=====================================================================////
 
 private 함수
 
 /======================================================================///*/

//bar의 위치를 변화시킨다.
void ScrollBar::__setBarPosition(float offset) {
    //type으로 따지지 않는 이유는 생성시 변화는 값을 따라 들어감으로..
    offset *= -1;
	//svFixSize,nowContentSize contentsize height
	//
    float result = (((offset- m_fScrollViewSize)/ m_fScrollViewContentSize) * m_fbarMoveableHeigth) + m_fBarSize;

    //세로
    if(result < 0|| result > (m_fbarMoveableHeigth - m_fBarSize + 1)) return;
        
    bar->setPosition(ccp(bar->getPosition().x, result + m_nBarFloatingHeight));
	selectBar->setPosition(ccp(bar->getPosition().x, result + m_nBarFloatingHeight));
}

//bar의 크기를 변화시킨다.
void ScrollBar::__setBarScale() {
    //type으로 따지지 않는 이유는 생성시 변화는 값을 따라 들어감으로..
	float fBarSize = (m_fScrollViewSize /m_fScrollViewContentSize) * m_fbarMoveableHeigth;
	if (fBarSize < m_nBarMinSize || fBarSize >= m_fScrollViewSize)
	{
		return;
	}

	auto size = bar->getContentSize();
	size.setSize(size.width, fBarSize);
	m_fBarSize = fBarSize;
	bar->setContentSize(size);

	selectBar->setContentSize(CCSize(size.width + 30, size.height));
	//selectBar->setPosition(ccp(bar->getContentSize().width / 2, 0));
}

/**=====================================================================////
 
 public 함수
 
 /======================================================================///*/

//주기적으로 호출 되면서 내용을 갱신할 함수 -> scrollViewDidScroll에서 호출됨.
void ScrollBar::OffsetRefresh(float offSet) 
{
    __setBarPosition(offSet);
}


void ScrollBar::ContentRefresh(float fContentSize)
{
	if (fContentSize <= m_fScrollViewSize)
	{
		m_bEnable = false;
		this->setVisible(false);
	}
	else
	{
		m_bEnable = true;
		this->setVisible(true);
	}

	if (m_bEnable)
	{
		if (fContentSize != m_fScrollViewContentSize)
		{
			m_fScrollViewContentSize = fContentSize;
			__setBarScale();
		}
	}
}


bool ScrollBar::onTouchBegan(Touch  *touch, Event  *event)
{
	if (m_bSchdule)
	{
		m_bSchdule = false;
		CCDirector::sharedDirector()->getScheduler()->unschedule(CC_SCHEDULE_SELECTOR(ScrollBar::update), this);
	}

	auto touchPose = touch->getLocation();
	for (int i = 0; i < 2; ++i)
	{
		MenuItemImage *btn = (MenuItemImage*)this->getChildByTag(i);
		if (btn)
		{
			Rect rect{};
			rect.size = btn->getContentSize();
			auto offset = m_scrollView->getContentOffset();

			if (isScreenPointInRect(touchPose, Camera::getVisitingCamera(), btn->getWorldToNodeTransform(), rect, nullptr))
			{

				int value = btn->getTag() > 0 ? 1 : -1;
				offset.y = offset.y + (m_fScrollViewSize / 10.0f) * value;
				//CCLOG("onTouchMoved111 (%3.2f)", offset.y);
				m_scrollView->setContentOffset(offset, false);

				btn->selected();
				CCDirector::sharedDirector()->getScheduler()->schedule(CC_SCHEDULE_SELECTOR(ScrollBar::update), this, 0.2f, false);
				m_bSchdule = true;
				m_nPressingButtonTag = i;
			}
		}
	}


	if (m_bDragging)
	{
		return true;
	}

	auto beginPos = touch->getLocation();
	if (m_scrollView)
	{
		m_fBeginOffset = m_scrollView->getContentOffset().y;
	}


	Rect rect{};
	rect.size = selectBar->getContentSize();
	auto clicked = isScreenPointInRect(beginPos, Camera::getVisitingCamera(), selectBar->getWorldToNodeTransform(), rect, nullptr);
	if (clicked)
	{
		m_bDragging = true;
		m_fBeginTouchPos = beginPos.y;
	}
	else
	{
		m_bDragging = false;
	}

	return true;
}

void ScrollBar::onTouchMoved(Touch  *touch, Event  *event)
{
	if (m_bSchdule)
	{
		if (m_nPressingButtonTag < 0 || m_nPressingButtonTag > 1)
		{
			m_bSchdule = false;
			CCDirector::sharedDirector()->getScheduler()->unschedule(CC_SCHEDULE_SELECTOR(ScrollBar::update), this);
			return;
		}

		auto touchPose = touch->getLocation();

		MenuItemImage *btn = (MenuItemImage*)this->getChildByTag(m_nPressingButtonTag);
		if (btn)
		{
			Rect rect{};
			rect.size = btn->getContentSize();
			if (!isScreenPointInRect(touchPose, Camera::getVisitingCamera(), btn->getWorldToNodeTransform(), rect, nullptr))
			{
				m_bSchdule = false;
				CCDirector::sharedDirector()->getScheduler()->unschedule(CC_SCHEDULE_SELECTOR(ScrollBar::update), this);
				m_nPressingButtonTag = -1;
			}
		}
	}

	if (m_bDragging)
	{
		auto currentPos = touch->getLocation();
		auto distant = (m_fBeginTouchPos - currentPos.y) / (m_fbarMoveableHeigth - m_fBarSize);
		if (m_scrollView)
		{
			auto currentScollOffset = m_scrollView->getContentOffset();

			currentScollOffset.y = m_fBeginOffset + (m_fScrollViewContentSize - m_fScrollViewSize) * distant;
			///*
			if (currentScollOffset.y < m_fScrollViewSize - m_fScrollViewContentSize)
			{
				currentScollOffset.y = m_fScrollViewSize - m_fScrollViewContentSize;
			}
			else if (currentScollOffset.y > 0)
			{
				currentScollOffset.y = 0;
			}
			//*/

			m_scrollView->setContentOffset(currentScollOffset, false);
		}
	}
}

void ScrollBar::onTouchEnded(Touch  *touch, Event  *event)
{

	m_bSchdule = false;
	CCDirector::sharedDirector()->getScheduler()->unschedule(CC_SCHEDULE_SELECTOR(ScrollBar::update), this);


	auto offset = m_scrollView->getContentOffset();
	
	auto touchPose = touch->getLocation();
	for (int i = 0; i < 2; ++i)
	{
		MenuItemImage *btn = (MenuItemImage*)this->getChildByTag(i);
		if (btn)
		{
			Rect rect{};
			rect.size = btn->getContentSize();
			/*
			if (isScreenPointInRect(touchPose, Camera::getVisitingCamera(), btn->getWorldToNodeTransform(), rect, nullptr))
			{
				int value = btn->getTag() > 0 ? 1 : -1;
				offset.y = offset.y + (svFixSize / 10.0f) * value;
				m_scrollView->setContentOffset(offset, false);
			}
			*/

			btn->unselected();
		}
	}


	m_bDragging = false;
}

void ScrollBar::update(float /*dt*/)
{
	int value = m_nPressingButtonTag > 0 ? 1 : -1;

	auto offset = m_scrollView->getContentOffset();
	offset.y = offset.y + (m_fScrollViewSize / 10.0f) * value;

	m_scrollView->setContentOffset(offset, false);

}