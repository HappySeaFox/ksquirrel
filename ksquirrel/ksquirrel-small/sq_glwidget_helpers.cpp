/***************************************************************************
                          sq_glwidget_helpers.cpp  -  description
                             -------------------
    begin                : Пнд Ноя 7 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#include <qpainter.h>

#include "sq_glwidget_helpers.h"

int SQ_ToolButtonPopup::id = 10;

SQ_ToolButtonPopup::SQ_ToolButtonPopup(const QPixmap &pix, const QString &textLabel, QWidget *parent)
    : KToolBarButton(pix, ++SQ_ToolButtonPopup::id, parent, 0, textLabel)
{
    setFixedWidth(SQ_ToolButton::fixedWidth());
}

SQ_ToolButtonPopup::~SQ_ToolButtonPopup()
{}

SQ_ToolButton::SQ_ToolButton(const QIconSet &iconSet, const QString &textLabel,
    QObject *receiver, const char *slot, QToolBar *parent, const char *name)
    : QToolButton(iconSet, textLabel, QString::null, receiver, slot, parent, name)
{
    setFixedWidth(SQ_ToolButton::fixedWidth());
}

SQ_ToolButton::~SQ_ToolButton()
{}

int SQ_ToolButton::fixedWidth()
{
    return 28;
}

SQ_ToolButtonPage::SQ_ToolButtonPage(QObject *receiver, const char *slot, QToolBar *parent, const char *name)
    : SQ_ToolButton(QIconSet(), QString::null, receiver, slot, parent, name), val(0)
{
    setPixmap(generatePixmap());
}

SQ_ToolButtonPage::~SQ_ToolButtonPage()
{}

void SQ_ToolButtonPage::advanceValue()
{
    val++;

    setPixmap(generatePixmap());
}

void SQ_ToolButtonPage::flush()
{
    val = 0;

    setPixmap(generatePixmap());
}

QPixmap SQ_ToolButtonPage::generatePixmap()
{
    QPixmap pix(SQ_ToolButton::fixedWidth(), SQ_ToolButton::fixedWidth());

    pix.fill(Qt::white);

    QPainter p(&pix);

    p.setPen(Qt::black);
    QFont f = font();
    f.setBold(true);
    f.setPointSize(8);
    p.setFont(f);

    p.drawText(0, 0, pix.width(), pix.height(), Qt::AlignCenter, QString::number(val));

    return pix;
}
