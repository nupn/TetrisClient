/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "TextFieldKR.h"



USING_NS_CC;

//#pragma comment(lib, "NPCppTool.lib")

#define CURSOR_TIME_SHOW_HIDE 0.5f
#define CURSOR_DEFAULT_CHAR '|'
#define PASSWORD_STYLE_TEXT_DEFAULT "\xe2\x80\xa2"
static std::size_t _calcCharCount(const char * text)
{
	int n = 0;
	char ch = 0;
	while ((ch = *text))
	{
		CC_BREAK_IF(!ch);

		if (0x80 != (0xC0 & ch))
		{
			++n;
		}
		++text;
	}
	return n;
}


//////////////////////////////////////////////////////////////////////////
// constructor and destructor
//////////////////////////////////////////////////////////////////////////

TextFieldKR::TextFieldKR()
	: _charCount(0)
	, _placeHolder("")   // prevent Label initWithString assertion
	, _colorText(Color4B::WHITE)
	, _secureTextEntry(false)
	, _passwordStyleText(PASSWORD_STYLE_TEXT_DEFAULT)
	, _cursorEnabled(false)
	, _cursorPosition(0)
	, _cursorChar(CURSOR_DEFAULT_CHAR)
	, _cursorShowingTime(0.0f)
	, _isAttachWithIME(false)
{
	_colorSpaceHolder.r = _colorSpaceHolder.g = _colorSpaceHolder.b = 127;
	_colorSpaceHolder.a = 255;
}

TextFieldKR::~TextFieldKR()
{
}

//////////////////////////////////////////////////////////////////////////
// static constructor
//////////////////////////////////////////////////////////////////////////

