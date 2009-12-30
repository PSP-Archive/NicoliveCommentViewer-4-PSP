#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <pspkernel.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>


#include "../GUDraw.h"
#include "../GUMenu.h"

#include "../intraFont/intraFont.h"

//�萔********************************************************
const float	GUMenu::GUMENU_ADJ_X = 0.0f;
const float	GUMenu::GUMENU_ADJ_Y = 13.0f;

const float	GUMenu::GUMENU_SPACE_X = 10.0f;
const float	GUMenu::GUMENU_SPACE_Y = 4.0f;

const int	GUMenu::GUMENU_STATE_HIDE = 0;
const int	GUMenu::GUMENU_STATE_SHOW = 1;
const int	GUMenu::GUMENU_STATE_OPENSUBMENU = 10;

const float	GUMenu::GUMENU_WIDTH	= 480.0f;
const float	GUMenu::GUMENU_HEIGHT	= 24.0f;

//const float	GUMenu::GUMENU_COLUMN_FONT_HEIGHT = 12.8f;
const float	GUMenu::GUMENU_COLUMN_FONT_HEIGHT = 13.0f;
const float	GUMenu::GUMENU_COLUMN_HEIGHT = GUMENU_HEIGHT;

//const float	GUMenu::GUMENU_ITEM_FONT_HEIGHT = 12.8f;
const float	GUMenu::GUMENU_ITEM_FONT_HEIGHT = 13.0f;
const float	GUMenu::GUMENU_ITEM_HEIGHT = GUMENU_ITEM_FONT_HEIGHT + GUMENU_SPACE_Y*2;


// onclick�̂��߂̃_�~�[�֐�
int Foo(int argc, void *argp)
{
	return 0;
}
 
//GUMenuObject********************************************************
char *GUMenuObject::setValue(const char *newvalue)
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

char *GUMenuObject::getValue(void)
{
	return this->_value;
}

void GUMenuObject::setValid(void)
{
	this->_valid = true;
}

void GUMenuObject::setInvalid(void)
{
	this->_valid = false;
}

void GUMenuObject::setEnabled(void)
{
	this->_enabled = true;
}

void GUMenuObject::setDisabled(void)
{
	this->_enabled = false;
}

bool GUMenuObject::isValid(void)
{
	return this->_valid;
}

bool GUMenuObject::isEnabled(void)
{
	return this->_enabled;
}

//GUMenuItem********************************************************
GUMenuItem::GUMenuItem()
{
	this->_value = NULL;
}

GUMenuItem::~GUMenuItem()
{
}

bool GUMenuItem::isSelected(void)
{
	int selId;
	selId = this->parent->getSelectedItemId();

	if(this->parent->getItem(selId) == this)
	{
		return true;
	}

	return false;
}

void GUMenuItem::setOnclick(GUMenuOnclick onclick)
{
	this->onclick = onclick;
}
//********************************************************

//GUMenuColumn********************************************************
GUMenuColumn::GUMenuColumn()
{
	this->_value = NULL;
	item_selectionid = 0;

	itemlist = new GUMenuItem[MAX_ITEM];
	for(int i=0; i<MAX_ITEM; i++)
	{
		this->getItem(i)->parent = this;
	}
}

GUMenuColumn::~GUMenuColumn()
{
	delete [] itemlist;
}

int GUMenuColumn::getFirstItemId(void)
{
	// �L���ȍ��ڂ�1������
	if(this->NValidItem() == 0)
	{
		return -1;
	}

	int firstId = 0;
	for(int i=0; i<MAX_ITEM; i++)
	{
		if(this->getItem(i)->isValid())
		{
			firstId = i;
			break;
		}
	}

	return firstId;
}

int GUMenuColumn::getLastItemId(void)
{
	// �L���ȍ��ڂ�1������
	if(this->NValidItem() == 0)
	{
		return -1;
	}

	int lastId = 0;;
	for(int i=0; i<MAX_ITEM; i++)
	{
		if(this->getItem(i)->isValid())
		{
			lastId = i;
		}
	}

	return lastId;
}

