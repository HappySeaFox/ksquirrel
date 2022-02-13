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
		KFile::FileView View;
		bool sing;

	public:
		SQ_DirOperator(const KURL &url = KURL(), QWidget *parent = 0, const char *name = "");
		virtual ~SQ_DirOperator();

		KFile::FileView getViewType();
		KFileView *new_view;

	protected:
		KFileView* createView(QWidget *parent, KFile::FileView view);

	protected slots:
		void slotDoubleClicked(QIconViewItem *item);
		void slotDoubleClicked(QListViewItem *item);
		void slotSelected(QIconViewItem*);
		void slotSelected(QListViewItem*);
	
	public slots:
		void setShowHiddenFilesF(bool s);
};

#endif
