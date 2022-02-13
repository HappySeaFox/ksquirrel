/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_CheckVersion::init()
{
    k = kl = false;

    qInitNetworkProtocols();

    kurlLabelK->hide();
    kurlLabelKL->hide();

    textAction->setText(tr2i18n("Creating&nbsp;temporary&nbsp;files..."));

    f1 = new KTempFile;
    f1->setAutoDelete(true);

    f2 = new KTempFile;
    f2->setAutoDelete(true);

    f1->close();
    f2->close();

    if(f1->status() || f2->status())
	textAction->setText(textAction->text() + tr2i18n("failed"));
    else
	slotCheck();
}

void SQ_CheckVersion::destroy()
{
    delete f2;
    delete f1;
}

void SQ_CheckVersion::slotCheck()
{
    textAction->setText(tr2i18n("Checking&nbsp;version&nbsp;of&nbsp;KSquirrel..."));
    
    QUrlOperator *op = new QUrlOperator();

    connect(op, SIGNAL(finished(QNetworkOperation*)), this, SLOT(slotFinished(QNetworkOperation*)));
    // ksquirrel.sourceforge.net
    op->copy(QString::fromLatin1("http://ksquirrel.sourceforge.net/sq_version"), f1->name(), false, false);}

void SQ_CheckVersion::slotFinished(QNetworkOperation *netop)
{
     QString kresult;

     if(netop->operation() == QNetworkProtocol::OpGet)
     {
	 if(netop->state() == QNetworkProtocol::StFailed)
	     goto S;

	 return;
    }

     if(netop->state() == QNetworkProtocol::StDone)
    {
	k = true;
	
	QFile f(f1->name());

	if(f.open(IO_ReadOnly))
	{
	    QString ver;
	    f.readLine(ver, 100);
	    f.close();

	    if(!ver.startsWith(SQ_VERSION))
		kv = ver;
	}
    }
    else
	k = false;

    S:
    kresult = (tr2i18n("Checking&nbsp;version&nbsp;of&nbsp;KSquirrel...") + "&nbsp;%1%2")
	      .arg(k ? tr2i18n("<b>ok</b>") : tr2i18n("<font color=red><b>failed</b></font>"))
	      .arg(k ? (kv.isEmpty() ? "<br>Version&nbsp;not&nbsp;changed" : QString::fromLatin1("<br>New&nbsp;version:&nbsp;<font color=magenta><b>%1</b></font>").arg(kv)) : QString::null);

    textResultK->setText(kresult);

    if(!kv.isEmpty())
    {
	 kurlLabelK->setURL(tr2i18n("ksquirrel-%1.tar.bz2").arg(kv));
	 kurlLabelK->setText(tr2i18n("Download ksquirrel-%1.tar.bz2").arg(kv));
	 kurlLabelK->show();
     }

    textAction->setText(tr2i18n("Checking&nbsp;version&nbsp;of&nbsp;ksquirrel-libs..."));   

    qApp->processEvents();

    QUrlOperator *op = new QUrlOperator();

    connect(op, SIGNAL(finished(QNetworkOperation*)), this, SLOT(slotFinishedKL(QNetworkOperation*)));

    op->copy(QString::fromLatin1("http://ksquirrel.sourceforge.net/sq_libs"), f2->name(), false, false);
}

void SQ_CheckVersion::slotFinishedKL(QNetworkOperation *netop)
{
     QString klresult;
     QString KL_VER;

     if(netop->operation() == QNetworkProtocol::OpGet)
     {
	 if(netop->state() == QNetworkProtocol::StFailed)
	     goto S2;

	     return;
    }

    if(netop->state() == QNetworkProtocol::StDone)
    {
	kl = true;

	QFile f(f2->name());
	QFile l("/usr/lib/ksquirrel-libs/version");

	if(l.open(IO_ReadOnly))
	{
	    l.readLine(KL_VER, 100);
	    l.close();

	    if(f.open(IO_ReadOnly))
	    {
		QString ver;
		f.readLine(ver, 100);
		f.close();

		if(!ver.startsWith(KL_VER))
		    klv = ver;
	    }
	    else
		kl = false;
	}
	else
	    kl = false;
    }
    else
	kl = false;

    S2:
    textAction->setText(tr2i18n("Done"));

    klresult = (tr2i18n("Checking&nbsp;version&nbsp;of&nbsp;ksquirrel-libs...") + "&nbsp;%1%2")
	       .arg(kl ? tr2i18n("<b>ok</b>") : tr2i18n("<font color=red><b>failed</b></font>"))
	       .arg(kl ? (klv.isEmpty() ? "<br>Version&nbsp;not&nbsp;changed" : QString::fromLatin1("<br>New&nbsp;version:&nbsp;<font color=magenta><b>%1</b></font>").arg(klv)) : QString::null);

    if(!klv.isEmpty())
    {
	 kurlLabelKL->setURL(tr2i18n("ksquirrel-libs-%1.tar.bz2").arg(klv));
	 kurlLabelKL->setText(tr2i18n("Download ksquirrel-libs-%1.tar.bz2").arg(klv));
	 kurlLabelKL->show();
     }

    textResultKL->setText(klresult);   

    updateSize();
}

void SQ_CheckVersion::slotDownloadK(const QString &url)
{
    new KRun(KURL(QString::fromLatin1("http://prdownloads.sourceforge.net/ksquirrel/%1?download").arg(url)));
}

void SQ_CheckVersion::slotDownloadKL(const QString &url)
{
    new KRun(KURL(QString::fromLatin1("http://prdownloads.sourceforge.net/ksquirrel/%1?download").arg(url)));
}

void SQ_CheckVersion::updateSize()
{
    qApp->processEvents();

    resize(sizeHint().width(), height());
}
