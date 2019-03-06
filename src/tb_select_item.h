// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifndef TB_SELECT_ITEM_H
#define TB_SELECT_ITEM_H

#include "tb_linklist.h"
#include "tb_list.h"
#include "tb_value.h"

namespace tb {

class TBSelectItemSource;

enum TB_SORT {
	TB_SORT_NONE,		///< No sorting. Items appear in list order.
	TB_SORT_ASCENDING,	///< Ascending sort.
	TB_SORT_DESCENDING	///< Descending sort.
};

/** TBSelectItemViewer is the viewer for items provided by TBSelectItemSource.
	There can be multiple viewers for each source. The viewer will recieve
	callbacks when the source is changed, so it can update itself.
*/
class TBSelectItemViewer : public TBLinkOf<TBSelectItemViewer>
{
public:
	TBSelectItemViewer() : m_source(nullptr) {}
	virtual ~TBSelectItemViewer() {}

	/** Set the source which should provide the items for this viewer.
		This source needs to live longer than this viewer.
		Set nullptr to unset currently set source. */
	void SetSource(TBSelectItemSource *source);
	TBSelectItemSource *GetSource() const { return m_source; }

	/** Called when the source has changed or been unset by calling SetSource. */
	virtual void OnSourceChanged() = 0;

	/** Called when the item at the given index has changed in a way that should
		update the viewer. */
	virtual void OnItemChanged(int index) = 0;

	/** Called when the item at the given index has been added. */
	virtual void OnItemAdded(int index) = 0;

	/** Called when the item at the given index has been removed. */
	virtual void OnItemRemoved(int index) = 0;

	/** Called when all items have been removed. */
	virtual void OnAllItemsRemoved() = 0;
protected:
	TBSelectItemSource *m_source;
};

/** TBSelectItemSource is a item provider interface for list widgets (TBSelectList and
	TBSelectDropdown).

	Instead of feeding all list widgets with all items all the time, the list widgets
	will ask TBSelectItemSource when it needs it. The list widgets may also apply
	filtering so only a subset of all the items are shown.

	CreateItemWidget can be overridden to create any set of widget content for each item.

	This class has no storage of items. If you want an array storage of items,
	use the subclass TBSelectItemSourceList. If you implement your own storage,
	remember to call InvokeItem[Added/...] to notify viewers that they need to update.
*/

class TBSelectItemSource : public TBTypedObject
{
public:
	TBSelectItemSource() : m_sort(TB_SORT_NONE) {}
	virtual ~TBSelectItemSource();
	TBOBJECT_SUBCLASS(TBSelectItemSource, TBTypedObject);

	/** Return true if an item matches the given filter text.
		By default, it returns true if GetItemString contains filter. */
	virtual bool Filter(int index, const TBStr & filter);

	/** Get the string of a item. If a item has more than one string,
		return the one that should be used for inline-find (pressing keys
		in the list will scroll to the item starting with the same letters),
		and for sorting the list. */
	virtual const TBStr & GetItemString(int index) const = 0;

	/** Get the source to be used if this item should open a sub menu. */
	virtual TBSelectItemSource *GetItemSubSource(int /*index*/) { return nullptr; }

	/** Get the skin image to be painted before the text for this item. */
	virtual TBID GetItemImage(int /*index*/) const { return TBID(); }

	/** Get the id of the item. */
	virtual TBID GetItemID(int /*index*/) const { return TBID(); }

	/** Create the item representation widget(s). By default, it will create
		a TBTextField for string-only items, and other types for items that
		also has image or submenu. */
	virtual TBWidget *CreateItemWidget(int index, TBSelectItemViewer *viewer);

	/** Get the number of items */
	virtual int GetNumItems() const = 0;

	/** Get the index of the first item having this id or -1 if not found */
	virtual int FindIDIndex(TBID id) const;

	/** Set sort type. Default is TB_SORT_NONE. */
	void SetSort(TB_SORT sort) { m_sort = sort; }
	TB_SORT GetSort() const { return m_sort; }

