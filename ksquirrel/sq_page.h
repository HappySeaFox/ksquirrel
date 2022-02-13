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
#include <kurlrequester.h>
#include <kfile.h>
#include <kfilefiltercombo.h>

#include "sq_diroperator.h"
#include "ksquirrel.h"

typedef struct
{
    KURL url;
    KFile::FileView View;
    int type;
}PAGESETT;

typedef struct
{
	QString name;
	QString filter;
}FILTER;

class SQ_Page : public QWidget
{
    Q_OBJECT

    private:
	QVBoxLayout *l;
	SQ_DirOperator *dirop;
	QToolBar *toolbar, *toolbar2;
	QString filter;
	KFileFilterCombo *filterCombo;
	KURLRequester *curPath;
	QToolButton *togHidden, *tbUp, *tbBack, *tbForward;
	KPopupMenu		*menuIconSize;
	QPopupMenu		*menuBookmarks;
	int Type;
	QValueList<FILTER> *fl;
	QToolButton *filterButton;
 	int menuBookmarksID;
	QPixmap	folderPix;
    public:
    
	SQ_Page(QWidget *parent, PAGESETT *ps);
    	SQ_Page(QWidget *parent = 0, KURL path = KURL("/"), KFile::FileView View = KFile::Simple, int type = 1);
	~SQ_Page();
																			
	PAGESETT* getPageSett();
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
       void slotDupTab();
	void slotCloseTab();
	void slotSetIconSize16();
	void slotSetIconSize22();
	void slotSetIconSize32();
	void slotSetIconSize48();
	void slotSetIconSize64();
	void slotSetURLfromMenu(int);
};

#endif
