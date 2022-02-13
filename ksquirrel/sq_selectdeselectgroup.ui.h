/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
void SQ_SelectDeselectGroup::init()
{
    comboMask->insertStringList(SQ_Config::instance()->readListEntry("Fileview", "selectdeselecthistory"));
}

int SQ_SelectDeselectGroup::exec(QString &mask)
{
    int result = QDialog::exec();

    if(result == QDialog::Accepted)
    {
	QStringList list;

	for(int i = 0;i < comboMask->count();i++)
	    list.append(comboMask->text(i));

	SQ_Config::instance()->setGroup("Fileview");
	SQ_Config::instance()->writeEntry("selectdeselecthistory", list);

	mask = comboMask->currentText();
    }

    return mask.isEmpty() ? QDialog::Rejected : result;
}
