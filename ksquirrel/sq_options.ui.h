/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/*
 *  SQ_Options is a configuration dialog for KSquirrel.
 */

// for KIPI defs
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SQ_HAVE_KIPI
#include <libkipi/pluginloader.h>
#endif

void SQ_Options::init()
{
    buttonOk->setIconSet(SQ_IconLoader::instance()->loadIcon("ok", KIcon::Desktop, KIcon::SizeSmall));
    int tp;
    SQ_Config *kconf = SQ_Config::instance();
    QColor color;

    pagesNumber->setRange(1, 1000, 1, true);

    kconf->setGroup("Main");

    checkKIPIDemand->setChecked(kconf->readBoolEntry("kipi_ondemand", true));
    tp = kconf->readNumEntry("applyto", SQ_CodecSettings::Both);
    buttonGroupCS->setButton(tp);
    checkMinimize->setChecked(kconf->readBoolEntry("minimize to tray", false));
    checkSync->setChecked(kconf->readBoolEntry("sync", false));
    checkSplash->setChecked(kconf->readBoolEntry("splash", true));
    checkAnime->setChecked(kconf->readBoolEntry("anime_dont", false));
    checkTreat->setChecked(kconf->readBoolEntry("treat", true));

    KFile::Mode mode = static_cast<KFile::Mode>(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);

    kconf->setGroup("Fileview");

    tp = kconf->readNumEntry("sync type", 0);
    buttonGroupSync->setButton(tp);
    tp = kconf->readNumEntry("set path", 0);
    buttonGroupSetPath->setButton(tp);
    kURLReqOpenCustom->setURL(kconf->readEntry("custom directory", "/"));

    spinLines->setRange(1, 20, 1, true);
    spinLines->setValue(kconf->readNumEntry("tooltips_lines", 6));
    checkTooltipPreview->setChecked(kconf->readBoolEntry("tooltips_preview", false));
    checkTooltips->setChecked(kconf->readBoolEntry("tooltips", false));
    checkInactive->setChecked(kconf->readBoolEntry("tooltips_inactive", true));
    checkSaveHistory->setChecked(kconf->readBoolEntry("history", true));
    checkRunUnknown->setChecked(kconf->readBoolEntry("run unknown", false));
    checkSupportAr->setChecked(kconf->readBoolEntry("archives", true));
    checkJumpFirst->setChecked(kconf->readBoolEntry("tofirst", true));
    checkDisableDirs->setChecked(kconf->readBoolEntry("disable_dirs", false));
    checkCalc->setChecked(kconf->readBoolEntry("calculate", false));
    checkDrag->setChecked(kconf->readBoolEntry("drag", true));

// Init GLView page
    kconf->setGroup("GL view");

    QPixmap p1;
    checkProgress->setChecked(kconf->readBoolEntry("progressiv", true));
    checkDrawQuads->setChecked(kconf->readBoolEntry("alpha_bkgr", true));
    checkMarks->setChecked(kconf->readBoolEntry("marks", true));    

    // tabs
    checkTabs->setChecked(kconf->readBoolEntry("tabs", false));    
    checkTabAsk->setChecked(kconf->readBoolEntry("tabs_ask", false));    
    checkTabClose->setChecked(kconf->readBoolEntry("tabs_close", true));    

    tp = kconf->readNumEntry("double_click", 0);
    buttonGroupDC->setButton(tp);
    tp = kconf->readNumEntry("load_pages", 0);
    buttonGroupPages->setButton(tp);
    pagesNumber->setEnabled(tp == 2);
    tp = kconf->readNumEntry("load_pages_number", 1);
    pagesNumber->setValue(tp);

    tp = kconf->readNumEntry("GL view background type", 1);
    buttonGroupColor->setButton(tp);
    widgetStack4->raiseWidget(tp);

    kColorSystem->setColor(colorGroup().color(QColorGroup::Base));

    tp = kconf->readNumEntry("zoom limit", 1);
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

    color.setNamedColor(kconf->readEntry("GL view background", "#4e4e4e"));
    kColorGLbackground->setColor(color);

    kconf->setGroup("Sidebar");

    tp = kconf->readNumEntry("recursion_type", 0);
    buttonGroupRecurs->setButton(tp);
    checkDevice->setChecked(kconf->readBoolEntry("mount_device", false));
    checkMountOptions->setChecked(kconf->readBoolEntry("mount_options", false));
    checkMountFS->setChecked(kconf->readBoolEntry("mount_fstype", true));
    color.setNamedColor(kconf->readEntry("preview_background", "#4e4e4e"));
    previewColor->setColor(color);
    color.setNamedColor(kconf->readEntry("preview_text", "#ffffff"));
    previewTextColor->setColor(color);
    preview->setChecked(kconf->readBoolEntry("preview", true));
    checkPreviewText->setChecked(kconf->readBoolEntry("preview_text_enable", true));
    previewDont->setChecked(kconf->readBoolEntry("preview_dont", true));
    previewDelay->setRange(50, 2000, 50, true);
    previewDelay->setValue(kconf->readNumEntry("preview_delay", 400));

    kconf->setGroup("Thumbnails");

    spinCacheSize->setRange(0, 104857, 10, true);
    spinMargin->setRange(0, 20, 1, true);
    spinMargin->setValue(kconf->readNumEntry("margin", 2));
    spinCacheSize->setValue(kconf->readNumEntry("cache", 1024*5));
    checkNoWriteThumbs->setChecked(kconf->readBoolEntry("dont write", false));
    checkMark->setChecked(kconf->readBoolEntry("mark", false));
    checkLazy->setChecked(kconf->readBoolEntry("lazy", true));
    spinLazy->setRange(10, 5000, 100, true);
    spinLazy->setValue(kconf->readNumEntry("lazy_delay", 500));
    spinRows->setRange(0, 5, 1, true);
    spinRows->setValue(kconf->readNumEntry("rows", 2));


    new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("display", KIcon::Desktop, KIcon::SizeMedium), i18n("General"));    
    new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, KIcon::SizeMedium), i18n("Filing"));
    new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("images", KIcon::Desktop, KIcon::SizeMedium), i18n("Thumbnails"));
    new SQ_IconListItem(listMain, QPixmap::fromMimeSource(locate("appdata", "images/listbox/image_win.png")), i18n("Image window"));
    new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("penguin", KIcon::Desktop, KIcon::SizeMedium), i18n("Sidebar"));

