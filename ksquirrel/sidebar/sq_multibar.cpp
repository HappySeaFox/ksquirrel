/***************************************************************************
                          sq_multibar.cpp  -  description
                             -------------------
    begin                : ??? Nov 28 2005
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

#include <qwidgetstack.h>
#include <qsignalmapper.h>

#include <kmultitabbar.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "sq_multibar.h"
#include "sq_config.h"

SQ_MultiBar::SQ_MultiBar(QWidget *parent, const char *name) : QHBox(parent, name)
{
    m_id = 0;
    m_selected = -1;

    SQ_Config::instance()->setGroup("Interface");
    m_width = SQ_Config::instance()->readNumEntry("splitter", 220);

    kdDebug() << "+SQ_MultiBar" << endl;

    mapper = new QSignalMapper(this);

    connect(mapper, SIGNAL(mapped(int)), this, SLOT(raiseWidget(int)));

    mt = new KMultiTabBar(KMultiTabBar::Vertical, this);

    // setup multibar: style = VSNET, show text labels on the left side
    mt->setStyle(KMultiTabBar::VSNET);
    mt->setPosition(KMultiTabBar::Left);
    mt->showActiveTabTexts(true);

    setSpacing(0);

    // QWigdetStack will contain all widgets
    stack = new QWidgetStack(this);
}

SQ_MultiBar::~SQ_MultiBar()
{
    kdDebug() << "-SQ_MultiBar" << endl;
}

void SQ_MultiBar::addWidget(QWidget *new_w, const QString &text, const QString &icon)
{
    // add widget to stack
    stack->addWidget(new_w, m_id);

    // add button
    mt->appendTab(KGlobal::iconLoader()->loadIcon(icon, KIcon::Desktop, 22), m_id, text);

    // since we cann't determine which tab was clicked,
    // we should use QSignalMapper to determine it.
    mapper->setMapping(mt->tab(m_id), m_id);

    connect(mt->tab(m_id), SIGNAL(clicked()), mapper, SLOT(map()));

    m_id++;
}

void SQ_MultiBar::raiseWidget(int id)
{
    if(m_selected != -1)
        mt->setTab(m_selected, false);

    if(mt->isTabRaised(id))
    {
        if(m_selected != -1)
            m_width = width();

        m_selected = id;

        setMinimumSize(QSize(0, 0));
        setMaximumSize(QSize(10000, 10000));
        setFixedWidth(m_width);
        stack->raiseWidget(id);
        stack->show();
    }
    else
    {
        m_selected = -1;
        m_width = width();
        stack->hide();
        setFixedWidth(mt->width());
    }
}

void SQ_MultiBar::updateLayout()
{
    setFixedWidth(mt->width());
    stack->hide();
}

void SQ_MultiBar::saveConfig()
{
    if(m_selected != -1)
        m_width = width();

    SQ_Config::instance()->setGroup("Interface");
    SQ_Config::instance()->writeEntry("splitter", m_width);
}

void SQ_MultiBar::updateWidth(const int w)
{
    if(m_selected != -1 && w > mt->width())
        setFixedWidth(w);
}

#include "sq_multibar.moc"
