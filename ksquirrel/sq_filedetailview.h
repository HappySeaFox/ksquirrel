#ifndef SQ_FILEDETAILVIEW_H
#define SQ_FILEDETAILVIEW_H

#include <qwidget.h>
#include <kfiledetailview.h>

class SQ_FileDetailView : public KFileDetailView
{
    Q_OBJECT

    public:
	SQ_FileDetailView(QWidget* parent, const char* name);
	~SQ_FileDetailView();

    protected slots:
	void slotSelected(QListViewItem* item, const QPoint &, int c);

};


#endif
