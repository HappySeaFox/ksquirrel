/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_Options::slotCreatePageToggle(bool is)
{
	radioButton1->setEnabled(is);
	radioButton2->setEnabled(is);
	radioButton3->setEnabled(is);
	checkBox4->setEnabled(is);
}


void SQ_Options::slotSelectPage(int page)
{
	widgetStack1->raiseWidget(page);
}
