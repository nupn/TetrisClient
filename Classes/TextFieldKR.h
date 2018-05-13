#pragma once

#include "cocos2d.h"
#include "NPCppTool/KoreanIME.h"
using namespace NPCL;

USING_NS_CC;

class TextFieldKR : public Label, public IMEDelegate
{
public:
	/**
	* Default constructor.
	* @js ctor
	*/
	TextFieldKR();

	/**
	* Default destructor.
	* @js NA
	* @lua NA
	*/
	virtual ~TextFieldKR();

	/** Creates a TextFieldTTF from a fontname, alignment, dimension and font size.
	* @js NA
	*/
	static TextFieldKR * textFieldWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize);

	/** Creates a TextFieldTTF from a fontname and font size.
	* @js NA
	*/
	static TextFieldKR * textFieldWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize);

	/** Initializes the TextFieldTTF with a font name, alignment, dimension and font size. */
	bool initWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize);

	/** Initializes the TextFieldTTF with a font name and font size. */
	bool initWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize);

	/**
	*@brief    Open keyboard and receive input text.
	*/
	virtual bool attachWithIME() override;

	/**
	*@brief    End text input and close keyboard.
	*/
	virtual bool detachWithIME() override;

	//////////////////////////////////////////////////////////////////////////
	// properties
	//////////////////////////////////////////////////////////////////////////

	/**
	* Query the currently inputed character count.
	*@return The total input character count.
	*/
	
	/**
	* Query the color of place holder.
	*@return The place holder color.
	*/
	virtual const Color4B& getColorSpaceHolder();

	/**
	*@brief Change input placeholder color.
	*@param color A color value in `Color3B`.
	*/
	virtual void setColorSpaceHolder(const Color3B& color);

	/**
	* Change the placeholder color.
	*@param color The placeholder color in Color4B.
	*/
	virtual void setColorSpaceHolder(const Color4B& color);

	/**
	* Change the color of input text.
	*@param textColor The text color in Color4B.
	*/
	virtual void setTextColor(const Color4B& textColor) override;

	/**
	* Change input text of TextField.
	*@param text The input text of TextField.
	*/
	virtual void setString(const std::string& text) override;

	/**
	* Append to input text of TextField.
	*@param text The append text of TextField.
	*/
	virtual void appendString(const std::string& text);

	/**
	* Query the input text of TextField.
	*@return Get the input text of TextField.
	*/
	std::string GetString() const;

	/**
	* Change placeholder text.
	* place holder text displayed when there is no text in the text field.
	*@param text  The placeholder string.
	*/
	virtual void setPlaceHolder(const std::string& text);

	/**
	* Query the placeholder string.
	*@return The placeholder string.
	*/
	virtual const std::string& getPlaceHolder() const;

	/**
	* Set enable secure text entry representation.
	* If you want to display password in TextField, this option is very helpful.
	*@param value Whether or not to display text with secure text entry.
	* @js NA
	*/
	virtual void setSecureTextEntry(bool value);
	virtual void setPasswordTextStyle(const std::string& text);
	const std::string& getPasswordTextStyle() const;

	/**
	* Query whether the currently display mode is secure text entry or not.
	*@return Whether current text is displayed as secure text entry.
	* @js NA
	*/
	virtual bool isSecureTextEntry()const;

	virtual void visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags) override;

	virtual void update(float delta) override;

	/**
	* Set enable cursor use.
	* @js NA
	*/
	void setCursorEnabled(bool enabled);

	/**
	* Set char showing cursor.
	* @js NA
	*/
	void setCursorChar(char cursor);

	/**
	* Set cursor position, if enabled
	* @js NA
	*/
	void setCursorPosition(std::size_t cursorPosition);

	/**
	* Set cursor position to hit letter, if enabled
	* @js NA
	*/
	void setCursorFromPoint(const Vec2 &point, const Camera* camera);

protected:
	//////////////////////////////////////////////////////////////////////////
	// IMEDelegate interface
	//////////////////////////////////////////////////////////////////////////

	virtual bool canAttachWithIME() override;
	virtual bool canDetachWithIME() override;
	virtual void didAttachWithIME() override;
	virtual void didDetachWithIME() override;
	virtual void insertText(const char * text, size_t len) override;
	virtual void deleteBackward() override;
	virtual const std::string& getContentText() override;
	virtual void controlKey(EventKeyboard::KeyCode keyCode) override;


	std::string _placeHolder;
	Color4B _colorSpaceHolder;
	Color4B _colorText;
	Color3B _colorText3B;
	Color3B _colorBackGround;


	bool _secureTextEntry;
	std::string _passwordStyleText;

	// Need use cursor
	bool _cursorEnabled;
	// Current position cursor
	std::size_t _cursorPosition;
	int _cursorCharacterIdx = 0;
	bool m_bShowBackgound = false;
	Sprite* m_pBackground = nullptr;

	// Char showing cursor
	char _cursorChar;
	// >0 - show, <0 - hide
	float _cursorShowingTime;

	bool _isAttachWithIME;

	void makeStringSupportCursor(std::string& displayText);
	void updateCursor();
	void setAttachWithIME(bool isAttachWithIME);

private:
	void UpdateString();

private:
	class LengthStack;
	LengthStack * _lens;
	NPCL::CKoreanIME m_koreanIME;
};
