#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <new>

#include <pspkernel.h>
#include <pspctrl.h>
#include <psputility.h>
#include <pspgu.h>
#include <pspgum.h>

#include "../GUDraw.h"
#include "../GUDiag.h"

#include "../intraFont/intraFont.h"

#define FREE(_Memory) if(_Memory!=NULL){free(_Memory);_Memory=NULL;}

#define tostr(n) tostr_(n)
#define tostr_(n) #n
#define TRACE pspDebugScreenPrintf("TRACE: (%s:%s)\n", __FILE__, tostr(__LINE__));

#define TRACEF {sprintf(trace_buf, "TRACE: (%s:%s)\r\n", __FILE__, tostr(__LINE__));fp=fopen("ms0:/debug.txt", "a");fputs(trace_buf, fp);fclose(fp);}
FILE *fp;
char trace_buf[64];

//’è”********************************************************
const int	GUButton::GUBUTTON_STATE_NORMAL = 0;
const int	GUButton::GUBUTTON_STATE_SELECTED = 1;
const int	GUButton::GUBUTTON_STATE_PUSHED = 2;

const float GUButton::GUBUTTON_FONT_HEIGHT = 13.0f; // 16 * 0.8f

const float GUButton::GUBUTTON_ADJ_X = 0.0f;
const float GUButton::GUBUTTON_ADJ_Y = GUButton::GUBUTTON_FONT_HEIGHT;

const float	GUButton::GUBUTTON_SPACE_X = 15.0f;
const float	GUButton::GUBUTTON_SPACE_Y = 6.0f;

//
const int	GUTextBox::GUTEXTBOX_TYPE_NORMAL = 0;
const int	GUTextBox::GUTEXTBOX_TYPE_EDITABLE = 1;

const int	GUTextBox::GUTEXTBOX_STATE_NORMAL = 0;
const int	GUTextBox::GUTEXTBOX_STATE_EDITING = 1;
const int	GUTextBox::GUTEXTBOX_STATE_SELECTED = 2;

const int	GUTextBox::GUTEXTBOX_STYLE_NOSCROLL = 0;
const int	GUTextBox::GUTEXTBOX_STYLE_SCROLL = 1;

const float	GUTextBox::GUTEXTBOX_FONT_HEIGHT = 13.0f;

const float GUTextBox::GUTEXTBOX_HEIGHT = GUTextBox::GUTEXTBOX_FONT_HEIGHT+GUTextBox::GUTEXTBOX_SPACE_Y*2;

const float GUTextBox::GUTEXTBOX_ADJ_X = 0.0f;
const float GUTextBox::GUTEXTBOX_ADJ_Y = GUTextBox::GUTEXTBOX_FONT_HEIGHT;

const float	GUTextBox::GUTEXTBOX_SPACE_X = 4.0f;
const float	GUTextBox::GUTEXTBOX_SPACE_Y = 6.0f;

const int	GUTextBox::GUTEXTBOX_RESULT_ERROR = -1;
const int	GUTextBox::GUTEXTBOX_RESULT_CHANGED = 0;
const int	GUTextBox::GUTEXTBOX_RESULT_NOTCHANGED = 1;

//
const int	GUDiag::GUDIAG_TYPE_ALERT = 0;
const int	GUDiag::GUDIAG_TYPE_YESNO = 1;

const int	GUDiag::GUDIAG_STATE_OPEN = 0;
const int	GUDiag::GUDIAG_STATE_CLOSED = 1;
const int	GUDiag::GUDIAG_STATE_YES = 10;
const int	GUDiag::GUDIAG_STATE_NO = 11;

const float GUDiag::GUDIAG_FONT_HEIGHT = 13.0f;

const float GUDiag::GUDIAG_HEADER = 50.0f;
const float GUDiag::GUDIAG_FOOTER = GUDiag::GUDIAG_HEADER;

const float GUDiag::GUDIAG_HEADLINE_WIDTH = 300.0f;
const float GUDiag::GUDIAG_FOOTLINE_WIDTH = GUDiag::GUDIAG_HEADLINE_WIDTH;

const float	GUDiag::GUDIAG_ADJ_X = 0.0f;
const float	GUDiag::GUDIAG_ADJ_Y = GUDiag::GUDIAG_FONT_HEIGHT;

