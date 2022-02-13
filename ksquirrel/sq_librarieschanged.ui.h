/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/




void SQ_LibrariesChanged::setLibsInfo( const QStringList &l, bool added )
{
    listNewLibs->clear();
    listNewLibs->insertItem((added)?"New libraries:":"Deleted libraries:");
    listNewLibs->insertStringList(l);
    listNewLibs->setCurrentItem(0);
}
