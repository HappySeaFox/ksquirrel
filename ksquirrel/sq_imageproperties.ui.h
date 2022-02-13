/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_ImageProperties::init()
{
    menu = new KPopupMenu;
    KAction *copy = KStdAction::copy(this, SLOT(slotCopyString()), 0);
    KAction *copyentry = new KAction(i18n("Copy entry"), 0, this, SLOT(slotCopyEntry()), 0);
    KAction *copyall = new KAction(i18n("Copy all entries"), 0, this, SLOT(slotCopyAll()), 0);
    
    copyentry->setIcon(copy->icon());
    copyall->setIcon(copy->icon());
    
    copy->plug(menu);
    copyentry->plug(menu);
    copyall->plug(menu);
    
    ok = KSquirrel::loader()->loadIcon("ok", KIcon::Desktop, 16);
    error = KSquirrel::loader()->loadIcon("error", KIcon::Desktop, 16);
}

void SQ_ImageProperties::setParams(QStringList &l)
{
    setFileParams();
    
    QStringList::Iterator it = l.begin();
    textType->setText(*it); ++it;
    textDimensions->setText(*it); ++it;
    textBpp->setText(*it); ++it;
    textColorModel->setText(*it); ++it;
    textCompression->setText(*it); ++it;
    textUncompressed->setText(*it); ++it;
    textRatio->setText(*it); ++it;
    textInterlaced->setText(*it); ++it;
    int errors = (*it).toInt(); ++it;
    textFrames->setText(*it); ++it;
    textFrame->setText(*it); ++it;
    QString s = QString::fromLatin1("%1").arg(i18n("1 error", "%n errors", errors));
    textStatus->setText((errors)?s:QString::null);
    textStatusIcon->setPixmap((errors)?error:ok);
    
    s = QString::fromLatin1("%1 ms.").arg(*it);
    textDelay->setText(s);
}

void SQ_ImageProperties::setFile( const QString &filen )
{
    file = filen;
}

void SQ_ImageProperties::setFileParams()
{
    QFileInfo fm(file);
    
    lineDirectory->setText(fm.dirPath(true));
    lineFile->setText(fm.fileName());
    textSize->setText(KIO::convertSize(fm.size()));
    textOwner->setText(QString("%1 (id: %2)").arg(fm.owner()).arg(fm.ownerId()));
    textGroup->setText(QString("%1 (id: %2)").arg(fm.group()).arg(fm.groupId()));
    textPermissions->setText(QString("%1%2%3%4%5%6%7%8%9")
		       .arg(fm.permission(QFileInfo::ReadUser)?"r":"-")
		       .arg(fm.permission(QFileInfo::WriteUser)?"w":"-")
		       .arg(fm.permission(QFileInfo::ExeUser)?"x":"-")
		       .arg(fm.permission(QFileInfo::ReadGroup)?"r":"-")
		       .arg(fm.permission(QFileInfo::WriteGroup)?"w":"-")
		       .arg(fm.permission(QFileInfo::ExeGroup)?"x":"-")
		       .arg(fm.permission(QFileInfo::ReadOther)?"r":"-")
		       .arg(fm.permission(QFileInfo::WriteOther)?"w":"-")
		       .arg(fm.permission(QFileInfo::ExeOther)?"x":"-"));
    
    QDateTime abs = fm.created();
    textCreated->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
    abs = fm.lastRead();
    textLastRead->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
    abs = fm.lastModified();
    textLastMod->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
}

void SQ_ImageProperties::setMetaInfo(QValueVector<QPair<QString,QString> > meta )
{
    QValueVector<QPair<QString,QString> >::iterator BEGIN = meta.begin();
    QValueVector<QPair<QString,QString> >::iterator END = meta.end();
    QListViewItem *after = 0, *item;
    
    for(QValueVector<QPair<QString,QString> >::iterator it = BEGIN;it != END;++it)
    {
	if(after)
	    item = new QListViewItem(listMeta, after, (*it).first+QString::fromLatin1("  "), (*it).second);
	else
	    after = item = new QListViewItem(listMeta, (*it).first+QString::fromLatin1("  "), (*it).second);
	
	listMeta->insertItem(item);
    }
    
    if(!listMeta->childCount())
    {
		listMeta->header()->hide();

		QWidget *w = tabWidget3->page(2);

		if(w)
			tabWidget3->changeTab(w, i18n("Metadata (no)"));
    }
}

void SQ_ImageProperties::slotContextMenu( QListViewItem *item, const QPoint &p, int z1 )
{
    if(item)
    {
	data = item;
	z = z1;
	menu->exec(p);
    }
}

void SQ_ImageProperties::slotCopyString()
{
    QApplication::clipboard()->setText(data->text(z), QClipboard::Clipboard);
}


void SQ_ImageProperties::slotCopyAll()
{
    if(!data)
	return;

    QString app;
    int count = listMeta->childCount();
    QListViewItem *item = listMeta->firstChild();
    
    for(int i = 0;i < count;i++)
    {
	if(item)
	    app.append(item->text(0) + "\n" + item->text(1) + "\n");
	
	item = item->itemBelow();
    }
    
    QApplication::clipboard()->setText(app, QClipboard::Clipboard);
}

void SQ_ImageProperties::slotCopyEntry()
{
    if(!data)
	return;
    QString app;
    
    app = data->text(0) + "\n" + data->text(1) + "\n";
    
    QApplication::clipboard()->setText(app, QClipboard::Clipboard);
}
