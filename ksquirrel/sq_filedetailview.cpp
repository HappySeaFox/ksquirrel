#include "sq_filedetailview.h"

SQ_FileDetailView::SQ_FileDetailView(QWidget* parent, const char* name) : KFileDetailView(parent, name)
{
    setDragEnabled(true);

    disconnect(this, SIGNAL(clicked(QListViewItem *, const QPoint&, int)), this, 0);
}


SQ_FileDetailView::~SQ_FileDetailView()
{}


void SQ_FileDetailView::slotSelected(QListViewItem *item, const QPoint &point, int c)
{
    emit doubleClicked(item, point, c);
}
