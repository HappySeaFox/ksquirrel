/***************************************************************************
                          sq_glwidget_stuff.cpp  -  description
                             -------------------
    begin                : Wed Oct 31 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel@mail.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qclipboard.h>
#include <qslider.h>
#include <qdragobject.h>
#include <qtimer.h>
#include <qlabel.h>

#include <kapplication.h>
#include <kaction.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <ktabbar.h>
#include <kcursor.h>
#include <ktempfile.h>
#include <kmessagebox.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_libraryhandler.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_glwidget_helpers.h"
#include "sq_imageproperties.h"
#include "sq_helpwidget.h"
#include "sq_imagebcg.h"
#include "sq_imagefilter.h"
#include "sq_filedialog.h"
#include "sq_iconloader.h"
#include "sq_codecsettingsskeleton.h"
#include "sq_externaltool.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_popupmenu.h"
#include "sq_glselectionpainter.h"
#include "sq_utils.h"

#ifdef SQ_HAVE_KEXIF
#include <libkexif/kexifdata.h>
#include <libexif/exif-data.h>
#endif

#include <ksquirrel-libs/fmt_codec_base.h>
#include <ksquirrel-libs/error.h>

#include "file_broken.xpm"

// Create actions
void SQ_GLWidget::createActions()
{
    pASelectionRect = new KToggleAction(i18n("Rectangle") + "\tCtrl+R", QPixmap(locate("appdata", "images/actions/glselection_rect.png")), 0, this, SLOT(slotSelectionRect()), ac, "SQ Selection Rect");
    pASelectionEllipse = new KToggleAction(i18n("Ellipse") + "\tCtrl+E", QPixmap(locate("appdata", "images/actions/glselection_ellipse.png")), 0, this, SLOT(slotSelectionEllipse()), ac, "SQ Selection Ellipse");
    pASelectionClear = new KAction(i18n("Clear") + "\tCtrl+C", 0, 0, this, SLOT(slotSelectionClear()), ac, "SQ Selection Clear");

    pAZoomW = new KToggleAction(i18n("Fit width"), QPixmap(locate("appdata", "images/actions/zoomW.png")), 0, this, SLOT(slotZoomW()), ac, "SQ ZoomW");
    pAZoomH = new KToggleAction(i18n("Fit height"), QPixmap(locate("appdata", "images/actions/zoomH.png")), 0, this, SLOT(slotZoomH()), ac, "SQ ZoomH");
    pAZoomWH = new KToggleAction(i18n("Fit image"), QPixmap(locate("appdata", "images/actions/zoomWH.png")), 0, this, SLOT(slotZoomWH()), ac, "SQ ZoomWH");
    pAZoom100 = new KToggleAction(i18n("Zoom 100%"), QPixmap(locate("appdata", "images/actions/zoom100.png")), 0, this, SLOT(slotZoom100()), ac, "SQ Zoom100");
    pAZoomLast = new KToggleAction(i18n("Leave previous zoom"), QPixmap(locate("appdata", "images/actions/zoomlast.png")), 0, this, SLOT(slotZoomLast()), ac, "SQ ZoomLast");
    pAIfLess = new KToggleAction(i18n("Ignore, if the image is less than window"), QPixmap(locate("appdata", "images/actions/ifless.png")), 0, 0, 0, ac, "if less");

    pAFull = new KToggleAction(QString::null, 0, 0, 0, ac, "SQ GL Full");

    QString squirrel_zoom_actions = QString::fromLatin1("squirrel_zoom_actions");
    pAZoomW->setExclusiveGroup(squirrel_zoom_actions);
    pAZoomH->setExclusiveGroup(squirrel_zoom_actions);
    pAZoomWH->setExclusiveGroup(squirrel_zoom_actions);
    pAZoom100->setExclusiveGroup(squirrel_zoom_actions);
    pAZoomLast->setExclusiveGroup(squirrel_zoom_actions);

    QString squirrel_selection_type = QString::fromLatin1("squirrel_selection_type");
    pASelectionEllipse->setExclusiveGroup(squirrel_selection_type);
    pASelectionRect->setExclusiveGroup(squirrel_selection_type);

    connect(pAIfLess, SIGNAL(toggled(bool)), this, SLOT(slotZoomIfLess()));
    connect(pAFull, SIGNAL(toggled(bool)), KSquirrel::app(), SLOT(slotFullScreen(bool)));

    SQ_Config::instance()->setGroup("GL view");

    pAIfLess->setChecked(SQ_Config::instance()->readBoolEntry("ignore", true));
}

void SQ_GLWidget::createToolbar()
{
    zoom = new KPopupMenu;
    selectionMenu = new KPopupMenu;
    SQ_ToolButton    *pATool;

    SQ_ToolBar *toolbar = SQ_GLView::window()->toolbar();

    pASelectionRect->plug(selectionMenu);
    pASelectionEllipse->plug(selectionMenu);
    selectionMenu->insertSeparator();
    pASelectionClear->plug(selectionMenu);

    pAZoom100->plug(zoom);
    pAZoomLast->plug(zoom);
    zoom->insertSeparator();
    pAZoomW->plug(zoom);
    pAZoomH->plug(zoom);
    pAZoomWH->plug(zoom);
    zoom->insertSeparator();
    pAIfLess->plug(zoom);

    switch(zoom_type)
    {
        case 0: pAZoomW->setChecked(true); break;
        case 1: pAZoomH->setChecked(true); break;
        case 3: pAZoom100->setChecked(true); break;
        case 4: pAZoomLast->setChecked(true); break;

        // "case 2" too
        default: pAZoomWH->setChecked(true);
    }

/*
 * We will create QToolButtons and put them in toolbar.
 * Of course, we can just KAction::plug(), BUT plugged KActions
 * will produce buttons, which cann't be clicked twise! I think
 * plugged KActions will treat our attempt as double-click, not two single-clicks.
 * On the other hand, we can click QToolButton as frequently as we want.
 *
 * Plugged KActions also don't know about autorepeat :(
 */
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_first.png")), i18n("Go to first image"), this, SLOT(slotFirst()), toolbar);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_prev.png")), i18n("Previous image"), this, SLOT(slotPrev()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_next.png")), i18n("Next image"), this, SLOT(slotNext()), toolbar);
    pATool->setAutoRepeat(true);
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_last.png")), i18n("Go to last image"), this, SLOT(slotLast()), toolbar);

    // some toolbuttons need autorepeat...
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/zoom+.png")), i18n("Zoom +"), this, SLOT(slotZoomPlus()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/zoom-.png")), i18n("Zoom -"), this, SLOT(slotZoomMinus()), toolbar);
    pATool->setAutoRepeat(true);
    pAToolZoom = new SQ_ToolButtonPopup(QPixmap(locate("appdata", "images/actions/zoom_template.png")), i18n("Zoom"), toolbar);
    pAToolZoom->setPopup(zoom);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/rotateL.png")), i18n("Rotate left"), this, SLOT(slotRotateLeft()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/rotateR.png")), i18n("Rotate right"), this, SLOT(slotRotateRight()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/flipV.png")), i18n("Flip vertically"), this, SLOT(slotFlipV()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/flipH.png")), i18n("Flip horizontally"), this, SLOT(slotFlipH()), toolbar);
    pATool->setAutoRepeat(true);
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/reload.png")), i18n("Normalize"), this, SLOT(slotMatrixReset()), toolbar);

    pAToolFull = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/fullscreen.png")), i18n("Fullscreen"), pAFull, SLOT(activate()), toolbar);
    pAToolFull->setToggleButton(true);
    pAToolImages = new SQ_ToolButtonPopup(QPixmap(locate("appdata", "images/actions/images.png")), i18n("Select image"), toolbar);
    pAToolImages->setPopup(images);
    SQ_ToolButtonPopup *pAToolSel = new SQ_ToolButtonPopup(QPixmap(locate("appdata", "images/actions/glselection.png")), i18n("Selection"), toolbar);
    pAToolSel->setPopup(selectionMenu);
    pAToolQuick = new SQ_ToolButton(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, 22), i18n("Codec Settings"), this, SLOT(slotShowCodecSettings()), toolbar);
    pAToolQuick->setEnabled(false);
    pAToolProp = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/prop.png")), i18n("Image Properties"), this, SLOT(slotProperties()), toolbar);
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/shownav.png")), i18n("Show navigator"), this, SLOT(slotShowNav()), toolbar);

    slider_zoom = new QSlider(1, 38, 2, 19, Qt::Horizontal, toolbar);
    slider_zoom->setTickmarks(QSlider::Below);
    slider_zoom->setTickInterval(19);
    connect(slider_zoom, SIGNAL(valueChanged(int)), this, SLOT(slotSetZoomPercents(int)));
    toolbar->insertWidget(1000, 0, slider_zoom);
    toolbar->setItemAutoSized(1000);
    toolbar->alignItemRight(1000);
}