const float GUDiag::GUDIAG_SPACE_X = 20.0f;
const float GUDiag::GUDIAG_SPACE_Y = 5.0f;

//

//GUButton********************************************************
GUButton::GUButton()
{
	this->_value = NULL;
	this->_state = GUBUTTON_STATE_NORMAL;

	memset(&currpad, 0, sizeof(SceCtrlData));
	memset(&oldpad, 0, sizeof(SceCtrlData));

	GUButtonColorsSetDefault(&this->colors);
}

GUButton::~GUButton()
{
	FREE(this->_value);	
}

float GUButton::getWidth(void)
{
	intraFontSetStyle(jpn0, 0.8f, this->colors.normal.text, 0, NULL);
	float val_width = 0.0f;
	val_width = intraFontMeasureText(jpn0, this->_value);

	return val_width + GUBUTTON_SPACE_X*2;
}

void GUButton::setValue(const char *newvalue)
{
	if(newvalue == NULL)
	{
		return;
	}

	FREE(this->_value);
	
	this->_value = (char *)malloc(strlen(newvalue)+1);
	strcpy(this->_value, newvalue);
}

char *GUButton::getValue(void)
{
	return this->_value;
}

void GUButton::setState(int GUBUTTON_STATE)
{
	this->_state = GUBUTTON_STATE;
}

int GUButton::getState(void)
{
	return this->_state;
}

void GUButton::Draw(float x, float y)
{
	float x1, y1, x2, y2 = 0.0f;

	x1 = x; // ƒ{ƒ^ƒ“‚Ì¶ãÀ•W
	y1 = y;

	float val_x, val_y = 0.0f;
	val_x = x1 + GUBUTTON_SPACE_X;
	val_y = y1 + GUBUTTON_SPACE_Y;

	// ƒ{ƒ^ƒ“‚Ì”wŒi‚Ì4‹÷À•W‚ðŒvŽZ
	intraFontSetStyle(jpn0, 0.8f, this->colors.normal.text, 0, NULL);
	float val_width = 0.0f;
	val_width = intraFontMeasureText(jpn0, this->getValue());

	x2 = val_x + val_width + GUBUTTON_SPACE_X;
	y2 = y1 + GUBUTTON_SPACE_Y + GUBUTTON_FONT_HEIGHT + GUBUTTON_SPACE_Y;

	switch(this->getState())
	{
	case GUBUTTON_STATE_NORMAL:
		// ƒ{ƒ^ƒ“‚Ì”wŒi
		DrawSquare(x1, y1, x2, y2, this->colors.normal.fill, true);

		// ƒ{ƒ^ƒ“‚Ì’l
		intraFontSetStyle(jpn0, 0.8f, this->colors.normal.text, 0, NULL);
		intraFontPrintf(jpn0, val_x+GUBUTTON_ADJ_X, val_y+GUBUTTON_ADJ_Y, "%s", this->getValue());

		// ƒ{ƒ^ƒ“‚Ì’Êí˜g
		DrawSquare(x1, y1, x2, y2, this->colors.normal.border, false);
		
		break;

	case GUBUTTON_STATE_SELECTED:
		// ƒ{ƒ^ƒ“‚Ì”wŒi
		DrawSquare(x1, y1, x2, y2, this->colors.selected.fill, true);

		// ƒ{ƒ^ƒ“‚Ì’l
		intraFontSetStyle(jpn0, 0.8f, this->colors.selected.text, 0, NULL);
		intraFontPrintf(jpn0, val_x+GUBUTTON_ADJ_X, val_y+GUBUTTON_ADJ_Y, "%s", this->getValue());

		// ƒ{ƒ^ƒ“‚Ì‘I‘ð˜g
		DrawSquare(x1, y1, x2, y2, this->colors.selected.border, false);
		break;

	default:
		break;
	}
}

void GUButton::Render(SceCtrlData *padData)
{
	if(padData == NULL)
	{
		return;
	}
	currpad = *padData;

	if(currpad.Buttons & PSP_CTRL_CIRCLE)
	{
		this->_state = GUBUTTON_STATE_PUSHED;
	}
	
	if(!(oldpad.Buttons & PSP_CTRL_CIRCLE)
		&& !(currpad.Buttons & PSP_CTRL_CIRCLE))
	{
		this->_state = GUBUTTON_STATE_NORMAL;
	}

	oldpad = currpad;
}

