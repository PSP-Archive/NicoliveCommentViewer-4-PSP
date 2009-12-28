#include "../GUDraw.h"
#include "../GUListView.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void GUListObject::setValue(const char *newvalue)
{
	if(newvalue == NULL)
	{
		return;
	}

	FREE(this->_value);
	this->_value = (char *)malloc(strlen(newvalue+1));
	strcpy(this->_value, newvalue);
}

void GUListObject::setValuePtr(char *ptrvalue)
{
	if(ptrvalue == NULL)
	{
		return;
	}

	FREE(this->_value);
	this->_value = ptrvalue;
}

char *GUListObject::getValue(void)
{
	return this->_value;
}


bool GUListObject::isValid(void)
{
	return this->_valid;
}

void GUListObject::setValid(void)
{
	this->_valid = true;
}

void GUListObject::setInvalid(void)
{
	this->_valid = false;
}

GUListItem::GUListItem()
{
	this->_value = NULL;
	this->prev = NULL;
	this->next = NULL;
}

GUListItem::~GUListItem()
{
	FREE(this->_value);
}


GUListColumn::GUListColumn()
{
	this->id = -1;
	this->width = 50.0f;
	this->_value = NULL;

	this->list_count = 0;
	this->list_filled = false;

	this->itemlist_first = NULL;
	this->itemlist_current = NULL;
}

GUListColumn::~GUListColumn()
{
	GUListItem *ptr;
	ptr = this->itemlist_first;

	while(1)
	{
		if(ptr->next != NULL)
		{
			ptr->next->prev = ptr;
			ptr = ptr->next;

			delete ptr->prev;
		}
		else
		{
			break;
		}
	}
}

void GUListColumn::setScrollStyle(int GUTEXTBOX_STYLE)
{	
	this->parent->dispcolumnlist[this->id].setStyle(GUTEXTBOX_STYLE);
	
	for(unsigned int i=0; i<this->parent->_disp_num; i++)
	{
		this->parent->dispitemlist[i][this->id].setStyle(GUTEXTBOX_STYLE);
	}
}

int GUListColumn::getScrollStyle(void)
{
	return this->parent->dispitemlist[0][this->id].getStyle();
}

GUListItem *GUListColumn::getItem(int itemid)
{
	if(itemid <= 0)
	{
		return this->itemlist_first;
	}

	if(this->itemlist_first == NULL)
	{
		return NULL;
	}

	GUListItem *ptr = itemlist_first;
	for(int i=0; i<itemid; i++)
	{
		if(ptr->next == NULL)
		{
			return NULL; // リストの末尾。
		}
		ptr = ptr->next;
	}

	return ptr;
}

void GUListColumn::addItem(const char *value)
{
	if(itemlist_first == NULL)
	{
		itemlist_first = new GUListItem;
		itemlist_first->prev = NULL;
		itemlist_first->next = NULL;
		itemlist_first->setValue(value);
		itemlist_first->setValid();

		itemlist_current = itemlist_first;
	}
	else
	{
		itemlist_current->next = new GUListItem;
		itemlist_current->next->prev = itemlist_current;
		itemlist_current->next->next = NULL;
		itemlist_current->next->setValue(value);
		itemlist_current->next->setValid();

		itemlist_current = itemlist_current->next;
		this->list_count++;
	}

	if(!this->list_filled && this->list_count == this->parent->_disp_num)
	{
		list_filled = true;
	}
	else
	{
		list_filled = false;
	}

	// アイテムが最大数から溢れないようにする
	if(this->list_count > this->parent->_max_item)
	{
		itemlist_first = itemlist_first->next;
		delete itemlist_first->prev;
	}
}

void GUListColumn::delItem(int itemid)
{
	if(itemid < 0 || this->itemlist_first == NULL)
	{
		return;
	}
	else if(itemid == 0)
	{
		itemlist_first = itemlist_first->next;
		delete itemlist_first->prev;
		itemlist_first->prev = NULL;

		list_count--;
		return;
	}


	// itemid番目の項目を探す
	GUListItem *ptr = itemlist_first;
	for(int i=0; i<itemid; i++)
	{
		if(ptr == NULL)
		{
			return; // itemid以前に、終端に到達してしまった
		}
		
		ptr = ptr->next;
	}

	// 指定されたアイテムが、
	// リストの最後のアイテムだった
	if(ptr->next == NULL)
	{
		ptr->prev->next = NULL;
		itemlist_current = ptr->prev;
		
		list_count--;
		delete ptr;
	}
	else
	{
		// 穴埋め
		ptr->next->prev = ptr->prev;
		ptr->prev->next = ptr->next;
		
		list_count--;
		delete ptr;
	}
}