#ifdef SQ_HAVE_KIPI
    new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("kipi", KIcon::Desktop, KIcon::SizeMedium), i18n("KIPI"));

	QWidget *pg6 = widgetStack->widget(5);
	QGridLayout *pageLayout6 = new QGridLayout(pg6, 2, 1, 0, -1, "pageLayout_6");
	kipi = new KIPI::ConfigWidget(pg6);
	pageLayout6->addWidget(textLabelKIPI, 0, 0);
	pageLayout6->addWidget(kipi, 1, 0);
	checkKIPIDemand->setEnabled(true);
#endif

    listMain->updateAndInstall(this);

    connect(listMain, SIGNAL(selectionChanged()), SLOT(slotShowPage()));

    kURLReqOpenCustom->setMode(mode);
}

int SQ_Options::start()
{
    int result = QDialog::exec();

    if(result == QDialog::Accepted)
    {
#ifdef SQ_HAVE_KIPI
	static_cast<KIPI::ConfigWidget *>(kipi)->apply();
#endif

        SQ_Config *kconf = SQ_Config::instance();

        kconf->setGroup("Fileview");
        kconf->writeEntry("set path", buttonGroupSetPath->selectedId());
        kconf->writeEntry("custom directory", kURLReqOpenCustom->url());
        kconf->writeEntry("sync type", buttonGroupSync->selectedId());
        kconf->writeEntry("history", checkSaveHistory->isChecked());
        kconf->writeEntry("run unknown", checkRunUnknown->isChecked());
        kconf->writeEntry("archives", checkSupportAr->isChecked());
        kconf->writeEntry("tofirst", checkJumpFirst->isChecked());
        kconf->writeEntry("disable_dirs", checkDisableDirs->isChecked());
        kconf->writeEntry("calculate", checkCalc->isChecked());
        kconf->writeEntry("drag", checkDrag->isChecked());
        kconf->writeEntry("tooltips", checkTooltips->isChecked());
        kconf->writeEntry("tooltips_inactive", checkInactive->isChecked());
        kconf->writeEntry("tooltips_lines", spinLines->value());
        kconf->writeEntry("tooltips_preview", checkTooltipPreview->isChecked());

        kconf->setGroup("Main");
        kconf->writeEntry("applyto", buttonGroupCS->selectedId());
        kconf->writeEntry("minimize to tray", checkMinimize->isChecked());
        kconf->writeEntry("sync", checkSync->isChecked());
        kconf->writeEntry("splash", checkSplash->isChecked());
        kconf->writeEntry("anime_dont", checkAnime->isChecked());
        kconf->writeEntry("kipi_ondemand", checkKIPIDemand->isChecked());
        kconf->writeEntry("treat", checkTreat->isChecked());

        kconf->setGroup("Thumbnails");
        kconf->writeEntry("margin", spinMargin->value());
        kconf->writeEntry("cache", spinCacheSize->value());
        kconf->writeEntry("dont write", checkNoWriteThumbs->isChecked());
        kconf->writeEntry("mark", checkMark->isChecked());
        kconf->writeEntry("lazy", checkLazy->isChecked());
        kconf->writeEntry("lazy_delay", spinLazy->value());
        kconf->writeEntry("rows", spinRows->value());

        kconf->setGroup("GL view");
        kconf->writeEntry("double_click", buttonGroupDC->selectedId());
        kconf->writeEntry("load_pages", buttonGroupPages->selectedId());
        kconf->writeEntry("load_pages_number", pagesNumber->value());
        kconf->writeEntry("GL view background", kColorGLbackground->color().name());
        kconf->writeEntry("GL view custom texture", custpixmap);
        kconf->writeEntry("GL view background type", buttonGroupColor->selectedId());
        kconf->writeEntry("zoom limit", buttonGroupZoomLimit->selectedId());
        kconf->writeEntry("alpha_bkgr", checkDrawQuads->isChecked());
        kconf->writeEntry("marks", checkMarks->isChecked());
        kconf->writeEntry("progressiv", checkProgress->isChecked());
        kconf->writeEntry("scroll", buttonGroupScrolling->selectedId());
        kconf->writeEntry("angle", sliderAngle->value());
        kconf->writeEntry("zoom", spinZoomFactor->value());
        kconf->writeEntry("zoom_min", spinZoomMin->value());
        kconf->writeEntry("zoom_max", spinZoomMax->value());
        kconf->writeEntry("move", sliderMove->value());
        kconf->writeEntry("tabs", checkTabs->isChecked());
        kconf->writeEntry("tabs_ask", checkTabAsk->isChecked());
        kconf->writeEntry("tabs_close", checkTabClose->isChecked());

        kconf->setGroup("Sidebar");
        kconf->writeEntry("recursion_type", buttonGroupRecurs->selectedId());
        kconf->writeEntry("mount_device", checkDevice->isChecked());
        kconf->writeEntry("mount_options", checkMountOptions->isChecked());
        kconf->writeEntry("mount_fstype", checkMountFS->isChecked());
        kconf->writeEntry("preview_background", previewColor->color().name());
        kconf->writeEntry("preview_text", previewTextColor->color().name());
        kconf->writeEntry("preview_text_enable", checkPreviewText->isChecked());
        kconf->writeEntry("preview_delay", previewDelay->value());
        kconf->writeEntry("preview_dont", previewDont->isChecked());
        kconf->writeEntry("preview", preview->isChecked());
    }

    return result;
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
        textCustomValidate->setText(i18n("Wrong dimensions: %1x%2.")
            .arg(p1.width())
            .arg(p1.height()));
}

void SQ_Options::slotShowPage()
{
    int id = listMain->currentItem();

    widgetStack->raiseWidget(id);
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
