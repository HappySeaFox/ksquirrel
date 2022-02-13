#include "sq_page.h"

#include <kiconloader.h>
#include <kaction.h>

#include <qlabel.h>

#include "sq_fileiconview.h"
#include "sq_filedetailview.h"

SQ_Page::SQ_Page(QWidget *parent, PAGESETT *ps)
{
    SQ_Page::SQ_Page(parent, ps->url, ps->View);
}
    
SQ_Page::SQ_Page(QWidget *parent, KURL path, KFile::FileView View, int type) : QWidget(parent), Type(type)
{
	l = new QVBoxLayout (this);
	menuBookmarksID = 8000;

	folderPix = sqLoader->loadIcon("folder", KIcon::Desktop, 16);

	sqConfig->setGroup("file browser");

	dirop = new SQ_DirOperator(path, this, "file view");
	dirop->readConfig(sqConfig, "file browser");
	dirop->setViewConfig(sqConfig, "file browser");
	dirop->setMode(KFile::Files);
	dirop->setView(View);

	if(View != KFile::Detail)
		dirop->view()->actionCollection()->action(Type)->activate();

	toolbar = new QToolBar("", 0, this, true, "tools");
	toolbar->setVerticalStretchable(false);
	toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	toolbar2 = new QToolBar("", 0, this, true, "tools2");
	toolbar2->setVerticalStretchable(false);
	toolbar2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	tbBack = new QToolButton(dirop->actionCollection()->action("back")->iconSet(KIcon::Desktop, 16), "Go back in history", QString::null, this, SLOT(slotBack()), toolbar);
	tbForward = new QToolButton(dirop->actionCollection()->action("forward")->iconSet(KIcon::Desktop, 16), "Go forward in history", QString::null, this, SLOT(slotForward()), toolbar);
	tbUp = new QToolButton(dirop->actionCollection()->action("up")->iconSet(KIcon::Desktop, 16), "Go up level", QString::null, this, SLOT(slotUp()), toolbar);
	toolbar->addSeparator();
	new QToolButton(dirop->actionCollection()->action("reload")->iconSet(KIcon::Desktop, 16), "Reload current directory", QString::null, this, SLOT(slotReload()), toolbar);
	new QToolButton(dirop->actionCollection()->action("home")->iconSet(KIcon::Desktop, 16), "Go your home directory", QString::null, this, SLOT(slotHome()), toolbar);
	toolbar->addSeparator();

	QToolButton *ww = new QToolButton(sqLoader->loadIcon("bookmark_add", KIcon::Desktop, 16), "Add current path to bookmarks", QString::null, this, SLOT(slotAddBookmark()), toolbar);
	menuBookmarks = new QPopupMenu(this);
	ww->setPopupDelay(250);
	ww->setPopup(menuBookmarks);
	connect(menuBookmarks, SIGNAL(activated(int)), this, SLOT(slotSetURLfromMenu(int)));
	toolbar->addSeparator();

	(new QLabel(" URL:", toolbar))->setFrameShape(QFrame::ToolBarPanel);
	curPath = new KURLRequester(path.path(), toolbar, "current url");
	curPath->setFrameShape(QFrame::ToolBarPanel);
	
	
	new QToolButton(dirop->actionCollection()->action("mkdir")->iconSet(KIcon::Desktop, 16), "Create new directory", QString::null, this, SLOT(slotMkdir()), toolbar2);
	new QToolButton(sqLoader->loadIcon("penguin", KIcon::Desktop, 16), "Properties", QString::null, this, SLOT(slotProp()), toolbar2);
	new QToolButton(dirop->actionCollection()->action("delete")->iconSet(KIcon::Desktop, 16), "Delete selected files", QString::null, this, SLOT(slotDelete()), toolbar2);
	toolbar2->addSeparator();

///////// insert button "icon size"	////////////////////////

	KActionCollection *actcoll = dirop->actionCollection();
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
	actionMenu->plug(toolbar);
	QToolButton *ics = new QToolButton(sqLoader->loadIcon("down", KIcon::Desktop, 16), QString::null, QString::null, this, 0, toolbar2);
	ics->setPopup(actionMenu->popupMenu());
	ics->setPopupDelay(10);
	ics->setUsesTextLabel(true);
	ics->setTextLabel(" Icon size", false);
	ics->setTextPosition(QToolButton::Right);
	toolbar2->addSeparator();

///////////////////////////////   insert filters   ////////////////////////////////////////////

	filterButton = new QToolButton(sqLoader->loadIcon("down", KIcon::Desktop, 16), QString::null, QString::null, this, 0, toolbar2);
	filterButton->setPopupDelay(10);
	filterButton->setUsesTextLabel(true);
	filterButton->setTextPosition(QToolButton::Right);
	toolbar2->addSeparator();

	QPopupMenu *actionFilterMenu = new QPopupMenu;
	filterButton->setPopup(actionFilterMenu);

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

	QToolButton *sortbyButton = new QToolButton(sqLoader->loadIcon("down", KIcon::Desktop, 16), QString::null, QString::null, this, 0, toolbar2);
	sortbyButton->setPopupDelay(10);
	sortbyButton->setUsesTextLabel(true);
	sortbyButton->setTextLabel(" Sort by", false);
	sortbyButton->setTextPosition(QToolButton::Right);
	sortbyButton->setPopup(((KActionMenu*)(dirop->actionCollection()->action("sorting menu")))->popupMenu());
	toolbar2->addSeparator();

//////////////////////////////////// insert View //////////////////////////////////////////////////

	QToolButton *viewButton = new QToolButton(sqLoader->loadIcon("down", KIcon::Desktop, 16), QString::null, QString::null, this, 0, toolbar2);
	viewButton->setPopupDelay(10);
	viewButton->setUsesTextLabel(true);
	viewButton->setTextLabel(" View", false);
	viewButton->setTextPosition(QToolButton::Right);
	viewButton->setPopup(((KActionMenu*)(dirop->actionCollection()->action("view menu")))->popupMenu());
	toolbar2->addSeparator();

	sqConfig->setGroup("Bookmarks");
	QString s, read_;

	for(int i = 1;;i++)
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

	toolbar2->setStretchableWidget(new QToolButton(QPixmap(0), QString::null, QString::null, this, 0, toolbar2));
	
////////////////////////////////////////////////////////////////////////////////////
	
	new QToolButton(sqLoader->loadIcon("tab_duplicate", KIcon::Desktop, 16), "Duplicate current page", QString::null, this, SLOT(slotDupTab()), toolbar2);	
	new QToolButton(sqLoader->loadIcon("tab_remove", KIcon::Desktop, 16), "Close page", QString::null, this, SLOT(slotCloseTab()), toolbar2);
    

	l->addWidget(toolbar2);
	l->addWidget(toolbar);
	l->addWidget(dirop);
    
	connect(dirop, SIGNAL(urlEntered(const KURL&)), this, SLOT(setURL(const KURL&)));
	connect(curPath, SIGNAL(returnPressed(const QString&)), this, SLOT(setURL(const QString&)));
	slotSetFilter(7000);
}

