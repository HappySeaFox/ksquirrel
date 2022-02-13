#ifndef SQ_TREEVIEWMENU_H
#define SQ_TREEVIEWMENU_H

#include <kpopupmenu.h>
#include <kurl.h>

class QPoint;

namespace KIO { class Job; }

class SQ_TreeViewMenu : public KPopupMenu
{
    Q_OBJECT

    public:
        SQ_TreeViewMenu(QWidget *parent = 0, const char *name = 0);
        virtual ~SQ_TreeViewMenu();

        enum Element { New, Rename, Delete, Properties, Clear };

        void setURL(const KURL &_url);
        KURL url() const;

        void reconnect(Element, QObject *receiver, const char *member);
        virtual void updateDirActions(bool isdir, bool isroot = false);

    protected slots:
        virtual void slotDirectoryNew();
        virtual void slotDirectoryRename();
        virtual void slotDirectoryDelete();
        virtual void slotDirectoryProperties();
        virtual void slotDirectoryResult(KIO::Job *job);

        void slotDirectoryClear();
        void slotEntries(KIO::Job *, const KIO::UDSEntryList &);
        void slotListResult(KIO::Job *);

    protected:
        int id_new, id_rename, id_delete, id_prop, id_clear;

    private:
        KURL m_url;
        KURL::List urlstodel;
};

inline
void SQ_TreeViewMenu::setURL(const KURL &_url)
{
    m_url = _url;
}

inline
KURL SQ_TreeViewMenu::url() const
{
    return m_url;
}

#endif
