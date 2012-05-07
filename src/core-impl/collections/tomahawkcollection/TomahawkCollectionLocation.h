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

#ifndef TOMAHAWKCOLLECTIONLOCATION_H
#define TOMAHAWKCOLLECTIONLOCATION_H

#include "core/collections/CollectionLocation.h"
#include "core/playlists/Playlist.h"
#include "TomahawkCollection.h"

#include <QWeakPointer>

class TomahawkCollectionLocation : public Collections::CollectionLocation
{
    Q_OBJECT

    public:
        TomahawkCollectionLocation( QWeakPointer<TomahawkCollection> parentCollection );
        virtual ~TomahawkCollectionLocation();

        // CollectionLocation methods:
        virtual Collections::Collection *collection() const;
        virtual QString prettyLocation() const;
        virtual bool isWritable() const;

        // overriden to get removeSources out of source location
        virtual void showDestinationDialog( const Meta::TrackList &tracks,
                                            bool removeSources,
                                            const Transcoding::Configuration &configuration );

        // TomahawkCollectionLocation specific methods:
        /**
         * Calling this causes that when the tracks are copied, they are added to iPod
         * playlist @param playlist
         */
        void setDestinationPlaylist( Playlists::PlaylistPtr destPlaylist,
                                     const QMap<Meta::TrackPtr, int> &trackPlaylistPositions );

        /**
         * This method is published so that TomahawkPlaylistProvider can hide removal dialog.
         */
        using Collections::CollectionLocation::setHidingRemoveConfirm;

    private:
        QWeakPointer<TomahawkCollection> m_coll;
        QMap<Meta::TrackPtr, int> m_trackPlaylistPositions;
        Playlists::PlaylistPtr m_destPlaylist;
};

#endif // TOMAHAWKCOLLECTIONLOCATION_H