void GUListColumn::flushItem(void)
{
	GUListItem *ptr = this->itemlist_first;

	while(1)
	{
		if(ptr->next == NULL)
		{
			delete ptr;
			break;
		}

		ptr = ptr->next;
		delete ptr->prev;
	}

	this->itemlist_first = NULL;
	this->itemlist_current = NULL;
	this->list_count = 0;
	this->list_filled = false;
}

unsigned int GUListColumn::NValidItem(void)
{
	int count = 0;
	int i=0;
	// 有効なアイテム数をカウント

	if(this->itemlist_first == NULL)
	{
		return 0;
	}

	while(1)
	{
		if(this->getItem(i)->isValid())
		{
			count++;
		}

		if(this->getItem(i)->next == NULL)
		{
			break;
		}

		i++;
	}

	return count;
}

GUListView::GUListView()
{
}

GUListView::GUListView(unsigned int ncolumn, unsigned int maxitem, unsigned int ndisprow)
{
	this->_disp_num = ndisprow;
	this->_column_num = ncolumn;
	this->_max_item = maxitem;

	unsigned int i, p, q;


	// パッド入力周りの変数を初期化
	memset(&oldpad, 0, sizeof(SceCtrlData));
	memset(&currpad, 0, sizeof(SceCtrlData));

	// レンダー用各種変数を初期化
	this->_index = 0; // 表示する最初の項目が先頭から何番目か
	pad_scr = 0; // パッドのスクロール量
	stick_neutral = 127; // アナログスティックのニュートラル位置
	stick_torelance = 25; // アナログスティックの個体差
	stick_scr = 0; // アナログスティックのスクロール量
	ltrigger_delay = 10;
	rtrigger_delay = 10;

	trigger_scr = 0; //Rトリガーのスクロール量
	cursor_max = 0;


	// コラム(GUListColumnの配列)、
	// アイテム(線形リスト)、
	// 表示用コラム(GUTextBoxの一次元配列)、
	// 表示用アイテム(GUTextBoxの二次元配列)
	// を初期化
	columnlist = new GUListColumn[ncolumn];
	for(i=0; i<ncolumn; i++)
	{
		columnlist[i].id = i;
		columnlist[i].width = 50;
		columnlist[i].parent = this;
		columnlist[i].setInvalid();
	}

	// 表示用コラム初期化
	this->dispcolumnlist = new GUTextBox[ncolumn];

	// 表示用アイテム初期化
	//this->dispitemlist = new GUTextBox[_disp_num][_column_num];
	this->dispitemlist = new GUTextBox*[_disp_num];
	for(i=0; i<_disp_num; i++)
	{
		this->dispitemlist[i] = new GUTextBox[_column_num];
	}
	for(p=0; p<_column_num; p++)
	{
		for(q=0; q<_disp_num; q++)
		{
			//this->dispitemlist[i][p].setValue("");
			this->dispitemlist[q][p].setState(GUTextBox::GUTEXTBOX_STATE_NORMAL);
			this->dispitemlist[q][p].setType(GUTextBox::GUTEXTBOX_TYPE_NORMAL);
			this->dispitemlist[q][p].colors.normal.fill = RGBA(255, 255, 255, 255);
			this->dispitemlist[q][p].colors.selected.fill = RGBA(255, 255, 255, 255);
		}
	}

}

GUListView::~GUListView()
{
	unsigned int i;
	for(i=0; i<this->_disp_num; i++)
	{
		delete [] this->dispitemlist[i];
	}
	delete [] this->dispitemlist;
	delete [] this->dispcolumnlist;
	delete [] this->columnlist;
}

void GUListView::setColumn(unsigned int columnid, const char *value)
{
	this->columnlist[columnid].id = columnid;
	this->columnlist[columnid].setValue(value);
	this->columnlist[columnid].setValid();

	this->dispcolumnlist[columnid].setValue(value);
}

