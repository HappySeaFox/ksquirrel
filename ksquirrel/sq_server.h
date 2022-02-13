/***************************************************************************
                          sq_server.h  -  description
                             -------------------
    begin                : ??? ??? 5 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@yandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_SERVER_H
#define SQ_SERVER_H

#include <qserversocket.h>
#include <qsocket.h>

/**
  *@author CKulT
  */

class ClientSocket : public QSocket
{
	Q_OBJECT

	public:
		ClientSocket(int sock, QObject *parent = 0, const char *name = 0);
		~ClientSocket();
};

class SQ_Server : public QServerSocket
{
	Q_OBJECT

	public:
		SQ_Server(Q_UINT16 port, QObject* parent = 0);
		~SQ_Server();

		void newConnection(int socket);
};

#endif
