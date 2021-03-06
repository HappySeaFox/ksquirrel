/***************************************************************************
                          sq_errorstring.h  -  description
                             -------------------
    begin                : ??? ??? 26 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_ERRORSTRING_H
#define SQ_ERRORSTRING_H

#include <qmap.h>
#include <qobject.h>

#include <klocale.h>

#define SQE_NOTFINISHED 10000

/*
 *  Class SQ_ErrorString will return translated string representation of
 *  error code.
 */

class SQ_ErrorString : public QObject
{
    public: 
        SQ_ErrorString(QObject *parent = 0);
        ~SQ_ErrorString();

        /*
         *  Get string representation of error.
         */
        QString string(const int code);

        /*
         *  string() + "\n"
         */
        QString stringSN(const int code);

        static SQ_ErrorString* instance() { return m_instance; }

    private:
        QMap<int, QString> messages;
        static SQ_ErrorString *m_instance;
};

#endif
