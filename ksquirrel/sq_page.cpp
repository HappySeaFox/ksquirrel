#include "sq_page.h"

#include <kiconloader.h>
#include <kaction.h>

#include <qlabel.h>

#include "sq_fileiconview.h"
#include "sq_filedetailview.h"

SQ_Page::SQ_Page(QWidget *parent, KURL path, KFile::FileView View, int type) : QWidget(parent), Type(type)
{
	pPageLayout = new QVBoxLayout (this);
	menuBookmarksID = 8000;

	folderPix = sqLoader->loadIcon("folder", KIcon::Desktop, 16);

	sqConfig->setGroup("file browser");

	pDirOperator = new SQ_DirOperator(path, this, "file view");
	pDirOperator->readConfig(sqConfig, "file browser");
	pDirOperator->setViewConfig(sqConfig, "file browser");
	pDirOperator->setMode(KFile::Files);
	pDirOperator->setView(View);

	if(View != KFile::Detail)
		pDirOperator->view()->actionCollection()->action(Type)->activate();

	pPageToolbar = new QToolBar("", 0, this, true, "tools");
	pPageToolbar->setVerticalStretchable(false);
	pPageToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	pPageToolbar2 = new QToolBar("", 0, this, true, "tools2");
	pPageToolbar2->setVerticalStretchable(false);
	pPageToolbar2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	pTBBack = new QToolButton(pDirOperator->actionCollection()->action("back")->iconSet(KIcon::Desktop, 16), "Go back in history", QString::null, this, SLOT(slotBack()), pPageToolbar);
	pTBForward = new QToolButton(pDirOperator->actionCollection()->action("forward")->iconSet(KIcon::Desktop, 16), "Go forward in history", QString::null, this, SLOT(slotForward()), pPageToolbar);
	pTBUp = new QToolButton(pDirOperator->actionCollection()->action("up")->iconSet(KIcon::Desktop, 16), "Go up level", QString::null, this, SLOT(slotUp()), pPageToolbar);
	pPageToolbar->addSeparator();
	new QToolButton(pDirOperator->actionCollection()->action("reload")->iconSet(KIcon::Desktop, 16), "Reload current directory", QString::null, this, SLOT(slotReload()), pPageToolbar);
	new QToolButton(pDirOperator->actionCollection()->action("home")->iconSet(KIcon::Desktop, 16), "Go your home directory", QString::null, this, SLOT(slotHome()), pPageToolbar);

	pTBBookmarkAdd = new QToolButton(sqLoader->loadIcon("bookmark_add", KIcon::Desktop, 16), "Add current path to bookmarks", QString::null, this, SLOT(slotAddBookmark()), pPageToolbar);
	menuBookmarks = new QPopupMenu(this);
	pTBBookmarkAdd->setPopupDelay(250);
	pTBBookmarkAdd->setPopup(menuBookmarks);
	connect(menuBookmarks, SIGNAL(activated(int)), this, SLOT(slotSetURLfromMenu(int)));

	pPageToolbar->addSeparator();

	new QToolButton(pDirOperator->actionCollection()->action("mkdir")->iconSet(KIcon::Desktop, 16), "Create new directory", QString::null, this, SLOT(slotMkdir()), pPageToolbar);
	new QToolButton(sqLoader->loadIcon("penguin", KIcon::Desktop, 16), "Properties of the selected files", QString::null, this, SLOT(slotProp()), pPageToolbar);
	new QToolButton(pDirOperator->actionCollection()->action("delete")->iconSet(KIcon::Desktop, 16), "Delete selected files", QString::null, this, SLOT(slotDelete()), pPageToolbar);
	pPageToolbar->addSeparator();

	new QLabel(" URL:", pPageToolbar2);
	pCurrentURL = new KHistoryCombo(true, pPageToolbar2, "history combobox");
	pPageToolbar2->setStretchableWidget(pCurrentURL);
	pCurrentURL->setDuplicatesEnabled(false);

	KURLCompletion *pURLCompletion = new KURLCompletion(KURLCompletion::DirCompletion);
	pURLCompletion->setDir("/");

	pCurrentURL->setCompletionObject(pURLCompletion);
	pCurrentURL->setAutoDeleteCompletionObject(true);
	pCurrentURL->setEditText(path.path());
	pCurrentURL->addToHistory(path.path());


///////// insert button "icon size"	////////////////////////

	KActionCollection *actcoll = pDirOperator->actionCollection();
	KRadioAction *i16 = new KRadioAction("16", 0, 0, this, SLOT(slotSetIconSize16()), actcoll, "ic16");
	KRadioAction *i22 = new KRadioAction("22", 0, 0, this, SLOT(slotSetIconSize22()), actcoll, "ic22");
	KRadioAction *i32 = new KRadioAction("32", 0, 0, this, SLOT(slotSetIconSize32()), actcoll, "ic32");
	KRadioAction *i48 = new KRadioAction("48", 0, 0, this, SLOT(slotSetIconSize48()), actcoll, "ic48");
	KRadioAction *i64 = new KRadioAction("64", 0, 0, this, SLOT(slotSetIconSize64()), actcoll, "ic64");
	i16->setExclusiveGroup("set_icon_size_actions");
	i22->setExclusiveGroup("set_icon_size_actions");
	i32->setExclusiveGroup("set_icon_size_actions");
	i48->setExclusiveGroup("set_icon_size_actions");
	i64->setExclusiveGroup("set_icon_size_actions");
	KActionMenu *actionMenu = new KActionMenu(actcoll, "icon_size_menu");
	actionMenu->insert(i16);
	actionMenu->insert(i22);
	actionMenu->insert(i32);
	actionMenu->insert(i48);
	actionMenu->insert(i64);
	actionMenu->plug(pPageToolbar);
	QToolButton *ics = new QToolButton(sqLoader->loadIcon("down", KIcon::Desktop, 16), QString::null, QString::null, this, 0, pPageToolbar);
	ics->setPopup(actionMenu->popupMenu());
	ics->setPopupDelay(10);
	ics->setUsesTextLabel(true);
	ics->setTextLabel(" Icon size", false);
	ics->setTextPosition(QToolButton::Right);
	pPageToolbar->addSeparator();

///////////////////////////////   insert filters   ////////////////////////////////////////////

	pFilterButton = new QToolButton(sqLoader->loadIcon("down", KIcon::Desktop, 16), QString::null, QString::null, this, 0, pPageToolbar);
	pFilterButton->setPopupDelay(10);
	pFilterButton->setUsesTextLabel(true);
	pFilterButton->setTextPosition(QToolButton::Right);
	pPageToolbar->addSeparator();

	QPopupMenu *actionFilterMenu = new QPopupMenu;
	pFilterButton->setPopup(actionFilterMenu);

	sqConfig->setGroup("Filters");
	fl = new QValueList<FILTER>;
	int def = sqConfig->readNumEntry("default", 1);

	for(int i = 1;;i += 2)
	{
		QString ext = QString("%1").arg(i, 0, 10);
		QString name = QString("%1").arg(i+1, 0, 10);
		QString tmp = sqConfig->readEntry(name, "");

		if(tmp == QString(""))
			break;

		FILTER tmp_filter = {tmp, sqConfig->readEntry(ext, "")};
		fl->append(tmp_filter);

		if(def != i)
			actionFilterMenu->insertItem(tmp, 7000+i/2);
		else
			actionFilterMenu->insertItem(sqLoader->loadIcon("ok", KIcon::Desktop, 16), tmp, 7000+i/2);
	}

	connect(actionFilterMenu, SIGNAL(activated(int)), this, SLOT(slotSetFilter(int)));

//////////////////////////////////// insert Sort By //////////////////////////////////////////////////
/*
	QToolButton *sortbyButton = new QToolButton(sqLoader->loadIcon("down", KIcon::Desktop, 16), QString::null, QString::null, this, 0, pPageToolbar2);
	sortbyButton->setPopupDelay(10);
	sortbyButton->setUsesTextLabel(true);
	sortbyButton->setTextLabel(" Sort by", false);
	sortbyButton->setTextPosition(QToolButton::Right);
	sortbyButton->setPopup(((KActionMenu*)(pDirOperator->actionCollection()->action("sorting menu")))->popupMenu());
	pPageToolbar2->addSeparator();
*/
//////////////////////////////////// insert View //////////////////////////////////////////////////
/*
	QToolButton *viewButton = new QToolButton(sqLoader->loadIcon("down", KIcon::Desktop, 16), QString::null, QString::null, this, 0, pPageToolbar2);
	viewButton->setPopupDelay(10);
	viewButton->setUsesTextLabel(true);
	viewButton->setTextLabel(" View", false);
	viewButton->setTextPosition(QToolButton::Right);
	viewButton->setPopup(((KActionMenu*)(pDirOperator->actionCollection()->action("view menu")))->popupMenu());
	pPageToolbar2->addSeparator();
*/
//////////////////////////////////// insert 'Bookmarks' toolbutton //////////////////////////////////////////////////	
	sqConfig->setGroup("Bookmarks");
	QString s, read_;

	if(sqBookmarks->count() > 0)
	{
		for(unsigned i = 0;i < sqBookmarks->count();i++)
			menuBookmarks->insertItem(folderPix, (*sqBookmarks)[i].path(), menuBookmarksID++);
	}
	else for(int i = 1;;i++)
	{
		s.sprintf("%d", i);
		read_ = sqConfig->readEntry(s, "");

		if(read_ == QString(""))
			break;

		if(sqBookmarks->find(KURL(read_)) == sqBookmarks->end())
		{
			sqBookmarks->append(KURL(read_));
                     menuBookmarks->insertItem(folderPix, read_, menuBookmarksID++);
		}
	}


////////////////////////////////////////////////////////////////////////////////////
	QToolButton *levak = new QToolButton(QPixmap(0), QString::null, QString::null, this, 0, pPageToolbar);
	pPageToolbar->setStretchableWidget(levak);
	levak->setEnabled(false);

	new QToolButton(sqLoader->loadIcon("tab_remove", KIcon::Desktop, 16), "Close page", QString::null, this, SLOT(slotCloseTab()), pPageToolbar);

	pPageLayout->addWidget(pPageToolbar);
	pPageLayout->addWidget(pPageToolbar2);
	pPageLayout->addWidget(pDirOperator);
    
	connect(pDirOperator, SIGNAL(urlEntered(const KURL&)), this, SLOT(setURL(const KURL&)));
	connect(pCurrentURL, SIGNAL(returnPressed(const QString&)), this, SLOT(setURL(const QString&)));
	connect(pCurrentURL, SIGNAL(activated(const QString&)), this, SLOT(setURL(const QString&)));
	slotSetFilter(7000);
}

