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

#include <kglobalsettings.h>
#include <kstandarddirs.h>
#include <kprogress.h>
#include <kdebug.h>

#include "sq_splashscreen.h"

SQ_SplashScreen::SQ_SplashScreen(QWidget * parent, const char *name) 
    : QWidget(parent, name, WStyle_NoBorder | WStyle_Customize | WDestructiveClose)
{
    kdDebug() << "+SQ_SplashScreen" << endl;

    setAutoMask(true);

    QPixmap pix;
    pix.load(locate("appdata", "images/splash_mask.png"));
    setPaletteBackgroundPixmap(pix);

    QRect rc = KGlobalSettings::splashScreenDesktopGeometry();

    setGeometry(rc.center().x() - pix.width()/2, rc.center().y() - pix.height()/2, pix.width(), pix.height());

    bm = *pix.mask();

    pr = new KProgress(23, this);
    pr->setGeometry(20, pix.height()-40, pix.width()-40, 12);
    pr->setTextEnabled(false);
}

SQ_SplashScreen::~SQ_SplashScreen()
{
    kdDebug() << "-SQ_SplashScreen" << endl;
}

void SQ_SplashScreen::mousePressEvent(QMouseEvent *)
{
    hide();
}

void SQ_SplashScreen::updateMask()
{
    if(bm.isNull())
        return;

    setMask(bm);
}

void SQ_SplashScreen::advance()
{
    pr->advance(1);
}

#include "sq_splashscreen.moc"