void GUButtonColorsSetDefault(GUButtonColors *colors)
{
	memset(colors, 0, sizeof(GUButtonColors));
	
	colors->normal.border	= RGBA(160, 160, 160, 255);
	colors->normal.fill		= RGBA(180, 180, 180, 255);
	colors->normal.text		= RGBA(0, 0, 0, 255);
	
	colors->selected.border	= RGBA(0, 0, 0, 255);
	colors->selected.fill	= RGBA(180, 180, 180, 255);
	colors->selected.text	= RGBA(0, 0, 0, 255);
}
//GUTextBox********************************************************
GUTextBox::GUTextBox(void)
{
	this->first_time_scroll = true;

	this->_value = NULL;
	this->setValue("");

	this->_edittype = 0;
	this->setEditType(PSP_UTILITY_OSK_INPUTTYPE_ALL);

	this->setType(GUTEXTBOX_TYPE_NORMAL);

	this->_title = NULL;
	this->setEditTitle("•ÒW");
	this->setState(GUTEXTBOX_STATE_NORMAL);
	this->_style = GUTEXTBOX_STYLE_SCROLL;

	memset(&currpad, 0, sizeof(SceCtrlData));
	memset(&oldpad, 0, sizeof(SceCtrlData));

	GUTextBoxColorsSetDefault(&this->colors);
}

GUTextBox::~GUTextBox()
{
	FREE(this->_value);
	FREE(this->_title);
}

void GUTextBox::setValue(const char *newvalue)
{
	if(newvalue == NULL)
	{
		return;
	}

	FREE(this->_value);
	
	this->_value = (char *)malloc(strlen(newvalue)+1);
	strcpy(this->_value, newvalue);
}

void GUTextBox::setValuePtr(char *ptvalue)
{
	this->_value = ptvalue;
}

char *GUTextBox::getValue(void)
{
	return this->_value;
}

void GUTextBox::setEditTitle(const char *newtitle)
{
	if(newtitle == NULL)
	{
		return;
	}

	FREE(this->_title);
	
	this->_title = (char *)malloc(strlen(newtitle)+1);
	strcpy(this->_title, newtitle);
}

char *GUTextBox::getEditTitle(void)
{
	return _title;
}

void GUTextBox::setEditType(int PSP_UTILITY_OSK_INPUTTYPE)
{
	this->_edittype = PSP_UTILITY_OSK_INPUTTYPE;
}

int GUTextBox::getEditType(void)
{
	return this->_edittype;
}

void GUTextBox::setType(int GUTEXTBOX_TYPE)
{
	this->_type = GUTEXTBOX_TYPE;
}

int GUTextBox::getType(void)
{
	return this->_type;
}

void GUTextBox::setState(int GUTEXTBOX_STATE)
{
	this->_state = GUTEXTBOX_STATE;
}

int GUTextBox::getState(void)
{
	return this->_state;
}

void GUTextBox::setStyle(int GUTEXTBOX_STYLE)
{
	this->_style = GUTEXTBOX_STYLE;
}

int GUTextBox::getStyle(void)
{
	return this->_style;
}

