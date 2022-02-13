/***************************************************************************
                          sq_imagebasket.h  -  description
                             -------------------
    begin                : ??? Feb 24 2007
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

#ifndef SQ_IMAGEBASKET_H
#define SQ_IMAGEBASKET_H

#include <kdiroperator.h>

class SQ_Dir;

/**
  *@author Baryshev Dmitry
  */

class SQ_ImageBasket : public KDirOperator
{
    Q_OBJECT

    public:
        SQ_ImageBasket(QWidget *parent = 0, const char *name = 0);
	~SQ_ImageBasket();

        void add(const KFileItemList &);

        KFileItemList realItems() const;

        void saveConfig();

        static SQ_ImageBasket* instance() { return m_inst; }

        private slots:
            // coming from KDirOperator
            void insertNewFiles(const KFileItemList &);
            void slotDropped(const KFileItem *, QDropEvent*, const KURL::List&);
            void slotBasketProperties();
            void slotSync();
            void slotViewChanged(KFileView *);
            void slotExecuted(const KFileItem *fi);

            void activatedMenu(const KFileItem *, const QPoint &pos);

        private:
            SQ_Dir *dir;
            static SQ_ImageBasket *m_inst;
};

#endif
