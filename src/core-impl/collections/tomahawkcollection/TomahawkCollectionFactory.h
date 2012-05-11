/****************************************************************************************
 * Copyright (c) 2012 Lucas Lira Gomes <x8lucas8x@gmail.com>                            *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#ifndef TOMAHAWKCOLLECTIONFACTORY_H
#define TOMAHAWKCOLLECTIONFACTORY_H

#include "core/collections/Collection.h"

#include <QMap>

#include "Source.h"
#include "Typedefs.h"

class Database;
class Servent;
class SipHandler;
class TomahawkCollection;

namespace Tomahawk
{
    namespace Accounts
    {
        class AccountManager;
    }
}

class TomahawkCollectionFactory : public Collections::CollectionFactory
{
    Q_OBJECT

    public:
        TomahawkCollectionFactory( QObject *parent, const QVariantList &args );
        virtual ~TomahawkCollectionFactory();

        virtual void init();

    signals:
        void libTomahawkLoaded();

    public slots:
        /**
         * Called when "tracked" collection is destroyed
         */
        void slotCollectionDestroyed( QObject *collection );

        void initServent();
        void initSIP();
        void accountManagerReady();

    private:
        /**
         * Attempts to create appropriate collection for already identified solid device
         * @param udi. Should emit newCollection() if the collection was successfully
         * created and should become visible to the user.
         */
        void createCollectionForPeer( const QString &udi );

    private slots:
        void onSourcesAdded( const QList<Tomahawk::source_ptr>& sources );
        void onSourceAdded( const Tomahawk::source_ptr& source );
        void onPlaylistsAdded( const QList<Tomahawk::playlist_ptr>& playlists );
        void onAutoPlaylistsAdded( const QList<Tomahawk::dynplaylist_ptr>& playlists );
        void onStationsAdded( const QList<Tomahawk::dynplaylist_ptr>& stations );

    private:
        /// udi to iPod collection map
        QMap<QString, TomahawkCollection *> m_collectionMap;

        void initPipeline();
        void initDatabase();
        void initLocalCollection();
        void registerMetaTypes();

        bool m_loaded;
        QWeakPointer<Servent> m_servent;
        QWeakPointer<Database> m_database;
        QWeakPointer< Tomahawk::Accounts::AccountManager > m_accountManager;
};

#endif // TOMAHAWKCOLLECTIONFACTORY_H
