#ifndef SQ_PAGE_H
#define SQ_PAGE_H

#include <qwidget.h>
#include <qlayout.h>
#include <qstring.h>
#include <qtabbar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>

#include <kpopupmenu.h>
#include <kurl.h>
#include <kcombobox.h>
#include <kurlcompletion.h>
#include <kfile.h>
#include <kfilefiltercombo.h>

#include "sq_diroperator.h"
#include "ksquirrel.h"

typedef struct
{
	QString name;
	QString filter;
}FILTER;

class SQ_Page : public QWidget
{
    Q_OBJECT

    private:
	QVBoxLayout		*pPageLayout;
	SQ_DirOperator	*pDirOperator;
	QToolBar		*pPageToolbar, *pPageToolbar2;
	KHistoryCombo	*pCurrentURL;
	QToolButton		*pTBAbstractButton, *pFilterButton;
	QToolButton		*pTBIconBigger, *pTBIconSmaller, *pTBIconView, *pTBListView, *pTBDetailView;
	KPopupMenu		*menuIconSize;
	QPopupMenu		*menuBookmarks;
	QPixmap		folderPix;

	QValueList<FILTER> *fl;
	QValueList<int>	*pIconSizeList;
	int				iCurrentListIndex;

	int menuBookmarksID;
  	int Type;
	
    public:
    
    	SQ_Page(QWidget *parent = 0, KURL path = KURL("/"), KFile::FileView View = KFile::Simple, int type = 1);
	~SQ_Page();
																			
	int getType() const;
	
    public slots:
	void slotDoNothing();
	void slotSetFilter(int);
	void setURL(const QString &newpath);
	void setURL(const KURL &newurl);
	void slotBack();
	void slotForward();
       void slotUp();
	void slotHome();
       void slotReload();
	void slotMkdir();
       void slotShowHidden(bool);
       void slotDelete();
       void slotProp();
       void slotAddBookmark();
	void slotCloseTab();
	void slotSetIconBigger();
	void slotSetIconSmaller();	
	void slotSetURLfromMenu(int);

	void slotPageIcon();
	void slotPageList();
	void slotPageDetailed();
};

#endif