SQ_Page::~SQ_Page()
{}

int SQ_Page::getType() const
{
	return Type;
}

void SQ_Page::slotDoNothing()
{}

void SQ_Page::slotSetFilter(int id)
{
	pDirOperator->setNameFilter(((*fl)[id-7000]).filter);
	pDirOperator->updateDir();
	QString t = QString(" Filter") + " (" + ((*fl)[id-7000]).name + ")";
	pFilterButton->setTextLabel(t, false);
}

void SQ_Page::setURL(const QString &newpath)
{
	KURL url(newpath);
	pDirOperator->setURL(url, false);
}

void SQ_Page::setURL(const KURL &newurl)
{
	pCurrentURL->setEditText(newurl.path());
	pCurrentURL->addToHistory(newurl.path());
	pDirOperator->setURL(newurl, false);
}

void SQ_Page::slotBack()
{
	pDirOperator->actionCollection()->action("back")->activate();
}

void SQ_Page::slotForward()
{
	pDirOperator->actionCollection()->action("forward")->activate();
}

void SQ_Page::slotUp()
{
	pDirOperator->actionCollection()->action("up")->activate();
}

void SQ_Page::slotHome()
{
	pDirOperator->actionCollection()->action("home")->activate();
}

void SQ_Page::slotReload()
{
	pDirOperator->actionCollection()->action("reload")->activate();
}