GUListColumn *GUListView::getColumn(unsigned int columnid)
{
	return &this->columnlist[columnid];
}

int GUListView::getLongestColumnId(void)
{
	unsigned int id = 0, nitem = 0;
	unsigned int i;
	for(i=0; i<this->_column_num; i++)
	{
		if(this->getColumn(i)->NValidItem() > nitem)
		{
			nitem = this->getColumn(i)->NValidItem();
			id = i;
		}
	}
	
	return id;
}

void GUListView::Render(SceCtrlData *padData, unsigned int *cursor)
{
	unsigned int i, p, q;
	index_changed = false;
	cursor_max = this->columnlist[this->getLongestColumnId()].NValidItem()
		- this->_disp_num;

	if(!(padData == NULL && cursor == NULL))
	{
		if(this->getColumn(this->getLongestColumnId())->list_filled)
		{
			// パッドの情報
			// 条件に関係無く、毎回保存しておく
			//memset(&currpad, 0, sizeof(SceCtrlData));
			if(padData != NULL)
			{
				currpad = *padData;
			}

			if(cursor != NULL)
			{
				// カーソル位置の指定があった場合は、
				// それに従う

				// 変更の必要無し
				//if(*cursor == _index)
				//{
				//	return;
				//}
				// いずれかのコラムが保持しているアイテムの数のうちの
				// 最大を超えていた場合、
				// 最後のアイテムへスクロール
				if(*cursor >= cursor_max)
				{
					index_changed = true;
					*cursor = cursor_max;
					_index = cursor_max;
				}
				else
				{
					index_changed = true;
					_index = *cursor;
				}
			}
			else
			{
				// カーソル位置の指定が特に無かった場合は
				// こっちで勝手に調整する
				// 優先順位:
				// アナログスティック>L/Rトリガー>パッド
				stick_scr = 0;
				trigger_scr = 0;
				pad_scr = 0;

				// アナログスティック補正値の算出
				// 0<= pad.Ly <= 255; neutral = 127+-25~30

				// neutral +- torelance
				if(stick_neutral-stick_torelance < (int)currpad.Ly
					&& (int)currpad.Ly <= stick_neutral+stick_torelance)
				{
					stick_scr = 0;
				}
				// neutral+torelance +- 60
				else if(stick_neutral-stick_torelance-60 > (int)currpad.Ly)
				{
					stick_scr = -1;
				}
				else if(stick_neutral+stick_torelance+60 < (int)currpad.Ly)
				{
					stick_scr = 1;
				}

				if(stick_scr == 0)
				{
				/*	if((oldpad.Buttons & PSP_CTRL_RTRIGGER)
						&& !(currpad.Buttons & PSP_CTRL_RTRIGGER))
					{
						// 前回はRが押されていたのに、
						// Rが離された

						rtrigger_delay = 20;	
					}
					if((oldpad.Buttons & PSP_CTRL_LTRIGGER)
						&& !(currpad.Buttons & PSP_CTRL_LTRIGGER))
					{
						// 前回はLが押されていたのに、
						// Lが離された

						ltrigger_delay = 20;
					}
				*/
					if(currpad.Buttons & PSP_CTRL_RTRIGGER)
					{
						rtrigger_delay++;
					}
					else if(currpad.Buttons & PSP_CTRL_LTRIGGER)
					{
						ltrigger_delay++;
					}

					if(rtrigger_delay > 20)
					{
						trigger_scr = 10;
						rtrigger_delay = 10;
					}
					else if(ltrigger_delay > 20)
					{
						trigger_scr = -10;
						ltrigger_delay = 10;
					}

					// そもそも、L/Rトリガーが押されてない
					if(!(currpad.Buttons & PSP_CTRL_RTRIGGER)
						&& !(currpad.Buttons & PSP_CTRL_LTRIGGER))
					{
						rtrigger_delay = 20;
						ltrigger_delay = 20;
					}

					// 「スティックもトリガーも」押されていない
					if(ltrigger_delay == 20
						&& rtrigger_delay == 20)
					{
						padmask = 0;
						if(!(oldpad.Buttons & PSP_CTRL_CIRCLE)
							&& (currpad.Buttons & PSP_CTRL_CIRCLE))
						{
							padmask |= PSP_CTRL_CIRCLE;
						}
						if(!(oldpad.Buttons & PSP_CTRL_CROSS)
							&& (currpad.Buttons & PSP_CTRL_CROSS))
						{
							padmask |= PSP_CTRL_CROSS;
						}
						if(!(oldpad.Buttons & PSP_CTRL_TRIANGLE)
							&& (currpad.Buttons & PSP_CTRL_TRIANGLE))
						{
							padmask |= PSP_CTRL_TRIANGLE;
						}
						if(!(oldpad.Buttons & PSP_CTRL_SQUARE)
							&& (currpad.Buttons & PSP_CTRL_SQUARE))
						{
							padmask |= PSP_CTRL_SQUARE;
						}
						if(!(oldpad.Buttons & PSP_CTRL_LEFT)
							&& (currpad.Buttons & PSP_CTRL_LEFT))
						{
							padmask |= PSP_CTRL_LEFT;
						}
						if(!(oldpad.Buttons & PSP_CTRL_RIGHT)
							&& (currpad.Buttons & PSP_CTRL_RIGHT))
						{
							padmask |= PSP_CTRL_RIGHT;
						}
						if(!(oldpad.Buttons & PSP_CTRL_UP)
							&& (currpad.Buttons & PSP_CTRL_UP))
						{
							padmask |= PSP_CTRL_UP;
						}
						if(!(oldpad.Buttons & PSP_CTRL_DOWN)
							&& (currpad.Buttons & PSP_CTRL_DOWN))
						{
							padmask |= PSP_CTRL_DOWN;
						}

						if(padmask & PSP_CTRL_UP)
						{
							pad_scr = -1;
						}
						else if(padmask & PSP_CTRL_DOWN)
						{
							pad_scr = 1;
						}
					}
				}

				// 結局、カーソル位置が変更されたかどうか
				if(stick_scr == 0 && trigger_scr == 0 && pad_scr == 0)
				{
					index_changed = false;
				}
				else
				{
					index_changed = true;

					// カーソル位置を調整
					int move_scr = stick_scr + trigger_scr + pad_scr;
					if(move_scr < 0)
					{
						if(_index < (unsigned int)abs(move_scr))
						{
							_index = 0;
						}
						else
						{
							_index += move_scr;
						}
					}
					else 
					{	
						if(_index+move_scr > cursor_max)
						{
							_index = cursor_max;
						}
						else
						{
							_index += move_scr;
						}
					}

				}
			}
		}
	}
	else
	{
		// 両方NULL
		index_changed = true;
	}

	// 表示用GUTextBoxに反映
	if(index_changed)
	{
		GUListItem *ptr;
		for(p=0; p<_column_num; p++)
		{
			ptr = this->columnlist[p].getItem(_index);

			for(q=0; q<_disp_num; q++)
			{
				if(ptr == NULL)
				{
					for(i=q; i<_disp_num; i++)
					{
						dispitemlist[i][p].setValuePtr(NULL);
						dispitemlist[i][p].setValue("");
					}
					break;
				}
				dispitemlist[q][p].setValuePtr(ptr->getValue());
				ptr = ptr->next;
			}
		}
	}

	// 条件に関係無くパッドのデータを保存
	if(padData != NULL)
	{
		oldpad = currpad;
	}
}

void GUListView::Draw(float x, float y)
{
	float disp_x, disp_y;
	unsigned int i, p, q;

	disp_x = x;
	disp_y = y;

	for(i=0; i<_column_num; i++)
	{
		if(this->getColumn(i)->isValid())
		{
			this->dispcolumnlist[i].Draw(disp_x, disp_y, this->getColumn(i)->width);
			disp_x += this->getColumn(i)->width;
		}
	}
	disp_x = x;
	disp_y += GUTextBox::GUTEXTBOX_HEIGHT;


	// 表示用テキストボックスの描画
	for(p=0; p<_column_num; p++)
	{
		if(this->columnlist[p].isValid())
		{
			for(q=0; q<_disp_num; q++)
			{
				this->dispitemlist[q][p].Draw(
					disp_x, disp_y,
					this->columnlist[p].width
					);
				disp_y += GUTextBox::GUTEXTBOX_HEIGHT;
			}
			disp_x += this->columnlist[p].width;
			disp_y = y + GUTextBox::GUTEXTBOX_HEIGHT;
		}
	}

}

