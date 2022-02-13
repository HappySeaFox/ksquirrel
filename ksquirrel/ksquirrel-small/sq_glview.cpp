/***************************************************************************
                          sq_glview.cpp  -  description
                             -------------------
    begin                : ??? ??? 5 2004
    copyright            : (C) 2004 by Baryshev Dmitry
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

#include <qlayout.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qhbox.h>

#include <kaction.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <klocale.h>
#include <kdebug.h>
#include <ksqueezedtextlabel.h>

#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_glwidget_helpers.h"
#include "sq_config.h"

#undef SQ_SECTION

#ifndef SQ_SMALL

#define SQ_SECTION "GL view"

#include "ksquirrel.h"
#include "sq_widgetstack.h"

#else

#define SQ_SECTION "ksquirrel-small"

#include <kurl.h>
#include <kwin.h>

#include "sq_hloptions.h"
#include "sq_libraryhandler.h"
#include "sq_librarylistener.h"
#include "sq_iconloader.h"
#include "sq_errorstring.h"

#endif

SQ_GLView * SQ_GLView::m_instance = NULL;

SQ_GLView::SQ_GLView(QWidget *parent, const char *name) : QVBox(parent, name)
{
    m_instance = this;

    kdDebug() << "+SQ_GLView" << endl;

#ifndef SQ_SMALL

    separate = parent ? false : true;

    createContent();

    installEventFilter(this);

#else

    new SQ_ErrorString(this);
    new SQ_IconLoader(this);

    libl = new SQ_LibraryListener(this);
    libh = new SQ_LibraryHandler(this);

    kconf = new SQ_Config("ksquirrelrc");

    connect(libl, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));

    // SQ_KLIBS must be known constant from
    // ksquirrellibs.pc
    KURL url = SQ_KLIBS;

    createContent();

    // SQ_LibraryListener will find libraries
    // and SQ_LibraryHandler will load them
    libl->slotOpenURL(url, false, true);

#endif

}

SQ_GLView::~SQ_GLView()
{
    kdDebug() << "-SQ_GLView" << endl;

#ifdef SQ_SMALL
    delete kconf;
#endif

}

void SQ_GLView::createContent()
{
    m_toolbar = new SQ_ToolBar(this);

    gl = new SQ_GLWidget(this);
    gl->glInitA();

    setStretchFactor(gl, 1);

    sbar = new KStatusBar(this);

    // create QLabels, fill 'names' with pointers
    QHBox *sqSBDecodedBox = new QHBox;
    sqSBDecodedBox->setSpacing(2);
    QLabel *sqSBDecodedI = new QLabel(QString::null, sqSBDecodedBox, "SBDecodedI");
    names.insert("SBDecodedI", sqSBDecodedI);
    QLabel *sqSBDecoded = new QLabel(QString::null, sqSBDecodedBox, "SBDecoded");
    names.insert("SBDecoded", sqSBDecoded);

    QLabel *sqSBGLZoom = new QLabel(QString::null, 0, "SBGLZoom");
    names.insert("SBGLZoom", sqSBGLZoom);
    QLabel *sqSBGLAngle = new QLabel(QString::null, 0, "SBGLAngle");
    names.insert("SBGLAngle", sqSBGLAngle);
    QLabel *sqSBGLCoord = new QLabel(QString::null, 0, "SBGLCoord");
    names.insert("SBGLCoord", sqSBGLCoord);
    QLabel *sqSBLoaded = new QLabel(QString::null, 0, "SBLoaded");
    names.insert("SBLoaded", sqSBLoaded);
    QLabel *sqSBFrame = new QLabel(QString::null, 0, "SBFrame");
    names.insert("SBFrame", sqSBFrame);
    QLabel *sqSBFile = new KSqueezedTextLabel(QString::null, NULL, "SBFile");
    names.insert("SBFile", sqSBFile);

    sqSBFrame->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter | Qt::ExpandTabs);
    sqSBFile->setAlignment(Qt::AlignRight | Qt::AlignTop);
    QFont font = sqSBFile->font();
    font.setBold(true);
    sqSBFile->setFont(font);

    // fill statusbar
    sbar->addWidget(sqSBDecodedBox, 0, true);
    sbar->addWidget(sqSBFrame, 0, true);
    sbar->addWidget(sqSBLoaded, 0, true);
    sbar->addWidget(sqSBGLZoom, 0, true);
    sbar->addWidget(sqSBGLAngle, 0, true);
    sbar->addWidget(sqSBGLCoord, 0, true);
    sbar->addWidget(sqSBFile, 1, true);

    resetStatusBar();

    SQ_Config::instance()->setGroup(SQ_SECTION);
    sbar->setShown(SQ_Config::instance()->readBoolEntry("statusbar", true));

#ifndef SQ_SMALL

    if(!separate)
        return;

#endif

    // restore geometry from config file
    restoreGeometry();

    gl->matrixChanged();
}

void SQ_GLView::closeEvent(QCloseEvent *e)
{
#ifndef SQ_SMALL

    // ignore event
    e->ignore();

    // let KSquirrel close SQ_GLView, since it can be built-in
    KSquirrel::app()->slotCloseGLWidget();

#else

    // in 'small' version accept close event,
    // and save geometry to config file
    kconf->setGroup(SQ_SECTION);
    saveGeometry();
    kconf->writeEntry("statusbar", dynamic_cast<KToggleAction *>(gl->actionCollection()->action("toggle status"))->isChecked());
    kconf->writeEntry("ignore", dynamic_cast<KToggleAction *>(gl->actionCollection()->action("if less"))->isChecked());
    kconf->writeEntry("zoom type", gl->zoomType());
    kconf->writeEntry("toolbars_hidden", gl->actionsHidden());
    kconf->writeEntry("zoom_nice", gl->isnice());

    kconf->sync();

    delete kconf;

    e->accept();

#endif
}

#ifndef SQ_SMALL

/*
 *  Is this widget separate ?
 */
