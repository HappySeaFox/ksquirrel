/***************************************************************************
                          sq_server.cpp  -  description
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

#include "sq_server.h"

#include "ksquirrel.h"
#include "sq_glwidget.h"
#include "sq_glview.h"

ClientSocket::ClientSocket(int sock, QObject *parent, const char *name) : QSocket(parent, name)
{
	connect(this, SIGNAL(readyRead()), sqGLWidget, SLOT(slotShowImage()));
//	connect( this, SIGNAL(connectionClosed()), SLOT(deleteLater()));
	setSocket(sock);
}

ClientSocket::~ClientSocket()
{}

SQ_Server::SQ_Server(Q_UINT16 port, QObject* parent) : QServerSocket(port, 1, parent)
{
	if(!ok())
	{
		qWarning("Failed to bind to port.");
		exit(1);
	}
}

SQ_Server::~SQ_Server()
{}

void SQ_Server::newConnection(int socket)
{
	new ClientSocket(socket, this);
}
