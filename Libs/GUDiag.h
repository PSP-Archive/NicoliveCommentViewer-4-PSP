#ifndef _GUDIAGDECL
#define _GUDIAGDECL
#include "Libs/GUDiag_osk.h"

#define ARGB(a, r, g, b) ((a << 24) | (r << 16) | (g << 8) | b)
#define RGBA(r, g, b, a) ((a << 24) | (b << 16) | (g << 8) | r)

struct intraFont;
extern intraFont *jpn0;


typedef struct _GUButtonColors
{
	struct _normal
	{
		int border;
		int fill;
		int text;
	}normal;

	struct _selected
	{
		int border;
		int fill;
		int text;
	}selected;
}GUButtonColors;

typedef struct _GUTextBoxColors
{
	struct _normal
	{
		int border;
		int fill;
		int text;
	}normal;

	struct _selected
	{
		int border;
		int fill;
		int text;
	}selected;
}GUTextBoxColors;

typedef struct _GUDiagColors
{
	int separator;
	int text;
}GUDiagColors;


class GUTextBox; // editable text box
//class GUText; // static text
class GUButton; // button
class GUDiag; // main dialog flame

class GUListObject;
class GUListItem;
class GUListObject;
class GUListView;

class GUButton
{
private:
	char *_value;
	int _state;

	SceCtrlData currpad;
	SceCtrlData oldpad;

public:
	static const int	GUBUTTON_STATE_NORMAL;
	static const int	GUBUTTON_STATE_SELECTED;
	static const int	GUBUTTON_STATE_PUSHED;

	static const float	GUBUTTON_FONT_HEIGHT;

	static const float	GUBUTTON_ADJ_X;
	static const float	GUBUTTON_ADJ_Y;

	static const float	GUBUTTON_SPACE_X;
	static const float	GUBUTTON_SPACE_Y;

	GUButtonColors colors;

	float getWidth(void);

	void setValue(const char *newvalue);
	char *getValue(void);

	void setState(int GUBUTTON_STATE);
	int getState(void);

	void Draw(float x, float y);
	void Render(SceCtrlData *padData);

	GUButton();
	~GUButton();
};

class GUTextBox
{
private:
	char *_value;
	char *_title;

	int _type;
	int _state;
	int	_style;
	int _edittype;

	bool first_time_scroll;
	float scr_x;

	SceCtrlData currpad;
	SceCtrlData oldpad;

public:
	static const int	GUTEXTBOX_TYPE_NORMAL;
	static const int	GUTEXTBOX_TYPE_EDITABLE;

	static const int	GUTEXTBOX_STATE_NORMAL;
	static const int	GUTEXTBOX_STATE_EDITING;
	static const int	GUTEXTBOX_STATE_SELECTED;

	static const int	GUTEXTBOX_STYLE_NOSCROLL;
	static const int	GUTEXTBOX_STYLE_SCROLL;

	static const int	GUTEXTBOX_RESULT_ERROR;
	static const int	GUTEXTBOX_RESULT_CHANGED;
	static const int	GUTEXTBOX_RESULT_NOTCHANGED;

	static const float	GUTEXTBOX_FONT_HEIGHT;

	static const float	GUTEXTBOX_HEIGHT;

	static const float	GUTEXTBOX_ADJ_X;
	static const float	GUTEXTBOX_ADJ_Y;

	static const float	GUTEXTBOX_SPACE_X;
	static const float	GUTEXTBOX_SPACE_Y;


	GUTextBoxColors colors;

	void setValue(const char *newvalue);
	void setValuePtr(char *ptvalue);
	char *getValue(void);


	void setEditTitle(const char *newtitle);
	char *getEditTitle(void);

	void setEditType(int PSP_UTILITY_OSK_INPUTTYPE);
	int getEditType(void);

	void setType(int GUTEXTBOX_TYPE);
	int getType(void);

	void setState(int GUTEXTBOX_STATE);
	int getState(void);

	void setStyle(int GUTEXTBOX_STYLE);
	int getStyle(void);

	void Draw(float x, float y, float width);
	void Draw(float x, float y, float width, void *list);
	int Render(SceCtrlData *padData);

	GUTextBox(void);
	~GUTextBox();
};


class GUDiag
{
private:
	char *_value;
	int _type;
	int _state;
	bool *_ctrlflg;

	void *_list;

	SceCtrlData oldpad;

	void setCtrlflgTrue(void);
	void setCtrlflgFalse(void);

public:
	static const float	GUDIAG_FONT_HEIGHT;

	static const float	GUDIAG_HEADER;
	static const float	GUDIAG_FOOTER;

	static const float	GUDIAG_HEADLINE_WIDTH;
	static const float	GUDIAG_FOOTLINE_WIDTH;


	static const float	GUDIAG_ADJ_X;
	static const float	GUDIAG_ADJ_Y;

	static const float	GUDIAG_SPACE_X;
	static const float	GUDIAG_SPACE_Y;

	static const int	GUDIAG_TYPE_ALERT;
	static const int	GUDIAG_TYPE_YESNO;

	static const int	GUDIAG_STATE_OPEN;
	static const int	GUDIAG_STATE_CLOSED;
	static const int	GUDIAG_STATE_YES;
	static const int	GUDIAG_STATE_NO;

	GUDiagColors colors;

	void setValue(const char *newvalue);
	char *getValue(void);

	void setType(int GUDIAG_TYPE);
	int getType(void);

	void setState(int GUDIAG_STATE);
	int getState(void);

	void Draw(int bgcolor_RGBA);
	void Open(void);
	void Open(const char *value);

	GUDiag(void *list);
	GUDiag(void *list, bool *ctrlflg);
	~GUDiag();
};

void GUButtonColorsSetDefault(GUButtonColors *colors);
void GUTextBoxColorsSetDefault(GUTextBoxColors *colors);
void GUDiagColorsSetDefault(GUDiagColors *colors);

#endif