// Show image properties.
void SQ_GLWidget::slotProperties()
{
    if(tab->broken || tab->finfo.image.empty()) return;

    // Stop animation...
    stopAnimation();

    const int real_size = tab->finfo.image[tab->current].w * tab->finfo.image[tab->current].h * sizeof(RGBA);
    QString sz = KIO::convertSize(real_size);
    QStringList list;

    QValueVector<QPair<QString,QString> > meta;

    if(!tab->finfo.meta.empty())
    {
        for(std::vector<fmt_metaentry>::iterator it = tab->finfo.meta.begin();it != tab->finfo.meta.end();++it)
        {
            meta.append(QPair<QString,QString>((*it).group, (*it).data));
        }
    }

    // save tab->current image parameters and some additioanl information
    // in list
    list    << tab->quickImageInfo
            << QString::fromLatin1("%1x%2").arg(tab->finfo.image[tab->current].w).arg(tab->finfo.image[tab->current].h)
            << QString::fromLatin1("%1").arg(tab->finfo.image[tab->current].bpp)
            << tab->finfo.image[tab->current].colorspace
            << tab->finfo.image[tab->current].compression
            << sz
            << QString::fromLatin1("%1").arg((double)real_size / tab->fmt_size, 0, 'f', 2)
            << ((tab->finfo.image[tab->current].interlaced) ? i18n("yes") : i18n("no"))
            << QString::fromLatin1("%1").arg(errors)
            << QString::fromLatin1("%1").arg(tab->finfo.image.size())
            << QString::fromLatin1("#%1").arg(tab->current+1)
            << QString::fromLatin1("%1").arg(tab->finfo.image[tab->current].delay);

    // create dialog and setup it
    SQ_ImageProperties prop(this);
    prop.setFile(tab->m_File);
    prop.setURL(tab->m_original);
    prop.setParams(list);
    prop.setMetaInfo(meta);

    // show!
    prop.exec();

    // restore animation
    if(!manualBlocked())
        startAnimation();
}

void SQ_GLWidget::findCloserTiles(int w, int h, std::vector<int> &x, std::vector<int> &y)
{
    static int s = 9;
    static int dims[10] = { 2, 4, 8, 16, 32, 64, 128, 256, 512 };

    int *dd[2] = { &w, &h };
    std::vector<int> *dv[2] = { &x, &y };
    int del;

    for(int ff = 0;ff < 2;ff++)
    {
        if(*dd[ff] == 1) *dd[ff]=2;
        else if((*dd[ff])%2) (*dd[ff])++;

        while((*dd[ff]) >= 512)
        {
            (*dv[ff]).push_back(512);
            (*dd[ff]) -= 512;
        }

        for(int i = 0;i < s-1;i++)
            if((*dd[ff]) >= dims[i] && (*dd[ff]) < dims[i+1])
            {
                del = dims[i] + (dims[i]>>1);

                if((*dd[ff]) <= del)
                {
                    (*dv[ff]).push_back(dims[i]);
                    (*dd[ff]) -= dims[i];
                }
                else
                {
                    (*dv[ff]).push_back(dims[i+1]);
                    (*dd[ff]) -= dims[i+1];
                }

                i = -1;
            }
    }
}

QPair<int, int> SQ_GLWidget::calcRealDimensions(Parts &p, int y, int x)
{
    int rw = 0, rh = 0;
    int toy = y == -1 ? p.tilesy.size() : y;
    int tox = x == -1 ? p.tilesx.size() : x;

    std::vector<int>::iterator itEnd = p.tilesx.end();

    for(std::vector<int>::iterator it = p.tilesx.begin();it != itEnd && tox--;++it)
        rw += (*it);

    itEnd = p.tilesy.end();

    for(std::vector<int>::iterator it = p.tilesy.begin();it != itEnd && toy--;++it)
        rh += (*it);

    return QPair<int, int>(rw, rh);
}

void SQ_GLWidget::slotShowCodecSettings()
{
    tab->lib = SQ_LibraryHandler::instance()->libraryForFile(tab->m_File);

    if(!tab->lib || tab->lib->config.isEmpty()) // oops ?
    {
        enableSettingsButton(false);
        return;
    }

    SQ_CodecSettingsSkeleton skel(this);

    connect(&skel, SIGNAL(apply()), this, SLOT(slotApplyCodecSettings()));

    skel.addSettingsWidget(tab->lib->config);
    skel.setCodecInfo(tab->lib->mime, tab->lib->quickinfo);
    skel.adjustSize();

    if(skel.exec(tab->lib->settings) == QDialog::Accepted)
        tab->lib->codec->set_settings(tab->lib->settings);
}

void SQ_GLWidget::slotApplyCodecSettings()
{
    if(tab->lib)
    {
        // new settings are already set by SQ_CodecSettingsSkeleton
        tab->lib->codec->set_settings(tab->lib->settings);
        m_original = tab->m_original;
        startDecoding(QString(tab->m_File));
    }
}

void SQ_GLWidget::slotSetCurrentImage(int id)
{
    if(tab->total == 1)
        return;

    images->setItemChecked(old_id, false);
    tab->current = images->itemParameter(id);

    images->setItemChecked(id, true);
    old_id = id;

    updateCurrentFileInfo();
    updateGL();
}

void SQ_GLWidget::slotImagesShown()
{
    if(tab->finfo.animated)
    {
        if(!timer_anim->isActive())
            blocked = false;
        else
        {
            stopAnimation();
            blocked = true;
        }
    }

    images->setItemChecked(old_id, false);
    int id = images->idAt(tab->current);
    images->setItemChecked(id, true);
    old_id = id;
}

void SQ_GLWidget::slotShowImages()
{
    images->exec(QCursor::pos());
}

