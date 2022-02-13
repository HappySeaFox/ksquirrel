/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_Options::init()
{
    int tp;
    SQ_Config *kconf = SQ_Config::instance();

    SQ_Config::instance()->setGroup("Main");

    checkMinimize->setChecked(kconf->readBoolEntry("minimize to tray", true));
    checkSync->setChecked(kconf->readBoolEntry("sync", true));

    SQ_Config::instance()->setGroup("Libraries");

    checkMonitor->setChecked(kconf->readBoolEntry("monitor", true));
    checkFAMMessage->setChecked(kconf->readBoolEntry("show dialog", true));

    KFile::Mode mode = static_cast<KFile::Mode>(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);

    SQ_Config::instance()->setGroup("Fileview");

    tp = kconf->readNumEntry("sync type", 0);
    buttonGroupSync->setButton(tp);
    tp = kconf->readNumEntry("set path", 0);
    buttonGroupSetPath->setButton(tp);
    kURLReqOpenCustom->setURL(kconf->readEntry("custom directory", "/"));
    tp = kconf->readNumEntry("click policy", 0);
    buttonGroupClick->setButton(tp);

    checkSaveHistory->setChecked(kconf->readBoolEntry("history", true));
    checkRunUnknown->setChecked(kconf->readBoolEntry("run unknown", true));
    checkSupportAr->setChecked(kconf->readBoolEntry("archives", true));
    checkJumpFirst->setChecked(kconf->readBoolEntry("tofirst", true));
    checkDisableDirs->setChecked(kconf->readBoolEntry("disable_dirs", false));

    SQ_Config::instance()->setGroup("GL view");

// Init GLView page
    QPixmap p1;
    checkStatus->setChecked(kconf->readBoolEntry("hide_sbar", true));
    checkDrawQuads->setChecked(kconf->readBoolEntry("alpha_bkgr", true));
    checkMarks->setChecked(kconf->readBoolEntry("marks", true));    

    tp = kconf->readNumEntry("GL view background type", 0);
    buttonGroupColor->setButton(tp);
    widgetStack4->raiseWidget(tp);

    kColorSystem->setColor(colorGroup().color(QColorGroup::Base));

    tp = kconf->readNumEntry("zoom limit", 0);
    buttonGroupZoomLimit->setButton(tp);
    spinZoomMin->setValue(kconf->readNumEntry("zoom_min", 1));
    spinZoomMax->setValue(kconf->readNumEntry("zoom_max", 10000));

    custpixmap = kconf->readEntry("GL view custom texture", "");
    kURLReqCustomTexture->setURL(custpixmap);

    if(!custpixmap.isEmpty())
        slotValidateCustomTexture();

    tp = kconf->readNumEntry("scroll", 0);
    buttonGroupScrolling->setButton(tp);
    sliderAngle->setRange(0, 180, 1, true);
    sliderAngle->setValue(kconf->readNumEntry("angle", 90));
    spinZoomFactor->setRange(0, 300, 1, true);
    spinZoomFactor->setValue(kconf->readNumEntry("zoom", 25));
    sliderMove->setRange(1, 50, 1, true);
    sliderMove->setValue(kconf->readNumEntry("move", 5));

    QColor color;
    color.setNamedColor(kconf->readEntry("GL view background", "#cccccc"));
    kColorGLbackground->setColor(color);

    SQ_Config::instance()->setGroup("Thumbnails");

    spinCacheSize->setRange(10, 104857, 10, true);
    spinMargin->setRange(0, 20, 1, true);
    spinMargin->setValue(kconf->readNumEntry("margin", 2));
    spinCacheSize->setValue(kconf->readNumEntry("cache", 1024*5));
    if(kconf->readBoolEntry("disable_mime", true)) checkMime->toggle();
    if(kconf->readBoolEntry("dont write", false)) checkNoWriteThumbs->toggle();
    if(kconf->readBoolEntry("extended", false)) checkExtended->toggle();
    if(kconf->readBoolEntry("tooltips", false)) checkTooltips->toggle();

    SQ_Config::instance()->setGroup("Edit tools");

    if(kconf->readBoolEntry("preview", false)) checkPreview->toggle();
    if(kconf->readBoolEntry("preview_dont", true)) checkDontGenerate->toggle();
    spinLargerW->setValue(kconf->readNumEntry("preview_larger_w", 1024));
    spinLargerH->setValue(kconf->readNumEntry("preview_larger_h", 768));
    if(kconf->readBoolEntry("multi", true)) checkMultiPaged->toggle();

    (void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("display", KIcon::Desktop, 32), i18n("Main"));    
    (void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, 32), i18n("Filing"));
    (void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("images", KIcon::Desktop, 32), i18n("Thumbnails"));
    (void)new SQ_IconListItem(listMain, QPixmap::fromMimeSource(locate("appdata", "images/listbox/image_win.png")), i18n("Image window"));
    (void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("binary", KIcon::Desktop, 32), i18n("Libraries"));
    (void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("edit", KIcon::Desktop, 32), i18n("Edit tools"));

    listMain->updateAndInstall(this);

    connect(listMain, SIGNAL(selectionChanged()), SLOT(slotShowPage()));

    kURLReqOpenCustom->setMode(mode);

    listGL->header()->hide();
    listGL->setSorting(-1);
    QListViewItem *item;
    QMap<QString, GLenum> v;

    v["GL_VENDOR"] = GL_VENDOR;
    v["GL_RENDERER"] = GL_RENDERER;
    v["GL_VERSION"] = GL_VERSION;

    for(QMap<QString, GLenum>::ConstIterator it = v.constBegin(); it != v.constEnd(); ++it)
    {
        item = listGL->findItem(it.key().latin1(), 0);
        if(item)
        {
            const GLubyte *b = glGetString(it.data());
            if(b)
            {
                QString s = (const char*)b;
                item->setText(1, s);
            }   
        }
    }

    fillAltCombo();

    SQ_Config::instance()->setGroup("Edit tools");

    QString lib = kconf->readEntry("altlibrary", "Portable Network Graphics");

    if(SQ_LibraryHandler::instance()->libraryByName(lib))
        comboAlt->setCurrentText(lib);
    else if(comboAlt->count() > 1)
        comboAlt->setCurrentItem(1);
}

