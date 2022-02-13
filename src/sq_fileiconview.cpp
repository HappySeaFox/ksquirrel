#include "sq_fileiconview.h"

#include <qstrlist.h>

#include <kurldrag.h>
#include <qdragobject.h>

SQ_FileIconView::SQ_FileIconView(QWidget *parent, const char *name) : KFileIconView(parent, name)
{
	disconnect(this, SIGNAL(clicked(QIconViewItem*, const QPoint&)), this, 0);
	setAcceptDrops(true);
	connect(this, SIGNAL(dropped(QDropEvent*, const QValueList<QIconDragItem>&)), this, SLOT(slotDrop(QDropEvent*, const QValueList<QIconDragItem>&)));
	setSorting(QDir::IgnoreCase);
}

SQ_FileIconView::~SQ_FileIconView()
{}

void SQ_FileIconView::slotSelected(QIconViewItem *item, const QPoint &point)
{
	emit doubleClicked(item, point);
}

QDragObject* SQ_FileIconView::dragObject()
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

void SQ_FileIconView::slotDrop(QDropEvent *e, const QValueList<QIconDragItem>&)
{
	if(!QUriDrag::canDecode(e))
	{
		e->ignore();
		return;
	}

	KURL::List urls;
	KURLDrag::decode(e, urls);

//	KURLDrag::newDrag(urls, e->source());

	e->accept(true);
}

void SQ_FileIconView::listingCompleted()
{
	clearSelection();
	setCurrentItem(firstFileItem());
	setSelected(firstFileItem(), true);

}
