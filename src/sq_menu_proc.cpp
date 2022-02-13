#include "sq_menu_proc.h"

SQ_MenuProcess::SQ_MenuProcess(const int _baseid) : baseid(_baseid),curid(0)
{
    plist = new QPtrList<PROCESSITEM>;
    
    QObject::QObject();
}

void SQ_MenuProcess::AddItem(const char *_name, const char *_program)
{
    PROCESSITEM *item = new PROCESSITEM;
    
    strcpy(item->name, _name);
    strcpy(item->program, _program);
    item->id = curid;
    
    plist->append(item);
    
    curid++;
    
//    delete item;
}

int SQ_MenuProcess::GetCount() const
{
    return plist->count();
}

QString SQ_MenuProcess::GetName(unsigned int index) const
{
    QString tmp((plist->at(index))->name);
    return tmp;
}

QString SQ_MenuProcess::GetProgName(unsigned int index) const
{
    QString tmp((plist->at(index))->program);
    return tmp;
}

void SQ_MenuProcess::slotRunCommand(int menu_id)
{
    QProcess _tmp(QString((plist->at(menu_id-baseid))->program));
    _tmp.start();
}

int SQ_MenuProcess::GetElementId(const int ind) const
{
    int k = ((plist->at(ind))->id);
    return k;
}
