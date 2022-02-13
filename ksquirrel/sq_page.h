#ifndef SQ_PAGE_H
#define SQ_PAGE_H

#include <qwidget.h>
#include <qlayout.h>
#include <qstring.h>
#include <qtabbar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <kurl.h>
#include <kurlrequester.h>
#include <kfile.h>
#include <kfilefiltercombo.h>

#include "sq_diroperator.h"
#include "ksquirrel.h"

typedef struct
{
    bool isSingleClick;
    KURL url;
    KFile::FileView View;
    int type;
}PAGESETT;


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
	QToolButton *togSingle, *togHidden, *tbUp, *tbBack, *tbForward;
//	SQ_MyTabWidget *tabwidget;
	int Type;
    
    public:
    
	SQ_Page(QWidget *parent, PAGESETT *ps);
    	SQ_Page(QWidget *parent = 0, KURL path = KURL("/"), KFile::FileView View = KFile::Simple, int type = 1, bool single = false);
	~SQ_Page();
																			
	PAGESETT* getPageSett();
//	void setAscTabWidget(SQ_MyTabWidget *t);
	int getType() const;
	
    public slots:
	void slotDoNothing();
	void slotSetFilter();
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
};

#endif