int SQ_Options::start()
{
    int result = this->exec();

    if(result == QDialog::Accepted)
    {
        SQ_Config *kconf = SQ_Config::instance();

        kconf->setGroup("Fileview");
        kconf->writeEntry("set path", buttonGroupSetPath->selectedId());
        kconf->writeEntry("custom directory", kURLReqOpenCustom->url());
        kconf->writeEntry("sync type", buttonGroupSync->selectedId());
        kconf->writeEntry("click policy", buttonGroupClick->selectedId());
        kconf->writeEntry("history", checkSaveHistory->isChecked());
        kconf->writeEntry("run unknown", checkRunUnknown->isChecked());
        kconf->writeEntry("archives", checkSupportAr->isChecked());
        kconf->writeEntry("tofirst", checkJumpFirst->isChecked());
        kconf->writeEntry("disable_dirs", checkDisableDirs->isChecked());

        kconf->setGroup("Main");
        kconf->writeEntry("minimize to tray", checkMinimize->isChecked());
        kconf->writeEntry("sync", checkSync->isChecked());

        kconf->setGroup("Thumbnails");
        kconf->writeEntry("margin", spinMargin->value());
        kconf->writeEntry("cache", spinCacheSize->value());
        kconf->writeEntry("disable_mime", checkMime->isChecked());
        kconf->writeEntry("dont write", checkNoWriteThumbs->isChecked());
        kconf->writeEntry("extended", checkExtended->isChecked());
        kconf->writeEntry("tooltips", checkTooltips->isChecked());
    
        kconf->setGroup("GL view");
        kconf->writeEntry("GL view background", (kColorGLbackground->color()).name());
        kconf->writeEntry("GL view custom texture", custpixmap);
        kconf->writeEntry("GL view background type", buttonGroupColor->selectedId());
        kconf->writeEntry("zoom limit", buttonGroupZoomLimit->selectedId());
        kconf->writeEntry("alpha_bkgr", checkDrawQuads->isChecked());
        kconf->writeEntry("marks", checkMarks->isChecked());
        kconf->writeEntry("hide_sbar", checkStatus->isChecked());
        kconf->writeEntry("scroll", buttonGroupScrolling->selectedId());
        kconf->writeEntry("angle", sliderAngle->value());
        kconf->writeEntry("zoom", spinZoomFactor->value());
        kconf->writeEntry("zoom_min", spinZoomMin->value());
        kconf->writeEntry("zoom_max", spinZoomMax->value());
        kconf->writeEntry("move", sliderMove->value());

        kconf->setGroup("Libraries");
        kconf->writeEntry("monitor", checkMonitor->isChecked());
        kconf->writeEntry("show dialog", checkFAMMessage->isChecked());

        kconf->setGroup("Edit tools");
        kconf->writeEntry("preview", checkPreview->isChecked());
        kconf->writeEntry("preview_dont", checkDontGenerate->isChecked());
        kconf->writeEntry("preview_larger_w", spinLargerW->value());
        kconf->writeEntry("preview_larger_h", spinLargerH->value());
        kconf->writeEntry("altlibrary", comboAlt->currentText());
        kconf->writeEntry("multi", checkMultiPaged->isChecked());
    }

    return result;
}

