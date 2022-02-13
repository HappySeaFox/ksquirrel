#ifndef MENU_RUN_PROCESSES
#define MENU_RUN_PROCESSES

#include <qprocess.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qobject.h>

#include <stdio.h>

typedef struct
{
    char name[20];
    char program[50];
    int id;
}PROCESSITEM;



class SQ_MenuProcess : public QObject
{
    Q_OBJECT
    
    public:
	SQ_MenuProcess(const int = 0);
	~SQ_MenuProcess() { plist->clear(); delete plist; }

	void AddItem(const char*, const char*);
	
	int  GetCount()               const;
	QString GetName(unsigned int) const;
	QString GetProgName(unsigned int) const;
	int GetElementId(const int)   const;


    public slots:
	void slotRunCommand(int menu_id);


    private:
	QPtrList<PROCESSITEM> *plist;
	int baseid, curid;
};


#endif