int GUMenuColumn::getNextItemId(int current)
{
	// �L���ȍ��ڂ�1������
	if(this->NValidItem() == 0)
	{
		return -1;
	}

	int nextId = current;
	for(int i=current+1; i<MAX_ITEM; i++)
	{
		if(this->getItem(i)->isValid())
		{
			nextId = i;
			break;
		}
	}

	if(nextId == current)
	{
		// ���݂̍��ڂ���ԉE�������̂ŁA
		// ��ԍ��̍��ڂ��擾
		for(int i=0; i<MAX_ITEM; i++)
		{
			if(this->getItem(i)->isValid())
			{
				nextId = i;
				break;
			}
		}
	}

	return nextId;
}

int GUMenuColumn::getPrevItemId(int current)
{
	// �L���ȍ��ڂ�1������
	if(NValidItem() == 0)
	{
		return -1;
	}

	int firstId = 0;
	int prevId = current;

	//��ԍŏ��̍��ڂ�ID���擾
	for(int i=0; i<MAX_ITEM; i++)
	{
		if(this->getItem(i)->isValid())
		{
			firstId = i;
			break;
		}
	}

	if(current != firstId)
	{
		// ���ʂɁA1�O��ID���擾
		for(int i=0; i<MAX_ITEM; i++)
		{
			if(i == current)
			{
				break;
			}

			if(this->getItem(i)->isValid())
			{
				prevId = i;
			}
		}
	}
	else
	{
		// �I�����ڂ���ԍ��������̂ŁA
		// ��ԉE�̍��ڂ��擾
		for(int i=0; i<MAX_ITEM; i++)
		{
			if(this->getItem(i)->isValid())
			{
				prevId = i;
			}
		}
	}
			
	return prevId;
}

int GUMenuColumn::NValidItem(void)
{
	int count = 0;

	for(int i=0; i<MAX_ITEM; i++)
	{
		if(this->getItem(i)->isValid())
		{
			count++;
		}
	}

	return count;
}

bool GUMenuColumn::isSelected(void)
{
	int selId;
	selId = this->parent->getSelectedColumnId();

	if(this->parent->getColumn(selId) == this)
	{
		return true;
	}

	return false;
}

void GUMenuColumn::setSelectedItemId(int itemid)
{
	item_selectionid = itemid;
}

int GUMenuColumn::getSelectedItemId(void)
{
	return item_selectionid;
}

bool GUMenuColumn::setItem(int itemid, const char *value, GUMenuOnclick onclick)
{
	//rewrites the old item.

	if(itemid >= 0 && itemid <= MAX_ITEM)
	{
		this->getItem(itemid)->setValid();
		this->getItem(itemid)->setValue(value);
		this->getItem(itemid)->setOnclick(onclick);
	}

	return 0;
}

GUMenuItem *GUMenuColumn::getItem(int itemid)
{
	return &itemlist[itemid];
}
//********************************************************

//GUMenu********************************************************
GUMenu::GUMenu()
{
	columncount = 0;
	col_selectionid = 0;

	this->_state = GUMENU_STATE_HIDE;
	memset(&currpad, 0, sizeof(SceCtrlData));
	memset(&oldpad, 0, sizeof(SceCtrlData));

	columnlist = new GUMenuColumn[MAX_COLUMN];
	for(int i=0; i<MAX_COLUMN; i++)
	{
		columnlist[i].parent = this;

		this->getColumn(i)->setInvalid(); 
		this->getColumn(i)->setDisabled();

		for(int p=0; p<MAX_ITEM; p++)
		{
			this->getColumn(i)->getItem(p)->setInvalid();
			this->getColumn(i)->getItem(p)->setDisabled();
			this->getColumn(i)->getItem(p)->setOnclick(Foo);
		}
	}

	GUMenuColorsSetDefault(&colors);
}

GUMenu::~GUMenu()
{
	delete [] columnlist;
}

int GUMenu::getFirstColumnId(void)
{
	// �L���ȍ��ڂ�1������
	if(NValidColumn() == 0)
	{
		return -1;
	}

	int firstId = 0;
	for(int i=0; i<MAX_COLUMN; i++)
	{
		if(this->getColumn(i)->isValid())
		{
			firstId = i;
			break;
		}
	}

	return firstId;
}

