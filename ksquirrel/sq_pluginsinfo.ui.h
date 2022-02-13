/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/*
 *  SQ_PluginsInfo shows all found by SQ_LibraryListener libraries.
 */

void SQ_PluginsInfo::init()
{
    showLibs();
}

void SQ_PluginsInfo::showLibs()
{
	int cnt = SQ_LibraryHandler::instance()->count();

	textLibs->setText(SQ_KLIBS);
	textCount->setNum(cnt);

	if(!cnt) return;

	QPixmap ok = SQ_IconLoader::instance()->loadIcon("ok", KIcon::Desktop, KIcon::SizeSmall);
	QPixmap configure = SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall);

	SQ_LibraryHandler::iterator itEnd = SQ_LibraryHandler::instance()->end();
	QListViewItem *item;

	for(SQ_LibraryHandler::iterator it = SQ_LibraryHandler::instance()->begin();it != itEnd;++it)
	{
		item = new QListViewItem(tableLib, QString::null, QString::null,
										(*it).quickinfo, (*it).version, (*it).filter);

		if(!(*it).config.isEmpty())
			item->setPixmap(0, configure);
		
		if((*it).readable)
			item->setPixmap(5, ok);

		if((*it).writestatic)
			item->setPixmap(6, ok);

		item->setPixmap(1, (*it).mime);

		tableLib->insertItem(item);
	}

	item = tableLib->firstChild();
	tableLib->setCurrentItem(item);
	tableLib->setSelected(item, true);
}

void SQ_PluginsInfo::slotDoubleClicked(QListViewItem *i)
{
        if(!i)
            return;

        SQ_LIBRARY *lib =  SQ_LibraryHandler::instance()->libraryByName(i->text(2));

        if(!lib || lib->config.isEmpty())
            return;

        SQ_CodecSettingsSkeleton skel(this);
        skel.addSettingsWidget(lib->config);
        skel.setCodecInfo(*i->pixmap(1), i->text(2));
        skel.adjustSize();
	skel.exec(lib->settings);
}

void SQ_PluginsInfo::slotMouseButtonClicked(int button, QListViewItem *i, const QPoint &, int c)
{
	if(button == Qt::LeftButton && i && !c)
		slotDoubleClicked(i);
}