void SQ_GLWidget::slotImagesHidden()
{
    if(blocked && tab->finfo.animated)
        startAnimation();
}

void SQ_GLWidget::slotShowHelp()
{
    SQ_HelpWidget help_w(this);

    help_w.exec();
}

void SQ_GLWidget::showExternalTools()
{
    SQ_ExternalTool::instance()->constPopupMenu()->exec(QCursor::pos());
}

// Delete current image (user pressed 'Delete' key).
void SQ_GLWidget::deleteWrapper()
{
    if(tab->m_original.isEmpty())
        return;

    KFileItemList list;
    list.append(new KFileItem(tab->m_original, QString::null, KFileItem::Unknown));

    SQ_WidgetStack::instance()->diroperator()->del(list, this);
}

void SQ_GLWidget::enableSettingsButton(bool enab)
{
    pAToolQuick->setEnabled(enab);
    menuImage->setItemEnabled(id_settings, enab);
}

void SQ_GLWidget::saveAs()
{
    if(!tab->lib || tab->finfo.image.empty()) // nothing to save
        return;

    SQ_FileDialog d(QString::null, this);

    // set filter: writable codecs without *.*
    d.setFilter(SQ_LibraryHandler::instance()->allFiltersFileDialogString(false, false));
    d.setOperationMode(KFileDialog::Saving);
    d.updateCombo(false);

    int result = d.exec();

    if(result == QDialog::Rejected || d.selectedURL().isEmpty())
        return;

    KURL url = d.selectedURL();
    QString path = url.isLocalFile() ? url.path() : tmp->name();

    SQ_LIBRARY *wlib = SQ_LibraryHandler::instance()->libraryByName(d.nameFilter());

    if(!wlib || !wlib->writestatic)
    {
        KMessageBox::error(this, i18n("Sorry, could not perfom write operation\nfor codec \"%1\"").arg(d.nameFilter()));
        return;
    }

    RGBA *buf = tab->parts[tab->current].buffer->data();

    fmt_image *im = &tab->finfo.image[tab->current];

    fmt_writeoptions opt;
    opt.interlaced = false;
    opt.alpha = im->hasalpha;
    opt.bitdepth = im->bpp;
    opt.compression_scheme = (wlib->opt.compression_scheme & CompressionNo) ? CompressionNo : CompressionInternal;
    opt.compression_level = wlib->opt.compression_def;

    int err = wlib->codec->write_init(QString(QFile::encodeName(path)), *im, opt);

    if(err != SQE_OK)
    {
        KMessageBox::error(this, i18n("Error writing image"));
        return;
    }

    err = wlib->codec->write_next();

    if(err != SQE_OK)
    {
        KMessageBox::error(this, i18n("Error writing image"));
        return;
    }

    wlib->codec->write_next_pass();

    int H = tab->parts[tab->current].h, W = tab->parts[tab->current].realw;
    int Y0 = wlib->opt.needflip ? (-H+1):0;
    int Y = wlib->opt.needflip ? 1:H;
    int f;

    for(int j = Y0;j < Y;j++)
    {
        f = (j < 0) ? -j : j;

        err = wlib->codec->write_scanline(buf + W*f);

        if(err != SQE_OK)
        {
            wlib->codec->write_close();
            KMessageBox::error(this, i18n("Error writing image"));
            return;
        }
    }

    wlib->codec->write_close();

    // copy to non-local directory
    if(!url.isLocalFile())
    {
        //             src   dst  perm  overwrite resume  progress
        KIO::Job *j = KIO::file_copy(path, url, -1,   true,     false,  false);

        connect(j, SIGNAL(result(KIO::Job *)), this, SLOT(slotCopyResult(KIO::Job *)));
    }
}

void SQ_GLWidget::slotCopyResult(KIO::Job *job)
{
    if(job->error())
    {
        if(KMessageBox::questionYesNoCancel(this, job->errorString() + "\n" + i18n("Try another location?")) == KMessageBox::Yes)
        {
            SQ_FileDialog d(QString::null, this);

            // set filter: writable codecs without *.*
            d.setFilter(SQ_LibraryHandler::instance()->allFiltersFileDialogString(false, false));
            d.setOperationMode(KFileDialog::Saving);
            d.updateCombo(false);

            int result = d.exec();

            if(result == QDialog::Rejected || d.selectedURL().isEmpty())
                return;

            KIO::Job *j = KIO::file_copy(tmp->name(), d.selectedURL(), -1, true, false, false);
            connect(j, SIGNAL(result(KIO::Job *)), this, SLOT(slotCopyResult(KIO::Job *)));
        }
    }
}

void SQ_GLWidget::toClipboard()
{
    if(!decoded || tab->broken)
        return;

    QImage im((uchar *)tab->parts[tab->current].buffer->data(), tab->parts[tab->current].realw, tab->parts[tab->current].realh, 32, 0, 0, QImage::LittleEndian);

    // image doesn't have extra regions
    if(tab->parts[tab->current].realw == tab->parts[tab->current].w && tab->parts[tab->current].realh == tab->parts[tab->current].h)
        KApplication::clipboard()->setImage(im, QClipboard::Clipboard);
    else
        KApplication::clipboard()->setImage(im.copy(0, 0, tab->parts[tab->current].w, tab->parts[tab->current].h), QClipboard::Clipboard);
}

void SQ_GLWidget::bcg()
{
    if(tab->broken || tab->finfo.image.empty()) return;

    SQ_ImageBCG _bcg(this);

    _bcg.setPreviewImage(generatePreview());

    connect(&_bcg, SIGNAL(bcg(SQ_ImageBCGOptions *)), this, SLOT(slotBCG(SQ_ImageBCGOptions *)));

    _bcg.exec();
}

void SQ_GLWidget::filter()
{
    if(tab->broken || tab->finfo.image.empty()) return;

    SQ_ImageFilter flt(this);

    flt.setPreviewImage(generatePreview());

    connect(&flt, SIGNAL(filter(SQ_ImageFilterOptions *)), this, SLOT(slotFilter(SQ_ImageFilterOptions *)));

    flt.exec();
}

