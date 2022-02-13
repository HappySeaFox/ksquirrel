/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/*
 *  SQ_LibrariesChanged shows information on found/deleted libraries.
 */

void SQ_LibrariesChanged::setLibsInfo(const QStringList &l, bool added)
{
    textLabel1->setText(added ? i18n("New libraries found:") : i18n("Deleted libraries:"));
    listNewLibs->insertStringList(l);
    listNewLibs->setCurrentItem(0);
}
