/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#define SQ_HIST_NUMBER 25

void SQ_SlideShow::init()
{
    KFile::Mode mode = static_cast<KFile::Mode>(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);
    kurl->setMode(mode);

    spinDelay->setValue(SQ_Config::instance()->readNumEntry("Slideshow", "delay", 1000));
    pushCurrent->setPixmap(SQ_IconLoader::instance()->loadIcon("folder_green", KIcon::Desktop, 16));
    pushHistory->setPixmap(SQ_IconLoader::instance()->loadIcon("history", KIcon::Desktop, 16));

    checkForce->setChecked(SQ_Config::instance()->readBoolEntry("Slideshow", "force", true));
    checkFull->setChecked(SQ_Config::instance()->readBoolEntry("Slideshow", "fullscreen", true));

    QPopupMenu *hist = new QPopupMenu;
    items = SQ_Config::instance()->readListEntry("Slideshow", "history");
    int i = 0;

    setPath(items.last());

    for(QStringList::iterator it = items.begin();it != items.end();++it)
    {
	if(i++ < SQ_HIST_NUMBER)
	    hist->insertItem(*it);
	else
	    break;
    }

    connect(hist, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));

    pushHistory->setPopup(hist);
}

int SQ_SlideShow::exec(QString &path)
{
    int result = QDialog::exec();

    if(result == QDialog::Accepted)
    {
	SQ_Config::instance()->setGroup("Slideshow");
	SQ_Config::instance()->writeEntry("delay", spinDelay->value());
	SQ_Config::instance()->writeEntry("fullscreen", checkFull->isChecked());
	SQ_Config::instance()->writeEntry("force", checkForce->isChecked());
	SQ_Config::instance()->writeEntry("history", items);
	path = kurl->url();
    }

    return result;
}

void SQ_SlideShow::slotDirectory(const QString &dir)
{
    if(!dir.isEmpty())
	appendPath(dir);
}

void SQ_SlideShow::setPath(const QString &path)
{
    kurl->setURL(path);
}

void SQ_SlideShow::slotActivated(int id)
{
    QString dir = pushHistory->popup()->text(id);

    if(!dir.isEmpty())
	setPath(dir);
}

void SQ_SlideShow::appendPath(const QString &path)
{
    if(items.findIndex(path) == -1)
    {
	items.append(path);
	pushHistory->popup()->insertItem(path);

	if(items.count() > SQ_HIST_NUMBER)
	    items.pop_front();
    }
}

void SQ_SlideShow::slotCurrent()
{
    QString path = SQ_WidgetStack::instance()->url().path();

    setPath(path);
    appendPath(path);
}