void GUTextBox::Draw(float x, float y, float width)
{
	float x1, y1, x2, y2 = 0.0f;

	x1 = x; // ¶ãÀ•W
	y1 = y;

	float val_x, val_y = 0.0f;	//•¶Žš‚Ì¶ãÀ•W
	val_x = x1 + GUTEXTBOX_SPACE_X;
	val_y = y1 + GUTEXTBOX_SPACE_Y;

	// TODO
	if(this->_value == NULL)
	{
		//this->_value = "";
	}

	if(this->first_time_scroll)
	{
		this->first_time_scroll = false;
		this->scr_x = val_x;
	}

	// TB‚Ì”wŒi‚Ì4‹÷À•W‚ðŒvŽZ
	intraFontSetStyle(jpn0, 0.8f, this->colors.normal.text, 0, NULL);
	float val_width = 0.0f;
	val_width = intraFontMeasureText(jpn0, this->getValue());

	bool value_overflow = false;
	// TB‚Ì’l‚ªTB‚Ì•‚æ‚è’·‚©‚Á‚½ê‡
	if((GUTEXTBOX_SPACE_X*2 + val_width) > width)
	{
		value_overflow = true;
	}

	x2 = x1 + width;
	y2 = y1 + GUTEXTBOX_SPACE_Y + GUTEXTBOX_FONT_HEIGHT + GUTEXTBOX_SPACE_Y;

	int col_fill = 0;
	int	col_text = 0;
	int col_border = 0;

	switch(this->getState())
	{
	case GUTEXTBOX_STATE_NORMAL:
		col_fill = this->colors.normal.fill;
		col_text = this->colors.normal.text;
		col_border = this->colors.normal.border;
		break;

	case GUTEXTBOX_STATE_SELECTED:
		col_fill = this->colors.selected.fill;
		col_text = this->colors.selected.text;
		col_border = this->colors.selected.border;
		break;

	default:
		break;
	}

	// TB‚Ì”wŒi
	DrawSquare(x1, y1, x2, y2, col_fill, true);

	// TB‚Ì’l
	if(this->_style == GUTEXTBOX_STYLE_SCROLL)
	{
		if(value_overflow)
		{
			intraFontSetStyle(jpn0, 0.8f, col_text, 0, INTRAFONT_SCROLL_LEFT);
			scr_x = intraFontPrintColumn(jpn0, scr_x+GUTEXTBOX_ADJ_X, val_y+GUTEXTBOX_ADJ_Y, width-GUTEXTBOX_SPACE_X*2, this->getValue());
		}
		else
		{
			intraFontSetStyle(jpn0, 0.8f, col_text, 0, INTRAFONT_SCROLL_LEFT);
			intraFontPrintColumn(jpn0, val_x+GUTEXTBOX_ADJ_X, val_y+GUTEXTBOX_ADJ_Y, width-GUTEXTBOX_SPACE_X*2, this->getValue());
		}
	}
	else if(this->_style == GUTEXTBOX_STYLE_NOSCROLL)
	{
		intraFontSetStyle(jpn0, 0.8f, col_text, 0, INTRAFONT_SCROLL_LEFT);
		intraFontPrintColumn(jpn0, val_x+GUTEXTBOX_ADJ_X, val_y+GUTEXTBOX_ADJ_Y, width-GUTEXTBOX_SPACE_X*2, this->getValue());
	}
	else
	{
		intraFontSetStyle(jpn0, 0.8f, col_text, 0, INTRAFONT_SCROLL_LEFT);
		intraFontPrintColumn(jpn0, val_x+GUTEXTBOX_ADJ_X, val_y+GUTEXTBOX_ADJ_Y, width-GUTEXTBOX_SPACE_X*2, this->getValue());
	}

	// TB‚Ì’Êí˜g
	DrawSquare(x1, y1, x2, y2, col_border, false);
}

