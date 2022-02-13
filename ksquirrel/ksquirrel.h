		#ifndef SPLITTER_MAIN_WIDGET
#define SPLITTER_MAIN_WIDGET

#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qtabwidget.h>
#include <qdockwindow.h>
#include <qvaluelist.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>

#include <ktoolbar.h>
#include <kstatusbar.h>
#include <ksystemtray.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kurl.h>
#include <kdockwidget.h>
#include <kpopupmenu.h>
#include <kmenubar.h>

#include "sq_tray.h"

class SQ_MyTabWidget : public QTabWidget
{
    Q_OBJECT

    public:
		SQ_MyTabWidget(QWidget *parent = 0, const char *name = 0) : QTabWidget(parent, name)
		{}

    public slots:
		void removePage(QWidget *w)
		{
			QTabWidget::removePage(w);
	    		w->hide();
			delete w;
		}

};


class Squirrel : public KDockMainWindow
{
    Q_OBJECT

    private:
	       int build;
		SQ_SystemTray	*tray;
		QToolButton		*pTBAbstractButton;
		KMenuBar		*menubar;
		KToolBar			*fileTools;
		QPopupMenu		*pmLaunch;
		KPopupMenu		*pop_file, *pop_view, *pop_edit;
		QPixmap		fullIcon, unfullIcon;

    public slots:
		void slotEdit();
		void slotOptions();
		void slotExit();
		void slotNewPageIcon();
		void slotNewPageList();
		void slotNewPageDetailed();
		void slotNewPageKonsole();
		void slotGrab();
		void slotCloseCurrentPage();
		void slotExecuteRunMenu();
		void slotDoNothing() {}

    public:
		Squirrel(QWidget *parent = 0, const char *name = 0);
		~Squirrel();

    protected:
		void closeEvent(QCloseEvent *e);

    public:
		static Squirrel *App;

		KConfig *kconf;
		KIconLoader *iconL;
		SQ_MyTabWidget	*pMainTabWidget;
		QValueList<KURL> *bookmarks;

		KStatusBar	*sbar;
		QLabel		*dirInfo;
		QLabel		*curFileInfo;
		QLabel		*fileIcon;
		QLabel		*fileName;
};

#define   sqApp              (Squirrel::App)
#define   sqConfig           (Squirrel::App->kconf)
#define   sqLoader           (Squirrel::App->iconL)
#define   sqStatus            (Squirrel::App->sbar)
#define   sqProgress        (Squirrel::App->progress)
#define   sqBookmarks     (Squirrel::App->bookmarks)
#define   sqTabWidget      (Squirrel::App->pMainTabWidget)

#define   sqSBdirInfo          (Squirrel::App->dirInfo)
#define   sqSBcurFileInfo    (Squirrel::App->curFileInfo)
#define   sqSBfileIcon         (Squirrel::App->fileIcon)
#define   sqSBfileName       (Squirrel::App->fileName)

#endif
