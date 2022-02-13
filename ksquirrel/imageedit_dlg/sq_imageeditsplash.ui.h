/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

static const QColor _cc = QColor(255,205,105);

void SQ_ImageEditSplash::init()
{
    setAutoMask(true);

    pix = QPixmap(locate("appdata", "images/imageedit/mask.png"));

    pushConvert->setPixmap(QPixmap(locate("appdata", "images/imageedit/splash_convert.png")));
    pushConvert->setText(tr2i18n("Convert"));
    pushResize->setPixmap(QPixmap(locate("appdata", "images/imageedit/splash_resize.png")));
    pushResize->setText(tr2i18n("Resize"));
    pushRotate->setPixmap(QPixmap(locate("appdata", "images/imageedit/splash_rotate.png")));
    pushRotate->setText(tr2i18n("Rotate & flip"));
    pushColorize->setPixmap(QPixmap(locate("appdata", "images/imageedit/splash_colorize.png")));
    pushColorize->setText(tr2i18n("Colorize"));
    pushFilter->setPixmap(QPixmap(locate("appdata", "images/imageedit/splash_filter.png")));
    pushFilter->setText(tr2i18n("Filter"));
    pushPrint->setPixmap(QPixmap(locate("appdata", "images/imageedit/splash_print.png")));
    pushPrint->setText(tr2i18n("Print"));

    setPaletteBackgroundColor(_cc);

    if(!pix.isNull())
    {
        setFixedWidth(pix.width());
        setFixedHeight(pix.height());
    }

    KFileItemList *items = (KFileItemList *)SQ_WidgetStack::instance()->selectedItems();

    textFiles->setPaletteForegroundColor(Qt::black);
    textFiles->setText(i18n("Do something with 1 file", "Do something with %n files", items->count()));
}

void SQ_ImageEditSplash::mousePressEvent(QMouseEvent *)
{
    close();
}

void SQ_ImageEditSplash::updateMask()
{
    if(!pix.mask())
        return;

    QBitmap bm = *pix.mask();

    setMask(bm);
}

void SQ_ImageEditSplash::slotConvert()
{
    hide();
    KSquirrel::app()->pAImageConvert->activate();
    close();
}

void SQ_ImageEditSplash::slotColorize()
{
    hide();
    KSquirrel::app()->pAImageBCG->activate();
    close();
}

void SQ_ImageEditSplash::slotResize()
{
    hide();
    KSquirrel::app()->pAImageResize->activate();
    close();
}

void SQ_ImageEditSplash::slotRotate()
{
    hide();
    KSquirrel::app()->pAImageRotate->activate();
    close();
}

void SQ_ImageEditSplash::slotFilter()
{
    hide();
    KSquirrel::app()->pAImageFilter->activate();
    close();
}

void SQ_ImageEditSplash::slotPrint()
{
    hide();
    KSquirrel::app()->pAPrintImages->activate();
    close();
}

bool SQ_ImageEditSplash::event(QEvent *e)
{
    if(e->type() == QEvent::WindowDeactivate)
    {
        close();
        return false;
    }

    return QWidget::event(e);
}

void SQ_ImageEditSplash::keyPressEvent(QKeyEvent *e)
{
    e->accept();

    switch(e->key())
    {
        case Qt::Key_1:
            slotConvert();
        break;
        case Qt::Key_2:
            slotResize();
        break;
        case Qt::Key_3:
            slotRotate();
        break;
        case Qt::Key_4:
            slotColorize();
        break;
        case Qt::Key_5:
            slotFilter();
        break;
        case Qt::Key_6:
            slotPrint();
        break;
    }
}