TextFieldKR * TextFieldKR::textFieldWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize)
{
	TextFieldKR *ret = new (std::nothrow) TextFieldKR();
	if (ret && ret->initWithPlaceHolder("", dimensions, alignment, fontName, fontSize))
	{
		ret->autorelease();
		if (placeholder.size()>0)
		{
			ret->setPlaceHolder(placeholder);
		}
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

TextFieldKR * TextFieldKR::textFieldWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize)
{
	TextFieldKR *ret = new (std::nothrow) TextFieldKR();
	if (ret && ret->initWithPlaceHolder("", fontName, fontSize))
	{
		ret->autorelease();
		if (placeholder.size()>0)
		{
			ret->setPlaceHolder(placeholder);
		}
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// initialize
//////////////////////////////////////////////////////////////////////////

bool TextFieldKR::initWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize)
{
	setDimensions(dimensions.width, dimensions.height);
	setAlignment(alignment, TextVAlignment::CENTER);

	return initWithPlaceHolder(placeholder, fontName, fontSize);
}
bool TextFieldKR::initWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize)
{
	_placeHolder = placeholder;

	do
	{
		// If fontName is ttf file and it corrected, use TTFConfig
		if (FileUtils::getInstance()->isFileExist(fontName))
		{
			TTFConfig ttfConfig(fontName, fontSize, GlyphCollection::DYNAMIC);
			if (setTTFConfig(ttfConfig))
			{
				break;
			}
		}

		setSystemFontName(fontName);
		setSystemFontSize(fontSize);

	} while (false);


	Label::setTextColor(_colorSpaceHolder);
	Label::setString(_placeHolder);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	// On desktop default enable cursor
	if (_currentLabelType == LabelType::TTF)
	{
		setCursorEnabled(true);
	}
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////
// IMEDelegate
//////////////////////////////////////////////////////////////////////////

bool TextFieldKR::attachWithIME()
{
	bool ret = IMEDelegate::attachWithIME();
	if (ret)
	{
		// open keyboard
		auto pGlView = Director::getInstance()->getOpenGLView();
		if (pGlView)
		{
			pGlView->setIMEKeyboardState(true);
		}
	}
	return ret;
}

bool TextFieldKR::detachWithIME()
{
	bool ret = IMEDelegate::detachWithIME();
	if (ret)
	{
		// close keyboard
		auto glView = Director::getInstance()->getOpenGLView();
		if (glView)
		{
			glView->setIMEKeyboardState(false);
		}
	}
	return ret;
}

void TextFieldKR::didAttachWithIME()
{
	setAttachWithIME(true);
}

void TextFieldKR::didDetachWithIME()
{
	setAttachWithIME(false);
}

bool TextFieldKR::canAttachWithIME()
{
	return true;
}

bool TextFieldKR::canDetachWithIME()
{
	return true;
}

void TextFieldKR::insertText(const char * text, size_t len)
{
	m_koreanIME.InputCharacter(*text);
	UpdateString();
}

void TextFieldKR::deleteBackward()
{
	m_koreanIME.OnDelete();
}

const std::string& TextFieldKR::getContentText()
{
	return m_koreanIME.GetString();
}

void TextFieldKR::setCursorPosition(std::size_t cursorPosition)
{
	if (_cursorEnabled && cursorPosition <= (std::size_t)_charCount)
	{
		_cursorPosition = cursorPosition;
		_cursorShowingTime = CURSOR_TIME_SHOW_HIDE * 2.0f;
	}
}

void TextFieldKR::setCursorFromPoint(const Vec2 &point, const Camera* camera)
{
	if (_cursorEnabled)
	{
		// Reset Label, no cursor
		bool oldIsAttachWithIME = _isAttachWithIME;
		_isAttachWithIME = false;
		updateCursorDisplayText();

		Rect rect;
		rect.size = getContentSize();
		if (isScreenPointInRect(point, camera, getWorldToNodeTransform(), rect, nullptr))
		{
			int latterPosition = 0;
			for (; latterPosition < _lengthOfString; ++latterPosition)
			{
				if (_lettersInfo[latterPosition].valid)
				{
					auto sprite = getLetter(latterPosition);
					rect.size = sprite->getContentSize();
					if (isScreenPointInRect(point, camera, sprite->getWorldToNodeTransform(), rect, nullptr))
					{
						setCursorPosition(latterPosition);
						break;
					}
				}
			}
			if (latterPosition == _lengthOfString)
			{
				setCursorPosition(latterPosition);
			}
		}

		// Set cursor
		_isAttachWithIME = oldIsAttachWithIME;
		updateCursorDisplayText();
	}
}

void TextFieldKR::setAttachWithIME(bool isAttachWithIME)
{
	if (isAttachWithIME != _isAttachWithIME)
	{
		_isAttachWithIME = isAttachWithIME;

		if (_isAttachWithIME)
		{
			setCursorPosition(_charCount);
		}
		updateCursorDisplayText();
	}
}

void TextFieldKR::setTextColor(const Color4B &color)
{
	_colorText = color;
	if (m_koreanIME.GetStringLen() > 0)
	{
		Label::setTextColor(_colorText);
	}
}

void TextFieldKR::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
	Label::visit(renderer, parentTransform, parentFlags);
}

void TextFieldKR::update(float delta)
{
	if (_cursorEnabled && _isAttachWithIME)
	{
		_cursorShowingTime -= delta;
		if (_cursorShowingTime < -CURSOR_TIME_SHOW_HIDE)
		{
			_cursorShowingTime = CURSOR_TIME_SHOW_HIDE;
		}
		// before cursor inserted '\b', need next letter
		auto sprite = getLetter((int)_cursorPosition + 1);

		if (sprite)
		{
			if (_cursorShowingTime >= 0.0f)
			{
				sprite->setOpacity(255);
			}
			else
			{
				sprite->setOpacity(0);
			}
			sprite->setDirty(true);
		}
	}
}

const Color4B& TextFieldKR::getColorSpaceHolder()
{
	return _colorSpaceHolder;
}

void TextFieldKR::setColorSpaceHolder(const Color3B& color)
{
	_colorSpaceHolder.r = color.r;
	_colorSpaceHolder.g = color.g;
	_colorSpaceHolder.b = color.b;
	_colorSpaceHolder.a = 255;
	if (m_koreanIME.GetStringLen() == 0)
	{
		Label::setTextColor(_colorSpaceHolder);
	}
}

void TextFieldKR::setColorSpaceHolder(const Color4B& color)
{
	_colorSpaceHolder = color;
	if (m_koreanIME.GetStringLen() == 0)
	{
		Label::setTextColor(_colorSpaceHolder);
	}
}

//////////////////////////////////////////////////////////////////////////
// properties
//////////////////////////////////////////////////////////////////////////

// input text property
void TextFieldKR::setString(const std::string &text)
{

}

void TextFieldKR::appendString(const std::string& text)
{
	insertText(text.c_str(), text.length());
}

void TextFieldKR::makeStringSupportCursor(std::string& displayText)
{
	if (_cursorEnabled && _isAttachWithIME)
	{
		if (displayText.empty())
		{
			// \b - Next char not change x position
			displayText.push_back((char)TextFormatter::NextCharNoChangeX);
			displayText.push_back(_cursorChar);
		}
		else
		{
			StringUtils::StringUTF8 stringUTF8;

			stringUTF8.replace(displayText);

			if (_cursorPosition > stringUTF8.length())
			{
				_cursorPosition = stringUTF8.length();
			}
			std::string cursorChar;
			// \b - Next char not change x position
			cursorChar.push_back((char)TextFormatter::NextCharNoChangeX);
			cursorChar.push_back(_cursorChar);
			stringUTF8.insert(_cursorPosition, cursorChar);

			displayText = stringUTF8.getAsCharSequence();
		}
	}
}

void TextFieldKR::updateCursorDisplayText()
{
	// Update Label content
	UpdateString();
}

void TextFieldKR::setCursorChar(char cursor)
{
	if (_cursorChar != cursor)
	{
		_cursorChar = cursor;
		updateCursorDisplayText();
	}
}

void TextFieldKR::controlKey(EventKeyboard::KeyCode keyCode)
{
	if (_cursorEnabled)
	{
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_HOME:
		case EventKeyboard::KeyCode::KEY_KP_HOME:
			setCursorPosition(0);
			updateCursorDisplayText();
			break;
		case EventKeyboard::KeyCode::KEY_END:
			setCursorPosition(_charCount);
			updateCursorDisplayText();
			break;
		case EventKeyboard::KeyCode::KEY_DELETE:
		case EventKeyboard::KeyCode::KEY_KP_DELETE:
			if (_cursorPosition < (std::size_t)_charCount)
			{
				deleteBackward();
			}
			break;
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			if (_cursorPosition)
			{
				setCursorPosition(_cursorPosition - 1);
				updateCursorDisplayText();
			}
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			if (_cursorPosition < (std::size_t)_charCount)
			{
				setCursorPosition(_cursorPosition + 1);
				updateCursorDisplayText();
			}
			break;
		case EventKeyboard::KeyCode::KEY_ESCAPE:
			detachWithIME();
			break;
		default:
			break;
		}
	}
}

