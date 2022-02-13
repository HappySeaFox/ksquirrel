/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void KLC::init()
{
    changed = false;

    listEnabled->setSorting(0);
    listEnabled->header()->hide();

    listDisabled->setSorting(0);
    listDisabled->header()->hide();

    lister = new KDirLister;
    connect(lister, SIGNAL(completed(const KURL &)), this, SLOT(slotCompleted(const KURL &)));
    connect(lister, SIGNAL(deleteItem(KFileItem *)), this, SLOT(slotDeleteItem(KFileItem *)));
    connect(lister, SIGNAL(newItems(const KFileItemList &)), this, SLOT(slotNewItems(const KFileItemList &)));

    base = KURL::fromPathOrURL(SQ_KLIBS);

    backup = base;
    backup.addPath("backup");

    backupDir = backup.path();
    
    KIO::NetAccess::mkdir(backup, this);

    QTimer::singleShot(0, this, SLOT(slotLoad()));
}

void KLC::slotLoad()
{
    lister->openURL(base);
    lister->openURL(backup, true);
}

void KLC::destroy()
{
    delete lister;
}

void KLC::slotApply()
{
    changed = true;

    moveCodecs(true);
    moveCodecs(false);
}

void KLC::moveCodecs(bool e2d)
{
    KURL::List list;
    KURL codec;

    QListViewItemIterator it(e2d ? listEnabled : listDisabled);
    QCheckListItem *li;

    while((li = static_cast<QCheckListItem *>(it.current())))
    {
        if(li->isOn())
        {
	    codec = e2d ? base : backup;
    	    codec.addPath(li->text());
	    list.append(codec);
        }

	++it;
    }

    KIO::Job *job = KIO::move(list, e2d ? backup : base);

    connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotDelResult(KIO::Job *)));
}

void KLC::slotCompleted(const KURL &u)
{
    if(u == base)
    {
	listEnabled->setEnabled(true);
	listEnabled->setCurrentItem(listEnabled->firstChild());
    }
    else
    {
	listDisabled->setEnabled(true);
	listDisabled->setCurrentItem(listDisabled->firstChild());
    }
}

void KLC::slotNewItems(const KFileItemList &list)
{
    KFileItemListIterator it(list);
    KFileItem *fi;
    QString soname = QString::fromLatin1(".so.%1").arg(SQ_KL_VER);

    while((fi = it.current()))
    {
	if(fi->isFile() && fi->name().endsWith(soname))
	    new  QCheckListItem(fi->url().directory().startsWith(backupDir) ? listDisabled:listEnabled, fi->name(), QCheckListItem::CheckBox);

	++it;
    }
}

void KLC::slotDeleteItem(KFileItem *fi)
{
    QListView *l = fi->url().directory().startsWith(backupDir) ? listDisabled:listEnabled;

    QListViewItemIterator it(l);
    QCheckListItem *li;
    QListViewItem *li2;

    while((li = static_cast<QCheckListItem *>(it.current())))
    {
        if(li->text() == fi->name())
        {
            li2 = li->itemBelow();

            if(!li2)
                li2 = li->itemAbove();

            delete li;

            l->setCurrentItem(li2);
            l->setSelected(li2, true);

            return;
        }

	++it;
    }
}

void KLC::slotDelResult(KIO::Job *job)
{
    if(job && job->error())
	job->showErrorDialog(this);
}

void KLC::closeEvent(QCloseEvent *ev)
{
    QString data;

    if(changed)
	kapp->dcopClient()->send("ksquirrel", "ksquirrel", "reload_codecs()", data);

    ev->accept();
}
