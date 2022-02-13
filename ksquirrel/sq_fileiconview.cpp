#include "sq_fileiconview.h"

SQ_FileIconView::SQ_FileIconView(QWidget *parent, const char *name) : KFileIconView(parent, name)
{
	disconnect(this, SIGNAL(clicked(QIconViewItem*, const QPoint&)), this, 0);
}

SQ_FileIconView::~SQ_FileIconView()
{}

void SQ_FileIconView::slotSelected(QIconViewItem *item, const QPoint &point)
{
	emit doubleClicked(item, point);
}
