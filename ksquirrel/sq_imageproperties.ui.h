/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/*
 *  SQ_ImageProperties shows image properties.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SQ_HAVE_KEXIF
#include <libkexif/kexifwidget.h>
#include <libkexif/kexifdata.h>
#include <qobjectlist.h>
#include "sq_config.h"
#endif

void SQ_ImageProperties::init()
{
    menu = new KPopupMenu;
    copy = KStdAction::copy(this, SLOT(slotCopyString()), 0);
    copyentry = new KAction(i18n("Copy entry"), 0, this, SLOT(slotCopyEntry()), 0);
    copyall = new KAction(i18n("Copy all entries"), 0, this, SLOT(slotCopyAll()), 0);

    copyentry->setIcon(copy->icon());
    copyall->setIcon(copy->icon());

    copy->plug(menu);
    copyentry->plug(menu);
    copyall->plug(menu);

    ok = SQ_IconLoader::instance()->loadIcon("ok", KIcon::Desktop, 16);
    error = SQ_IconLoader::instance()->loadIcon("error", KIcon::Desktop, 16);
}

void SQ_ImageProperties::destroy()
{
#ifdef SQ_HAVE_KEXIF
    SQ_Config::instance()->setGroup("GL view");
    SQ_Config::instance()->writeEntry("exifmode", exifMode);
#endif

    delete copyall;
    delete copyentry;
    delete copy;
    delete menu;
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

    s = QString::fromLatin1("%1%2").arg(*it).arg(i18n(" ms."));
    textDelay->setText(s);
}

void SQ_ImageProperties::setURL(const KURL &_url)
{
    url = _url;
}

void SQ_ImageProperties::setFileParams()
{
    KIO::Job *stjob = KIO::stat(url, false);
    connect(stjob, SIGNAL(result(KIO::Job *)), this, SLOT(slotStatResult(KIO::Job *)));

#ifdef SQ_HAVE_KEXIF
        SQ_Config::instance()->setGroup("GL view");
        exifMode = SQ_Config::instance()->readNumEntry("exifmode", 0);

        QWidget *tabWidgetEXIF = new QWidget(tabWidget, "tabWidgetEXIF");
        tabWidget->addTab(tabWidgetEXIF, i18n("EXIF"));
        QGridLayout *pageLayout6 = new QGridLayout(tabWidgetEXIF, 2, 1, 11, 6, "tabWidgetEXIFLayout");

        QButtonGroup *gr = new QButtonGroup(tabWidgetEXIF, "EXIF mode");
        gr->setFrameShape(QFrame::NoFrame);
        gr->setExclusive(true);
        connect(gr, SIGNAL(clicked(int)), this, SLOT(slotModeClicked(int)));

        QGridLayout *pageLayoutGR = new QGridLayout(gr, 1, 3, 0, -1, "tabWidgetEXIFGRLayout");

        QPushButton *mode1 = new QPushButton(i18n("Simple"), gr, "mode1");
        mode1->setToggleButton(true);

        QPushButton *mode2 = new QPushButton(i18n("Full"), gr, "mode2");
        mode2->setToggleButton(true);

        QSpacerItem *spacer = new QSpacerItem(15, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

        // create KEXIF widget and load metadata from file
        KExifWidget *kew1 = new KExifWidget(tabWidgetEXIF);
        kew1->loadFile(file);

        // hack to workaround poor libkexif API
        QObjectList *ch = const_cast<QObjectList *>(kew1->children());
        for(QObjectList::iterator it = ch->begin();it != ch->end();++it)
        {
            if((*it)->inherits("QListView"))
            {
                QListView *l = dynamic_cast<QListView *>(*it);
                QWidget   *w = tabWidget->page(3);

                if(l && w && !l->childCount())
                    tabWidget->changeTab(w, i18n("EXIF (no)"));

                break;
            }
        }

        // set proper button on
        if(exifMode)
        {
            mode2->setOn(true);
            kew1->setMode(KExifWidget::FULL);
        }
        else
        {
            mode1->setOn(true);
            kew1->setMode(KExifWidget::SIMPLE);
        }

        // finally, add all widgets to layouts
        pageLayoutGR->addWidget(mode1, 0, 0);
        pageLayoutGR->addWidget(mode2, 0, 1);
        pageLayoutGR->addItem(spacer, 0, 2);

        pageLayout6->addWidget(gr, 0, 0);
        pageLayout6->addWidget(kew1, 1, 0);
        kew = kew1;
#endif
}

void SQ_ImageProperties::setMetaInfo(QValueVector<QPair<QString,QString> > meta )
{
    QListViewItem *after = 0, *item;
    
    for(QValueVector<QPair<QString,QString> >::iterator it = meta.begin();it != meta.end();++it)
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

        QWidget *w = tabWidget->page(2);

        if(w)
            tabWidget->changeTab(w, i18n("Metadata (no)"));
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
    QListViewItem *item = listMeta->firstChild();

    for(;item;item = item->itemBelow())
    {
        app.append(item->text(0) + "\n" + item->text(1) + "\n");
    }

    QApplication::clipboard()->setText(app, QClipboard::Clipboard);
}

void SQ_ImageProperties::slotCopyEntry()
{
    if(!data)
        return;

    QString app = data->text(0) + "\n" + data->text(1) + "\n";

    QApplication::clipboard()->setText(app, QClipboard::Clipboard);
}

void SQ_ImageProperties::slotModeClicked(int id)
{
// change mode: simple or full
#ifdef SQ_HAVE_KEXIF
    exifMode = id;
    static_cast<KExifWidget *>(kew)->setMode(id ? KExifWidget::FULL : KExifWidget::SIMPLE);
#endif
}

void SQ_ImageProperties::slotStatResult(KIO::Job *job)
{
    if(!job->error())
    {
	KIO::UDSEntry entry = static_cast<KIO::StatJob*>(job)->statResult();
	KFileItem fi(entry, url);

	KURL t = url;
	t.cd("..");
	lineDirectory->setText(t.isLocalFile() ? t.path() : t.prettyURL());
	lineFile->setText(fi.name());

	textSize->setText(KIO::convertSize(fi.size()));
	textOwner->setText(QString("%1").arg(fi.user()));
	textGroup->setText(QString("%1").arg(fi.group()));
	textPermissions->setText(fi.permissionsString());
    
	QDateTime abs;
	abs.setTime_t(fi.time(KIO::UDS_CREATION_TIME));
	textCreated->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
	abs.setTime_t(fi.time(KIO::UDS_ACCESS_TIME));
	textLastRead->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
	abs.setTime_t(fi.time(KIO::UDS_MODIFICATION_TIME));
	textLastMod->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
    }
}

// set local file for KEXIF
void SQ_ImageProperties::setFile(const QString &_file)
{
    file = _file;
}
