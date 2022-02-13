#ifndef SQ_TREEVIEW_H
#define SQ_TREEVIEW_H

#include <kfiletreeview.h>


class SQ_TreeView : public KFileTreeView
{
    Q_OBJECT

	private:
        
	public:
		SQ_TreeView(QWidget *parent = 0, const char *name = 0);
		~SQ_TreeView();

	
	public slots:
		void slotDoubleClicked(QListViewItem*);
		void slotSetupClick(QListViewItem*);
};

#endif