void SQ_GLWidget::slotFilter(SQ_ImageFilterOptions *filtopt)
{
    QImage im((uchar *)tab->parts[tab->current].buffer->data(), tab->parts[tab->current].realw, tab->parts[tab->current].realh, 32, 0, 0, QImage::LittleEndian);
    QImage img = gls->valid() ? im.copy(tab->sx, tab->sy, tab->sw, tab->sh) : im;

    fmt_filters::image image((unsigned char *)img.bits(), img.width(), img.height());

    fmt_filters::rgba c = fmt_filters::white;

    switch(filtopt->type)
    {
        case F::fblend:      fmt_filters::blend(image, filtopt->rgb1, filtopt->_float); break;
        case F::fblur:       fmt_filters::blur(image, filtopt->_double1, filtopt->_double2);break;
        case F::fdesaturate: fmt_filters::desaturate(image, filtopt->_float); break;
        case F::fdespeckle:  fmt_filters::despeckle(image); break;
        case F::fedge:       fmt_filters::edge(image, filtopt->_double1); break;
        case F::femboss:     fmt_filters::emboss(image, filtopt->_double1, filtopt->_double2); break;
        case F::fequalize:   fmt_filters::equalize(image); break;
        case F::ffade:       fmt_filters::fade(image, filtopt->rgb1, filtopt->_float); break;
        case F::fflatten:    fmt_filters::flatten(image, filtopt->rgb1, filtopt->rgb2); break;
        case F::fimplode:    fmt_filters::implode(image, filtopt->_double1, c); break;
        case F::fnegative:   fmt_filters::negative(image); break;
        case F::fnoise:      fmt_filters::noise(image, (fmt_filters::NoiseType)filtopt->_uint); break;
        case F::foil:        fmt_filters::oil(image, filtopt->_double1); break;
        case F::fshade:      fmt_filters::shade(image, filtopt->_bool, filtopt->_double1, filtopt->_double2); break;
        case F::fsharpen:    fmt_filters::sharpen(image, filtopt->_double1, filtopt->_double2); break;
        case F::fsolarize:   fmt_filters::solarize(image, filtopt->_double1); break;
        case F::fspread:     fmt_filters::spread(image, filtopt->_uint); break;
        case F::fswapRGB:    fmt_filters::swapRGB(image, filtopt->_uint); break;
        case F::fswirl:      fmt_filters::swirl(image, filtopt->_double1, c); break;
        case F::fthreshold:  fmt_filters::threshold(image, filtopt->_uint); break;
        case F::fgray:       fmt_filters::gray(image); break;
        case F::fredeye:     fmt_filters::redeye(image, image.w, image.h, 0, 0, filtopt->_uint); break;
    }

    if(gls->valid()) bitBlt(&im, tab->sx, tab->sy, &img, 0, 0, img.width(), img.height());

    editUpdate();

    SQ_ImageFilter::instance()->setPreviewImage(generatePreview());
}

void SQ_GLWidget::slotBCG(SQ_ImageBCGOptions *bcgopt)
{
    QImage im((uchar *)tab->parts[tab->current].buffer->data(), tab->parts[tab->current].realw, tab->parts[tab->current].realh, 32, 0, 0, QImage::LittleEndian);
    QImage img = gls->valid() ? im.copy(tab->sx, tab->sy, tab->sw, tab->sh) : im;

    fmt_filters::image image((unsigned char *)img.bits(), img.width(), img.height());

    if(bcgopt->b)
        fmt_filters::brightness(image, bcgopt->b);

    if(bcgopt->c)
        fmt_filters::contrast(image, bcgopt->c);

    if(bcgopt->g != 100)
        fmt_filters::gamma(image, (double)bcgopt->g / 100.0);

    if(bcgopt->red || bcgopt->green || bcgopt->blue)
        fmt_filters::colorize(image, bcgopt->red, bcgopt->green, bcgopt->blue);

    if(gls->valid()) bitBlt(&im, tab->sx, tab->sy, &img, 0, 0, img.width(), img.height());

    editUpdate();

    SQ_ImageBCG::instance()->setPreviewImage(generatePreview());
}

QImage SQ_GLWidget::generatePreview()
{
    QImage im((uchar *)tab->parts[tab->current].buffer->data(), tab->parts[tab->current].realw, tab->parts[tab->current].realh, 32, 0, 0, QImage::LittleEndian);
    QImage img, ret;

    if(!gls->valid())
    {
        if(tab->parts[tab->current].realw == tab->parts[tab->current].w && tab->parts[tab->current].realh == tab->parts[tab->current].h)
            img = im;
        else
            img = im.copy(0, 0, tab->parts[tab->current].w, tab->parts[tab->current].h);
    }
    else
        img = im.copy(tab->sx, tab->sy, tab->sw, tab->sh);

    ret = SQ_Utils::scaleImage((unsigned char *)img.bits(), img.width(), img.height(), 160).swapRGB();

    SQ_Utils::exifRotate(QString::null, ret, tab->orient);

    return ret;
}

void SQ_GLWidget::editUpdate()
{
    int tlsy = tab->parts[tab->current].tilesy.size();

    glDeleteLists(tab->parts[tab->current].m_parts[0].list, tlsy);

    for(int i = 0;i < tlsy;i++)
        showFrames(i, &tab->parts[tab->current], false);

    updateGL();
}

void SQ_GLWidget::slotShowNav()
{
    KSquirrel::app()->setDemo(false);
    KSquirrel::app()->activate();
}

