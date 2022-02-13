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
    id_clear  = insertItem(i18n("Clear contents"), this, SLOT(slotDirectoryClear()));
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
    else if(elem == SQ_TreeViewMenu::Clear)
        id = id_clear;
    else
        id = id_prop;

    disconnectItem(id, 0, 0);
    connectItem(id, receiver, member);
}

void SQ_TreeViewMenu::updateDirActions(bool isdir, bool isroot)
{
    setItemEnabled(id_new, isdir);
    setItemEnabled(id_clear, isdir);

    setItemEnabled(id_delete, !isroot);
    setItemEnabled(id_rename, isdir && !isroot);
}

void SQ_TreeViewMenu::slotDirectoryNew()
{
    if(!m_url.isEmpty())
    {
        bool ok;

        QString mNewFilename = KInputDialog::getText(i18n("Create Subfolder"),
                i18n("<p>Create new folder in <b>%1</b>:</p>").arg(QStyleSheet::escape(m_url.filename())),
                QString::null, &ok, KSquirrel::app());

        if(ok)
        {
            KURL dstURL = m_url;
            dstURL.addPath(mNewFilename);
            KIO::Job *job = KIO::mkdir(dstURL);

            connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotDirectoryResult(KIO::Job *)));
        }
    }
}

void SQ_TreeViewMenu::slotDirectoryRename()
{
    if(!m_url.isEmpty())
    {
        KURL renameSrcURL = m_url;
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
    if(!m_url.isEmpty())
    {
        QString dir = QStyleSheet::escape(m_url.path());

        if(KMessageBox::questionYesNo(KSquirrel::app(),
            "<qt>" + i18n("Are you sure you want to delete <b>%1</b>?").arg(dir) + "</qt>") == KMessageBox::No)
            return;

        KIO::Job *job = KIO::del(m_url);

        connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotDirectoryResult(KIO::Job *)));
    }
}

void SQ_TreeViewMenu::slotDirectoryClear()
{
    if(!m_url.isEmpty())
    {
        urlstodel.clear();

        if(KMessageBox::questionYesNo(KSquirrel::app(),
            "<qt>" + i18n("Are you sure you want to delete contents of <b>%1</b>?").arg(m_url.path()) + "</qt>") == KMessageBox::No)
            return;

        KIO::Job *job = KIO::listDir(m_url, false, true);

        connect(job, SIGNAL(entries(KIO::Job *, const KIO::UDSEntryList &)), this, SLOT(slotEntries(KIO::Job *, const KIO::UDSEntryList &)));
        connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotListResult(KIO::Job *)));
    }
}

void SQ_TreeViewMenu::slotEntries(KIO::Job *, const KIO::UDSEntryList &list)
{
    KIO::UDSEntryListConstIterator itEnd = list.end();
    QString suff;
    KURL u;

    static const QString &dot    = KGlobal::staticQString(".");
    static const QString &dotdot = KGlobal::staticQString("..");

    // go through list of KIO::UDSEntrys
    for(KIO::UDSEntryListConstIterator it = list.begin(); it != itEnd; ++it)
    {
        KIO::UDSEntry entry = *it;
        KIO::UDSEntry::ConstIterator itEnd = entry.end();

        for(KIO::UDSEntry::ConstIterator it = entry.begin(); it != itEnd; ++it)
        {
            if((*it).m_uds == KIO::UDS_NAME)
            {
                suff = (*it).m_str;

                if(suff != dot && suff != dotdot)
                {
                    u = m_url;
                    u.addPath(suff);
                    urlstodel.append(u);
                }

                break;
            }
        }
    }
}

void SQ_TreeViewMenu::slotListResult(KIO::Job *job)
{
    if(!job) return;

    if(job->error())
        job->showErrorDialog(KSquirrel::app());
    else if(!urlstodel.isEmpty())
    {
        KIO::Job *job = KIO::del(urlstodel);
        connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotDirectoryResult(KIO::Job *)));
    }
}

void SQ_TreeViewMenu::slotDirectoryResult(KIO::Job *job)
{
    if(job && job->error())
        job->showErrorDialog(KSquirrel::app());
}

void SQ_TreeViewMenu::slotDirectoryProperties()
{
    if(!m_url.isEmpty())
        (void)new KPropertiesDialog(m_url, KSquirrel::app());
}

#include "sq_treeviewmenu.moc"
