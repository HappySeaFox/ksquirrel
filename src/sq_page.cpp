#include "sq_page.h"

#include <kiconloader.h>
#include <kaction.h>

#include <qlabel.h>
#include <qbuttongroup.h>

#include "sq_fileiconview.h"
#include "sq_filedetailview.h"

SQ_Page::SQ_Page(QWidget *parent, KURL path, KFile::FileView View, int type) : QWidget(parent), Type(type)
{
	pPageLayout = new QVBoxLayout (this);
	pIconSizeList = new QValueList<int>;
	pIconSizeList->append(16);
	pIconSizeList->append(22);
	pIconSizeList->append(32);
	pIconSizeList->append(48);
	pIconSizeList->append(64);
	pIconSizeList->append(96);
	pIconSizeList->append(128);
	pIconSizeList->append(192);
	pIconSizeList->append(256);

	sqConfig->setGroup("file browser");

	pDirOperator = new SQ_DirOperator(path, this, "file view");
	pDirOperator->readConfig(sqConfig, "file browser");
	pDirOperator->setViewConfig(sqConfig, "file browser");
	pDirOperator->setMode(KFile::Files);
	pDirOperator->setView(View);
/*
	int count = pDirOperator->view()->actionCollection()->count();

	for(int i = 0;i < count;i++)
	{
		KAction *a = pDirOperator->view()->actionCollection()->action(i);
		printf("%s,%s\n", (a->text()).ascii(), (a->group()).ascii());
	}
*/
	if(View != KFile::Detail)
		pDirOperator->view()->actionCollection()->action(Type)->activate();

	if(pDirOperator->getViewType() == KFile::Simple)
		iCurrentListIndex = pIconSizeList->findIndex(((SQ_FileIconView*)pDirOperator->new_view)->iconSize());
	else
		iCurrentListIndex = 0;


	pPageToolbar = new KToolBar(this, "tools");
	pPageToolbar->setVerticalStretchable(false);
	pPageToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	pPageToolbar2 = new KToolBar(this, "tools2");
	pPageToolbar2->setVerticalStretchable(false);
	pPageToolbar2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	KAction *pABack = pDirOperator->actionCollection()->action("back");
	pTBAbstractButton = new QToolButton(pABack->iconSet(KIcon::Desktop, 16), "Go back in history", QString::null, this, SLOT(slotBack()), pPageToolbar);
	pABack->setEnabled(false);
	pTBAbstractButton->setEnabled(false);
	connect(pABack, SIGNAL(enabled(bool)), pTBAbstractButton, SLOT(setEnabled(bool)));

	KAction *pAForw = pDirOperator->actionCollection()->action("forward");
	pTBAbstractButton = new QToolButton(pAForw->iconSet(KIcon::Desktop, 16), "Go forward in history", QString::null, this, SLOT(slotForward()), pPageToolbar);
	pAForw->setEnabled(false);
	pTBAbstractButton->setEnabled(false);
	connect(pAForw, SIGNAL(enabled(bool)), pTBAbstractButton, SLOT(setEnabled(bool)));

	KAction *pAUp = pDirOperator->actionCollection()->action("up");
	pTBAbstractButton = new QToolButton(pAUp->iconSet(KIcon::Desktop, 16), "Go up level", QString::null, this, SLOT(slotUp()), pPageToolbar);
	connect(pAUp, SIGNAL(enabled(bool)), pTBAbstractButton, SLOT(setEnabled(bool)));

	pPageToolbar->addSeparator();

	(new QToolButton(sqLoader->loadIcon("previous", KIcon::Desktop, 22), "Previous file", QString::null, this, SLOT(slotPrevious()), pPageToolbar))->setAutoRepeat(true);
	(new QToolButton(sqLoader->loadIcon("next", KIcon::Desktop, 22), "Next file", QString::null, this, SLOT(slotNext()), pPageToolbar))->setAutoRepeat(true);
	pPageToolbar->addSeparator();
	
	new QToolButton(pDirOperator->actionCollection()->action("reload")->iconSet(KIcon::Desktop, 16), "Reload current directory", QString::null, this, SLOT(slotReload()), pPageToolbar);
	new QToolButton(pDirOperator->actionCollection()->action("home")->iconSet(KIcon::Desktop, 16), "Go your home directory", QString::null, this, SLOT(slotHome()), pPageToolbar);

	pPageToolbar->addSeparator();

	new QToolButton(pDirOperator->actionCollection()->action("mkdir")->iconSet(KIcon::Desktop, 16), "Create new directory", QString::null, this, SLOT(slotMkdir()), pPageToolbar);
	new QToolButton(sqLoader->loadIcon("penguin", KIcon::Desktop, 16), "Properties of the selected files", QString::null, this, SLOT(slotProp()), pPageToolbar);
	new QToolButton(pDirOperator->actionCollection()->action("delete")->iconSet(KIcon::Desktop, 16), "Delete selected files", QString::null, this, SLOT(slotDelete()), pPageToolbar);
	pPageToolbar->addSeparator();

/////////  insert 'URL' button  ////////////////////////

	QToolButton *pTBClearAdress = new QToolButton(sqLoader->loadIcon("locationbar_erase", KIcon::Desktop, 16), "Clear adress", QString::null, 0, 0, pPageToolbar2);
	new QLabel("URL:", pPageToolbar2, "kde toolbar widget");
	pCurrentURL = new KHistoryCombo(true, pPageToolbar2, "history combobox");
	pPageToolbar2->setStretchableWidget(pCurrentURL);
	new QToolButton(sqLoader->loadIcon("goto", KIcon::Desktop, 16), "Go!", QString::null, this, SLOT(slotGo()), pPageToolbar2);	
	pCurrentURL->setDuplicatesEnabled(false);

	KURLCompletion *pURLCompletion = new KURLCompletion(KURLCompletion::DirCompletion);
	pURLCompletion->setDir("/");

	pCurrentURL->setCompletionObject(pURLCompletion);
	pCurrentURL->setAutoDeleteCompletionObject(true);
	pCurrentURL->setEditText(path.path());
	pCurrentURL->addToHistory(path.path());

	connect(pTBClearAdress, SIGNAL(clicked()), this, SLOT(slotClearAdress()));

///////// insert button "icon size"	////////////////////////

	pTBIconBigger  = new QToolButton(sqLoader->loadIcon("viewmag+", KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotSetIconBigger()), pPageToolbar);
	pTBIconSmaller = new QToolButton(sqLoader->loadIcon("viewmag-", KIcon::Desktop, 16), QString::null, QString::null, this, SLOT(slotSetIconSmaller()), pPageToolbar);
	static KAction *inc_ = new KAction("_Increase icon size", "", KShortcut(CTRL+Qt::Key_Plus), this, SLOT(slotSetIconBigger()), sqApp->actionCollection(), "_Increase icon size");
	static KAction *dec_ = new KAction("_Decrease icon size", "", KShortcut(CTRL+Qt::Key_Minus), this, SLOT(slotSetIconSmaller()), sqApp->actionCollection(), "_Decrease icon size");

	pPageToolbar->addSeparator();

	QButtonGroup *pBG = new QButtonGroup(0);
	pBG->setExclusive(true);
	
	pTBIconView = new QToolButton(sqLoader->loadIcon("view_icon", KIcon::Desktop, 16), "Icon View", QString::null, this, SLOT(slotPageIcon()), pPageToolbar);
	pTBListView = new QToolButton(sqLoader->loadIcon("view_choose", KIcon::Desktop, 16), "List View", QString::null, this, SLOT(slotPageList()), pPageToolbar);
	pTBDetailView = new QToolButton(sqLoader->loadIcon("view_detailed", KIcon::Desktop, 16), "Detailed View", QString::null, this, SLOT(slotPageDetailed()), pPageToolbar);
	pTBIconView->setToggleButton(true);
	pTBListView->setToggleButton(true);
	pTBDetailView->setToggleButton(true);
	pBG->insert(pTBIconView);
	pBG->insert(pTBListView);
	pBG->insert(pTBDetailView);
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

	slotSetFilter(7000+def/2);
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

////////////////////////////////////////////////////////////////////////////////////

	pTBAbstractButton = new QToolButton(QPixmap(0), QString::null, QString::null, this, 0, pPageToolbar);
	pPageToolbar->setStretchableWidget(pTBAbstractButton);
	pTBAbstractButton->setEnabled(false);

	new QToolButton(sqLoader->loadIcon("tab_remove", KIcon::Desktop, 16), "Close page", QString::null, this, SLOT(slotCloseTab()), pPageToolbar);
	
	pPageLayout->addWidget(pPageToolbar);
	pPageLayout->addWidget(pPageToolbar2);
	pPageLayout->addWidget(pDirOperator);
    
	connect(pDirOperator, SIGNAL(urlEntered(const KURL&)), this, SLOT(setURL(const KURL&)));
	connect(pCurrentURL, SIGNAL(returnPressed(const QString&)), this, SLOT(setURL(const QString&)));
	connect(pCurrentURL, SIGNAL(activated(const QString&)), this, SLOT(setURL(const QString&)));
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
	QString t = QString(" Filter") + " (" + ((*fl)[id-7000]).name + ")";
	pFilterButton->setTextLabel(t, false);
	slotReload();
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

void SQ_Page::slotSetIconBigger()
{
	if(pDirOperator->getViewType() != KFile::Simple)
		return;

		if(iCurrentListIndex != (signed)pIconSizeList->count()-1)
		{
			iCurrentListIndex++;

			int iconsize = *(pIconSizeList->at(iCurrentListIndex));

			((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(iconsize);
		}
}

void SQ_Page::slotSetIconSmaller()
{
	if(pDirOperator->getViewType() != KFile::Simple)
		return;

		if((unsigned)iCurrentListIndex != 0)
		{
			iCurrentListIndex--;

			int iconsize = *(pIconSizeList->at(iCurrentListIndex));

			((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(iconsize);
		}
}

void SQ_Page::slotPageIcon()
{
	if(pDirOperator->getViewType() == KFile::Detail)
		pDirOperator->actionCollection()->action("short view")->activate();

	int iconsize = *(pIconSizeList->at(iCurrentListIndex));

	if(pDirOperator->getViewType() == KFile::Simple)
		((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(iconsize);

	pDirOperator->view()->actionCollection()->action(1)->activate();
}

void SQ_Page::slotPageList()
{
	if(pDirOperator->getViewType() == KFile::Detail)
		pDirOperator->actionCollection()->action("short view")->activate();

	int iconsize = *(pIconSizeList->at(iCurrentListIndex));

	if(pDirOperator->getViewType() == KFile::Simple)
		((SQ_FileIconView*)pDirOperator->new_view)->setIconSize(iconsize);

 	pDirOperator->view()->actionCollection()->action(2)->activate();
}

void SQ_Page::slotPageDetailed()
{
	pDirOperator->actionCollection()->action("detailed view")->activate();
}

void SQ_Page::slotClearAdress()
{
	pCurrentURL->setEditText("");
}

void SQ_Page::slotGo()
{
	pDirOperator->setURL(pCurrentURL->currentText(), false);
}

void SQ_Page::slotPrevious()
{
	KFileView *local;

	if(pDirOperator->getViewType() == KFile::Simple)
		local = (SQ_FileIconView*)pDirOperator->new_view;
	else
		local = (SQ_FileDetailView*)pDirOperator->new_view;

	local->setSelected(local->currentFileItem(), false);
	local->setCurrentItem(local->prevItem(local->currentFileItem()));
	local->setSelected(local->currentFileItem(), true);
}

void SQ_Page::slotNext()
{
	KFileView *local;

	if(pDirOperator->getViewType() == KFile::Simple)
		local = (SQ_FileIconView*)pDirOperator->new_view;
	else
		local = (SQ_FileDetailView*)pDirOperator->new_view;

	local->setSelected(local->currentFileItem(), false);
	local->setCurrentItem(local->nextItem(local->currentFileItem()));
	local->setSelected(local->currentFileItem(), true);
}