void SQ_GLWidget::initAccelsAndMenu()
{
    QPopupMenu *menuRotate = new QPopupMenu(menu);
    QPopupMenu *menuZoom = new QPopupMenu(menu);
    QPopupMenu *menuMove = new QPopupMenu(menu);
    QPopupMenu *menuWindow = new QPopupMenu(menu);

    menuImage = new QPopupMenu(menu);
    menuFile = new QPopupMenu(menu);

    menu->insertItem(SQ_IconLoader::instance()->loadIcon("icons", KIcon::Desktop, KIcon::SizeSmall), i18n("File"), menuFile);
    menu->insertItem(SQ_IconLoader::instance()->loadIcon("view_orientation", KIcon::Desktop, KIcon::SizeSmall), i18n("Rotate"), menuRotate);
    menu->insertItem(SQ_IconLoader::instance()->loadIcon("viewmag", KIcon::Desktop, KIcon::SizeSmall), i18n("Zoom"), menuZoom);
    menu->insertItem(i18n("Move"), menuMove);
    menu->insertItem(i18n("Window"), menuWindow);
    menu->insertItem(i18n("Image"), menuImage);

#define SQ_ADD_KACTION(b) \
    (new KAction(QString::null, b, this, SLOT(slotAccelActivated()), ac, QString::fromLatin1("action_%1").arg(b)))

    id_saveas = menuFile->insertItem(SQ_IconLoader::instance()->loadIcon("filesaveas", KIcon::Desktop, KIcon::SizeSmall), i18n("Save As..."), SQ_ADD_KACTION(Qt::Key_S), SLOT(activate()));
    menuFile->insertSeparator();
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/next16.png")), i18n("Next") + "\tPageDown", SQ_ADD_KACTION(Qt::Key_PageDown), SLOT(activate()));
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/prev16.png")), i18n("Previous") + "\tPageUp", SQ_ADD_KACTION(Qt::Key_PageUp), SLOT(activate()));
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/first16.png")), i18n("First") + "\tHome", SQ_ADD_KACTION(Qt::Key_Home), SLOT(activate()));
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/last16.png")), i18n("Last") + "\tEnd", SQ_ADD_KACTION(Qt::Key_End), SLOT(activate()));
    menuFile->insertSeparator();
    id_f5 = menuFile->insertItem(i18n("Copy to...") + "\tF5", SQ_ADD_KACTION(Qt::Key_F5), SLOT(activate()));
    id_f6 = menuFile->insertItem(i18n("Move to...") + "\tF6", SQ_ADD_KACTION(Qt::Key_F7), SLOT(activate()));
    id_f7 = menuFile->insertItem(i18n("Copy to last folder") + "\tF7", SQ_ADD_KACTION(Qt::Key_F6), SLOT(activate()));
    id_f8 = menuFile->insertItem(i18n("Move to last folder") + "\tF8", SQ_ADD_KACTION(Qt::Key_F8), SLOT(activate()));
    menuFile->insertSeparator();
    id_del = menuFile->insertItem(i18n("Delete") + "\tDelete", SQ_ADD_KACTION(Qt::Key_Delete), SLOT(activate()));

    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/rotateLeft16.png")), i18n("Rotate left") + "\tCtrl+Left", SQ_ADD_KACTION(Qt::Key_Left+CTRL), SLOT(activate()));
    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/rotateRight16.png")), i18n("Rotate right") + "\tCtrl+Right", SQ_ADD_KACTION(Qt::Key_Right+CTRL), SLOT(activate()));
    menuRotate->insertSeparator();
    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/18016.png")), i18n("Rotate 180'") + "\tCtrl+Up", SQ_ADD_KACTION(Qt::Key_Up+CTRL), SLOT(activate()));
    menuRotate->insertSeparator();
    menuRotate->insertItem(i18n("Rotate 1' left") + "\tAlt+Left", SQ_ADD_KACTION(Qt::Key_Left+ALT), SLOT(activate()));
    menuRotate->insertItem(i18n("Rotate 1' right") + "\tAlt+Right", SQ_ADD_KACTION(Qt::Key_Right+ALT), SLOT(activate()));

    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom+16.png")), i18n("Zoom +") + "\t+", SQ_ADD_KACTION(Qt::Key_Plus), SLOT(activate()));
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom-16.png")), i18n("Zoom -") + "\t-", SQ_ADD_KACTION(Qt::Key_Minus), SLOT(activate()));
    menuZoom->insertItem(i18n("Zoom 2x") + "\tCtrl++", SQ_ADD_KACTION(Qt::Key_Plus+CTRL), SLOT(activate()));
    menuZoom->insertItem(i18n("Zoom 1/2x") + "\tCtrl+-", SQ_ADD_KACTION(Qt::Key_Minus+CTRL), SLOT(activate()));
    menuZoom->insertSeparator();
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom10016.png")), QString::fromLatin1("100%") + "\t1", SQ_ADD_KACTION(Qt::Key_1), SLOT(activate()));
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom20016.png")), QString::fromLatin1("200%") + "\t2", SQ_ADD_KACTION(Qt::Key_2), SLOT(activate()));
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom30016.png")), QString::fromLatin1("300%") + "\t3", SQ_ADD_KACTION(Qt::Key_3), SLOT(activate()));
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom50016.png")), QString::fromLatin1("500%") + "\t5", SQ_ADD_KACTION(Qt::Key_5), SLOT(activate()));
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom70016.png")), QString::fromLatin1("700%") + "\t7", SQ_ADD_KACTION(Qt::Key_7), SLOT(activate()));
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom90016.png")), QString::fromLatin1("900%") + "\t9", SQ_ADD_KACTION(Qt::Key_9), SLOT(activate()));
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom100016.png")), QString::fromLatin1("1000%") + "\t0", SQ_ADD_KACTION(Qt::Key_0), SLOT(activate()));

    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveLeft16.png")), i18n("Move left") + "\tRight", SQ_ADD_KACTION(Qt::Key_Right), SLOT(activate()));
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveRight16.png")), i18n("Move right") + "\tLeft", SQ_ADD_KACTION(Qt::Key_Left), SLOT(activate()));
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveUp16.png")), i18n("Move up") + "\tDown", SQ_ADD_KACTION(Qt::Key_Down), SLOT(activate()));
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveDown16.png")), i18n("Move down") + "\tUp", SQ_ADD_KACTION(Qt::Key_Up), SLOT(activate()));

    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/animate16.png")), i18n("Start/stop animation") + "\tA", SQ_ADD_KACTION(Qt::Key_A), SLOT(activate()));
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/background16.png")), i18n("Hide/show background") + "\tB", SQ_ADD_KACTION(Qt::Key_B), SLOT(activate()));
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/tickmarks16.png")), i18n("Hide/show tickmarks") + "\tK", SQ_ADD_KACTION(Qt::Key_K), SLOT(activate()));
    menuImage->insertSeparator();
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/flipV16.png")), i18n("Flip vertically") + "\tV", SQ_ADD_KACTION(Qt::Key_V), SLOT(activate()));
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/flipH16.png")), i18n("Flip horizontally") + "\tH", SQ_ADD_KACTION(Qt::Key_H), SLOT(activate()));
    menuImage->insertSeparator();
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page116.png")), i18n("First page") + "\tF1", SQ_ADD_KACTION(Qt::Key_F1), SLOT(activate()));
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page216.png")), i18n("Previous page") + "\tF2", SQ_ADD_KACTION(Qt::Key_F2), SLOT(activate()));
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page316.png")), i18n("Next page") + "\tF3", SQ_ADD_KACTION(Qt::Key_F3), SLOT(activate()));
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page416.png")), i18n("Last page") + "\tF4", SQ_ADD_KACTION(Qt::Key_F4), SLOT(activate()));
    menuImage->insertSeparator();
    menuImage->insertItem(i18n("To clipboard") + "\tQ", SQ_ADD_KACTION(Qt::Key_Q), SLOT(activate()));
    menuImage->insertSeparator();
    id_settings = menuImage->insertItem(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall), i18n("Codec Settings") + "\tC", SQ_ADD_KACTION(Qt::Key_C), SLOT(activate()));
    menuImage->setItemEnabled(id_settings, false);
    menuImage->insertSeparator();
    menuImage->insertItem(i18n("Color balance...") + "\tD", SQ_ADD_KACTION(Qt::Key_D), SLOT(activate()));
    menuImage->insertItem(i18n("Apply filter...") + "\tU", SQ_ADD_KACTION(Qt::Key_U), SLOT(activate()));
    menuImage->insertItem(i18n("Crop") + "\tY", SQ_ADD_KACTION(Qt::Key_Y), SLOT(activate()));

    menuWindow->insertItem(QPixmap(locate("appdata", "images/menu/fullscreen16.png")), i18n("Fullscreen") + "\tF", SQ_ADD_KACTION(Qt::Key_F), SLOT(activate()));
    menuWindow->insertSeparator();
    menuWindow->insertItem(i18n("Previous tab") + "\tShift+Left", SQ_ADD_KACTION(Qt::Key_Left+SHIFT), SLOT(activate()));
    menuWindow->insertItem(i18n("Next tab") + "\tShift+Right", SQ_ADD_KACTION(Qt::Key_Right+SHIFT), SLOT(activate()));
    menuWindow->insertSeparator();
    menuWindow->insertItem(i18n("Close tab") + "\tW", SQ_ADD_KACTION(Qt::Key_W), SLOT(activate()));

    menu->insertSeparator();
    menu->insertItem(QPixmap(locate("appdata", "images/menu/reset16.png")), i18n("Reset") + "\tR", SQ_ADD_KACTION(Qt::Key_R), SLOT(activate()));
    id_prop = menu->insertItem(QPixmap(locate("appdata", "images/menu/prop16.png")), i18n("Properties") + "\tP", SQ_ADD_KACTION(Qt::Key_P), SLOT(activate()));
    menu->insertSeparator();
    menu->insertItem(i18n("Hotkeys") + "\t/", SQ_ADD_KACTION(Qt::Key_Slash), SLOT(activate()));
    menu->insertSeparator();
    menu->insertItem(QPixmap(locate("appdata", "images/menu/close16.png")), i18n("Close") + "\tX", SQ_ADD_KACTION(Qt::Key_X), SLOT(activate()));

    SQ_ADD_KACTION(Qt::Key_Down+CTRL);
    SQ_ADD_KACTION(Qt::Key_Equal);
    SQ_ADD_KACTION(Qt::Key_Equal+CTRL);
    SQ_ADD_KACTION(Qt::Key_N);
    SQ_ADD_KACTION(Qt::Key_Space);
    SQ_ADD_KACTION(Qt::Key_BackSpace);
    SQ_ADD_KACTION(Qt::Key_Escape);
    SQ_ADD_KACTION(Qt::Key_Return);
    SQ_ADD_KACTION(Qt::Key_Enter);
    SQ_ADD_KACTION(Qt::Key_Z);
    SQ_ADD_KACTION(Qt::Key_I);
    SQ_ADD_KACTION(Qt::Key_E);
    SQ_ADD_KACTION(Qt::Key_R+CTRL);
    SQ_ADD_KACTION(Qt::Key_E+CTRL);
    SQ_ADD_KACTION(Qt::Key_C+CTRL);
    SQ_ADD_KACTION(Qt::Key_Menu);
    SQ_ADD_KACTION(Qt::Key_M);
    SQ_ADD_KACTION(Qt::Key_4);
    SQ_ADD_KACTION(Qt::Key_6);
    SQ_ADD_KACTION(Qt::Key_8);
    SQ_ADD_KACTION(Qt::Key_Comma);
    SQ_ADD_KACTION(Qt::Key_Period);
    SQ_ADD_KACTION(Qt::Key_Asterisk);
    SQ_ADD_KACTION(Qt::Key_L);
}


