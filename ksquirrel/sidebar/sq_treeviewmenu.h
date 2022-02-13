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
        ~SQ_TreeViewMenu();

        enum Element { New, Rename, Delete, Properties };

        void setURL(const KURL &_url);

        void reconnect(Element, QObject *receiver, const char *member);
        void updateDirActions(bool isdir);

    private slots:
        void slotDirectoryNew();
        void slotDirectoryRename();
        void slotDirectoryDelete();
        void slotDirectoryProperties();
        void slotDirectoryResult(KIO::Job *job);

    private:
        KURL url;
        int id_new, id_rename, id_delete, id_prop;
};

inline
void SQ_TreeViewMenu::setURL(const KURL &_url)
{
    url = _url;
}

#endif