void GUTextBox::Draw(float x, float y, float width, void *list)
{
	float x1, y1, x2, y2 = 0.0f;

	x1 = x; // ¶ãÀ•W
	y1 = y;

	float val_x, val_y = 0.0f;	//•¶Žš‚Ì¶ãÀ•W
	val_x = x1 + GUTEXTBOX_SPACE_X;
	val_y = y1 + GUTEXTBOX_SPACE_Y;

	// TODO
	if(this->_value == NULL)
	{
		//this->_value = "";
	}

	if(this->first_time_scroll)
	{
		this->first_time_scroll = false;
		this->scr_x = val_x;
	}

	sceGuStart(GU_DIRECT, list);

	// TB‚Ì”wŒi‚Ì4‹÷À•W‚ðŒvŽZ
	intraFontSetStyle(jpn0, 0.8f, this->colors.normal.text, 0, NULL);
	float val_width = 0.0f;
	val_width = intraFontMeasureText(jpn0, this->getValue());

	bool value_overflow = false;
	// TB‚Ì’l‚ªTB‚Ì•‚æ‚è’·‚©‚Á‚½ê‡
	if((GUTEXTBOX_SPACE_X*2 + val_width) > width)
	{
		value_overflow = true;
	}

	x2 = x1 + width;
	y2 = y1 + GUTEXTBOX_SPACE_Y + GUTEXTBOX_FONT_HEIGHT + GUTEXTBOX_SPACE_Y;

	int col_fill = 0;
	int	col_text = 0;
	int col_border = 0;

	switch(this->getState())
	{
	case GUTEXTBOX_STATE_NORMAL:
		col_fill = this->colors.normal.fill;
		col_text = this->colors.normal.text;
		col_border = this->colors.normal.border;
		break;

	case GUTEXTBOX_STATE_SELECTED:
		col_fill = this->colors.selected.fill;
		col_text = this->colors.selected.text;
		col_border = this->colors.selected.border;
		break;

	default:
		break;
	}

	// TB‚Ì”wŒi
	DrawSquare(x1, y1, x2, y2, col_fill, true);

	// TB‚Ì’l
	if(this->_style == GUTEXTBOX_STYLE_SCROLL)
	{
		if(value_overflow)
		{
			intraFontSetStyle(jpn0, 0.8f, col_text, 0, INTRAFONT_SCROLL_LEFT);
			scr_x = intraFontPrintColumn(jpn0, scr_x+GUTEXTBOX_ADJ_X, val_y+GUTEXTBOX_ADJ_Y, width-GUTEXTBOX_SPACE_X*2, this->getValue());
		}
		else
		{
			intraFontSetStyle(jpn0, 0.8f, col_text, 0, INTRAFONT_SCROLL_LEFT);
			intraFontPrintColumn(jpn0, val_x+GUTEXTBOX_ADJ_X, val_y+GUTEXTBOX_ADJ_Y, width-GUTEXTBOX_SPACE_X*2, this->getValue());
		}
	}
	else if(this->_style == GUTEXTBOX_STYLE_NOSCROLL)
	{
		intraFontSetStyle(jpn0, 0.8f, col_text, 0, INTRAFONT_SCROLL_LEFT);
		intraFontPrintColumn(jpn0, val_x+GUTEXTBOX_ADJ_X, val_y+GUTEXTBOX_ADJ_Y, width-GUTEXTBOX_SPACE_X*2, this->getValue());
	}
	else
	{
		intraFontSetStyle(jpn0, 0.8f, col_text, 0, INTRAFONT_SCROLL_LEFT);
		intraFontPrintColumn(jpn0, val_x+GUTEXTBOX_ADJ_X, val_y+GUTEXTBOX_ADJ_Y, width-GUTEXTBOX_SPACE_X*2, this->getValue());
	}

	// TB‚Ì’Êí˜g
	DrawSquare(x1, y1, x2, y2, col_border, false);

	GUENABLE(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, 0);
}


int GUTextBox::Render(SceCtrlData *padData)
{
	if(padData == NULL)
	{
		return GUTEXTBOX_RESULT_ERROR;
	}
	currpad = *padData;

	int result = GUTEXTBOX_RESULT_NOTCHANGED;
	char buf[1024+1];
	if(!(oldpad.Buttons & PSP_CTRL_CIRCLE)
		&& (currpad.Buttons & PSP_CTRL_CIRCLE))
	{
		switch(this->getState())
		{
			case GUTEXTBOX_STATE_NORMAL:
				break;

			case GUTEXTBOX_STATE_SELECTED:
				// OSKŒÄ‚Ño‚µ
				switch(GetOSKText(buf, 1024, 0, this->getEditTitle(), this->getValue()))
				{
					case PSP_UTILITY_OSK_RESULT_CANCELLED:
					case PSP_UTILITY_OSK_RESULT_UNCHANGED:
						break;

					// ’l‚ª•ÏX‚³‚ê‚½ê‡‚Ì‚Ý”½‰f
					case PSP_UTILITY_OSK_RESULT_CHANGED:
						this->setValue(buf);
						result = GUTEXTBOX_RESULT_CHANGED;
						break;

					default:
						break;
				}
				break;
			
			default:
				break;
		}
	}


	oldpad = currpad;
	return result;
}

