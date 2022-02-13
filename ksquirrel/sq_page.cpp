#include "sq_page.h"

#include <kiconloader.h>
#include <kaction.h>

#include <qlabel.h>

SQ_Page::SQ_Page(QWidget *parent, PAGESETT *ps)
{
    SQ_Page::SQ_Page(parent, ps->url, ps->View, ps->isSingleClick);
}
    
SQ_Page::SQ_Page(QWidget *parent, KURL path, KFile::FileView View, int type, bool single) : QWidget(parent), Type(type)
{
	l = new QVBoxLayout (this);

	sqConfig->setGroup("file browser");
	bool showHidden = sqConfig->readBoolEntry("Show hidden files", false);
    
	dirop = new SQ_DirOperator(path, this, "file view", single);
	dirop->readConfig(sqConfig, "file browser");
	dirop->setViewConfig(sqConfig, "file browser");
	dirop->setMode(KFile::Files);
	dirop->setView(View);

	if(View != KFile::Detail)
		dirop->view()->actionCollection()->action(type)->activate();
    
	toolbar = new QToolBar("", 0, this, true, "tools");
	toolbar->setVerticalStretchable(false);
	toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	toolbar2 = new QToolBar("", 0, this, true, "tools2");
	toolbar2->setVerticalStretchable(false);
	toolbar2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	togSingle = new QToolButton(sqLoader->loadIcon("mousecursor", KIcon::Desktop, 16), QString::null, QString::null, this, 0, toolbar);
	togSingle->setToggleButton(true);
	togHidden = new QToolButton(sqLoader->loadIcon("desktop", KIcon::Desktop, 16), QString::null, QString::null, this, 0, toolbar);
	togHidden->setToggleButton(true);
	/*KToggleAction *actionHidden = */new KToggleAction((KToggleAction*)(dirop->actionCollection()->action("show hidden")));
	toolbar->addSeparator();

	new QLabel(" URL:", toolbar);
	curPath = new KURLRequester(path.path(), toolbar, "current url");

	tbBack = new QToolButton(dirop->actionCollection()->action("back")->iconSet(KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotBack()), toolbar2);
	tbForward = new QToolButton(dirop->actionCollection()->action("forward")->iconSet(KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotForward()), toolbar2);
	tbUp = new QToolButton(dirop->actionCollection()->action("up")->iconSet(KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotUp()), toolbar2);
    
	toolbar2->addSeparator();   
	new QToolButton(dirop->actionCollection()->action("reload")->iconSet(KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotReload()), toolbar2);
	new QToolButton(dirop->actionCollection()->action("home")->iconSet(KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotHome()), toolbar2);
	toolbar2->addSeparator();       
	new QToolButton(dirop->actionCollection()->action("mkdir")->iconSet(KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotMkdir()), toolbar2);
	new QToolButton(sqLoader->loadIcon("penguin", KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotProp()), toolbar2);
	new QToolButton(dirop->actionCollection()->action("delete")->iconSet(KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotDelete()), toolbar2);
	toolbar2->addSeparator();
	new QLabel("  Filter:", toolbar2);

	filterCombo = new KFileFilterCombo(toolbar2, "filter");
	filterCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	filter = "";    

	sqConfig->setGroup("Filters");

	int entries = sqConfig->readNumEntry("Number", 0);

	for(int i = 1;i < entries*2;i += 2)
	{
		QString ext = QString("%1").arg(i, 0, 10);
		QString name = QString("%1").arg(i+1, 0, 10);
		QString ext_ = sqConfig->readEntry(ext, "");
		
		filter += ext_ + "|"
			+ sqConfig->readEntry(name, "")
			+ " (";

		if(ext_.length() > 15)
		{
			ext_.truncate(12);
			ext_ += "...";
		}
                      
		filter += ext_
			+ ((i+1==entries*2)?")":")\n");
	}

	filterCombo->setFilter(filter);
	filterCombo->setEditable(false);
	filterCombo->setSizeLimit(12);
	toolbar2->addSeparator();
	new QToolButton(sqLoader->loadIcon("bookmark_add", KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotAddBookmark()), toolbar2);
	new QToolButton(sqLoader->loadIcon("tab_duplicate", KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotDupTab()), toolbar2);	
	new QToolButton(sqLoader->loadIcon("tab_remove", KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotCloseTab()), toolbar2);
//	toolbar2->setStretchableWidget(lw);
    
	if(single)			togSingle->toggle();
	if(showHidden)	togHidden->toggle();

	((KToggleAction*)(dirop->actionCollection()->action("show hidden")))->setChecked(showHidden);
	
	connect(togSingle, SIGNAL(toggled(bool)), dirop, SLOT(slotSetSingleClick(bool)));
	connect(togHidden, SIGNAL(toggled(bool)), this, SLOT(slotShowHidden(bool)));
//	connect(actionHidden, SIGNAL(toggled(bool)), togHidden, SIGNAL(toggled(bool)));
    
	l->addWidget(toolbar2);    
	l->addWidget(toolbar);
	l->addWidget(dirop);
    
	connect(dirop, SIGNAL(urlEntered(const KURL&)), this, SLOT(setURL(const KURL&)));
	connect(curPath, SIGNAL(returnPressed(const QString&)), this, SLOT(setURL(const QString&)));
	connect(filterCombo, SIGNAL(filterChanged()), this, SLOT(slotSetFilter()));
	slotSetFilter();
}

SQ_Page::~SQ_Page()
{}

PAGESETT* SQ_Page::getPageSett()
{
    PAGESETT *ps = new PAGESETT;
    
    ps->url = dirop->url();
    ps->View = dirop->getViewType();
    ps->isSingleClick = dirop->isSingleClickF();
    ps->type = getType();
    
    return ps;
} 

int SQ_Page::getType() const
{
	return Type;
}
/*			
void SQ_Page::setAscTabWidget(SQ_MyTabWidget *t)
{
    tabwidget = t;
}
*/
void SQ_Page::slotDoNothing()
{}

void SQ_Page::slotSetFilter()
{
    dirop->setNameFilter(filterCombo->currentFilter());
    dirop->updateDir();
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

	SQ_Page *n = new SQ_Page (0, ps->url, ps->View, ps->type, ps->isSingleClick);

	sqTabWidget->addTab(n, (ps->View == KFile::Detail)?"Detailed view": ((ps->type == 1)?"Icon view":"List view"));

	delete ps;
}

void SQ_Page::slotAddBookmark()
{
	sqBookmarks->append(dirop->url());
}
