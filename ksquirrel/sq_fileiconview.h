#ifndef SQ_FILEICONVIEW_H
#define SQ_FILEICONVIEW_H

#include <qwidget.h>
#include <qpoint.h>
#include <kfileiconview.h>

class SQ_FileIconView : public KFileIconView
{
    Q_OBJECT

	public:
		SQ_FileIconView(QWidget *parent = 0, const char *name = "");
	        ~SQ_FileIconView();

	        // virtual methods
		virtual void insertItem(KFileItem *);

	protected slots:
		void slotSelected(QIconViewItem *item, const QPoint &point);
		void slotDrop(QDropEvent*, const QValueList<QIconDragItem>&);

	protected:
		QDragObject *dragObject();
};


#endif
