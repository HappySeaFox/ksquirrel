/***************************************************************************
                          sq_progressbox.cpp  -  description
                             -------------------
    begin                : ??? Jan 21 2007
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

#include <qpushbutton.h>

#include "sq_iconloader.h"
#include "sq_progressbox.h"

SQ_ProgressBox::SQ_ProgressBox(QWidget *parent, const char *name) : QHBox(parent, name)
{
//    QToolBar *progressBoxBar = new QToolBar(QString::null, NULL, this);
    buttonStop = new QPushButton(this);
    buttonStop->setFlat(true);
    buttonStop->setPixmap(SQ_IconLoader::instance()->loadIcon("player_stop", KIcon::Desktop, KIcon::SizeSmall));
    connect(buttonStop, SIGNAL(clicked()), parent, SLOT(slotThumbnailUpdateToggle()));

    // create progress bar
    p = new SQ_Progress(this);

    // setup progress bar
    setFrameShape(QFrame::NoFrame);
    setSpacing(0);
    setMargin(0);
    setStretchFactor(p, 1);
    setGeometry(5, 5, 160, 22);

    hide();
}

SQ_ProgressBox::~SQ_ProgressBox()
{}

void SQ_ProgressBox::startButtonPixmap()
{
    buttonStop->setPixmap(SQ_IconLoader::instance()->loadIcon("player_play", KIcon::Desktop, KIcon::SizeSmall));
}

void SQ_ProgressBox::stopButtonPixmap()
{
    buttonStop->setPixmap(SQ_IconLoader::instance()->loadIcon("player_stop", KIcon::Desktop, KIcon::SizeSmall));
}
