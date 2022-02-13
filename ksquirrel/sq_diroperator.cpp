#include "sq_diroperator.h"

#include <krun.h>
#include <kcombiview.h>

#include "sq_fileiconview.h"
#include "sq_filedetailview.h"

SQ_DirOperator::SQ_DirOperator(const KURL &url, QWidget *parent, const char *name, bool single) : KDirOperator(url, parent, name),isSingleClick(single)
{}

SQ_DirOperator::~SQ_DirOperator()
{}

KFile::FileView SQ_DirOperator::getViewType()
{
    return View;
}

bool SQ_DirOperator::isSingleClickF()
{
    return isSingleClick;
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
        KFileView* v = 0L;
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
        connect((SQ_FileDetailView*)new_view, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotDoubleClicked(QListViewItem*)));
	 slotSetSingleClick(isSingleClick);
    }
    else if(view == KFile::Simple)
    {
        SQ_FileIconView *tt = new SQ_FileIconView(parent, "simple view");
        new_view = tt;
	
        new_view->setViewName("Short View");
        connect((SQ_FileIconView*)new_view, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotDoubleClicked(QIconViewItem*)));

	tt->setMaxItemTextLength(12);
	slotSetSingleClick(isSingleClick);
    }
    else;

    return new_view;
}

void SQ_DirOperator::slotDoubleClicked(QIconViewItem *item)
{
    if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
        if(f->fileInfo()->isFile())
	{
	    QString newpath = url().path() + /*"/" + */item->text();
	    QString mime(KMimeType::findByPath(/*QFileInfo(newpath).dirPath(true) + "/" + QFileInfo(newpath).fileName().lower()*/newpath)->name());
	    KRun::runURL(KURL(newpath), mime);
	}
}

void SQ_DirOperator::slotDoubleClicked(QListViewItem *item)
{
    if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
        if(f->fileInfo()->isFile())
	{
	    QString newpath = url().path() + /*"/" + */item->text(0);
	    QString mime(KMimeType::findByPath(/*QFileInfo(newpath).dirPath(true) + "/" + QFileInfo(newpath).fileName().lower()*/newpath)->name());
	    KRun::runURL(KURL(newpath), mime);
	}
}

void SQ_DirOperator::slotSetSingleClick(bool isSingle)
{
    if(View == KFile::Simple)
    {
	if(isSingle)
        {
	    connect((SQ_FileIconView*)new_view, SIGNAL(clicked(QIconViewItem*, const QPoint&)), (SQ_FileIconView*)new_view, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
	    connect((SQ_FileIconView*)new_view, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotDoubleClicked(QIconViewItem*)));
        }
        else
        {
	    disconnect((SQ_FileIconView*)new_view, SIGNAL(clicked(QIconViewItem*, const QPoint&)), (SQ_FileIconView*)new_view, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
	    disconnect((SQ_FileIconView*)new_view, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotDoubleClicked(QIconViewItem*)));
	}
    }
    else if(View == KFile::Detail)
    {
        if(isSingle)
        {
            connect((SQ_FileDetailView*)new_view, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), (SQ_FileDetailView*)new_view, SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
	    connect((SQ_FileDetailView*)new_view, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotDoubleClicked(QListViewItem*)));
        }
	else
	{
	    disconnect((SQ_FileDetailView*)new_view, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), (SQ_FileDetailView*)new_view, SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
	    disconnect((SQ_FileDetailView*)new_view, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotDoubleClicked(QListViewItem*)));
	}
    }
}

void SQ_DirOperator::setShowHiddenFilesF(bool s)
{
    this->setShowHiddenFiles(s);
//	    this->actionCollection()->action("showHidden")->activate();
}