int GUMenu::getLastColumnId(void)
{
	// �L���ȍ��ڂ�1������
	if(NValidColumn() == 0)
	{
		return -1;
	}

	int lastId = 0;;
	for(int i=0; i<MAX_COLUMN; i++)
	{
		if(this->getColumn(i)->isValid())
		{
			lastId = i;
		}
	}

	return lastId;
}

int GUMenu::getNextColumnId(int current)
{
	// �L���ȍ��ڂ�1������
	if(NValidColumn() == 0)
	{
		return -1;
	}

	int nextId = current;
	for(int i=current+1; i<MAX_COLUMN; i++)
	{
		if(this->getColumn(i)->isValid())
		{
			nextId = i;
			break;
		}
	}

	if(nextId == current)
	{
		// ���݂̍��ڂ���ԉE�������̂ŁA
		// ��ԍ��̍��ڂ��擾
		for(int i=0; i<MAX_COLUMN; i++)
		{
			if(this->getColumn(i)->isValid())
			{
				nextId = i;
				break;
			}
		}
	}

	return nextId;
}

int GUMenu::getPrevColumnId(int current)
{
	// �L���ȍ��ڂ�1������
	if(NValidColumn() == 0)
	{
		return -1;
	}

	int firstId = 0;
	int prevId = current;

	//��ԍŏ��̍��ڂ�ID���擾
	for(int i=0; i<MAX_COLUMN; i++)
	{
		if(this->getColumn(i)->isValid())
		{
			firstId = i;
			break;
		}
	}

	if(current != firstId)
	{
		// ���ʂɁA1�O��ID���擾
		for(int i=0; i<MAX_COLUMN; i++)
		{
			if(i == current)
			{
				break;
			}

			if(this->getColumn(i)->isValid())
			{
				prevId = i;
			}
		}
	}
	else
	{
		// �I�����ڂ���ԍ��������̂ŁA
		// ��ԉE�̍��ڂ��擾
		for(int i=0; i<MAX_COLUMN; i++)
		{
			if(this->getColumn(i)->isValid())
			{
				prevId = i;
			}
		}
	}
			
	return prevId;
}

int GUMenu::NValidColumn(void)
{
	int count = 0;

	for(int i=0; i<MAX_COLUMN; i++)
	{
		if(this->getColumn(i)->isValid())
		{
			count++;
		}
	}

	return count;
}

void GUMenu::setState(int GUMENU_STATE)
{
	this->_state = GUMENU_STATE;
}

int GUMenu::getState(void)
{
	return this->_state;
}

void GUMenu::setSelectedColumnId(int columnid)
{
	col_selectionid = columnid;
}

int GUMenu::getSelectedColumnId(void)
{
	return col_selectionid;
}

bool GUMenu::setColumn(int columnid, const char *value)
{
	//rewrites the old column.

	if(columnid >= 0 && columnid <= MAX_COLUMN)
	{
		this->getColumn(columnid)->setValid();
		this->getColumn(columnid)->setValue(value);
	}
	else
	{
		return 1;
	}

	return 0;
}

GUMenuColumn *GUMenu::getColumn(int columnid)
{
	return &columnlist[columnid];
}

