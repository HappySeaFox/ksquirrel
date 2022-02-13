/***************************************************************************
                          sq_ksquirrelpart.h  -  description
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

#ifndef KSQUIRRELPART_H
#define KSQUIRRELPART_H

#include <kparts/part.h>

class KAboutData;
class KToggleAction;
class KSelectAction;

class SQ_GLWidget;

class KSquirrelPart;

class KSquirrelPartBrowserExtension: public KParts::BrowserExtension
{
    Q_OBJECT

    public:
        KSquirrelPartBrowserExtension(KSquirrelPart* viewPart, const char *name = 0);
        ~KSquirrelPartBrowserExtension();

    public slots:
        void print();

    private:
        KSquirrelPart *mPart;
};

/***********************************************************/

class KSquirrelPart : public KParts::ReadOnlyPart
{
	Q_OBJECT

    public:
	KSquirrelPart(QWidget*, const char*, QObject*, const char*, const QStringList &);
	virtual ~KSquirrelPart();

	static KAboutData* createAboutData();

	QString filePath();

	void print();

    protected:
       void partActivateEvent(KParts::PartActivateEvent *);

    protected slots:
        virtual bool openFile();
	virtual void setKonquerorWindowCaption(const KURL &url, const QString &filename);

    private slots:
        void slotSelectionRect(bool);
        void slotSelectionEllipse(bool);
        void slotZoom();

    private:
        SQ_GLWidget *gl;
        KToggleAction *ar, *ae;
        KSelectAction *sa;
        KSquirrelPartBrowserExtension *ext;
};

inline
QString KSquirrelPart::filePath()
{
    return m_file;
}

#endif
