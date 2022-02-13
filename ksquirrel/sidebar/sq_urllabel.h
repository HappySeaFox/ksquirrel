#include <qlabel.h>

/*
 *  Simple replacement for KURLLabel.
 */

class SQ_URLLabel : public QLabel
{
    Q_OBJECT

    public:
        SQ_URLLabel(QWidget *parent = 0, const char *name = 0);
        ~SQ_URLLabel();

    protected:

        /*
         *  Reimplement enterEvent() anf leaveEvent()
         *  to highlight our url.
         */
        void enterEvent(QEvent *);
        void leaveEvent(QEvent *);

        /*
         *  just "emit clicked()"
         */
        void mousePressEvent(QMouseEvent *);

    signals:
        void clicked();
};
