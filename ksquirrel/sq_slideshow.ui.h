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
    spinDelay->setValue(SQ_Config::instance()->readNumEntry("Slideshow", "delay", 1000));
    pushPut->setPixmap(SQ_IconLoader::instance()->loadIcon("fileopen", KIcon::Desktop, 16));
    pushCurrent->setPixmap(SQ_IconLoader::instance()->loadIcon("folder_green", KIcon::Desktop, 16));
    pushHistory->setPixmap(SQ_IconLoader::instance()->loadIcon("history", KIcon::Desktop, 16));

    KURLCompletion *pURLCompletion = new KURLCompletion(KURLCompletion::DirCompletion);
    pURLCompletion->setDir("/");

    checkForce->setChecked(SQ_Config::instance()->readBoolEntry("Slideshow", "force", true));
    checkFull->setChecked(SQ_Config::instance()->readBoolEntry("Slideshow", "fullscreen", true));

    lineDir->setCompletionObject(pURLCompletion);

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
	path = dir;
    }

    return result;
}

void SQ_SlideShow::slotDirectory()
{
    dir = KFileDialog::getExistingDirectory(lineDir->text(), this);    

    if(!dir.isEmpty())
    {
	appendPath(dir);
	lineDir->setText(dir);
    }
}

void SQ_SlideShow::setPath(const QString &path)
{
    lineDir->setText(path);
    dir = path;
}

void SQ_SlideShow::slotActivated(int id)
{
    dir = pushHistory->popup()->text(id);

    if(!dir.isEmpty())
	lineDir->setText(dir);
}

void SQ_SlideShow::appendPath(const QString &path)
{
    if(items.findIndex(path) == -1)
    {
	items.append(dir);
	pushHistory->popup()->insertItem(dir);

	if(items.count() > SQ_HIST_NUMBER)
	    items.pop_front();
    }
}

void SQ_SlideShow::slotCurrent()
{
    QString path = SQ_WidgetStack::instance()->getURL().path();

    setPath(path);
    appendPath(path);
}