void GUTextBoxColorsSetDefault(GUTextBoxColors *colors)
{
	memset(colors, 0, sizeof(GUTextBoxColors));
	
	colors->normal.border	= RGBA(160, 160, 160, 255);
	colors->normal.fill		= RGBA(180, 180, 180, 255);
	colors->normal.text		= RGBA(0, 0, 0, 255);
	
	colors->selected.border	= RGBA(0, 0, 0, 255);
	colors->selected.fill	= RGBA(180, 180, 180, 255);
	colors->selected.text	= RGBA(0, 0, 0, 255);
}

//GUDiag********************************************************
GUDiag::GUDiag(void *list)
{
	memset((void *)&this->oldpad, 0, sizeof(SceCtrlData));

	this->_list = list;
	this->_ctrlflg = NULL;
	this->_value = NULL;
	this->setValue("");
	this->setState(GUDIAG_STATE_CLOSED);
	this->setType(GUDIAG_TYPE_ALERT);

	GUDiagColorsSetDefault(&this->colors);
}

GUDiag::GUDiag(void *list, bool *ctrlflg)
{
	memset((void *)&this->oldpad, 0, sizeof(SceCtrlData));

	this->_list = list;
	this->_ctrlflg = ctrlflg;
	this->_value = NULL;
	this->setValue("");
	this->setState(GUDIAG_STATE_CLOSED);
	this->setType(GUDIAG_TYPE_ALERT);

	GUDiagColorsSetDefault(&this->colors);
}

GUDiag::~GUDiag()
{
	FREE(this->_value);
}

void GUDiag::setCtrlflgTrue(void)
{
	if(_ctrlflg != NULL)
	{
		*_ctrlflg = true;
	}
}

void GUDiag::setCtrlflgFalse(void)
{
	if(_ctrlflg != NULL)
	{
		*_ctrlflg = false;
	}
}

void GUDiag::setValue(const char *newvalue)
{
	if(newvalue == NULL)
	{
		return;
	}

	FREE(this->_value);
	
	this->_value = (char *)malloc(strlen(newvalue)+1);
	strcpy(this->_value, newvalue);
}

char *GUDiag::getValue(void)
{
	return this->_value;
}

void GUDiag::setType(int GUDIAG_TYPE)
{
	this->_type = GUDIAG_TYPE;
}

int GUDiag::getType(void)
{
	return this->_type;
}

void GUDiag::setState(int GUDIAG_STATE)
{
	this->_state = GUDIAG_STATE;
}

int GUDiag::getState(void)
{
	return this->_state;
}

void GUDiag::Draw(int bgcolor_RGBA)
{
	sceGuStart(GU_DIRECT, this->_list);
		
	// ”wŒii’Êí‚Í”¼“§–¾‚Åj
	DrawSquare(0, 0, 480, 272, bgcolor_RGBA, true);
	
	// ã‰º‚Ì”’ü
	float headline_x, headline_y = 0.0f;
	float headline_width = 5.0f;
	float footline_x, footline_y = 0.0f;
	float footline_width = headline_width;

	headline_x = (480 - GUDIAG_HEADLINE_WIDTH)/2;
	headline_y = GUDIAG_HEADER;

	footline_x = (480 - GUDIAG_FOOTLINE_WIDTH)/2;
	footline_y = 272 - GUDIAG_FOOTER - footline_width;


	switch(this->getType())
	{
	case GUDIAG_TYPE_ALERT:
		// ã‰º‚Ì”’ü
		DrawLineEx(headline_x, headline_y, GUDIAG_HEADLINE_WIDTH, headline_y, headline_width, this->colors.separator);
		DrawLineEx(footline_x, footline_y, GUDIAG_FOOTLINE_WIDTH, footline_y, footline_width, this->colors.separator);

		// “à—e
		intraFontSetStyle(jpn0, 0.8f, this->colors.text, 0, INTRAFONT_ALIGN_LEFT);
		intraFontPrintColumn(jpn0, 
			headline_x+GUDIAG_SPACE_X+GUDIAG_ADJ_X,
			headline_y+headline_width+GUDIAG_SPACE_Y+GUDIAG_ADJ_Y,
			GUDIAG_HEADLINE_WIDTH-GUDIAG_SPACE_X*2,
			this->getValue());

		intraFontSetStyle(jpn0, 0.8f, this->colors.text, 0, INTRAFONT_ALIGN_CENTER);
		intraFontPrint(jpn0, 240, footline_y-5.0f, "~ –ß‚é");
		break;

	case GUDIAG_TYPE_YESNO:
		// ã‰º‚Ì”’ü
		DrawLineEx(headline_x, headline_y, GUDIAG_HEADLINE_WIDTH, headline_y, headline_width, this->colors.separator);
		DrawLineEx(footline_x, footline_y, GUDIAG_FOOTLINE_WIDTH, footline_y, footline_width, this->colors.separator);

		// “à—e
		intraFontSetStyle(jpn0, 0.8f, this->colors.text, 0, INTRAFONT_ALIGN_LEFT);
		intraFontPrintColumn(jpn0, 
			headline_x+GUDIAG_SPACE_X+GUDIAG_ADJ_X,
			headline_y+headline_width+GUDIAG_SPACE_Y+GUDIAG_ADJ_Y,
			GUDIAG_HEADLINE_WIDTH-GUDIAG_SPACE_X*2,
			this->getValue());

		intraFontSetStyle(jpn0, 0.8f, this->colors.text, 0, INTRAFONT_ALIGN_CENTER);
		intraFontPrint(jpn0, 240, footline_y-5.0f, "› Œˆ’è@~ ƒLƒƒƒ“ƒZƒ‹");
		break;

	default:
		break;
	}

	GUENABLE(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, 0);
}

