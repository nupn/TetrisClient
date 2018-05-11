
#ifndef __ExTableView__Scrollbar__
#define __ExTableView__Scrollbar__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;

class ScrollBar : public LayerColor {
public:
    //자신의 크기를 정하고, 바를 생성
    ScrollBar(ScrollView* table);
    virtual ~ScrollBar();
    static ScrollBar* creatScrollbar(ScrollView* table);
    
    //주기적으로 호출 되면서 내용을 갱신할 함수
    void OffsetRefresh(float offSet);
	void ContentRefresh(float fContentSize);

	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) override;
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;

	
	void update(float /*dt*/);


private:
	void __setBarPosition(float offset);
	void __setBarScale();


private:
	bool	m_bEnable = false;
	int		m_nBarMinSize			 = 0;
	int		m_nBarFloatingHeight	 = 0;
    float	m_fScrollViewSize		 = 0;
	float	m_fScrollViewContentSize = 0;
	float	m_fbarMoveableHeigth	 = 0;
    float	m_fBarSize				= 0 ;
	int		m_nPressingButtonTag	= 0;


    
    //scrollbar 생성 단계에서 생성되며, 중간에 변형된다. 
    cocos2d::extension::Scale9Sprite* bar;
    
	float m_fBeginTouchPos;
	float m_fBeginOffset;

	bool		m_bDragging = false;
	bool		m_bSchdule = false;
	ScrollView* m_scrollView;
	Sprite*		selectBar;
};

#endif /* defined(__ExTableView__Scrollbar__) */
