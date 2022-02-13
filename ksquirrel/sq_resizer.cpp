/***************************************************************************
                          sq_resizer.cpp  -  description
                             -------------------
    begin                : ??? Mar 3 2005
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

#include <kprogress.h>
#include <klocale.h>

#include "ksquirrel.h"
#include "sq_resizer.h"
#include "sq_resizedialog.h"

SQ_Resizer * SQ_Resizer::rsz = 0L;

SQ_Resizer::SQ_Resizer() : SQ_EditBase()
{
	rsz = this;
}

SQ_Resizer::~SQ_Resizer()
{}

void SQ_Resizer::startEditPrivate()
{
	res = new SQ_ResizeDialog(KSquirrel::app());
	res->setCaption(i18n("Resize 1 file", "Resize %n files", files.count()));

	connect(res, SIGNAL(_resize()), this, SLOT(slotStartResize()));
	connect(this, SIGNAL(convertText(const QString &, bool)), res, SLOT(slotDebugText(const QString &, bool)));
	connect(this, SIGNAL(oneFileProcessed()), res, SLOT(slotOneProcessed()));

	res->progress->setProgress(0);
	res->progress->setTotalSteps(files.count());

	res->exec();
}

void SQ_Resizer::slotStartResize()
{

}

SQ_Resizer* SQ_Resizer::instance()
{
	return rsz;
}
