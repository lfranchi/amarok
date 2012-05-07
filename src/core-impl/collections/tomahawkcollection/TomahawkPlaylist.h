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

#ifndef TOMAHAWKPLAYLIST_H
#define TOMAHAWKPLAYLIST_H

#include "core/playlists/Playlist.h"
#include "TomahawkCollectionLocation.h"

class TomahawkCollection;

// we cannot use QMap<Track, int> because it doesn't preserve order
typedef QPair<Meta::TrackPtr, int> TrackPosition;
typedef QList<TrackPosition> TrackPositionList;

/**
 * Represents playlist on the iPod. Takes ownership of the m_playlist pointer.
 */
class TomahawkPlaylist : public Playlists::Playlist
{
    public:
        /**
         * Create Amarok iPod playlist out of existing itdb playlist
         */
        TomahawkPlaylist( TomahawkCollection *collection );

        /**
         * Create new Amarok iPod playlist. Some @param tracks may not be in corresponding
         * iPod collection, these are copied to iPod (unless not matched by meta tags)
         *
         * @param type whether this playlist is an ordinatory one or a kind of special
         */
        TomahawkPlaylist( const Meta::TrackList &tracks, const QString &name,
                      TomahawkCollection *collection );

        virtual ~TomahawkPlaylist();

        virtual KUrl uidUrl() const;
        virtual QString name() const;
        virtual void setName( const QString &name );

        virtual Playlists::PlaylistProvider *provider() const;

        virtual int trackCount() const;
        virtual Meta::TrackList tracks();
        virtual void addTrack( Meta::TrackPtr track, int position = -1 );
        virtual void removeTrack( int position );

    private:
        Q_DISABLE_COPY( TomahawkPlaylist )

        QWeakPointer<TomahawkCollection> m_coll;
        Meta::TrackList m_tracks; // playlists tracks, in fact MemoryMeta::Track objects
};

#endif // TOMAHAWKPLAYLIST_H