void SQ_GLWidget::slotAccelActivated()
{
    KAction *accel = static_cast<KAction *>(const_cast<QObject *>(sender()));

    KShortcut ks = accel->shortcut();

         if(!ks.compare(Qt::Key_Left))        matrix_move(movefactor, 0);
    else if(!ks.compare(Qt::Key_Right))       matrix_move(-movefactor, 0);
    else if(!ks.compare(Qt::Key_Up))          matrix_move(0, -movefactor);
    else if(!ks.compare(Qt::Key_Down))        matrix_move(0, movefactor);
    else if(!ks.compare(Qt::Key_Equal) ||
            !ks.compare(Qt::Key_Plus))        slotZoomPlus();
    else if(!ks.compare(Qt::Key_Minus))       slotZoomMinus();
    else if(!ks.compare(Qt::Key_Equal+CTRL)  ||
            !ks.compare(Qt::Key_Plus+CTRL))   matrix_zoom(2.0f);
    else if(!ks.compare(Qt::Key_Minus+CTRL))  matrix_zoom(0.5f);
    else if(!ks.compare(Qt::Key_Q))           toClipboard();
    else if(!ks.compare(Qt::Key_V))           slotFlipV();
    else if(!ks.compare(Qt::Key_H))           slotFlipH();
    else if(!ks.compare(Qt::Key_Left+CTRL))   slotRotateLeft();
    else if(!ks.compare(Qt::Key_Right+CTRL))  slotRotateRight();
    else if(!ks.compare(Qt::Key_R))           slotMatrixReset();
    else if(!ks.compare(Qt::Key_Up+CTRL))     matrix_rotate(180.0f);
    else if(!ks.compare(Qt::Key_Down+CTRL))   matrix_rotate(-180.0f);
    else if(!ks.compare(Qt::Key_Left+ALT))    matrix_rotate(-1.0f);
    else if(!ks.compare(Qt::Key_Right+ALT))   matrix_rotate(1.0f);
    else if(!ks.compare(Qt::Key_Left+SHIFT))  SQ_GLView::window()->leftTab();
    else if(!ks.compare(Qt::Key_Right+SHIFT)) SQ_GLView::window()->rightTab();
    else if(!ks.compare(Qt::Key_W))           slotCloseRequest(SQ_GLView::window()->tabbar()->indexOf(SQ_GLView::window()->tabbar()->currentTab()));
    else if(!ks.compare(Qt::Key_N))           updateFilter(!linear);
    else if(!ks.compare(Qt::Key_PageDown) ||
            !ks.compare(Qt::Key_Space))       slotNext();
    else if(!ks.compare(Qt::Key_PageUp) ||
            !ks.compare(Qt::Key_BackSpace))   slotPrev();
    else if(!ks.compare(Qt::Key_X)      ||
            !ks.compare(Qt::Key_Escape) ||
            !ks.compare(Qt::Key_Return) ||
            !ks.compare(Qt::Key_Enter))      KSquirrel::app()->closeGLWidget();
    else if(!ks.compare(Qt::Key_P))          slotProperties();
    else if(!ks.compare(Qt::Key_Home))       slotFirst();
    else if(!ks.compare(Qt::Key_End))        slotLast();
    else if(!ks.compare(Qt::Key_C))          slotShowCodecSettings();
    else if(!ks.compare(Qt::Key_F))          toggleFullScreen();
    else if(!ks.compare(Qt::Key_Z))          slotZoomMenu();
    else if(!ks.compare(Qt::Key_S))          saveAs();
    else if(!ks.compare(Qt::Key_A))          slotToggleAnimate();
    else if(!ks.compare(Qt::Key_I))          slotShowImages();
    else if(!ks.compare(Qt::Key_F1))         jumpToImage(false);
    else if(!ks.compare(Qt::Key_F2))         prevImage();
    else if(!ks.compare(Qt::Key_F3))         nextImage();
    else if(!ks.compare(Qt::Key_F4))         jumpToImage(true);
    else if(!ks.compare(Qt::Key_F5) || !ks.compare(Qt::Key_F6))
    {
        // select a directory
        KURL url = KFileDialog::getExistingURL(lastCopy.prettyURL(), this);

        if(url.isEmpty())
            return;

        lastCopy = url;
        KIO::Job *job;

        if(!ks.compare(Qt::Key_F5))
            job = KIO::copy(tab->m_original, url);
        else
            job = KIO::move(tab->m_original, url);

        job->setWindow(this);
        connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotCopyJobResult(KIO::Job *)));
    }
    else if(!ks.compare(Qt::Key_F7) || !ks.compare(Qt::Key_F8))
    {
        KIO::Job *job;

        if(!ks.compare(Qt::Key_F6))
            job = KIO::copy(tab->m_original, lastCopy);
        else
            job = KIO::move(tab->m_original, lastCopy);

        job->setWindow(this);
        connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotCopyJobResult(KIO::Job *)));
    }
    else if(!ks.compare(Qt::Key_Slash))      slotShowHelp();
    else if(!ks.compare(Qt::Key_B))          toggleDrawingBackground();
    else if(!ks.compare(Qt::Key_K))          toogleTickmarks();
    else if(!ks.compare(Qt::Key_E))          showExternalTools();
    else if(!ks.compare(Qt::Key_Delete))     deleteWrapper();
    else if(!ks.compare(Qt::Key_D))          bcg();
    else if(!ks.compare(Qt::Key_Y))          crop();
    else if(!ks.compare(Qt::Key_U))          filter();
    else if(!ks.compare(Qt::Key_R+CTRL))          slotSelectionRect();
    else if(!ks.compare(Qt::Key_E+CTRL))          slotSelectionEllipse();
    else if(!ks.compare(Qt::Key_C+CTRL))          slotSelectionClear();
    else if(!ks.compare(Qt::Key_Menu) ||
            !ks.compare(Qt::Key_M))          menu->exec(QCursor::pos());
    else if(!ks.compare(Qt::Key_Comma))      slotZoomW();
    else if(!ks.compare(Qt::Key_Period))     slotZoomH();
    else if(!ks.compare(Qt::Key_Asterisk))   slotZoomWH();
    else if(!ks.compare(Qt::Key_L))
    {
        bool b = pAIfLess->isChecked();
        pAIfLess->setChecked(!b);
        slotZoomIfLess();
    }
    else
    {
        int val = -1;

             if(!ks.compare(Qt::Key_1)) val = 1;
        else if(!ks.compare(Qt::Key_2)) val = 2;
        else if(!ks.compare(Qt::Key_3)) val = 3;
        else if(!ks.compare(Qt::Key_4)) val = 4;
        else if(!ks.compare(Qt::Key_5)) val = 5;
        else if(!ks.compare(Qt::Key_6)) val = 6;
        else if(!ks.compare(Qt::Key_7)) val = 7;
        else if(!ks.compare(Qt::Key_8)) val = 8;
        else if(!ks.compare(Qt::Key_9)) val = 9;
        else if(!ks.compare(Qt::Key_0)) val = 10;

        if(val != -1)
        {
            if(tab->broken || tab->finfo.image.empty())
                return;

            oldZoom = getZoom();
            matrix_reset(false);
            matrix_zoom(val);
        }
    }
}

