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

#ifndef TOMAHAWKPLAYLISTPROVIDER_H
#define TOMAHAWKPLAYLISTPROVIDER_H

#include "TomahawkPlaylist.h"

#include "core-impl/playlists/providers/user/UserPlaylistProvider.h"

class TomahawkCollection;

class TomahawkPlaylistProvider : public Playlists::UserPlaylistProvider, private Playlists::PlaylistObserver
{
    Q_OBJECT

    public:
        TomahawkPlaylistProvider( TomahawkCollection *collection );
        virtual ~TomahawkPlaylistProvider();

        // PlaylistProvider methods:
        virtual QString prettyName() const;
        virtual KIcon icon() const;

        virtual int playlistCount() const;
        virtual Playlists::PlaylistList playlists();

        virtual Playlists::PlaylistPtr addPlaylist( Playlists::PlaylistPtr playlist );
        virtual Meta::TrackPtr addTrack( Meta::TrackPtr track );

        // UserPlaylistProvider methods:
        virtual Playlists::PlaylistPtr save( const Meta::TrackList &tracks,
                                             const QString& name = QString() );

        virtual QActionList providerActions();
        virtual QActionList playlistActions( Playlists::PlaylistPtr playlist );
        virtual QActionList trackActions( Playlists::PlaylistPtr playlist,
                                               int trackIndex );

        virtual bool isWritable();
        virtual void rename( Playlists::PlaylistPtr playlist, const QString &newName );
        virtual bool deletePlaylists( Playlists::PlaylistList playlistlist );

        // PlaylistObserver methods:
        virtual void trackAdded( Playlists::PlaylistPtr playlist, Meta::TrackPtr track, int position );
        virtual void trackRemoved( Playlists::PlaylistPtr playlist, int position );

    signals:
        /**
         * Signals to TomahawkCollection that the database has been dirtied and it has to
         * write the database in some point in time.
         */
        void startWriteDatabaseTimer();

    private:
        TomahawkCollection *m_coll;
        Playlists::PlaylistList m_playlists;
        QAction *m_consolidateAction;
};

#endif // TOMAHAWKPLAYLISTPROVIDER_H