QString SQ_Options::stringList2QString(QStringList list )
{
    QStringList::Iterator it = list.begin();
    QString out;
 
    while(it != list.end())
    {
        out = out + "*." + *it + " ";
        ++it;
    }
    
    out += "*.jpg *.jpe";

    return out.lower();
}

bool SQ_Options::validPixmap(QPixmap pix)
{
    return (isPowerOf2(pix.width()) && isPowerOf2(pix.height()) && pix.width() <= 256 && pix.width() == pix.height() && !pix.isNull());
}

bool SQ_Options::isPowerOf2( int tested )
{
    int i = 1;
    
    if(tested == 1)
        return true;
    
    for(;tested > i;i <<= 1)
    ;

    return i == tested;
}

void SQ_Options::slotNewCustomTexture( const QString & path)
{
    QPixmap p1(path);
    
    if(validPixmap(p1))
    {
        custpixmap = path;
        textCustomValidate->setText(QString::fromLatin1("%1x%2: OK")
            .arg(p1.width())
            .arg(p1.height()));
    }
    else
        textCustomValidate->setText(i18n(QString::fromLatin1("Wrong dimensions: %1x%2."))
            .arg(p1.width())
            .arg(p1.height()));
}

void SQ_Options::slotShowPage()
{
    int id = listMain->currentItem();
    
    widgetStack1->raiseWidget(id);
    widgetStackLines->raiseWidget(id);
}

void SQ_Options::slotCustomTextureToggled( bool en)
{
    kURLReqCustomTexture->setEnabled(en);
    groupBox3->setEnabled(en);
}

void SQ_Options::slotValidateCustomTexture()
{
    slotNewCustomTexture(kURLReqCustomTexture->url());
}

void SQ_Options::paletteChange( const QPalette &oldPalette )
{
    QDialog::paletteChange(oldPalette);

    kColorSystem->setColor(colorGroup().color(QColorGroup::Base));
}

void SQ_Options::slotSetSystemBack( const QColor & )
{
    kColorSystem->setColor(colorGroup().color(QColorGroup::Base));
}

void SQ_Options::fillAltCombo()
{
    QPixmap pix;

    // return anyway
    if(!SQ_LibraryHandler::instance()->count())
        return;

    QValueVector<SQ_LIBRARY>::iterator   BEGIN = SQ_LibraryHandler::instance()->begin();
    QValueVector<SQ_LIBRARY>::iterator      END = SQ_LibraryHandler::instance()->end();

    for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;++it)
    {
        if(pix.convertFromImage((*it).mime))
        {
            if((*it).writable)
            comboAlt->insertItem(pix, (*it).quickinfo);
        }
    }
}