bool SQ_GLView::isSeparate() const
{
    return separate;
}

/*
 *  Make widget built-in with reparenting it.
 */
void SQ_GLView::reparent(QWidget *parent, const QPoint &p, bool showIt)
{
    // reparent
    QVBox::reparent(parent, p, showIt);

    // store current state
    separate = (parent) ? false : true;
}

bool SQ_GLView::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == this)
    {
        // stop animation, if SQ_GLView is hidden (animation becomes useless)
        if(/*e->type() == QEvent::WindowDeactivate || */e->type() == QEvent::Hide)
        {
            gl->stopAnimation();
            return true;
        }
        // resume animation, if needed
        else if(/*e->type() == QEvent::WindowActivate || */e->type() == QEvent::Show)
        {
            if(!gl->manualBlocked())
                gl->startAnimation();

        return true;
        }
        else // call to default eventFilter()
            return QVBox::eventFilter(watched, e);
    }
    else
        return QVBox::eventFilter(watched, e);
}

// evil stuff, but MOC doesn't understand "#ifdef" in .h files.
// It means we MUST create slots, even if they are not used.
void SQ_GLView::slotContinueLoading()
{}

void SQ_GLView::slotFullScreen(bool)
{}

#else

void SQ_GLView::slotContinueLoading()
{
    // now all libraries are loaded, we can
    // show an image
    show();

    disconnect(libl, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));

    if(!SQ_HLOptions::instance()->path.isEmpty())
    {
        // if it is known image type - let's load it
        if(SQ_LibraryHandler::instance()->supports(SQ_HLOptions::instance()->path))
            gl->slotStartDecoding(SQ_HLOptions::instance()->path, true);
    }
}

/*
 *  Goto fullscreen. If current version is NOT
 *  'small', fullscreen state will be managed by KSquirrel.
 */
void SQ_GLView::slotFullScreen(bool full)
{
    WId id = winId();

    SQ_Config::instance()->setGroup(SQ_SECTION);

    // hide statusbar in fullscreen mode ?
    if(kconf->readBoolEntry("hide_sbar", true))
    {
        sbar->setShown(!full);
        dynamic_cast<KToggleAction *>(gl->actionCollection()->action("toggle status"))->setChecked(!full);
    }

    // hide toolbar in fullscreen mode ?
    if(kconf->readBoolEntry("hide_toolbar", true))
    {
        m_toolbar->setShown(!full);
        dynamic_cast<KToggleAction *>(SQ_GLWidget::window()->actionCollection()->action("toggle toolbar"))->setChecked(full);
    }

    // goto fullscreen
    if(full)
        KWin::setState(id, NET::FullScreen);
    else // leave fullscreen
        KWin::clearState(id, NET::FullScreen);
}

#endif

void SQ_GLView::saveGeometry()
{
    SQ_Config::instance()->writeEntry("pos", pos());
    SQ_Config::instance()->writeEntry("size", size());
}

void SQ_GLView::restoreGeometry()
{
    QPoint p_def(0,0);
    QSize  sz_def(660, 480);

    QPoint p = SQ_Config::instance()->readPointEntry("pos", &p_def);
    QSize sz = SQ_Config::instance()->readSizeEntry("size", &sz_def);

    move(p);
    resize(sz);
}

/*
 *  Reset all statusbar's labels to default values.
 */
void SQ_GLView::resetStatusBar()
{
    QString fl = QString::fromLatin1("----");

    names["SBGLCoord"]->setText(fl);
    names["SBGLZoom"]->setText(fl);
    names["SBGLAngle"]->setText(fl);
    names["SBLoaded"]->setText(fl);
    names["SBDecodedI"]->setText(QString::fromLatin1("--"));
    names["SBFile"]->setText(QString::fromLatin1("--------"));
    names["SBDecoded"]->setText(fl);
    names["SBFrame"]->setText(QString::fromLatin1("0/0"));
}

SQ_ToolBar::SQ_ToolBar(QWidget *parent) : KToolBar(parent)
{
    kdDebug() << "+SQ_ToolBar" << endl;

    setFixedHeight(SQ_ToolButton::fixedWidth() + 5);
    boxLayout()->setSpacing(0);
}

SQ_ToolBar::~SQ_ToolBar()
{
    kdDebug() << "-SQ_ToolBar" << endl;
}

void SQ_ToolBar::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
}