void SQ_GLWidget::slotCopyJobResult(KIO::Job *job)
{
    if(job->error())
        job->showErrorDialog(this);
}

void SQ_GLWidget::exifRotate(bool U)
{
#ifdef SQ_HAVE_KEXIF
    switch(tab->orient)
    {
        // flipping
        case KExifData::HFLIP: tab->isflippedH = !tab->isflippedH; flip(0, U); break;
        case KExifData::VFLIP: tab->isflippedV = !tab->isflippedV; flip(4, U); break;

        // rotating
        case KExifData::ROT_90:  matrix_rotate(90, U);  break;
        case KExifData::ROT_180: matrix_rotate(180, U); break;
        case KExifData::ROT_270: matrix_rotate(270, U); break;

        // flipping & rotating
        case KExifData::ROT_90_HFLIP: tab->isflippedH = !tab->isflippedH; flip(0, false); matrix_rotate(90, U); break;
        case KExifData::ROT_90_VFLIP: tab->isflippedV = !tab->isflippedV; flip(4, false); matrix_rotate(90, U); break;

        // normal rotation or unspecified
        default: if(U) updateGL();
    }
#else
    if(U)
        updateGL();
#endif
}

void SQ_GLWidget::enableActions(bool U)
{
    pASelectionRect->setEnabled(U);
    pASelectionEllipse->setEnabled(U);
    pASelectionClear->setEnabled(U);

    menuFile->setItemEnabled(id_f5, U);
    menuFile->setItemEnabled(id_f6, U);
    menuFile->setItemEnabled(id_f7, U);
    menuFile->setItemEnabled(id_f8, U);
    menuFile->setItemEnabled(id_del, U);
    menuFile->setItemEnabled(id_saveas, U);

    menu->setItemEnabled(id_prop, U);

    menuImage->setEnabled(U);

    pAToolProp->setEnabled(U);
}

void SQ_GLWidget::crop()
{
    if(tab->broken
        || tab->finfo.image.empty()
        || !gls->valid()
        || (tab->sw == tab->parts[tab->current].w && tab->sh == tab->parts[tab->current].h))
        return;

    const int RW = tab->parts[tab->current].realw;
    RGBA *img = tab->parts[tab->current].buffer->data() + tab->sy * RW + tab->sx;

    Parts pp;
    memoryPart *pt;

    SQ_GLWidget::findCloserTiles(tab->sw, tab->sh, pp.tilesx, pp.tilesy);
    QPair<int, int> pair = SQ_GLWidget::calcRealDimensions(pp);
    pp.realw = pair.first;
    pp.realh = pair.second;
    pp.w = tab->sw;
    pp.h = tab->sh;

    if(!pp.makeParts())
    {
        KMessageBox::error(this,
                    i18n("Memory allocation failed for %1 of memory")
                    .arg(KIO::convertSize(pp.realw * pp.realh * sizeof(RGBA))));
        return;
    }

    pt = new memoryPart(pp.realw * pp.realh);
    pt->create();

    if(!pt->valid())
    {
        pp.removeParts();
        return;
    }

    memset(pt->data(), 0, pp.realw * pp.realh * sizeof(RGBA));

    for(int i = 0;i < tab->sh;i++)
        memcpy(pt->data()+i*pp.realw, img + i*RW, tab->sw * sizeof(RGBA));

    pp.computeCoords();
    pp.buffer = pt;

    tab->parts[tab->current].removeParts();
    tab->parts[tab->current].deleteBuffer();
    tab->finfo.image[tab->current].w = tab->sw;
    tab->finfo.image[tab->current].h = tab->sh;

    int tlsy = pp.tilesy.size();
    for(int i = 0;i < tlsy;i++)
        showFrames(i, &pp, false);

    tab->parts[tab->current] = pp;

    slotSelectionClear();
    updateCurrentFileInfo();
    tab->isflippedH = tab->isflippedV = false;
    slotZoomIfLess();
    matrixChanged();
}

void SQ_GLWidget::slotChangeTab(int id)
{
    id = SQ_GLView::window()->tabbar()->indexOf(id);

    Tab *newtab = (id == -1) ? &taborig : &tabs[id];

    if(tab == newtab)
        return;

    if(tab)
    {
        tab->removeParts();
        gls->setVisible(false);
    }

    tab = newtab;

    images->clear();
    old_id = -1;
    stopAnimation();

    enableSettingsButton(id != -1 && tab->lib && !tab->lib->config.isEmpty());
    enableActions(!tab->broken && id != -1);

    if(id == -1)
    {
        SQ_GLView::window()->resetStatusBar();
        KSquirrel::app()->setCaption(QString::null);
        decoded = false;
    }
    else
    {
        // fill menu
        std::vector<fmt_image>::iterator itEnd = tab->finfo.image.end();
        std::vector<fmt_image>::iterator it = tab->finfo.image.begin();
        int mid, i = 0, first_id = 0;

        for(;it != itEnd;++it, ++i)
        {
            mid = images->insertItem(QString::fromLatin1("#%1 [%2x%3@%4]").arg(i+1).arg((*it).w).arg((*it).h).arg((*it).bpp));
            images->setItemParameter(mid, i);

            if(i == tab->current)
                old_id = first_id = mid;
        }

        images->setItemChecked(first_id, true);
        updateCurrentFileInfo();
        SQ_GLView::window()->sbarWidget("SBFile")->setText(tab->m_original.fileName(false));
        KSquirrel::app()->setCaption(originalURL());
        enableActions(!tab->broken);

        std::vector<Parts>::iterator itp = tab->parts.begin();
        std::vector<Parts>::iterator itpEnd = tab->parts.end();
        int tlsy;

        tab->remakeParts();

        if(tab->glselection != -1)
        {
            if(!gls->valid())
                gls->begin(static_cast<SQ_GLSelection::Type>(tab->glselection), 0, 0);

            gls->setGeometry(tab->srect);
        }
        else
            gls->end();

        gls->setVisible(tab->glselection != -1);

        for(;itp != itpEnd;++itp)
        {
            tlsy = (*itp).tilesy.size();

            for(int i = 0;i < tlsy;i++)
                showFrames(i, &(*itp), false);
        }

        if(!manualBlocked())
            startAnimation();
    }

    matrixChanged();
    updateGL();
}

