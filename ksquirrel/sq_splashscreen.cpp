/*
 *  (C) 2006 Baryshev Dmitry, KSquirrel project.
 *  Originally based on kstartuplogo.cpp from KDEvelop project
 */

/***************************************************************************
                           kstartuplogo.cpp  -  description
                             -------------------
    artwork              : KDevelop Project / Ralf Nolden <nolden@kde.org>
    begin                : Mon Oct 1 2001
    copyright            : (C) 2001 by Kai Heitkamp
    email                : koncd@kai-heitkamp.de 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307   *
 *   USA.                                                                  *
 *                                                                         *
 *   For license exceptions see LICENSE.EXC file, attached with the source *
 *   code.                                                                 *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kglobalsettings.h>
#include <kstandarddirs.h>
#include <kprogress.h>
#include <kdebug.h>

#include "sq_splashscreen.h"
#include "sq_progress.h"

SQ_SplashScreen * SQ_SplashScreen::m_inst = 0;

SQ_SplashScreen::SQ_SplashScreen(QWidget * parent, const char *name) 
    : QWidget(parent, name,
                        Qt::WStyle_Customize |
                        Qt::WStyle_NoBorder |
                        Qt::WStyle_StaysOnTop |
                        Qt::WDestructiveClose |
                        Qt::WX11BypassWM)
{
    kdDebug() << "+SQ_SplashScreen" << endl;

    m_inst = this;

    QPixmap pix = QPixmap(locate("data", "images/splash.png"));

    setPaletteBackgroundPixmap(pix);

    QRect rc = KGlobalSettings::splashScreenDesktopGeometry();

    move(rc.center().x() - pix.width()/2, rc.center().y() - pix.height()/2);
    setFixedWidth(pix.width());
    setFixedHeight(pix.height());

    pr = new SQ_Progress(this);
    pr->setGeometry(201, 255, 162, 14);
    pr->setShowText(false);
    pr->setColor(QColor(170,100,110));
    pr->setTotalSteps(10);
}

SQ_SplashScreen::~SQ_SplashScreen()
{
    kdDebug() << "-SQ_SplashScreen" << endl;

    m_inst = 0;
}

void SQ_SplashScreen::finish()
{
    if(SQ_SplashScreen::instance())
    {
        SQ_SplashScreen::instance()->progress()->flush();
        SQ_SplashScreen::instance()->close();
    }
}

void SQ_SplashScreen::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    hide();
}

void SQ_SplashScreen::advance()
{
    if(SQ_SplashScreen::instance())
        SQ_SplashScreen::instance()->progress()->advance();
}
