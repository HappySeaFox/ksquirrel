#ifndef SQ_DIROPERATOR_H
#define SQ_DIROPERATOR_H

#include <qwidget.h>
#include <kurl.h>
#include <kfileview.h>
#include <kdiroperator.h>

class QIconViewItem;
class QListViewItem;

class SQ_DirOperator : public KDirOperator
{
    Q_OBJECT

    private:
	bool isSingleClick;
	KFileView *new_view;
	KFile::FileView View;

    public:
	SQ_DirOperator(const KURL &url = KURL(), QWidget *parent = 0, const char *name = "", bool single = false);

	virtual ~SQ_DirOperator();

	KFile::FileView getViewType();

	bool isSingleClickF();


    protected:
	KFileView* createView(QWidget *parent, KFile::FileView view);

    protected slots:
	void slotDoubleClicked(QIconViewItem *item);
	void slotDoubleClicked(QListViewItem *item);

    public slots:
	void slotSetSingleClick(bool isSingle);
        void setShowHiddenFilesF(bool s);
};

#endif