void SQ_GLWidget::slotCloseRequest(int index)
{
    if(index < 0)
        return;

    SQ_GLView::window()->tabbar()->blockSignals(true);
    SQ_GLView::window()->removePage(index);
    emit tabCountChanged();
    SQ_GLView::window()->tabbar()->blockSignals(false);

    // workaround bug in KTabBar
    QMouseEvent ev(QEvent::MouseMove,
                        QCursor::pos(),
                        SQ_GLView::window()->tabbar()->mapFromGlobal(QCursor::pos()),
                        Qt::NoButton,
                        Qt::NoButton);

    KApplication::sendEvent(SQ_GLView::window()->tabbar(), &ev);

    std::vector<Tab>::iterator itEnd = tabs.end();
    std::vector<Tab>::iterator it = tabs.begin();
    int i = 0;

    for(;(it != itEnd && i != index);++it, ++i)
        ;

    (*it).clearParts();
    tabs.erase(it);
    tab = 0;

    slotChangeTab(SQ_GLView::window()->tabbar()->currentTab());
}

void SQ_GLWidget::initBrokenImage()
{
    memoryPart *pt;
    QImage broken = QPixmap(file_broken_xpm).convertToImage().swapRGB();
    broken.setAlphaBuffer(true);

    parts_broken = new Parts;

    // setup parts_broken. It will have only one 64x64 tile
    parts_broken->tilesx.push_back(broken.width());
    parts_broken->tilesy.push_back(broken.height());
    parts_broken->realw = broken.width();
    parts_broken->realh = broken.height();
    parts_broken->w = broken.width();
    parts_broken->h = broken.height();
    parts_broken->makeParts();
    parts_broken->computeCoords();

    pt = new memoryPart(broken.width() * broken.width());
    pt->create();

    memcpy(pt->data(), broken.bits(), broken.numBytes());

    parts_broken->buffer = pt;

    showFrames(0, parts_broken, false);

    image_broken.w = parts_broken->w;
    image_broken.h = parts_broken->h;
    image_broken.bpp = broken.depth();
    image_broken.compression = "-";
    image_broken.colorspace = "RGBA";
    image_broken.hasalpha = false;

    // we don't need memory buffer any more...
    parts_broken->deleteBuffer();
}

// Accept drop events.
void SQ_GLWidget::dropEvent(QDropEvent *e)
{
    QStringList files;

    if(QUriDrag::decodeLocalFiles(e, files))
    {
        // go through array and find first supported image format
        for(QStringList::iterator it = files.begin();it != files.end();++it)
        {
            if(SQ_LibraryHandler::instance()->libraryForFile(*it))
            {
                KURL u = KURL::fromPathOrURL(*it);
                m_expected = u;
                m_original = u;
                startDecoding(u);
                break;
            }
        }
    }
}

// Accept drag events.
void SQ_GLWidget::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(QUriDrag::canDecode(e));
}

/*
 *  Set clear color for context.
 */
void SQ_GLWidget::setClearColor()
{
    QColor color;
    QString path;

    SQ_Config::instance()->setGroup("GL view");

    switch(SQ_Config::instance()->readNumEntry("GL view background type", 1))
    {
        // system color
        case 0:
            color = colorGroup().color(QColorGroup::Base);
        break;

        // custom color
        case 1:
            color.setNamedColor(SQ_Config::instance()->readEntry("GL view background", "#4e4e4e"));
        break;

        // repeated texture
        case 2:
            path = SQ_Config::instance()->readEntry("GL view custom texture", "");
            BGpixmap.load(path);

            if(BGpixmap.isNull())
            {
                SQ_Config::instance()->writeEntry("GL view background type", 0);
                setClearColor();
                return;
            }

            BGpixmap.convertDepth(32);
            BGpixmap = BGpixmap.swapRGB();

            changed = true;
        break;

        default: ;
    }

    // update clear color
    qglClearColor(color);

    if(decoded)
        updateGL();
}

void SQ_GLWidget::removeCurrentParts()
{
    // if tab->broken, 'tab->parts' has no members
    if(decoded && !tab->broken)
    {
        std::vector<Parts>::iterator itEnd = tab->parts.end();

        for(std::vector<Parts>::iterator it = tab->parts.begin();it != itEnd;++it)
        {
            // delete textures and memory buffers
            (*it).removeParts();
            (*it).deleteBuffer();
        }

        tab->parts.clear();
    }
}

void SQ_GLWidget::removeCurrentTabs()
{
    std::vector<Tab>::iterator itEnd = tabs.end();

    for(std::vector<Tab>::iterator it = tabs.begin();it != itEnd;++it)
        (*it).clearParts();

    tabs.clear();
    tab = &taborig;
}

void SQ_GLWidget::removeNonCurrentTabs(int index)
{
    std::vector<Tab>::iterator itEnd = tabs.end();
    Tab tm;
    int i = 0;

    for(std::vector<Tab>::iterator it = tabs.begin();it != itEnd;++it, ++i)
    {
        if(i == index)
        {
            tm = *it;
            continue;
        }

        (*it).clearParts();
    }

    tabs.clear();
    tabs.push_back(tm);
    tab = &tabs[0];
}

void SQ_GLWidget::startDecoding(const KURL &url)
{
    startDecoding(url.path());
}

// Show/hide background for transparent image.
void SQ_GLWidget::toggleDrawingBackground()
{
    SQ_Config::instance()->setGroup("GL view");

    bool b = SQ_Config::instance()->readBoolEntry("alpha_bkgr", true);

    b = !b;

    SQ_Config::instance()->writeEntry("alpha_bkgr", b);

    updateGL();
}

void SQ_GLWidget::createMarks()
{
    mm[0] = QImage(locate("appdata", "images/marks/mark_1.png"));
    mm[1] = QImage(locate("appdata", "images/marks/mark_2.png"));
    mm[2] = QImage(locate("appdata", "images/marks/mark_3.png"));
    mm[3] = QImage(locate("appdata", "images/marks/mark_4.png"));

    marks = (mm[0].isNull() || mm[1].isNull() || mm[2].isNull() || mm[3].isNull()) ? false : true;

    if(!marks)
        return;

    for(int i = 0;i < 4;i++)
    {
        mm[i] = mm[i].convertDepth(32);
        mm[i].setAlphaBuffer(true);
    }
}

/*
 *  Show current image's width, height and bpp in statusbar.
 */
void SQ_GLWidget::updateCurrentFileInfo()
{
    QString status = QString::fromLatin1("%1x%2@%3")
    .arg(tab->finfo.image[tab->current].w)
    .arg(tab->finfo.image[tab->current].h)
    .arg(tab->finfo.image[tab->current].bpp);

    SQ_GLView::window()->sbarWidget("SBDecoded")->setText(status);
}

// Show/hide tickmarks around the image.
void SQ_GLWidget::toogleTickmarks()
{
    SQ_Config::instance()->setGroup("GL view");

    bool b = SQ_Config::instance()->readBoolEntry("marks", true);

    b = !b;

    SQ_Config::instance()->writeEntry("marks", b);

    updateGL();
}
