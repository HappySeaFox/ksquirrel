#include "sq_filedetailview.h"

#include <kurldrag.h>

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

QDragObject* SQ_FileDetailView::dragObject()
{
	const KFileItemList *list = KFileView::selectedItems();

	if(list->isEmpty())
		return 0;

	QPtrListIterator<KFileItem> it(*list);
	KURL::List urls;

	for(; it.current(); ++it)
		urls.append(it.current()->url());

	return KURLDrag::newDrag(urls, viewport());
}