int GUMenu::Render(SceCtrlData *padData)
{
	int ret = 0;
	
	// ���ݑI������Ă��郁�j���[����
	GUMenuColumn *selColumn = this->getColumn(this->getSelectedColumnId());;

	currpad = *padData;
	if(!(oldpad.Buttons & PSP_CTRL_TRIANGLE)
		&& (currpad.Buttons & PSP_CTRL_TRIANGLE))
	{
		//���j���[�o�[�̕\��/��\��
		if(this->getState() == GUMENU_STATE_HIDE)
		{
			this->setState(GUMENU_STATE_SHOW); // �\��
			setSelectedColumnId(getFirstColumnId()); //��ԍŏ��̍��ڂ�I��
		}
		else
		{
			this->setState(GUMENU_STATE_HIDE); // ��\��
		}
	}
	else if(!(oldpad.Buttons & PSP_CTRL_CROSS)
		&& (currpad.Buttons & PSP_CTRL_CROSS))
	{
		if(this->getState() == GUMENU_STATE_SHOW)
		{
			this->setState(GUMENU_STATE_HIDE); // ���j���[���\��
		}
		else if(this->getState() == GUMENU_STATE_OPENSUBMENU)
		{
			this->setState(GUMENU_STATE_SHOW); // �T�u���j���[�����
		}
		else
		{
			// other this->getState()
		}
	}
	else if(!(oldpad.Buttons & PSP_CTRL_CIRCLE)
		&& (currpad.Buttons & PSP_CTRL_CIRCLE))
	{
		if(this->getState() == GUMENU_STATE_SHOW)
		{
			// �T�u���j���[��W�J
			this->setState(GUMENU_STATE_OPENSUBMENU);
			
			// �ŏ��̃A�C�e����I��
			selColumn->setSelectedItemId(selColumn->getFirstItemId());
		}
		else if(this->getState() == GUMENU_STATE_OPENSUBMENU)
		{
			// �A�C�e���������ꂽ
			ret = this->getSelectedColumnId() * 10
				+ selColumn->getSelectedItemId();

			this->setState(GUMENU_STATE_SHOW);
		}
	}
	else if(!(oldpad.Buttons & PSP_CTRL_LEFT)
		&& (currpad.Buttons & PSP_CTRL_LEFT))
	{
		if(this->getState() == GUMENU_STATE_SHOW)
		{
			if(this->NValidColumn() > 1)
			{
				if(this->getSelectedColumnId() != this->getFirstColumnId())
				{
					// �I�����ڂ�1���ɂ��炷
					this->setSelectedColumnId(this->getPrevColumnId(this->getSelectedColumnId())); 
				}
				else
				{
					// �I�����ڂ���ԉE�ɂ���
					this->setSelectedColumnId(this->getLastColumnId()); 
				}
			}
			else
			{
				// �L���ȃ��j���[���ڂ�1�����Ȃ���
				// �I�����ڂ𓮂����Ȃ�
			}
		}
		else if(this->getState() == GUMENU_STATE_OPENSUBMENU)
		{
			// �g���A�C�e������Ȃ��ꍇ
			this->setState(GUMENU_STATE_OPENSUBMENU);
			
			// ���̃��j���[���ڂ�I��
			this->setSelectedColumnId(this->getPrevColumnId(this->getSelectedColumnId()));
			
			// �ŏ��̃A�C�e����I��
			this->getColumn(getSelectedColumnId())->setSelectedItemId(this->getColumn(this->getSelectedColumnId())->getFirstItemId());
		}
		else
		{
			// other state
		}
	}
	else if(!(oldpad.Buttons & PSP_CTRL_RIGHT)
		&& (currpad.Buttons & PSP_CTRL_RIGHT))
	{
		if(this->getState() == GUMENU_STATE_SHOW)
		{
			if(NValidColumn() > 1)
			{
				if(this->getSelectedColumnId() != this->getLastColumnId())
				{
					// �I�����ڂ�1�E�ɂ��炷
					this->setSelectedColumnId(this->getNextColumnId(this->getSelectedColumnId()));
				}
				else
				{
					// �I�����ڂ���ԍ��ɂ���
					this->setSelectedColumnId(this->getFirstColumnId());
				}
			}
			else
			{
				// �L���ȃ��j���[���ڂ�1�����Ȃ���
				// �I�����ڂ𓮂����Ȃ�
			}
		}
		else if(this->getState() == GUMENU_STATE_OPENSUBMENU)
		{
			// �g���A�C�e������Ȃ��ꍇ
			this->setState(GUMENU_STATE_OPENSUBMENU);
			
			// ���̃��j���[���ڂ�I��
			this->setSelectedColumnId(this->getNextColumnId(this->getSelectedColumnId()));
			
			// �ŏ��̃A�C�e����I��
			this->getColumn(getSelectedColumnId())->setSelectedItemId(this->getColumn(this->getSelectedColumnId())->getFirstItemId());
		}
		else
		{
			// other state
		}
	}
	else if(!(oldpad.Buttons & PSP_CTRL_UP)
		&& (currpad.Buttons & PSP_CTRL_UP))
	{
		if(this->getState() == GUMENU_STATE_OPENSUBMENU)
		{
			// �J�[�\������ԏゾ�����ꍇ
			if(selColumn->getSelectedItemId() == selColumn->getFirstItemId())
			{
				selColumn->setSelectedItemId(selColumn->getLastItemId());
			}
			else // ���ʂ�
			{
				// �P��ɁB
				selColumn->setSelectedItemId(selColumn->getPrevItemId(selColumn->getSelectedItemId()));
			}
		}
	}
	else if(!(oldpad.Buttons & PSP_CTRL_DOWN)
		&& (currpad.Buttons & PSP_CTRL_DOWN))
	{
		if(this->getState() == GUMENU_STATE_OPENSUBMENU)
		{
			// �J�[�\������ԉ��������ꍇ
			if(selColumn->getSelectedItemId() == selColumn->getLastItemId())
			{
				// ��ԏ�ɖ߂�
				selColumn->setSelectedItemId(selColumn->getFirstItemId());
			}
			else // ���ʂ�
			{
				// 1���ɁB
				selColumn->setSelectedItemId(selColumn->getNextItemId(selColumn->getSelectedItemId()));
			}
		}
	}
	else
	{
		// other key
	}

	oldpad = currpad;
	return ret;
}

