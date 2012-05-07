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

#ifndef TOMAHAWKCOLLECTION_H
#define TOMAHAWKCOLLECTION_H

#include "core/collections/Collection.h"

#include <QSharedPointer>
#include <QTimer>

namespace Collections { class MemoryCollection; }
namespace TomahawkMeta { class Track; }

class TomahawkPlaylistProvider;

class TomahawkCollection : public Collections::Collection, public Meta::Observer
{
    Q_OBJECT

    public:
        static const QString s_uidUrlProtocol;

        /**
         * Creates an iPod collection on top of already-mounted filesystem.
         *
         * @param mountPoint actual iPod mount point to use, must be already mounted and
         * accessible. When eject is requested, solid StorageAccess with this mount point
         * is searched for to perform unmounting.
         */
        explicit TomahawkCollection();

        virtual ~TomahawkCollection();

        // TrackProvider methods:
        virtual bool possiblyContainsTrack( const KUrl &url ) const;
        virtual Meta::TrackPtr trackForUrl( const KUrl &url );

        // CollectionBase methods:
        virtual bool hasCapabilityInterface( Capabilities::Capability::Type type ) const;
        virtual Capabilities::Capability* createCapabilityInterface( Capabilities::Capability::Type type );

        // Collection methods:
        virtual Collections::QueryMaker *queryMaker();

        virtual QString uidUrlProtocol() const;
        virtual QString collectionId() const;
        virtual QString prettyName() const;
        virtual KIcon icon() const;

        virtual bool hasCapacity() const;
        virtual float usedCapacity() const;
        virtual float totalCapacity() const;

        virtual Collections::CollectionLocation *location();
        virtual bool isOrganizable() const;
        float capacityMargin() const;

        // Observer methods:
        virtual void metadataChanged( Meta::TrackPtr track );
        // so that the compiler doesn't complain about hidden virtual functions:
        using Meta::Observer::metadataChanged;

        // TomahawkCollection methods:

        /**
         * Return pointer to playlist provider associated with this iPod. May be null in
         * special cases (iPod not yet initialised etc.)
         */
        Playlists::UserPlaylistProvider *playlistProvider() const;

        Meta::TrackPtr trackForUidUrl( const QString &uidUrl );

    signals:
        /**
         * Start a count-down that emits updated() signal after it expires.
         * Resets the timer to original timeout if already running. This is to ensure
         * that we emit update() max. once per <timeout> for batch updates.
         *
         * Timers can only be started from "their" thread so use signals & slots for that.
         */
        void startUpdateTimer();

        /**
         * Start a count-down that initiates iTunes database wrtiging after it expires.
         * Resets the timer to original timeout if already running. This is to ensure
         * that we dont write the database all the time for batch updates.
         *
         * Timers can only be started from "their" thread so use signals & slots for that.
         */
        void startWriteDatabaseTimer();

    public slots:
        /**
         * Destroy the collection, try to write back iTunes database (if dirty)
         */
        void slotDestroy();

        /**
         * Shows the configuration dialog in a non-modal window. If m_itdb is null, shows
         * some info and a button to try to initialize iPod.
         */
        void slotShowConfigureDialog( const QString &errorMessage = QString() );

    private slots:
        /**
         * Tries to initialize iPod, read the database, add tracks. (Re)shows the
         * configuration dialog with info about initialization.
         */
        void slotInitialize();

        /**
         * Sets iPod name to the name in configure dialog.
         */
        void slotApplyConfiguration();

        /**
         * Starts a timer that emits updated() signal after 2 seconds.
         */
        void slotStartUpdateTimer();

        /**
         * Starts a timer that initiates iTunes database writing after 30 seconds.
         */
        void slotStartWriteDatabaseTimer();

        /**
         * Enqueues a job in a thread that writes iTunes database back to iPod. Should
         * only be called from m_writeDatabaseTimer's timeout() signal. (with exception
         * when TomahawkCollection is about to destroy itself)
         */
        void slotInitiateDatabaseWrite();

    private:
        /**
         * In-fact constructor used to share code between different constructors.
         */
        void init();

        // method for TomahawkParseTracksJob and TomahawkCopyTracksJob:
        /**
         * Add an iPod track to the collection.
         *
         * This method adds it to the collection, master playlist (if not already there)
         * etc. The file must be already physically copied to iPod. (Re)Sets track's
         * collection to this collection. Takes ownership of the track (passes it to
         * KSharedPtr)
         *
         * This method is thread-safe.
         *
         * @return pointer to newly added track if successful, null pointer otherwise
         */
        Meta::TrackPtr addTrack( TomahawkMeta::Track *track );

        // method for TomahawkDeleteTracksJob:
        /**
         * Removes a track from iPod collection. Does not delete the file physically,
         * caller must do it after calling this method.
         *
         * @param track a track from associated MemoryCollection to delete. Accepts also
         * underlying TomahawkMeta::Track, this is treated as if MemoryMeta::Track track
         * proxy it was passed.
         *
         * This method is thread-safe.
         */
        void removeTrack( const Meta::TrackPtr &track );

        // method for TomahawkWriteDatabaseJob and destructor:
        /**
         * Calls itdb_write() directly. Logs a message about success/failure in Amarok
         * interface.
         */
        bool writeDatabase();

        QString m_prettyName;
        QSharedPointer<Collections::MemoryCollection> m_mc;
        TomahawkPlaylistProvider *m_playlistProvider;
};

#endif // TOMAHAWKCOLLECTION_H
