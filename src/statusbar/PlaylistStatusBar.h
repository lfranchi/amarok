/***************************************************************************
 *   Copyright (C) 2005 Max Howell <max.howell@methylblue.com>             *
 *   Copyright (C) 2007 Seb Ruiz <ruiz@kde.org>                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#ifndef AMAROK_STATUSBAR_H
#define AMAROK_STATUSBAR_H

#include "engineobserver.h" //baseclass
#include "statusBarBase.h"  //baseclass
//TODO:PORT to new playlist
// #include "queueLabel.h"

#include <Q3ValueStack>
//Added by qt3to4:
#include <QLabel>

class QTimer;

namespace Amarok
{
    class AMAROK_EXPORT ContextStatusBar : public KDE::StatusBar, public EngineObserver
    {
        Q_OBJECT

        static StatusBar* s_instance;

        public:
            explicit StatusBar( QWidget *parent, const char *name = 0 );
            static   StatusBar* instance() { return s_instance; }

        public slots:
            /** update total song count */
            void slotItemCountChanged( int newCount ); //TODO improve
            //FIXME: PORT
            // void updateQueueLabel() { m_queueLabel->update(); }

        protected:  /* reimpl from engineobserver */
            virtual void engineStateChanged( Engine::State state, Engine::State oldState = Engine::Empty );
            virtual void engineNewMetaData( const MetaBundle &bundle, bool trackChanged );

        private:
            QLabel *m_itemCountLabel;
            //FIXME: Port
            // QueueLabel *m_queueLabel;
    };
    
    /**
     * Is used to queue up longMessages for the StatusBar before the StatusBar
     * is created.
     */
    class MessageQueue
    {
        public:
            static MessageQueue* instance();
            void addMessage ( const QString & );
            void sendMessages();
        private:
            MessageQueue();
            Q3ValueStack<QString> m_messages;
            bool m_queueMessages;
    };
}

namespace The
{
    inline Amarok::StatusBar *statusbar() { return Amarok::StatusBar::instance(); }
}

#endif