const std::string& TextFieldKR::getString() const
{
	return m_koreanIME.GetString();
}

// place holder text property
void TextFieldKR::setPlaceHolder(const std::string& text)
{
	_placeHolder = text;
	if (m_koreanIME.GetStringLen() <= 0)
	{
		Label::setTextColor(_colorSpaceHolder);
		Label::setString(_placeHolder);
	}
}

const std::string& TextFieldKR::getPlaceHolder() const
{
	return _placeHolder;
}

void TextFieldKR::setCursorEnabled(bool enabled)
{
	if (_currentLabelType == LabelType::TTF)
	{
		if (_cursorEnabled != enabled)
		{
			_cursorEnabled = enabled;
			if (_cursorEnabled)
			{
				_cursorPosition = _charCount;

				scheduleUpdate();
			}
			else
			{
				_cursorPosition = 0;

				unscheduleUpdate();
			}
		}
	}
	else
	{
		CCLOG("TextFieldKR cursor worked only LabelType::TTF");
	}
}

// secureTextEntry
void TextFieldKR::setSecureTextEntry(bool value)
{
	if (_secureTextEntry != value)
	{
		_secureTextEntry = value;
		UpdateString();
	}
}

void TextFieldKR::setPasswordTextStyle(const std::string &text)
{
	if (text.length() < 1)
	{
		return;
	}

	if (text != _passwordStyleText) {
		_passwordStyleText = text;
		UpdateString();
	}
}

const std::string& TextFieldKR::getPasswordTextStyle() const
{
	return _passwordStyleText;
}

bool TextFieldKR::isSecureTextEntry() const
{
	return _secureTextEntry;
}


void TextFieldKR::UpdateString()
{
	std::string displayText;

	std::size_t charCount = m_koreanIME.GetStringLen();

	if (charCount > 0)
	{
		displayText = m_koreanIME.GetString();
		charCount = m_koreanIME.GetStringLen();
		if (_secureTextEntry)
		{
			displayText = "";
			size_t length = charCount;
			while (length)
			{
				displayText.append(_passwordStyleText);
				--length;
			}
		}
	}

	if (_cursorEnabled && charCount != _charCount)
	{
		_cursorPosition = charCount;
	}

	if (_cursorEnabled)
	{
		// Need for recreate all letters in Label
		Label::removeAllChildrenWithCleanup(false);
	}

	// if there is no input text, display placeholder instead
	if (charCount <= 0 && (!_cursorEnabled || !_isAttachWithIME))
	{
		Label::setTextColor(_colorSpaceHolder);
		Label::setString(_placeHolder);
	}
	else
	{
		makeStringSupportCursor(displayText);

		Label::setTextColor(_colorText);
		Label::setString(displayText);
	}
	_charCount = charCount;
}