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

#include "TomahawkCollectionLocation.h"

#include "core/support/Debug.h"
#include <ThreadWeaver/Weaver>

TomahawkCollectionLocation::TomahawkCollectionLocation( QWeakPointer<TomahawkCollection> parentCollection )
    : CollectionLocation()  // we implement collection(), we need not pass parentCollection
    , m_coll( parentCollection )
{
}

TomahawkCollectionLocation::~TomahawkCollectionLocation()
{
}

Collections::Collection*
TomahawkCollectionLocation::collection() const
{
    // overriden to avoid dangling pointers
    return m_coll.data();
}

QString
TomahawkCollectionLocation::prettyLocation() const
{
    if( m_coll )
        return m_coll.data()->prettyName();
    // match string with TomahawkCopyTracksJob::slotDisplaySorryDialog()
    return i18n( "Disconnected iPod/iPad/iPhone" );
}

bool
TomahawkCollectionLocation::isWritable() const
{
    if( !m_coll )
        return false;
    return m_coll.data()->isWritable(); // no infinite loop, TomahawkCollection iplements this
}

void TomahawkCollectionLocation::showDestinationDialog( const Meta::TrackList &tracks,
                                                    bool removeSources,
                                                    const Transcoding::Configuration &configuration )
{
    setGoingToRemoveSources( removeSources ); // otherwise it is impossible to get it from source
    Collections::CollectionLocation::showDestinationDialog( tracks, removeSources, configuration );
}

void
TomahawkCollectionLocation::setDestinationPlaylist( Playlists::PlaylistPtr destPlaylist, const QMap<Meta::TrackPtr, int> &trackPlaylistPositions )
{
    m_destPlaylist = destPlaylist;
    m_trackPlaylistPositions = trackPlaylistPositions;
}

#include "TomahawkCollectionLocation.moc"
