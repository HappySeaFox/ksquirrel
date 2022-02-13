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
    KFile::Mode mode = static_cast<KFile::Mode>(KFile::Directory | KFile::ExistingOnly);
    kurl->setMode(mode);

    SQ_Config::instance()->setGroup("Slideshow");

    checkRecurs->setChecked(SQ_Config::instance()->readBoolEntry("recurs", false));
    spinDelay->setValue(SQ_Config::instance()->readNumEntry("delay", 1000));
    spinCycle->setValue(SQ_Config::instance()->readNumEntry("repeat", 0));
    pushCurrent->setPixmap(SQ_IconLoader::instance()->loadIcon("folder_green", KIcon::Desktop, 16));
    pushHistory->setPixmap(SQ_IconLoader::instance()->loadIcon("history", KIcon::Desktop, 16));
    checkMessages->setChecked(SQ_Config::instance()->readBoolEntry("messages", true));
    checkName->setChecked(SQ_Config::instance()->readBoolEntry("messages_name", true));
    checkSize->setChecked(SQ_Config::instance()->readBoolEntry("messages_size", false));
    checkPos->setChecked(SQ_Config::instance()->readBoolEntry("messages_pos", true));

    QColor col;
    col.setNamedColor(SQ_Config::instance()->readEntry("background", "#4e4e4e"));
    bcolor->setColor(col);
    col.setNamedColor(SQ_Config::instance()->readEntry("message_text", "#ffffff"));
    tcolor->setColor(col);

    QPopupMenu *hist = new QPopupMenu;
    items = SQ_Config::instance()->readListEntry("history");
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
        // save parameters
        SQ_Config::instance()->setGroup("Slideshow");
        SQ_Config::instance()->writeEntry("delay", spinDelay->value());
        SQ_Config::instance()->writeEntry("repeat", spinCycle->value());
        SQ_Config::instance()->writeEntry("background", bcolor->color().name());
        SQ_Config::instance()->writeEntry("message_text", tcolor->color().name());
        SQ_Config::instance()->writeEntry("messages", checkMessages->isChecked());
        SQ_Config::instance()->writeEntry("messages_name", checkName->isChecked());
        SQ_Config::instance()->writeEntry("messages_size", checkSize->isChecked());
        SQ_Config::instance()->writeEntry("messages_pos", checkPos->isChecked());
        SQ_Config::instance()->writeEntry("history", items);
        SQ_Config::instance()->writeEntry("recurs", checkRecurs->isChecked());
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
    QString path = SQ_WidgetStack::instance()->url().prettyURL();

    setPath(path);
    appendPath(path);
}

void SQ_SlideShow::slotMessages( bool b)
{
    checkName->setEnabled(b);
    checkPos->setEnabled(b);
    checkSize->setEnabled(b);
    textName->setEnabled(b);
    textSize->setEnabled(b);
    textPos->setEnabled(b);
    textColor->setEnabled(b);
    tcolor->setEnabled(b);
}
