/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SQ_HAVE_KREGEXP
#include <kregexpeditorinterface.h>
#include <kparts/componentfactory.h>
#include <ktrader.h>
#endif

void SQ_SelectDeselectGroup::init()
{
    SQ_Config::instance()->setGroup("Fileview");
    comboMask->insertStringList(SQ_Config::instance()->readListEntry("selectdeselecthistory"));

#ifdef SQ_HAVE_KREGEXP
    // not installed
    if(KTrader::self()->query("KRegExpEditor/KRegExpEditor").isEmpty())
	pushEdit->setEnabled(false);
#else
    pushEdit->setEnabled(false);
#endif
}

int SQ_SelectDeselectGroup::exec(QString &mask)
{
    int result = QDialog::exec();

    if(result == QDialog::Accepted)
    {
        QStringList list;
        int cc = comboMask->count();

        for(int i = 0;i < cc;i++)
            list.append(comboMask->text(i));

        // save history
        SQ_Config::instance()->setGroup("Fileview");
        SQ_Config::instance()->writeEntry("selectdeselecthistory", list);

        // save mask
        mask = comboMask->currentText();
    }

    return mask.isEmpty() ? QDialog::Rejected : result;
}

void SQ_SelectDeselectGroup::slotEdit()
{
#ifdef SQ_HAVE_KREGEXP
    QDialog *editorDialog = KParts::ComponentFactory::createInstanceFromQuery<QDialog>("KRegExpEditor/KRegExpEditor");

    if(editorDialog)
    {
	KRegExpEditorInterface *editor = static_cast<KRegExpEditorInterface *>(editorDialog->qt_cast("KRegExpEditorInterface"));

	Q_ASSERT(editor);
	editor->setRegExp(comboMask->currentText());

	if(editorDialog->exec())
        {
	    comboMask->insertItem(editor->regExp());
            comboMask->setCurrentText(editor->regExp());
        }
    }
#endif
}
