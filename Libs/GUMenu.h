#ifndef _GUMENUDECL
#define _GUMENUDECL
#include <pspctrl.h>
#define FREE(_Memory) if(_Memory!=NULL){free(_Memory);_Memory=NULL;}


struct intraFont;
extern intraFont *jpn0;

typedef struct _GUMenuColors
{
	struct _normal
	{
		int border;
		int fill;
		int text;
	}normal;

	struct _onfocus
	{
		int border;
		int fill;
		int text;
	}onfocus;

	struct _pushed
	{
		int border;
		int fill;
		int text;
	}pushed;

	struct _submenu
	{
		int border;
		int fill;
		int text;
	}submenu;

}GUMenuColors;


class GUMenuObject;
class GUMenuItem;
class GUMenuColumn;
class GUMenu;



class GUMenuObject
{
protected:
	char *_value;
	bool _valid;
	bool _enabled;

public:
	static const int MAX_COLUMN = 5;
	static const int MAX_ITEM = 5;

	char *setValue(const char *newvalue)
	{
		if(newvalue == NULL)
		{
			return NULL;
		}
		FREE(this->_value);
		this->_value = (char *)malloc(strlen(newvalue) + 1);
		strcpy(this->_value, newvalue);
		return this->_value;
	}

	char *getValue(void)
	{
		return this->_value;
	}

	void setValid(void)
	{
		this->_valid = true;
	}

	void setInvalid(void)
	{
		this->_valid = false;
	}

	void setEnabled(void)
	{
		this->_enabled = true;
	}

	void setDisabled(void)
	{
		this->_enabled = false;
	}

	bool isValid(void)
	{
		return this->_valid;
	}

	bool isEnabled(void)
	{
		return this->_enabled;
	}

};

class GUMenuItem : public GUMenuObject
{
private:
	int (*onclick) (void *args, int argc);

public:
	GUMenuColumn *parent;
	bool isSelected(void);
	void setOnclick(int (*func_onclick) (void *args, int argc));

	GUMenuItem();
	~GUMenuItem();
};

class GUMenuColumn : public GUMenuObject
{
private:
	int item_selectionid;

public:
	GUMenu *parent;
	GUMenuItem *itemlist;

	int				getFirstItemId(void);
	int				getLastItemId(void);
	int				getNextItemId(int current);
	int				getPrevItemId(int current);
	int				NValidItem(void);

	void			setSelectedItemId(int itemid);
	int				getSelectedItemId(void);
	bool			setItem(int itemid, const char *value, int (*onclick)(void *args, int argc));
	GUMenuItem		*getItem(int itemid);

	bool	isSelected(void);

	GUMenuColumn();
	~GUMenuColumn();
};

class GUMenu : public GUMenuObject
{
private:
	static const float	GUMENU_ADJ_X;
	static const float	GUMENU_ADJ_Y;

	static const float	GUMENU_SPACE_X;
	static const float	GUMENU_SPACE_Y;

	static const float	GUMENU_WIDTH;
	static const float	GUMENU_HEIGHT;

	static const float	GUMENU_COLUMN_FONT_HEIGHT;
	static const float	GUMENU_COLUMN_HEIGHT;

	static const float	GUMENU_ITEM_FONT_HEIGHT;
	static const float	GUMENU_ITEM_HEIGHT;

	int columncount;
//	int itemcount;

	int _state;
	int col_selectionid;

	float x, y; // 文字の座標
	float sub_x1, sub_y1; // サブメニューに使う座標
	float sub_x2, sub_y2;
	float x1, y1, x2, y2; // 選択項目の枠とかに使う座標

	float width, max_width; // サブメニューの大きさ測定に使う

	int validitemcount;

	GUMenuColors colors;
	GUMenuColumn *columnlist;

public:
	static const int	GUMENU_STATE_HIDE;
	static const int	GUMENU_STATE_SHOW;
	static const int	GUMENU_STATE_OPENSUBMENU;

	int				getFirstColumnId(void);
	int				getLastColumnId(void);
	int				getNextColumnId(int current);
	int				getPrevColumnId(int current);
	int				NValidColumn(void);

	void			setState(int GUMENU_STATE);
	int				getState(void);
	void			setSelectedColumnId(int columnid);
	int				getSelectedColumnId(void);
	bool			setColumn(int columnid, const char *value);
	GUMenuColumn	*getColumn(int columnid);

	void Render(unsigned int PSP_CTRL);
	void Draw(void);


	GUMenu();
	~GUMenu();
};

void GUMenuColorsSetDefault(GUMenuColors *colors);
#endif