void SQ_Page::slotMkdir()
{
	pDirOperator->actionCollection()->action("mkdir")->activate();
}

void SQ_Page::slotShowHidden(bool)
{
	pDirOperator->actionCollection()->action("show hidden")->activate();
}

void SQ_Page::slotDelete()
{
	pDirOperator->actionCollection()->action("delete")->activate();
}

void SQ_Page::slotProp()
{
	pDirOperator->actionCollection()->action("properties")->activate();
}

void SQ_Page::slotCloseTab()
{
	sqTabWidget->removePage(this);
}

void SQ_Page::slotAddBookmark()
{
	KURL ur = pDirOperator->url();

	if(sqBookmarks->find(ur) == sqBookmarks->end())
	{
		sqBookmarks->append(ur);
		menuBookmarks->insertItem(folderPix, ur.path(), menuBookmarksID++);
	}
}

void SQ_Page::slotSetIconSize16()
{
	if(pDirOperator->getViewType() == KFile::Simple)
		((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(16);
}

void SQ_Page::slotSetIconSize22()
{
	if(pDirOperator->getViewType() == KFile::Simple)
		((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(22);
}

void SQ_Page::slotSetIconSize32()
{
	if(pDirOperator->getViewType() == KFile::Simple)
		((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(32);
}

void SQ_Page::slotSetIconSize48()
{
	if(pDirOperator->getViewType() == KFile::Simple)
		((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(48);
}

void SQ_Page::slotSetIconSize64()
{
	if(pDirOperator->getViewType() == KFile::Simple)
		((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(64);
}

void SQ_Page::slotSetURLfromMenu(int id)
{
	setURL(menuBookmarks->text(id));
}
