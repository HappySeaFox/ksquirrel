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

/*
 *  Widget representing file and image actions, like
 *  in Windows(tm) XP. Currently it contains only file
 *  actions (copy, move...) and image actions (resize,
 *  rotate, convert...), but can be enlarged in the future.
 */

void SQ_SideBarActions::init()
{
    headLabel1->setText(tr2i18n("File actions"));
    headLabel2->setText(tr2i18n("Image actions"));

    frameFile->setPaletteBackgroundColor(KGlobalSettings::baseColor());
    frameImage->setPaletteBackgroundColor(KGlobalSettings::baseColor());

    line1->setPaletteBackgroundColor(KGlobalSettings::highlightColor());
    line2->setPaletteBackgroundColor(KGlobalSettings::highlightColor());
    line3->setPaletteBackgroundColor(KGlobalSettings::highlightColor());

    fa1->setText(tr2i18n("Copy"));
    fa2->setText(tr2i18n("Cut"));
    fa3->setText(tr2i18n("Copy to..."));
    fa4->setText(tr2i18n("Move to..."));
    fa5->setText(tr2i18n("Link to..."));
    fa6->setText(tr2i18n("Paste"));
    fa7->setText(tr2i18n("Run"));

    fa1_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("editcopy", KIcon::Desktop, KIcon::SizeSmall));
    fa2_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("editcut", KIcon::Desktop, KIcon::SizeSmall));
    fa3_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("editcopy", KIcon::Desktop, KIcon::SizeSmall));
    fa4_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("editcut", KIcon::Desktop, KIcon::SizeSmall));
    fa5_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("www", KIcon::Desktop, KIcon::SizeSmall));
    fa6_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("editpaste", KIcon::Desktop, KIcon::SizeSmall));
    fa7_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("launch", KIcon::Desktop, KIcon::SizeSmall));

    ia1->setText(tr2i18n("Resize"));
    ia2->setText(tr2i18n("Rotate"));
    ia3->setText(tr2i18n("Colorize"));
    ia4->setText(tr2i18n("Convert"));
    ia5->setText(tr2i18n("Filter"));
    ia6->setText(tr2i18n("Print"));

    ia1_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("flag", KIcon::Desktop, KIcon::SizeSmall));
    ia2_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("rotate", KIcon::Desktop, KIcon::SizeSmall));
    ia3_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("colorize", KIcon::Desktop, KIcon::SizeSmall));
    ia4_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("filesaveas", KIcon::Desktop, KIcon::SizeSmall));
    ia5_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("effect", KIcon::Desktop, KIcon::SizeSmall));
    ia6_pixmap->setPixmap(SQ_IconLoader::instance()->loadIcon("fileprint", KIcon::Desktop, KIcon::SizeSmall));

    connect(fa1, SIGNAL(clicked()), SQ_WidgetStack::instance(), SLOT(slotFileCopy()));
    connect(fa2, SIGNAL(clicked()), SQ_WidgetStack::instance(), SLOT(slotFileCut()));
    connect(fa3, SIGNAL(clicked()), SQ_WidgetStack::instance(), SLOT(slotFileCopyTo()));
    connect(fa4, SIGNAL(clicked()), SQ_WidgetStack::instance(), SLOT(slotFileMoveTo()));
    connect(fa5, SIGNAL(clicked()), SQ_WidgetStack::instance(), SLOT(slotFileLinkTo()));
    connect(fa6, SIGNAL(clicked()), SQ_WidgetStack::instance(), SLOT(slotFilePaste()));
    connect(fa7, SIGNAL(clicked()), SQ_WidgetStack::instance(), SLOT(slotRunSeparately()));

    connect(ia1, SIGNAL(clicked()), SQ_Resizer::instance(), SLOT(slotStartEdit()));
    connect(ia2, SIGNAL(clicked()), SQ_Rotater::instance(), SLOT(slotStartEdit()));
    connect(ia3, SIGNAL(clicked()), SQ_Contraster::instance(), SLOT(slotStartEdit()));
    connect(ia4, SIGNAL(clicked()), SQ_Converter::instance(), SLOT(slotStartEdit()));
    connect(ia5, SIGNAL(clicked()), SQ_Filter::instance(), SLOT(slotStartEdit()));
    connect(ia6, SIGNAL(clicked()), SQ_Printer::instance(), SLOT(slotStartEdit()));
}