	/** Invoke OnItemChanged on all open viewers for this source. */
	void InvokeItemChanged(int index, TBSelectItemViewer *exclude_viewer = nullptr);
	void InvokeItemAdded(int index);
	void InvokeItemRemoved(int index);
	void InvokeAllItemsRemoved();
private:
	friend class TBSelectItemViewer;
	TBLinkListOf<TBSelectItemViewer> m_viewers;
	TB_SORT m_sort;
};

/** TBSelectItemSourceList is a item provider for list widgets (TBSelectList and
	TBSelectDropdown). It stores items of the type specified by the template in an array. */

template<class T>
class TBSelectItemSourceList : public TBSelectItemSource
{
public:
	TBOBJECT_SUBCLASS(TBSelectItemSourceList, TBSelectItemSource);
	TBSelectItemSourceList() {}
	virtual ~TBSelectItemSourceList()					{ DeleteAllItems(); }
	virtual const TBStr & GetItemString(int index) const { return GetItem(index)->str; }
	virtual TBSelectItemSource *GetItemSubSource(int index){ return GetItem(index)->sub_source; }
	virtual TBID GetItemImage(int index) const			{ return GetItem(index)->skin_image; }
	virtual TBID GetItemID(int index) const				{ return GetItem(index)->id; }
	virtual int GetNumItems() const						{ return m_items.GetNumItems(); }
	virtual TBWidget *CreateItemWidget(int index, TBSelectItemViewer *viewer)
	{
		if (TBWidget *widget = TBSelectItemSource::CreateItemWidget(index, viewer))
		{
			T *item = m_items[index];
			widget->SetID(item->id);
			widget->SetStateRaw(item->state);
			return widget;
		}
		return nullptr;
	}

	/** Add a new item at the given index. */
	bool AddItem(T *item, int index)
	{
		if (m_items.Add(item, index))
		{
			InvokeItemAdded(index);
			return true;
		}
		return false;
	}

	/** Add a new item last. */
	bool AddItem(T *item)				{ return AddItem(item, m_items.GetNumItems()); }

	/** Get the item at the given index. */
	T *GetItem(int index)				{ return m_items[index]; }

	/** Get the item at the given index. */
	const T *GetItem(int index) const	{ return m_items[index]; }

	/** Get the item at the given index. */
	T *FindItemByID(TBID id)			{ int index = FindIDIndex(id); return index >= 0 ? m_items[index] : nullptr; }

	/** Get the item at the given index. */
	const T *FindItemByID(TBID id) const { int index = FindIDIndex(id); return index >= 0 ? m_items[index] : nullptr; }

	/** Delete the item at the given index. */
	void DeleteItem(int index)
	{
		if (!m_items.GetNumItems())
			return;
		m_items.Delete(index);
		InvokeItemRemoved(index);
	}

	/** Delete all items. */
	void DeleteAllItems()
	{
		if (!m_items.GetNumItems())
			return;
		m_items.DeleteAll();
		InvokeAllItemsRemoved();
	}
private:
	TBListOf<T> m_items;
};

/** TBGenericStringItem item for TBGenericStringItemSource.
	It has a string and may have a skin image and sub item source. */
class TBGenericStringItem
{
public:
#define def_skin TBIDC("NormalItem")
	TBGenericStringItem(TBStr str) : str(str), id(str) {}
	TBGenericStringItem(TBStr str, TBID id, TBValue val = TBValue(), TBID img_id = def_skin)
		: str(str), id(id), skin_image(img_id), tag(val) {}
    TBGenericStringItem(TBStr str, TBSelectItemSource *sub_source, TBID img_id = def_skin)
		: str(str), id(str), skin_image(img_id), sub_source(sub_source) {}

	// protect the sub_source pointer from copy, since we own it.
	const TBGenericStringItem& operator = (const TBGenericStringItem &other) = delete;
	TBGenericStringItem(const TBGenericStringItem& other) = delete;

	~TBGenericStringItem() { if (sub_source) delete sub_source; }
	void SetSkinImage(const TBID &image) { skin_image = image; }

 public:
	TBStr str;
	TBID id;
	TBID skin_image = def_skin;
	TBSelectItemSource *sub_source = nullptr;
	WIDGET_STATE state = WIDGET_STATE_NONE;

	/** This value is free to use for anything. It's not used internally. */
	TBValue tag;
};

/** Item Separator */
class TBGenericSeparatorItem : public TBGenericStringItem {
public:
	TBGenericSeparatorItem() : TBGenericStringItem("-", nullptr, TBID()) {}
};

/** TBGenericStringItemSource is a item source list providing items of type TBGenericStringItem. */

class TBGenericStringItemSource : public TBSelectItemSourceList<TBGenericStringItem> {
public:
    TBOBJECT_SUBCLASS(TBGenericStringItemSource, TBSelectItemSourceList<TBGenericStringItem>);
};

} // namespace tb

#endif // TB_SELECT_ITEM_H
