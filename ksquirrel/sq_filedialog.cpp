/***************************************************************************
                          sq_filedialog.cpp  -  description
                             -------------------
    begin                : Mon Mar 5 2007
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

#include <kfilefiltercombo.h>

#include "sq_filedialog.h"

SQ_FileDialog::SQ_FileDialog(const QString &path, QWidget *parent)
    : KFileDialog(path, QString::null, parent, "select_a_file", true)
{}

SQ_FileDialog::~SQ_FileDialog()
{}

void SQ_FileDialog::updateCombo(bool enable)
{
    filterWidget->setEditable(enable);
}

QString SQ_FileDialog::nameFilter() const
{
    return filterWidget->currentText();
}
