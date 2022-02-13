#include "sq_treeview.h"
#include "sq_specialpage.h"
#include "sq_page.h"

#include <qdir.h>

SQ_TreeView::SQ_TreeView(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
   QPixmap homePix = sqLoader->loadIcon("gohome", KIcon::Desktop, 16);
   QPixmap rootPix = sqLoader->loadIcon("hdd_mount", KIcon::Desktop, 16);   

    KFileTreeBranch *root = addBranch(KURL(QDir::rootDirPath()), " /", rootPix);
    KFileTreeBranch *home = addBranch(KURL(QDir().home().absPath()), " Home", homePix);
    addColumn("Name");
    
    setDirOnlyMode(root, true);
    setDirOnlyMode(home, true);
    
    setRootIsDecorated(true);
    setShowSortIndicator(true);

    home->setOpen(true);

    connect(this, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotDoubleClicked(QListViewItem*)));
    connect(this, SIGNAL(spacePressed(QListViewItem*)), SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(returnPressed(QListViewItem*)),  SIGNAL(doubleClicked(QListViewItem*)));
}

SQ_TreeView::~SQ_TreeView()
{}

void SQ_TreeView::slotDoubleClicked(QListViewItem *item)
{
    KFileTreeViewItem *cur = static_cast<KFileTreeViewItem*>(item);
    KURL Curl = cur->url();

    if(sqTabWidget->count() > 0)
	    if(((SQ_SpecialPage*)(sqTabWidget->currentPage()))->getType() != 0xff)
    		((SQ_Page*)(sqTabWidget->currentPage()))->setURL(Curl);
}
