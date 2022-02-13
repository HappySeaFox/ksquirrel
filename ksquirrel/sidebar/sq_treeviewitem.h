/***************************************************************************
                          sq_treeviewitem.h  -  description
                             -------------------
    begin                : Feb 22 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel@tut.by
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_TREEVIEWITEM_H
#define SQ_TREEVIEWITEM_H

#include <kfiletreeviewitem.h>

class KFileTreeBranch;

class SQ_TreeViewItem : public KFileTreeViewItem
{
    public:
        SQ_TreeViewItem(KFileTreeViewItem *parentItem, KFileItem *fileItem, KFileTreeBranch *parentBranch);
        SQ_TreeViewItem(KFileTreeView *parent, KFileItem *fileItem, KFileTreeBranch *parentBranch);
        ~SQ_TreeViewItem();

        bool checked() const;
        void setChecked(bool c);

        int files() const;
        int dirs() const;

        void setCount(int c1, int c2, bool use_c1, bool use_c2);

        virtual void paintFocus(QPainter *, const QColorGroup &, const QRect &);

    protected:
        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);

    private:
        bool m_checked;
        int count_files, count_dirs;
};

inline
bool SQ_TreeViewItem::checked() const
{
    return m_checked;
}

inline
void SQ_TreeViewItem::setChecked(bool c)
{
    m_checked = c;
    repaint();
}

inline
int SQ_TreeViewItem::files() const
{
    return count_files;
}

inline
int SQ_TreeViewItem::dirs() const
{
    return count_dirs;
}

#endif
