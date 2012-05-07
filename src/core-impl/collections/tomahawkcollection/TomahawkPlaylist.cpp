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

#include "TomahawkPlaylist.h"

#include "core-impl/collections/support/MemoryMeta.h"
#include "core-impl/playlists/providers/user/UserPlaylistProvider.h"
#include "core/playlists/PlaylistProvider.h"
#include "core/support/Debug.h"
#include "TomahawkCollection.h"
#include "TomahawkMeta.h"
#include "TomahawkPlaylistProvider.h"

TomahawkPlaylist::TomahawkPlaylist( TomahawkCollection *collection )
    : m_coll( collection )
{
//    Q_ASSERT( m_playlist && collection );
//    for( GList *members = m_playlist->members; members; members = members->next )
//    {
//        Itdb_Track *itdbTrack = (Itdb_Track *) members->data;
//        Q_ASSERT( itdbTrack );
//        Meta::TrackPtr track = TomahawkMeta::Track::fromTomahawkTrack( itdbTrack );
//        Q_ASSERT( track );
//        track = collection->trackForUidUrl( track->uidUrl() ); // get MemoryMeta proxy track
//        Q_ASSERT( track );
//        m_tracks << track;
//    }
}

TomahawkPlaylist::TomahawkPlaylist( const Meta::TrackList &tracks, const QString &name,
                            TomahawkCollection *collection )
    : m_coll( collection )
{
//    m_playlist = itdb_playlist_new( name.toUtf8(), false /* Smart playlist */ );
//    Q_ASSERT( m_playlist );

//    if( m_type != Normal )
//    {
//        m_tracks = tracks;
//        return; // m_playlist holds just the name in this case
//    }

//    int position = 0;
//    int finalPosition = 0;
//    foreach( Meta::TrackPtr track, tracks )
//    {
//        if( track->collection() == collection ) // track from associated collection
//        {
//            addTomahawkTrack( track, position );
//            position++;
//        }
//        else
//            m_tracksToCopy << TrackPosition( track, finalPosition );
//        finalPosition++;  // yes increment every time, tracks are inserted in order so this is correct
//    }

//    if( !m_tracksToCopy.isEmpty() )
//        scheduleCopyAndInsert();
}

TomahawkPlaylist::~TomahawkPlaylist()
{
}

KUrl
TomahawkPlaylist::uidUrl() const
{
//    // integer reading is atomic, no lock needed
//    QString collId = m_coll ? m_coll.data()->collectionId() : "removedtomahawkcolleciton:/";
//    return QString( "%1/playlists/%2" ).arg( collId ).arg( m_playlist->id );

    return KUrl();
}

QString
TomahawkPlaylist::name() const
{
//    QReadLocker locker( &m_playlistLock );
//    return QString::fromUtf8( m_playlist->name );

    return "";
}

void
TomahawkPlaylist::setName( const QString &name )
{
//    QWriteLocker locker( &m_playlistLock );
//    g_free( m_playlist->name );
//    m_playlist->name = g_strdup( name.toUtf8() );
}

Playlists::PlaylistProvider*
TomahawkPlaylist::provider() const
{
    return m_coll ? m_coll.data()->playlistProvider() : 0;
}

int
TomahawkPlaylist::trackCount() const
{
    return m_tracks.count();
}

Meta::TrackList
TomahawkPlaylist::tracks()
{
    return m_tracks;
}

void
TomahawkPlaylist::addTrack( Meta::TrackPtr track, int position )
{
//    if( m_type != Normal || !m_coll || !m_coll.data()->isWritable() )
//        return;

//    if( position < 0 || position > m_tracks.count() )
//        position = m_tracks.count();

//    if( track->collection() == m_coll.data() ) // track from associated collection
//        addTomahawkTrack( track, position );
//    else
//    {
//        m_tracksToCopy << TrackPosition( track, position );
//        scheduleCopyAndInsert();
//    }
}

void
TomahawkPlaylist::removeTrack( int position )
{
//    // we should fail only if position is incorrect, prevent infinite loops in
//    // TomahawkPlaylistProvider::removeTrackFromPlaylists()
//    if( position < 0 || position >= m_tracks.count() )
//        return;

//    Meta::TrackPtr removedTrack = m_tracks.takeAt( position );
//    if( m_type == Stale || m_type == Orphaned )
//    {
//        notifyObserversTrackRemoved( position );
//        return; // do not fire following machinery for special playlists
//    }

//    KSharedPtr<MemoryMeta::Track> proxyTrack = KSharedPtr<MemoryMeta::Track>::dynamicCast( removedTrack );
//    if( !proxyTrack )
//    {
//        error() << __PRETTY_FUNCTION__ << "track" << removedTrack.data() << "from m_track was not MemoryMeta track!";
//        return;
//    }

//    KSharedPtr<TomahawkMeta::Track> tomahawkTrack = KSharedPtr<TomahawkMeta::Track>::dynamicCast( proxyTrack->originalTrack() );
//    if( !proxyTrack )
//    {
//        error() << __PRETTY_FUNCTION__ << "originalTrack of the proxyTrack was not TomahawkMeta track!";
//        return;
//    }

//    {
//        // notify observers _without_ the lock held
//        QWriteLocker locker( &m_playlistLock );
//        itdb_playlist_remove_track( m_playlist, tomahawkTrack->itdbTrack() );
//    }
//    notifyObserversTrackRemoved( position );
}

//void
//TomahawkPlaylist::addTomahawkTrack( Meta::TrackPtr track, int position )
//{
////    Q_ASSERT( position >= 0 && position <= m_tracks.count() );

////    Meta::TrackPtr proxyTrack = Meta::TrackPtr();
////    KSharedPtr<MemoryMeta::Track> memoryTrack = KSharedPtr<MemoryMeta::Track>::dynamicCast( track );
////    if( memoryTrack )
////    {
////        track = memoryTrack->originalTrack();  // iPod track is usually hidden below MemoryMeta proxy
////        proxyTrack = track;
////    }
////    KSharedPtr<TomahawkMeta::Track> tomahawkTrack = KSharedPtr<TomahawkMeta::Track>::dynamicCast( track );
////    if( !tomahawkTrack )
////    {
////        error() << __PRETTY_FUNCTION__ << "Could not get TomahawkMeta::Track out of supplied track."
////                << ( memoryTrack ? "(but cast to MemoryMeta::Track succeeded)"
////                                 : "(cast to MemoryMeta::Track failed too)" );
////        return;
////    }

////    if( !proxyTrack)  // we got TomahawkTrack directly, expose its MemoryMeta proxy
////        proxyTrack = m_coll ? m_coll.data()->trackForUidUrl( tomahawkTrack->uidUrl() ) : Meta::TrackPtr();
////    if( !proxyTrack )
////    {
////        error() << __PRETTY_FUNCTION__ << "was passed TomahawkMeta::Track but we could not find"
////                << "MemoryMeta::Track proxy for it.";
////        return;
////    }

////    Itdb_Track *itdbTrack = tomahawkTrack->itdbTrack();
////    /* There is following code in libgpod's itdb_playlist_add_track():
////     *     g_return_if_fail (pl->itdb);
////     *     track->itdb = pl->itdb;
////     * Just fool libgpod by setting itdb to assumed value
////     */
////    Itdb_iTunesDB *save = m_playlist->itdb;
////    m_playlist->itdb = itdbTrack->itdb;
////    itdb_playlist_add_track( m_playlist, itdbTrack, -1 );
////    m_playlist->itdb = save;

////    m_tracks.insert( position, proxyTrack );
////    notifyObserversTrackAdded( proxyTrack, position );
//}
