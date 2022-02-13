/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void SQ_ImageProperties::setParams(QStringList &l)
{
    setFileParams();
    
    QStringList::Iterator it = l.begin();
    textType->setText(*it); it++;
    textDimensions->setText(*it); it++;
    textBpp->setText(*it); it++;
    textColorModel->setText(*it); it++;
    textCompression->setText(*it); it++;
    textFrames->setText(*it); it++;
    textUncompressed->setText(*it); it++;
    textRatio->setText(*it);
}


void SQ_ImageProperties::setFile( const QString &filen )
{
    file = filen;
}


void SQ_ImageProperties::setFileParams()
{
    QFileInfo fm(file);
    
    lineDirectory->setText(fm.dirPath(true));
    lineFile->setText(fm.fileName());
    textSize->setText(KIO::convertSize(fm.size()));
    textOwner->setText(QString("%1 (id: %2)").arg(fm.owner()).arg(fm.ownerId()));
    textGroup->setText(QString("%1 (id: %2)").arg(fm.group()).arg(fm.groupId()));
    textPermissions->setText(QString("%1%2%3%4%5%6%7%8%9")
		       .arg(fm.permission(QFileInfo::ReadUser)?"r":"-")
		       .arg(fm.permission(QFileInfo::WriteUser)?"w":"-")
		       .arg(fm.permission(QFileInfo::ExeUser)?"x":"-")
		       .arg(fm.permission(QFileInfo::ReadGroup)?"r":"-")
		       .arg(fm.permission(QFileInfo::WriteGroup)?"w":"-")
		       .arg(fm.permission(QFileInfo::ExeGroup)?"x":"-")
		       .arg(fm.permission(QFileInfo::ReadOther)?"r":"-")
		       .arg(fm.permission(QFileInfo::WriteOther)?"w":"-")
		       .arg(fm.permission(QFileInfo::ExeOther)?"x":"-"));
    
    QDateTime abs = fm.created();
    textCreated->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
    abs = fm.lastRead();
    textLastRead->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
    abs = fm.lastModified();
    textLastMod->setText(abs.toString("dd/MM/yyyy hh:mm:ss"));
}
