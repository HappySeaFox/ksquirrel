/***************************************************************************
                          sq_filedialog.h  -  description
                             -------------------
    begin                : Mon Mar 5 2007
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

#ifndef SQ_FILEDIALOG_H
#define SQ_FILEDIALOG_H

#include <kfiledialog.h>

/**
  *@author Baryshev Dmitry
  */

class SQ_FileDialog : public KFileDialog
{
    public:
        SQ_FileDialog(const QString &path, QWidget *parent);
        ~SQ_FileDialog();

        QString nameFilter() const;
        void updateCombo(bool enable);

        void setCurrentFilter(const QString &);
};

#endif
