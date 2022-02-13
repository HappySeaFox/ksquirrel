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

SRect rec;

void SQ_Selector::init()
{
    connect(canvasWidget->editor(), SIGNAL(mouse(int,int)), this, SLOT(slotMouseMove(int,int)));
    connect(canvasWidget->editor(), SIGNAL(contentsMoving(int,int)), this, SLOT(slotMoving(int,int)));
    connect(canvasWidget->editor(), SIGNAL(rectSelected(const SRect &)), this, SLOT(slotRectSelected(const SRect &)));

    kRuler1->setRulerMetricStyle(KRuler::Pixel);
    kRuler1->setRange(0, 1000);
    kRuler1->setShowEndLabel(false);

    kRuler2->setRulerMetricStyle(KRuler::Pixel);
    kRuler2->setRange(0, 1000);
    kRuler2->setShowEndLabel(false);
}

void SQ_Selector::slotMoving(int x, int y)
{
    kRuler1->setRange(x, x+canvasWidget->width());
    kRuler2->setRange(y, y+canvasWidget->height());
}

void SQ_Selector::slotMouseMove(int x, int y)
{
    kRuler1->setValue(x);
    kRuler2->setValue(y);

    point->setText(QString::fromLatin1("%1; %2").arg(x).arg(y));
}

void SQ_Selector::slotRectSelected(const SRect &r)
{
    rec = r;

    status2->setText(QString::fromLatin1("%1x%2").arg(r.width).arg(r.height));

    xy1->setText(QString::fromLatin1("%1,%2").arg(r.x).arg(r.y));
    xy2->setText(QString::fromLatin1("%1,%2").arg(r.width+r.x).arg(r.height+r.y));
}

int SQ_Selector::exec(SRect *rc, bool *b)
{
    int r = QDialog::exec();

    if(r == QDialog::Accepted)
    {
        *rc = rec;
        *b = checkAll->isChecked();
    }

    return r;
}

void SQ_Selector::setImage(const QImage &im)
{
    canvasWidget->setImage(im);
}
