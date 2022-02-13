#include <qpoint.h>
#include <qwidget.h>
#include <qstylesheet.h>

#include <klocale.h>
#include <kurl.h>
#include <kio/job.h>
#include <kpropertiesdialog.h>
#include <kinputdialog.h>
#include <kmessagebox.h>

#include "sq_treeviewmenu.h"
#include "ksquirrel.h"
#include "sq_iconloader.h"

SQ_TreeViewMenu::SQ_TreeViewMenu(QWidget *parent, const char *name) : KPopupMenu(parent, name)
{
    id_new = insertItem(SQ_IconLoader::instance()->loadIcon("folder_new", KIcon::Desktop, KIcon::SizeSmall), i18n("New folder..."), this, SLOT(slotDirectoryNew()));
    insertSeparator();
    id_rename = insertItem(i18n("Rename"), this, SLOT(slotDirectoryRename()));
    id_delete = insertItem(SQ_IconLoader::instance()->loadIcon("editdelete", KIcon::Desktop, KIcon::SizeSmall), i18n("Delete"), this, SLOT(slotDirectoryDelete()));
    insertSeparator();
    id_prop = insertItem(i18n("Properties"), this, SLOT(slotDirectoryProperties()));
}

SQ_TreeViewMenu::~SQ_TreeViewMenu()
{}

void SQ_TreeViewMenu::reconnect(Element elem, QObject *receiver, const char *member)
{
    int id;

    if(elem == SQ_TreeViewMenu::New)
        id = id_new;
    else if(elem == SQ_TreeViewMenu::Delete)
        id = id_delete;
    else if(elem == SQ_TreeViewMenu::Rename)
        id = id_rename;
    else
        id = id_prop;

    disconnectItem(id, 0, 0);
    connectItem(id, receiver, member);
}

void SQ_TreeViewMenu::updateDirActions(bool isdir)
{
    setItemEnabled(id_rename, isdir);
}

void SQ_TreeViewMenu::slotDirectoryNew()
{
    if(!url.isEmpty())
    {
        bool ok;

        QString mNewFilename = KInputDialog::getText(i18n("Create Subfolder"),
                i18n("<p>Create new folder in <b>%1</b>:</p>").arg(QStyleSheet::escape(url.filename())),
                QString::null, &ok, KSquirrel::app());

        if(ok)
        {
            KURL dstURL = url;
            dstURL.addPath(mNewFilename);
            KIO::Job *job = KIO::mkdir(dstURL);

            connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotDirectoryResult(KIO::Job *)));
        }
    }
}

void SQ_TreeViewMenu::slotDirectoryRename()
{
    if(!url.isEmpty())
    {
        KURL renameSrcURL = url;
        bool ok;

        QString filename = QStyleSheet::escape(renameSrcURL.filename());

        QString mNewFilename = KInputDialog::getText(i18n("Rename Folder"),
                i18n("<p>Rename folder <b>%1</b> to:</p>").arg(filename),
                renameSrcURL.filename(), &ok, KSquirrel::app());

        if(ok)
        {
            KURL renameDstURL = renameSrcURL;
            renameDstURL.setFileName(mNewFilename);

            KIO::Job *job = KIO::rename(renameSrcURL, renameDstURL, true);

            connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotDirectoryResult(KIO::Job *)));
        }
    }
}

void SQ_TreeViewMenu::slotDirectoryDelete()
{
    if(!url.isEmpty())
    {
        KIO::Job* job;

        QString dir = QStyleSheet::escape(url.path());

        int response = KMessageBox::questionYesNo(KSquirrel::app(),
                "<qt>" + i18n("Are you sure you want to delete <b>%1</b> ?").arg(dir) + "</qt>");

        if(response == KMessageBox::No)
            return;

        job = KIO::del(url);

        connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotDirectoryResult(KIO::Job *)));
    }
}

void SQ_TreeViewMenu::slotDirectoryResult(KIO::Job *job)
{
    if(job->error())
        job->showErrorDialog(KSquirrel::app());
}

void SQ_TreeViewMenu::slotDirectoryProperties()
{
    if(!url.isEmpty())
        (void)new KPropertiesDialog(url, KSquirrel::app());
}

#include "sq_treeviewmenu.moc"