SQ_Page::~SQ_Page()
{}

PAGESETT* SQ_Page::getPageSett()
{
    PAGESETT *ps = new PAGESETT;
    
    ps->url = dirop->url();
    ps->View = dirop->getViewType();
    ps->type = getType();
    
    return ps;
} 

int SQ_Page::getType() const
{
	return Type;
}

void SQ_Page::slotDoNothing()
{}

void SQ_Page::slotSetFilter(int id)
{
	dirop->setNameFilter(((*fl)[id-7000]).filter);
	dirop->updateDir();
	QString t = QString(" Filter") + " (" + ((*fl)[id-7000]).name + ")";
	filterButton->setTextLabel(t, false);
}

void SQ_Page::setURL(const QString &newpath)
{
	KURL url(newpath);
	dirop->setURL(url, false);
}

void SQ_Page::setURL(const KURL &newurl)
{
	curPath->setURL(newurl.path());
	dirop->setURL(newurl, false);
}

void SQ_Page::slotBack()
{
	dirop->actionCollection()->action("back")->activate();
}

void SQ_Page::slotForward()
{
	dirop->actionCollection()->action("forward")->activate();
}

void SQ_Page::slotUp()
{
	dirop->actionCollection()->action("up")->activate();
}

void SQ_Page::slotHome()
{
	dirop->actionCollection()->action("home")->activate();
}

void SQ_Page::slotReload()
{
	dirop->actionCollection()->action("reload")->activate();
}

void SQ_Page::slotMkdir()
{
	dirop->actionCollection()->action("mkdir")->activate();
}
	
void SQ_Page::slotShowHidden(bool)
{
	dirop->actionCollection()->action("show hidden")->activate();
}	

void SQ_Page::slotDelete()
{
	dirop->actionCollection()->action("delete")->activate();
}

void SQ_Page::slotProp()
{
	dirop->actionCollection()->action("properties")->activate();
}

void SQ_Page::slotCloseTab()
{
	sqTabWidget->removePage(this);
}

void SQ_Page::slotDupTab()
{
	PAGESETT *ps = new PAGESETT;

	ps = this->getPageSett();

	SQ_Page *n = new SQ_Page (0, ps->url, ps->View, ps->type);

	sqTabWidget->addTab(n, (ps->View == KFile::Detail)?"Detailed view": ((ps->type == 1)?"Icon view":"List view"));

	delete ps;
}

void SQ_Page::slotAddBookmark()
{
	KURL ur = dirop->url();
	
	if(sqBookmarks->find(ur) == sqBookmarks->end())
	{
		sqBookmarks->append(ur);
		menuBookmarks->insertItem(folderPix, ur.path(), menuBookmarksID++);
	}
}

void SQ_Page::slotSetIconSize16()
{
	if(dirop->getViewType() == KFile::Simple)
		((SQ_FileIconView*)dirop->new_view)->setIconSize(16);
}

void SQ_Page::slotSetIconSize22()
{
	if(dirop->getViewType() == KFile::Simple)
		((SQ_FileIconView*)dirop->new_view)->setIconSize(22);
}

void SQ_Page::slotSetIconSize32()
{
	if(dirop->getViewType() == KFile::Simple)
		((SQ_FileIconView*)dirop->new_view)->setIconSize(32);
}

void SQ_Page::slotSetIconSize48()
{
	if(dirop->getViewType() == KFile::Simple)
		((SQ_FileIconView*)dirop->new_view)->setIconSize(48);
}

void SQ_Page::slotSetIconSize64()
{
	if(dirop->getViewType() == KFile::Simple)
		((SQ_FileIconView*)dirop->new_view)->setIconSize(64);
}

void SQ_Page::slotSetURLfromMenu(int id)
{
	setURL(menuBookmarks->text(id));
}
