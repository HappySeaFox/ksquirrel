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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qlayout.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qhbox.h>

#include <kaction.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <klocale.h>
#include <ksqueezedtextlabel.h>
#include <ktabbar.h>

#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_glwidget_helpers.h"
#include "sq_config.h"
#include "sq_iconloader.h"

#include "ksquirrel.h"
#include "sq_widgetstack.h"

SQ_GLView * SQ_GLView::m_instance = 0;

SQ_GLView::SQ_GLView(QWidget *parent) : QVBox(parent, "ksquirrel-image-window")
{
    m_instance = this;

    createContent();

    installEventFilter(this);
}

SQ_GLView::~SQ_GLView()
{}

void SQ_GLView::createContent()
{
    box = new QVBox(this);

    m_toolbar = new SQ_ToolBar(box);
    m_toolbar->setMouseTracking(true);

    SQ_Config::instance()->setGroup("GL view");

    // read settings
    m_tabs = SQ_Config::instance()->readBoolEntry("tabs", false);
    m_tabsclose = SQ_Config::instance()->readBoolEntry("tabs_close", true);

    m_tabbar = new KTabBar(box);
    m_tabbar->setHoverCloseButton(m_tabsclose);
    m_tabbar->setHoverCloseButtonDelayed(false);
    m_tabbar->setMouseTracking(true);
    m_tabbar->setFocusPolicy(QWidget::NoFocus);

    box->setStretchFactor(m_toolbar, 1);

    m_tabbar->setShown(false);

    gl = new SQ_GLWidget(this, "ksquirrel-opengl-widget");
    gl->glInitA();
    gl->setFocus();

    connect(m_tabbar, SIGNAL(selected(int)), gl, SLOT(slotChangeTab(int)));
    connect(m_tabbar, SIGNAL(closeRequest(int)), gl, SLOT(slotCloseRequest(int)));
    connect(m_tabbar, SIGNAL(mouseMiddleClick(int)), gl, SLOT(slotCloseRequest(int)));
    connect(gl, SIGNAL(tabCountChanged()), this, SLOT(slotTabCountChanged()));

    setStretchFactor(gl, 1);

    sbar = new KStatusBar(this);
    sbar->setMouseTracking(true);

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
    QLabel *sqSBLoaded = new QLabel(QString::null, 0, "SBLoaded");
    QToolTip::add(sqSBLoaded, i18n("Loading time"));
    names.insert("SBLoaded", sqSBLoaded);
    QLabel *sqSBFrame = new QLabel(QString::null, 0, "SBFrame");
    names.insert("SBFrame", sqSBFrame);
    QLabel *sqSBFile = new KSqueezedTextLabel(QString::null, 0, "SBFile");
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
    sbar->addWidget(sqSBFile, 1, true);

    resetStatusBar();

    sbar->setShown(SQ_Config::instance()->readBoolEntry("statusbar", true));

    // restore geometry from config file
    restoreGeometry();

    gl->matrixChanged();
}

void SQ_GLView::closeEvent(QCloseEvent *e)
{
    // ignore event
    e->ignore();

    // let KSquirrel close SQ_GLView, since it can be built-in
    KSquirrel::app()->closeGLWidget();
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

void SQ_GLView::saveGeometry()
{
    SQ_Config::instance()->writeEntry("pos", pos());
    SQ_Config::instance()->writeEntry("size", size());
}

void SQ_GLView::restoreGeometry()
{
    QPoint p_def(0,0);
    QSize  sz_def(660, 480);

    SQ_Config::instance()->setGroup("GL view");

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
    setFixedHeight(SQ_ToolButton::fixedWidth() + 5);
    boxLayout()->setSpacing(0);
    boxLayout()->setMargin(0);
}

SQ_ToolBar::~SQ_ToolBar()
{}

void SQ_ToolBar::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
}

void SQ_GLView::addPage(const QString &label)
{
    QTab *tab = new QTab(label);

    m_tabbar->addTab(tab);

    SQ_Config::instance()->setGroup("GL view");

    if(m_tabsclose)
        tab->setIconSet(SmallIcon("fileclose"));
}

void SQ_GLView::removePage(int index)
{
    m_tabbar->removeTab(m_tabbar->tabAt(index));
    m_tabbar->setHidden(m_tabbar->count() <= 1);
}

void SQ_GLView::removeTabs()
{
    blockSignals(true);
    int cnt = m_tabbar->count();

    while(cnt--)
        m_tabbar->removeTab(m_tabbar->tabAt(0));

    blockSignals(false);
}

void SQ_GLView::setupTabbar()
{
    SQ_Config::instance()->setGroup("GL view");

    int cnt;

    if(m_tabsclose != SQ_Config::instance()->readBoolEntry("tabs_close", true))
    {
        m_tabsclose = SQ_Config::instance()->readBoolEntry("tabs_close", true);

        // change iconset
        cnt = m_tabbar->count();

        for(int i = 0;i < cnt;i++)
            m_tabbar->tabAt(i)->setIconSet(m_tabsclose ? SmallIcon("fileclose") : QPixmap());

        m_tabbar->setHoverCloseButton(m_tabsclose);
        m_tabbar->layoutTabs();
    }

    if(m_tabs == SQ_Config::instance()->readBoolEntry("tabs", false))
        return;

    blockSignals(true);

    m_tabs = SQ_Config::instance()->readBoolEntry("tabs", false);

    m_tabbar->setHoverCloseButton(m_tabsclose);
    m_tabbar->setShown(m_tabs);

    int id = m_tabbar->currentTab();

    if(id == -1)
    {
        if(m_tabs) m_tabbar->setHidden(m_tabbar->count() <= 1);
        blockSignals(false);
        return;
    }

    int index = m_tabbar->indexOf(id);

    // remove other tabs if needed
    if(!m_tabs)
    {
        int ind = 0;
        cnt = m_tabbar->count();
        cnt--;

        while(cnt--)
        {
            if(m_tabbar->tabAt(ind)->identifier() == id)
            {
                ind++;
                continue;
            }

            m_tabbar->removeTab(m_tabbar->tabAt(ind));
        }
    }
    else
        m_tabbar->setHidden(m_tabbar->count() <= 1);

    blockSignals(false);

    if(!m_tabs)
        gl->removeNonCurrentTabs(index);
}

void SQ_GLView::slotTabCountChanged()
{
    if(m_tabs)
        m_tabbar->setHidden(m_tabbar->count() <= 1);
}

void SQ_GLView::leftTab()
{
    int index = m_tabbar->indexOf(m_tabbar->currentTab());

    // smth went wrong
    if(index < 0) return;

    index--;
    if(index < 0)
        index = m_tabbar->count() - 1;

    if(index < 0) return;

    m_tabbar->setCurrentTab(m_tabbar->tabAt(index));
}

void SQ_GLView::rightTab()
{
    int index = m_tabbar->indexOf(m_tabbar->currentTab());

    // smth went wrong
    if(index < 0) return;

    index++;
    if(index >= m_tabbar->count())
        index = 0;

    m_tabbar->setCurrentTab(m_tabbar->tabAt(index));
}

#include "sq_glview.moc"
