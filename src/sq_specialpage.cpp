/***************************************************************************
                          sq_specialpage.cpp  -  description
                             -------------------
    begin                : ??? ??? 12 2004
    copyright            : (C) 2004 by ckult
    email                : 
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
#include <klocale.h>

#include <qlayout.h>

#include "sq_specialpage.h"
#include "ksquirrel.h"

SQ_SpecialPage::SQ_SpecialPage(int type, QWidget * parent, const char* name) : QWidget(parent, name), Type(type)
{
	QVBoxLayout *l = new QVBoxLayout(this);
	
	factory = KLibLoader::self()->factory("libkonsolepart");
	part = 0L;

	if (factory)
	{
		part = static_cast<KParts::ReadOnlyPart*>(factory->create(this, "libkonsolepart", "KParts::ReadOnlyPart"));

		if (part)
		{
			KGlobal::locale()->insertCatalogue("konsole");
			l->addWidget(part->widget());
			part->widget()->show();
		}
	}
}

int SQ_SpecialPage::getType() const
{
	return Type;
}

SQ_SpecialPage::~SQ_SpecialPage()
{}

QWidget* SQ_SpecialPage::getWidget() const
{
	return part->widget();
}
