/***************************************************************************
                          sq_treeviewitem.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qpainter.h>

#include "sq_treeviewitem.h"

SQ_TreeViewItem::SQ_TreeViewItem(KFileTreeViewItem *parentItem, KFileItem *fileItem, KFileTreeBranch *parentBranch)
    : KFileTreeViewItem(parentItem, fileItem, parentBranch), m_checked(false), count_files(0), count_dirs(0)
{}

SQ_TreeViewItem::SQ_TreeViewItem(KFileTreeView *parent, KFileItem *fileItem, KFileTreeBranch *parentBranch)
    : KFileTreeViewItem(parent, fileItem, parentBranch), m_checked(false), count_files(0), count_dirs(0)
{}

SQ_TreeViewItem::~SQ_TreeViewItem()
{}

void SQ_TreeViewItem::paintFocus(QPainter *, const QColorGroup &, const QRect &)
{}

void SQ_TreeViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    KListView *klv = static_cast<KListView *>(listView());

    // mark item
    if(column)
    {
        int h = height();
        int w = klv->columnWidth(1);
        const int m = 6;
        const int marg = 2;

        p->fillRect(0,0,w,h,cg.base());

        p->setPen(black);
        p->setBrush(cg.base());
        p->drawRect(marg, marg, w-marg*2, h-marg*2);

        if(m_checked)
            p->fillRect((w-m)/2, (h-m)/2, m, m, cg.highlight());
    }
    else // file name and pixmap
    {
        QColorGroup cc = cg;

        if(m_checked)
        {
            if(klv->viewport()->backgroundMode() == Qt::FixedColor)
                cc.setColor(QColorGroup::Background, cg.highlight());
            else
                cc.setColor(QColorGroup::Base, cg.highlight());
        }
        else if(isAlternate())
        {
            if(klv->viewport()->backgroundMode() == Qt::FixedColor)
                cc.setColor(QColorGroup::Background, klv->alternateBackground());
            else
                cc.setColor(QColorGroup::Base, klv->alternateBackground());
        }

        QListViewItem::paintCell(p, cc, column, width, alignment);
    }
}

void SQ_TreeViewItem::setCount(int c1, int c2, bool use_c1, bool use_c2)
{
    count_files = c1 < 0 ? 0 : c1;
    count_dirs = c2 < 0 ? 0 : c2;

    QString s;

    if(use_c1 && use_c2) // files + dirs: show these two values anyway
        s = QString::fromLatin1(" [%1/%2]").arg(count_files).arg(count_dirs);
    else if(use_c1 && count_files) // files, file count is > 0
        s = QString::fromLatin1(" [%1]").arg(count_files);
    else if(use_c2 && count_dirs) // dirs, dir count is > 0
        s = QString::fromLatin1(" [%1]").arg(count_dirs);

    setText(0, fileItem()->name() + s);
}
