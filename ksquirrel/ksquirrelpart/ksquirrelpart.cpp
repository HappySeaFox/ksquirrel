/***************************************************************************
                          sq_ksquirrelpart.cpp  -  description
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

#include <kaction.h>
#include <kparts/browserextension.h>
#include <kparts/genericfactory.h>
#include <kfileitem.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include "ksquirrelpart.h"
#include "sq_glwidget.h"

#include "sq_glview.h"
#include "sq_config.h"
#include "sq_diroperator.h"
#include "sq_libraryhandler.h"
#include "sq_iconloader.h"
#include "sq_externaltool.h"
#include "sq_errorstring.h"

typedef KParts::GenericFactory<KSquirrelPart> KSquirrelFactory;
K_EXPORT_COMPONENT_FACTORY(libksquirrelpart, KSquirrelFactory)

KSquirrelPart::KSquirrelPart(QWidget *parentWidget, const char *,
                            QObject* parent, const char *name,
                            const QStringList &) : KParts::ReadOnlyPart(parent, name)
{
    kdDebug() << "+KSquirrelPart" << endl;

    setInstance(KSquirrelFactory::instance());

    ext = new KSquirrelPartBrowserExtension(this);

    KGlobal::dirs()->addResourceType("data", KStandardDirs::kde_default("data") + QString::fromLatin1("ksquirrel"));
    KGlobal::locale()->setActiveCatalogue(KSquirrelFactory::instance()->instanceName());

    if(!SQ_ErrorString::instance())
        new SQ_ErrorString(parentWidget);

    if(!SQ_Config::instance())
        new SQ_Config;

    if(!SQ_IconLoader::instance())
        new SQ_IconLoader;

    if(!SQ_ExternalTool::instance())
        new SQ_ExternalTool;

    SQ_ExternalTool::instance()->newPopupMenu();

    // This will allow global object of SQ_LibraryHandler, which
    // will exist until Konqueror will be closed
    if(!SQ_LibraryHandler::instance())
        new SQ_LibraryHandler;

    new SQ_DirOperator;

    gl = new SQ_GLWidget(parentWidget, "ksquirrelpart-opengl-widget");
    gl->glInitA();

    connect(gl, SIGNAL(message(const QString &)), this, SIGNAL(setStatusBarText(const QString&)));
    setWidget(gl);

    KAction *a;

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_Plus));
    new KAction(i18n("Zoom +"), "viewmag+", 0, a,  SLOT(activate()), actionCollection(), "ksquirrelpart zoom in");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_Minus));
    new KAction(i18n("Zoom -"), "viewmag-", 0, a, SLOT(activate()), actionCollection(), "ksquirrelpart zoom out");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_P));
    new KAction(i18n("Properties"), "image",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart properties");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_R));
    new KAction(i18n("Normalize"), "rebuild",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart normalize");

    // colorize & filters
    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_D));
    new KAction(i18n("Color balance..."), "colorize",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart colorbalance");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_U));
    new KAction(i18n("Apply filter..."), "effect",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart filter");

    // rotate
    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_Left+CTRL));
    new KAction(i18n("Rotate left"), "rotate_ccw",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart rotateleft");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_Right+CTRL));
    new KAction(i18n("Rotate right"), "rotate_cw",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart rotateright");

    // copy/move
    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_F5));
    new KAction(i18n("Copy to..."), "editcopy",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart copyto");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_F7));
    new KAction(i18n("Move to..."), "editcut",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart moveto");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_F6));
    new KAction(i18n("Copy to last folder"), "",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart copy");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_F8));
    new KAction(i18n("Move to last folder"), "",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart move");

    a = gl->actionCollection()->action(QString("action_%1").arg(Qt::Key_S));
    new KAction(i18n("Save As..."), "filesaveas",  0, a, SLOT(activate()), actionCollection(), "ksquirrelpart saveas");

    sa = new KSelectAction(i18n("Zoom"), 0, actionCollection(), "ksquirrelpart zoom");
    sa->setEditable(true);

    QStringList zooms;
    zooms << QString::number(10) + '%';
    zooms << QString::number(25) + '%';
    zooms << QString::number(33) + '%';
    zooms << QString::number(50) + '%';
    zooms << QString::number(75) + '%';
    zooms << QString::number(100) + '%';
    zooms << QString::number(150) + '%';
    zooms << QString::number(200) + '%';
    zooms << QString::number(250) + '%';
    zooms << QString::number(300) + '%';
    zooms << QString::number(350) + '%';
    zooms << QString::number(400) + '%';
    zooms << QString::number(450) + '%';
    zooms << QString::number(500) + '%';
    zooms << QString::number(600) + '%';
    zooms << QString::number(700) + '%';
    zooms << QString::number(800) + '%';
    zooms << QString::number(900) + '%';
    zooms << QString::number(1000) + '%';

    sa->setItems(zooms);
    sa->setCurrentItem(5);
    connect(sa, SIGNAL(activated(int)), this, SLOT(slotZoom()));

    QString group = "selection_group";

    ar = new KToggleAction(i18n("Rectangle"), "rectangle", KShortcut(Qt::Key_R+CTRL), 0, 0, actionCollection(), "ksquirrelpart rectangle");
    ar->setExclusiveGroup(group);
    connect(ar, SIGNAL(toggled(bool)), this, SLOT(slotSelectionRect(bool)));

    ae = new KToggleAction(i18n("Ellipse"), "circle", KShortcut(Qt::Key_E+CTRL), 0, 0, actionCollection(), "ksquirrelpart ellipse");
    ae->setExclusiveGroup(group);
    connect(ae, SIGNAL(toggled(bool)), this, SLOT(slotSelectionEllipse(bool)));

    setXMLFile("ksquirrelpart/ksquirrelpart.rc");
}

KSquirrelPart::~KSquirrelPart()
{
    kdDebug() << "-KSquirrelPart" << endl;
}

void KSquirrelPart::slotZoom()
{
    bool ok;

    int z = sa->currentText().replace(QChar('%'), "").toInt(&ok);

    if(ok)
        gl->zoom((float)z / 100.0f);
}

void KSquirrelPart::slotSelectionRect(bool b)
{
    if(b)
        gl->slotSelectionRect();
    else if(!ae->isChecked())
        gl->slotSelectionClear();
}

void KSquirrelPart::slotSelectionEllipse(bool b)
{
    if(b)
        gl->slotSelectionEllipse();
    else if(!ar->isChecked())
        gl->slotSelectionClear();
}

void KSquirrelPart::print()
{
    gl->slotPrint();
}

KAboutData* KSquirrelPart::createAboutData()
{
    KAboutData *kAboutData = new KAboutData(
        "ksquirrel",
        I18N_NOOP("KSquirrelPart"),
	"0.1.2",
        I18N_NOOP("Image Viewer"),
	KAboutData::License_GPL,
	"(c) 2007, Dmitry Baryshev <ksquirrel.iv@gmail.com>");

    return kAboutData;
}

bool KSquirrelPart::openFile()
{
    emit started(0);

    KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, m_url);
    SQ_DirOperator::instance()->execute(&fi);

    emit setWindowCaption(m_url.prettyURL());

    emit completed(0);

    return true;
}

void KSquirrelPart::setKonquerorWindowCaption(const KURL &, const QString &filename)
{
    QString caption = QString("%1").arg(filename);
    emit setWindowCaption(caption);
}

void KSquirrelPart::partActivateEvent(KParts::PartActivateEvent *e)
{
    if(e->activated())
    {
        if(!gl->manualBlocked())
            gl->startAnimation();
    }
    else
    {
        // stop animation when KPart becomes inactive
        gl->stopAnimation();

        SQ_LibraryHandler::instance()->sync();
        SQ_Config::instance()->sync();
    }

    KParts::ReadOnlyPart::partActivateEvent(e);
}

/***************************************************************/

KSquirrelPartBrowserExtension::KSquirrelPartBrowserExtension(KSquirrelPart *viewPart, const char *name)
        :KParts::BrowserExtension(viewPart, name)
{
    mPart = viewPart;
    emit enableAction("print", true);
}

KSquirrelPartBrowserExtension::~KSquirrelPartBrowserExtension()
{}

void KSquirrelPartBrowserExtension::print()
{
    mPart->print();
}

#include "ksquirrelpart.moc"
