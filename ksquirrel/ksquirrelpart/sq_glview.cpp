/***************************************************************************
                          sq_glview.cpp  -  description
                             -------------------
    begin                : Thu Nov 29 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kglobal.h>

#include "sq_glview.h"

SQ_GLView * SQ_GLView::m_inst = 0;

/***************************************/

SQ_TextSetter::SQ_TextSetter(QObject *parent) : QObject(parent)
{
    dest = "---";
}

SQ_TextSetter::~SQ_TextSetter()
{}

void SQ_TextSetter::setText(const QString &s)
{
    dest = s;

    emit changed();
}

/***************************************/

SQ_GLView::SQ_GLView() : QObject()
{
    m_inst = this;

    map.insert("SBDecoded", new SQ_TextSetter(this));
    map.insert("SBFrame",   new SQ_TextSetter(this));
    map.insert("SBLoaded",  new SQ_TextSetter(this));
    map.insert("SBGLZoom",  new SQ_TextSetter(this));
    map.insert("SBGLAngle", new SQ_TextSetter(this));
    map.insert("SBFile",    new SQ_TextSetter(this));

    tmp = new SQ_TextSetter(this);

    SQ_Setters::iterator itEnd = map.end();

    for(SQ_Setters::iterator it = map.begin();it != itEnd;++it)
        connect(it.data(), SIGNAL(changed()), this, SLOT(slotChanged()));
}

SQ_GLView::~SQ_GLView()
{}

SQ_TextSetter* SQ_GLView::sbarWidget(const QString &name)
{
    SQ_Setters::iterator it = map.find(name);

    return (it == map.end() ? tmp : it.data());
}

void SQ_GLView::resetStatusBar()
{
    SQ_Setters::iterator itEnd = map.end();

    for(SQ_Setters::iterator it = map.begin();it != itEnd;++it)
    {
        it.data()->blockSignals(true);
        it.data()->setText("---");
        it.data()->blockSignals(false);
    }

    slotChanged();
}

void SQ_GLView::slotChanged()
{
    QString result;

    static const QString &line = KGlobal::staticQString(" | ");

    result.append(map["SBDecoded"]->text());
    result.append(line);

    result.append(map["SBFrame"]->text());
    result.append(line);

    result.append(map["SBLoaded"]->text());
    result.append(line);

    result.append(map["SBGLZoom"]->text());
    result.append(line);

    result.append(map["SBGLAngle"]->text());
    result.append(line);

    result.append("<b>");
    result.append(map["SBFile"]->text());
    result.append("</b>");

    emit message(result);
}

#include "sq_glview.moc"
