/***************************************************************************
                          sq_config.h  -  description
                             -------------------
    begin                : ??? ??? 14 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_CONFIG_H
#define SQ_CONFIG_H

#include <qobject.h>
#include <kconfig.h>

class KConfig;

/*
 *  Class for reading/writing config file
 */

class SQ_Config : public QObject
{
    public: 
        SQ_Config(QObject *parent = 0);
        ~SQ_Config();

        static SQ_Config* instance() { return m_instance; }

        void sync();

        void setGroup(const QString &group);
        bool hasGroup(const QString &group) const;
        bool deleteGroup( const QString& group, bool bDeep = true, bool bGlobal = false );
        QString readEntry(const QString& pKey,  const QString& aDefault = QString::null ) const;
        QStringList readListEntry( const QString& pKey, char sep = ',' ) const;
        int readNumEntry( const QString& pKey, int nDefault = 0 ) const;
        bool readBoolEntry( const QString& pKey, bool bDefault = false ) const;
        QRect readRectEntry( const QString& pKey, const QRect* pDefault = 0L ) const;
        QPoint readPointEntry( const QString& pKey, const QPoint* pDefault = 0L ) const;
        QSize readSizeEntry( const QString& pKey, const QSize* pDefault = 0L ) const;
        double readDoubleNumEntry( const QString& pKey, double nDefault = 0.0 ) const;
        QValueList<int> readIntListEntry( const QString& pKey ) const;

        void writeEntry( const QString& pKey, const QString& pValue, bool bPersistent = true, bool bGlobal = false, bool bNLS = false );
        void writeEntry( const QString& pKey, const QStringList &rValue, char sep = ',', bool bPersistent = true, bool bGlobal = false, bool bNLS = false );
        void writeEntry( const QString& pKey, int nValue, bool bPersistent = true, bool bGlobal = false,  bool bNLS = false );
        void writeEntry( const QString& pKey, bool bValue, bool bPersistent = true, bool bGlobal = false, bool bNLS = false );
        void writeEntry( const QString& pKey, const QRect& rValue, bool bPersistent = true, bool bGlobal = false, bool bNLS = false );
        void writeEntry( const QString& pKey, const QPoint& rValue, bool bPersistent = true, bool bGlobal = false, bool bNLS = false );
        void writeEntry( const QString& pKey, const QSize& rValue, bool bPersistent = true, bool bGlobal = false, bool bNLS = false );
        void writeEntry( const QString& pKey, double nValue, bool bPersistent = true, bool bGlobal = false, char format = 'g', int precision = 6, bool bNLS = false );
        void writeEntry( const QString& pKey, const QValueList<int>& rValue, bool bPersistent = true, bool bGlobal = false, bool bNLS = false );

    private:
        KConfig *kconf;
        static SQ_Config *m_instance;
};

inline
void SQ_Config::sync() { kconf->sync(); }

inline
void SQ_Config::setGroup(const QString &group) { kconf->setGroup(group) ; }

inline
bool SQ_Config::hasGroup(const QString &group) const { return kconf->hasGroup(group) ; }

inline
bool SQ_Config::deleteGroup(const QString& group, bool bDeep, bool bGlobal) { return kconf->deleteGroup(group, bDeep, bGlobal) ; }

inline
QString SQ_Config::readEntry(const QString& pKey,  const QString& aDefault) const { return kconf->readEntry(pKey,  aDefault) ; }

inline
QStringList SQ_Config::readListEntry(const QString& pKey, char sep) const { return kconf->readListEntry(pKey, sep) ; }

inline
int SQ_Config::readNumEntry(const QString& pKey, int nDefault) const { return kconf->readNumEntry(pKey, nDefault) ; }

inline
bool SQ_Config::readBoolEntry(const QString& pKey, bool bDefault) const { return kconf->readBoolEntry(pKey, bDefault ) ; }

inline
QRect SQ_Config::readRectEntry(const QString& pKey, const QRect* pDefault) const { return kconf->readRectEntry(pKey, pDefault) ; }

inline
QPoint SQ_Config::readPointEntry(const QString& pKey, const QPoint* pDefault) const { return kconf->readPointEntry(pKey, pDefault) ; }

inline
QSize SQ_Config::readSizeEntry(const QString& pKey, const QSize* pDefault) const { return kconf->readSizeEntry(pKey, pDefault) ; }

inline
double SQ_Config::readDoubleNumEntry(const QString& pKey, double nDefault) const { return kconf->readDoubleNumEntry(pKey, nDefault); }

inline
QValueList<int> SQ_Config::readIntListEntry( const QString& pKey ) const { return kconf->readIntListEntry(pKey); }

/**********************************************/

inline
void SQ_Config::writeEntry(const QString& pKey, const QString& pValue, bool bPersistent, bool bGlobal, bool bNLS) { kconf->writeEntry( pKey, pValue, bPersistent, bGlobal, bNLS ) ; }

inline
void SQ_Config::writeEntry(const QString& pKey, const QStringList &rValue, char sep, bool bPersistent, bool bGlobal, bool bNLS ) { kconf->writeEntry(pKey, rValue, sep, bPersistent, bGlobal, bNLS ) ; }

inline
void SQ_Config::writeEntry(const QString& pKey, int nValue, bool bPersistent, bool bGlobal,  bool bNLS ) { kconf->writeEntry(pKey, nValue, bPersistent, bGlobal, bNLS ) ; }

inline
void SQ_Config::writeEntry(const QString& pKey, bool bValue, bool bPersistent, bool bGlobal, bool bNLS ) { kconf->writeEntry(pKey, bValue, bPersistent, bGlobal, bNLS ) ; }

inline
void SQ_Config::writeEntry(const QString& pKey, const QRect& rValue, bool bPersistent, bool bGlobal, bool bNLS ) { kconf->writeEntry(pKey, rValue, bPersistent, bGlobal, bNLS ) ; }

inline
void SQ_Config::writeEntry(const QString& pKey, const QPoint& rValue, bool bPersistent, bool bGlobal, bool bNLS ) { kconf->writeEntry(pKey, rValue, bPersistent, bGlobal, bNLS ) ; }

inline
void SQ_Config::writeEntry(const QString& pKey, const QSize& rValue, bool bPersistent, bool bGlobal, bool bNLS ) { kconf->writeEntry(pKey,rValue, bPersistent, bGlobal, bNLS ) ; }

inline
void SQ_Config::writeEntry( const QString& pKey, double nValue, bool bPersistent, bool bGlobal, char format, int precision, bool bNLS) { kconf->writeEntry(pKey, nValue, bPersistent, bGlobal, format, precision, bNLS); }

inline
void SQ_Config::writeEntry( const QString& pKey, const QValueList<int>& rValue, bool bPersistent, bool bGlobal, bool bNLS) { kconf->writeEntry(pKey, rValue, bPersistent, bGlobal, bNLS); }

#endif
