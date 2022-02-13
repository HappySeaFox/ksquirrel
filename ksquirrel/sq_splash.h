#include <qvbox.h>
#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>

#ifndef QP_SPLASH_H
#define QP_SPLASH_H

class SQ_Splash : public QVBox 
{
    Q_OBJECT

    public:
	SQ_Splash(QWidget *parent = 0, const char *name = 0);
	~SQ_Splash();

    public: 
	void addInfo(const QString &);

    protected:
	void mousePressEvent(QMouseEvent *);

    private:
	QLabel *m_infoBox;
};

#endif
