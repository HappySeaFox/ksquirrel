/*
	KSQuirrel - image viewer for KDE with OpenGL support
*/
/*
Gwenview - A simple image viewer for KDE
Copyright 2000-2003 Aur?lien G?teau

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "sq_thumbnailsize.h"

SQ_ThumbnailSize::SQ_ThumbnailSize(Size value) : mValue(value)
{}

SQ_ThumbnailSize::SQ_ThumbnailSize(const QString& str)
{
	QString low = str.lower();

	if(low == "small")
		mValue = Small;
	else if(low == "medium")
		mValue = Medium;
	else if(low == "large")
		mValue = Large;
	else
		mValue = Huge;
}

void SQ_ThumbnailSize::setPixelSize(const QString &size)
{
	QString low = size.lower();

	if(low == "small")
		mValue = Small;
	else if(low == "medium")
		mValue = Medium;
	else if(low == "large")
		mValue = Large;
	else
		mValue = Huge;
}

int SQ_ThumbnailSize::pixelSize() const
{
	switch (mValue)
	{
		case Small:
			return 48;
		case Medium:
			return 64;
		case Large:
			return 96;
		default:
			return 128;
	}
}

SQ_ThumbnailSize::operator const QString&() const
{
	static QString sizeStr[4] = {"small","medium","large","huge"};

	return sizeStr[int(mValue)];
}

int SQ_ThumbnailSize::value() const
{
	return (int)mValue;
}
