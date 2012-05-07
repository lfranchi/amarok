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

#include "TomahawkPlaylistProvider.h"

#include "TomahawkCollection.h"
#include "TomahawkPlaylist.h"
#include "core/capabilities/ActionsCapability.h"
#include "core/interfaces/Logger.h"
#include "core/support/Components.h"
#include "core/support/Debug.h"
#include "core-impl/collections/support/FileCollectionLocation.h"
#include "core-impl/meta/file/File.h"

TomahawkPlaylistProvider::TomahawkPlaylistProvider( TomahawkCollection* collection )
    : UserPlaylistProvider( collection )
    , m_coll( collection )
{
}

TomahawkPlaylistProvider::~TomahawkPlaylistProvider()
{
}

QString
TomahawkPlaylistProvider::prettyName() const
{
    return m_coll->prettyName();
}

KIcon
TomahawkPlaylistProvider::icon() const
{
    return m_coll->icon();
}

int
TomahawkPlaylistProvider::playlistCount() const
{
    return m_playlists.count();
}

Playlists::PlaylistList
TomahawkPlaylistProvider::playlists()
{
    return m_playlists;
}

Playlists::PlaylistPtr
TomahawkPlaylistProvider::addPlaylist( Playlists::PlaylistPtr playlist )
{
    return save( playlist->tracks(), playlist->name() );
}

Meta::TrackPtr
TomahawkPlaylistProvider::addTrack( Meta::TrackPtr track )
{
    QString name = KGlobal::locale()->formatDateTime( QDateTime::currentDateTime() );
    return save( Meta::TrackList() << track , name )->tracks().last();
}

Playlists::PlaylistPtr
TomahawkPlaylistProvider::save( const Meta::TrackList &tracks, const QString &name )
{
//    if( !isWritable() )
//        return Playlists::PlaylistPtr();

//    TomahawkPlaylist *playlist = new TomahawkPlaylist( tracks, name, m_coll );
//    itdb_playlist_add( m_coll->m_itdb, playlist->itdbPlaylist(), -1 );
//    Playlists::PlaylistPtr playlistPtr( playlist );
//    m_playlists << playlistPtr;
//    subscribeTo( playlistPtr );
//    emit playlistAdded( playlistPtr );
//    emit startWriteDatabaseTimer();
//    return playlistPtr;

    return Playlists::PlaylistPtr();
}

QActionList
TomahawkPlaylistProvider::providerActions()
{
    QActionList actions = Playlists::UserPlaylistProvider::providerActions();
    Capabilities::ActionsCapability *ac = m_coll->create<Capabilities::ActionsCapability>();
    actions << ac->actions();
    delete ac;
//    if( m_stalePlaylist || m_orphanedPlaylist )
//        actions << m_consolidateAction;
    return actions;
}

QActionList
TomahawkPlaylistProvider::playlistActions( Playlists::PlaylistPtr playlist )
{
    QList<QAction *> actions;
    if( !m_playlists.contains( playlist ) )  // make following static cast safe
        return actions;
    KSharedPtr<TomahawkPlaylist> tomahawkPlaylist = KSharedPtr<TomahawkPlaylist>::staticCast( playlist );

    actions << Playlists::UserPlaylistProvider::playlistActions( playlist );

    return actions;
}

QActionList
TomahawkPlaylistProvider::trackActions( Playlists::PlaylistPtr playlist, int trackIndex )
{
    QList<QAction *> actions;
    if( !m_playlists.contains( playlist ) )  // make following static cast safe
        return actions;
    KSharedPtr<TomahawkPlaylist> tomahawkPlaylist = KSharedPtr<TomahawkPlaylist>::staticCast( playlist );

    actions << Playlists::UserPlaylistProvider::trackActions( playlist, trackIndex );

    return actions;
}

bool
TomahawkPlaylistProvider::isWritable()
{
    return m_coll->isWritable();
}

void
TomahawkPlaylistProvider::rename( Playlists::PlaylistPtr playlist, const QString &newName )
{
    if( !m_playlists.contains( playlist ) )  // make following static cast safe
        return;
    KSharedPtr<TomahawkPlaylist> tomahawkPlaylist = KSharedPtr<TomahawkPlaylist>::staticCast( playlist );

    playlist->setName( newName );
    emit updated();
    emit startWriteDatabaseTimer();
}

bool
TomahawkPlaylistProvider::deletePlaylists( Playlists::PlaylistList playlistlist )
{
//    if( !isWritable() )
//        return false;

//    foreach( Playlists::PlaylistPtr playlist, playlistlist )
//    {
//        if( !m_playlists.contains( playlist ) )
//            continue;
//        if( KSharedPtr<TomahawkPlaylist>::staticCast( playlist )->type() != TomahawkPlaylist::Normal )
//            continue;  // special playlists cannot be removed using this method
//        m_playlists.removeOne( playlist );

//        unsubscribeFrom( playlist );
//        TomahawkPlaylist *tomahawkPlaylist = static_cast<TomahawkPlaylist *>( playlist.data() );
//        itdb_playlist_unlink( tomahawkPlaylist->itdbPlaylist() );

//        emit playlistRemoved( playlist );
//        emit startWriteDatabaseTimer();
//    }
//    return true;

    return false;
}

void
TomahawkPlaylistProvider::trackAdded( Playlists::PlaylistPtr, Meta::TrackPtr, int )
{
    emit startWriteDatabaseTimer();
}

void
TomahawkPlaylistProvider::trackRemoved( Playlists::PlaylistPtr, int )
{
    emit startWriteDatabaseTimer();
}

#include "TomahawkPlaylistProvider.moc"