void GUDiag::Open()
{
	if(this->getState() == GUDIAG_STATE_OPEN)
	{
		return;
	}

	this->setCtrlflgFalse();
	this->setState(GUDIAG_STATE_OPEN);
	
	SceCtrlData pad;
	while(this->getState() == GUDIAG_STATE_OPEN)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if(!(oldpad.Buttons & PSP_CTRL_CROSS)
			&& (pad.Buttons & PSP_CTRL_CROSS))
		{
			this->setCtrlflgTrue();

			switch(this->getType())
			{
			case GUDIAG_TYPE_ALERT:
				this->setState(GUDIAG_STATE_CLOSED);
				break;

			case GUDIAG_TYPE_YESNO:
				this->setState(GUDIAG_STATE_NO);
				break;

			default:
				break;
			}
		}
		else if(!(oldpad.Buttons & PSP_CTRL_CIRCLE)
			&& (pad.Buttons & PSP_CTRL_CIRCLE))
		{
			switch(this->getType())
			{
			case GUDIAG_TYPE_YESNO:
				this->setCtrlflgTrue();
				this->setState(GUDIAG_STATE_YES);
				break;

			default:
				break;
			}
		}

		oldpad = pad;
		this->Draw(RGBA(128, 128, 128, 128));
	}
}

void GUDiag::Open(const char *value)
{
	if(this->getState() == GUDIAG_STATE_OPEN)
	{
		return;
	}

	this->setCtrlflgFalse();
	this->setValue(value);
	this->setState(GUDIAG_STATE_OPEN);
	
	SceCtrlData pad;
	while(this->getState() == GUDIAG_STATE_OPEN)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if(!(oldpad.Buttons & PSP_CTRL_CROSS)
			&& (pad.Buttons & PSP_CTRL_CROSS))
		{
			this->setCtrlflgTrue();

			switch(this->getType())
			{
			case GUDIAG_TYPE_ALERT:
				this->setState(GUDIAG_STATE_CLOSED);
				break;

			case GUDIAG_TYPE_YESNO:
				this->setState(GUDIAG_STATE_NO);
				break;

			default:
				break;
			}
		}
		else if(!(oldpad.Buttons & PSP_CTRL_CIRCLE)
			&& (pad.Buttons & PSP_CTRL_CIRCLE))
		{
			switch(this->getType())
			{
			case GUDIAG_TYPE_YESNO:
				this->setCtrlflgTrue();				
				this->setState(GUDIAG_STATE_YES);
				break;

			default:
				break;
			}
		}

		oldpad = pad;
		this->Draw(RGBA(128, 128, 128, 128));
	}
}

void GUDiagColorsSetDefault(GUDiagColors *colors)
{
	memset(colors, 0, sizeof(GUDiagColors));

	colors->separator = RGBA(128, 128, 128, 255);
	colors->text = RGBA(255, 255, 255, 255);
}
