/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

static const int F = 105;

void SQ_ResizeDialog::init()
{
    pushPut->setPixmap(KSquirrel::loader()->loadIcon("folder", KIcon::Desktop, 16));
    g1 = groupBox1->colorGroup().background();
    g2 = groupBox2->colorGroup().background();
    groupBox1->setPaletteBackgroundColor(g1.dark(F));
}

void SQ_ResizeDialog::startResizing()
{
    lastlen = 0;
    paragraph = 0;
}

void SQ_ResizeDialog::slotOneProcessed()
{
    progress->advance(1);
    paragraph++;
    lastlen = 0;
 }

void SQ_ResizeDialog::slotDebugText(const QString &text, bool bold)
{
    textResult->setBold(bold);
    textResult->insertAt(text, paragraph, lastlen);
    textResult->setBold(false);
    lastlen = text.length();
}

void SQ_ResizeDialog::slotPutConverted()
{
    putto = KFileDialog::getExistingDirectory(QString::null, this);    
}

QString SQ_ResizeDialog::putTo() const
{
    return putto;
}

void SQ_ResizeDialog::slotRadio1Clicked()
{
    radioPercentage->setChecked(true);
    radioPixels->setChecked(false);
    groupBox2->setPaletteBackgroundColor(g2);
    groupBox1->setPaletteBackgroundColor(g1.dark(F));
}

void SQ_ResizeDialog::slotRadio2Clicked()
{
    radioPixels->setChecked(true);
    radioPercentage->setChecked(false);
    groupBox1->setPaletteBackgroundColor(g1);
    groupBox2->setPaletteBackgroundColor(g2.dark(F));
}
