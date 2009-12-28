#ifndef _GULVDECL
#define _GULVDECL

#include <pspkernel.h>
#include <pspctrl.h>

#include "Libs/GUDiag.h"

class GUListObject;
class GUListItem;
class GUListColumn;
class GUListView;

class GUListObject
{
protected:
	bool _valid;
	char *_value;

public:
	int id;

	void setValue(const char *newvalue);
	void setValuePtr(char *ptrvalue);
	char *getValue(void);

	bool isValid(void);
	void setValid(void);
	void setInvalid(void); 

};

class GUListItem : public GUListObject
{
private:

public:
	GUListItem *prev;
	GUListItem *next;

	GUListItem();
	~GUListItem();
};

class GUListColumn : public GUListObject
{
private:

public:
	GUListView *parent;
	GUListItem *itemlist_first;
	GUListItem *itemlist_current;
	float width;

	unsigned int list_count;
	bool list_filled;

	void			setScrollStyle(int GUTEXTBOX_STYLE);
	int				getScrollStyle(void);

	GUListItem		*getItem(int itemid);
	void			addItem(const char *value);
	void			delItem(int itemid);
	
	void			flushItem(void);
	unsigned int	NValidItem(void);

	GUListColumn();
	~GUListColumn();
};


class GUListView
{
private:
	// ÉåÉìÉ_Å[óp
	bool			index_changed;

	SceCtrlData		oldpad;
	SceCtrlData		currpad;
	unsigned int	padmask;
	int				pad_scr;
	unsigned int	stick_neutral;
	unsigned int	stick_torelance;
	int				stick_scr;

	int				ltrigger_delay;
	int				rtrigger_delay;
	int				trigger_firstdelay;
	bool			trigger_isfirstdelay;
	int				trigger_scr;

	unsigned int	cursor_max;

public:
	GUListColumn *columnlist;
	GUTextBox *dispcolumnlist;
	GUTextBox **dispitemlist;

	unsigned int	_index;
	unsigned int	_disp_num;
	unsigned int	_column_num;
	unsigned int	_max_item;

	void			setColumn(unsigned int columnid, const char *value);
	GUListColumn	*getColumn(unsigned int columnid);
	int				getLongestColumnId(void);

	void Render(SceCtrlData *padData, unsigned int *cursor);
	void Draw(float x, float y);

	GUListView();
	GUListView(unsigned int ncolumn, unsigned int maxitem, unsigned int ndisprow);
	~GUListView();

};

#endif