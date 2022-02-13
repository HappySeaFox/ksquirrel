#include "sq_diroperator.h"

#include <krun.h>
#include <kcombiview.h>

#include "sq_fileiconview.h"
#include "sq_filedetailview.h"

#include "ksquirrel.h"

SQ_DirOperator::SQ_DirOperator(const KURL &url, QWidget *parent, const char *name) : KDirOperator(url, parent, name)
{
	sing = KGlobalSettings::singleClick();
}

SQ_DirOperator::~SQ_DirOperator()
{}

KFile::FileView SQ_DirOperator::getViewType()
{
    return View;
}

KFileView* SQ_DirOperator::createView(QWidget *parent, KFile::FileView view)
{
	new_view = 0L;
	View = view;

	bool separateDirs = KFile::isSeparateDirs(view);

	if(separateDirs)
	{
		KCombiView *combi = new KCombiView(parent, "combi view");
		combi->setOnlyDoubleClickSelectsFiles(onlyDoubleClickSelectsFiles());

		KFileView *v = 0L;

		if((view & KFile::Simple) == KFile::Simple)
			v = createView(combi, KFile::Simple);
		else
			v = createView(combi, KFile::Detail);
			
		combi->setRight(v);
		new_view = combi;
	}
	else if(view == KFile::Detail)
	{
		new_view = new SQ_FileDetailView(parent, "detail view");

		if(sing)
		{
			connect((SQ_FileDetailView*)new_view, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), (SQ_FileDetailView*)new_view, SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
			connect((SQ_FileDetailView*)new_view, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotDoubleClicked(QListViewItem*)));
       	}
		else
			connect((SQ_FileDetailView*)new_view, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotDoubleClicked(QListViewItem*)));
		
		connect((SQ_FileDetailView*)new_view, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(slotSelected(QListViewItem*)));
	}
	else if(view == KFile::Simple)
	{
		SQ_FileIconView *tt = new SQ_FileIconView(parent, "simple view");
		new_view = tt;
	
		new_view->setViewName("Short View");

		if(sing)
		{
			connect((SQ_FileIconView*)new_view, SIGNAL(clicked(QIconViewItem*, const QPoint&)), (SQ_FileIconView*)new_view, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
			connect((SQ_FileIconView*)new_view, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotDoubleClicked(QIconViewItem*)));
		}
		else
			connect((SQ_FileIconView*)new_view, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotDoubleClicked(QIconViewItem*)));
		
		connect((SQ_FileIconView*)new_view, SIGNAL(currentChanged(QIconViewItem*)), this, SLOT(slotSelected(QIconViewItem*)));
	}

	return new_view;
}

void SQ_DirOperator::slotDoubleClicked(QIconViewItem *item)
{
	if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
		if(f->fileInfo()->isFile())
			f->fileInfo()->run();
}

void SQ_DirOperator::slotDoubleClicked(QListViewItem *item)
{
    if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
        if(f->fileInfo()->isFile())
		f->fileInfo()->run();
}

void SQ_DirOperator::slotSelected(QIconViewItem *item)
{
	if(!item) return;
	
	if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
	{
		KFileItem *fi = f->fileInfo();
		if(fi->isFile() || fi->isDir())
		{
			QString str;
			str.sprintf(" Total %d (%d dirs, %d files) ", new_view->count(), new_view->numDirs(), new_view->numFiles());
			QString str2 = " " + fi->timeString() + " ";
			QPixmap px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, 16);
			sqSBdirInfo->setText(str);
			sqSBcurFileInfo->setText(str2);
			sqSBfileIcon->setPixmap(px);
			sqSBfileName->setText("  " + fi->text() + ((fi->isDir())?"":(" (" + KIO::convertSize(fi->size()) + ")")));
		}
	}
}

void SQ_DirOperator::slotSelected(QListViewItem *item)
{
	if(!item) return;

	if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
	{
		KFileItem *fi = f->fileInfo();
		if(fi->isFile() || fi->isDir())
		{
			QString str;
			str.sprintf(" Total %d (%d dirs, %d files) ", new_view->count(), new_view->numDirs(), new_view->numFiles());
			QString str2 = " " + fi->timeString() + " ";
			QPixmap px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, 16);
			sqSBdirInfo->setText(str);
			sqSBcurFileInfo->setText(str2);
			sqSBfileIcon->setPixmap(px);
			sqSBfileName->setText("  " + fi->text() + ((fi->isDir())?"":(" (" + KIO::convertSize(fi->size()) + ")")));
		}
	}
}

void SQ_DirOperator::setShowHiddenFilesF(bool s)
{
    this->setShowHiddenFiles(s);
}