void GUMenu::Draw()
{
	// �I������Ă��郁�j���[����
	GUMenuColumn *selColumn = this->getColumn(this->getSelectedColumnId());

	intraFontSetStyle(jpn0, 0.8f, BLACK, 0, NULL);

	switch(this->getState())
	{
	case GUMENU_STATE_HIDE: // ��\����
		intraFontSetStyle(jpn0, 0.8f, BLACK, 0, NULL);
		break;

	case GUMENU_STATE_SHOW: // �ʏ펞
		intraFontSetStyle(jpn0, 0.8f, BLACK, 0, NULL);
	
		// ���j���[�o�[�S�̂̔w�i
		DrawSquare(0.0f, 0.0f, GUMENU_WIDTH, GUMENU_HEIGHT, colors.normal.fill, true);

		//srand(time(NULL));	
		//setSelectedColumnId((rand() % 2 == 1) ? 0 : 1);

		x = 0.0f;
		y = 0.0f + GUMENU_SPACE_Y;
		for(int i=0; i<MAX_COLUMN; i++)
		{
			if(getColumn(i)->isValid())
			{
				x += GUMENU_SPACE_X;
				if(getColumn(i)->isSelected())
				{
					x1 = x - GUMENU_SPACE_X;
					y1 = 0.0f;

					intraFontSetStyle(jpn0, 0.8f, colors.onfocus.text, 0, NULL);
					x = intraFontPrintf(jpn0, x+GUMENU_ADJ_X, y+GUMENU_ADJ_Y, "%s", getColumn(i)->getValue());
				
					x2 = x + GUMENU_SPACE_X;
					y2 = GUMENU_HEIGHT;

					// �g
					DrawSquare(x1, y1, x2, y2, colors.onfocus.border, false);
				}
				else
				{
					intraFontSetStyle(jpn0, 0.8f, colors.normal.text, 0, NULL);
					x = intraFontPrintf(jpn0, x+GUMENU_ADJ_X, y+GUMENU_ADJ_Y, "%s", getColumn(i)->getValue());
				}
				x += GUMENU_SPACE_X;
			}
		}
		break;

	case GUMENU_STATE_OPENSUBMENU: // �T�u���j���[�W�J��
		intraFontSetStyle(jpn0, 0.8f, BLACK, 0, NULL);

		// ���j���[�o�[�S�̂̔w�i
		DrawSquare(0.0f, 0.0f, GUMENU_WIDTH, GUMENU_HEIGHT, colors.normal.fill, true);

		// ���j���[���ڂ�`��
		x = 0.0f;
		y = 0.0f + GUMENU_SPACE_Y;
		for(int i=0; i<MAX_COLUMN; i++)
		{
			if(getColumn(i)->isValid())
			{
				x += GUMENU_SPACE_X;
				if(getColumn(i)->isSelected())
				{
					sub_x1 = x - GUMENU_SPACE_X; // �T�u���j���[�̍���̍��W��ۑ�
					sub_y1 = GUMENU_HEIGHT;

					intraFontSetStyle(jpn0, 0.8f, colors.pushed.text, 0, NULL);
				}
				else
				{
					intraFontSetStyle(jpn0, 0.8f, colors.normal.text, 0, NULL);
				}
				x = intraFontPrintf(jpn0, x+GUMENU_ADJ_X, y+GUMENU_ADJ_Y, "%s", getColumn(i)->getValue());
				x += GUMENU_SPACE_X;
			}
		}


		// �T�u���j���[
		intraFontSetStyle(jpn0, 0.8f, BLACK, 0, NULL);

		// �T�u���j���[�̉E���̍��W���擾���邽�߂ɁA
		// �Œ��̃��j���[���ڂ̉E�����W+�X�y�[�T�[���擾
		for(int i=0; i<MAX_ITEM; i++)
		{
			if(selColumn->getItem(i)->isValid())
			{	
				width = intraFontMeasureText(jpn0, selColumn->getItem(i)->getValue());
				if(width > max_width)
				{
					max_width = width;
				}
			}
		}

		sub_x2 = sub_x1 + GUMENU_SPACE_X + max_width + GUMENU_SPACE_X;
		sub_y2 = GUMENU_HEIGHT + GUMENU_ITEM_HEIGHT * selColumn->NValidItem();

		// �T�u���j���[�̔w�i
		DrawSquare(sub_x1, sub_y1, sub_x2, sub_y2, colors.submenu.fill, true);
	
		// �A�C�e��
		intraFontSetStyle(jpn0, 0.8f, colors.submenu.text, 0, NULL);

		for(int i=0; i<MAX_ITEM; i++)
		{
			if(selColumn->getItem(i)->isValid())
			{
				validitemcount++;

				x = sub_x1 + GUMENU_SPACE_X;
				y = sub_y1 + (validitemcount-1) * GUMENU_ITEM_HEIGHT + GUMENU_SPACE_Y;
				if(selColumn->getItem(i)->isSelected())
				{
					x1 = x - GUMENU_SPACE_X; // �I��g�̍�����W
					y1 = y - GUMENU_SPACE_Y;
					
					x = intraFontPrintf(jpn0, x+GUMENU_ADJ_X, y+GUMENU_ADJ_Y, "%s", selColumn->getItem(i)->getValue());
					y += GUMENU_ITEM_FONT_HEIGHT;

					y += GUMENU_SPACE_Y;

					x2 = sub_x2; // �I��g�̉E�����W
					y2 = y;
				}
				else
				{
					// �I������Ă��Ȃ�
					intraFontPrintf(jpn0, x+GUMENU_ADJ_X, y+GUMENU_ADJ_Y, "%s", selColumn->getItem(i)->getValue());
				}
			}
		}

		// �A�C�e���̑I��g
		DrawSquare(x1, y1, x2, y2, colors.submenu.border, false);
		break;

	default:
		break;
	}

}
//********************************************************



void GUMenuColorsSetDefault(GUMenuColors *colors)
{
	memset(colors, 0, sizeof(GUMenuColors));
	
	colors->normal.border	= RGBA(0, 0, 0, 255);
	colors->normal.fill		= RGBA(128, 128, 128, 255);
	colors->normal.text		= RGBA(0, 0, 0, 255);

	colors->onfocus.border	= RGBA(0, 0, 0, 255);
	colors->onfocus.fill	= RGBA(128, 128, 128, 255);
	colors->onfocus.text	= RGBA(0, 0, 0, 255);

	colors->pushed.border	= RGBA(0, 0, 0, 255);
	colors->pushed.fill		= RGBA(128, 128, 128, 255);
	colors->pushed.text		= RGBA(0, 0, 0, 255);

	colors->submenu.border	= RGBA(0, 0, 0, 255);
	colors->submenu.fill	= RGBA(128, 128, 128, 255);
	colors->submenu.text	= RGBA(0, 0, 0, 255);
}

