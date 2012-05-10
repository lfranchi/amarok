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

#include "TomahawkCollectionFactory.h"

#include "core-impl/collections/support/MemoryMeta.h"
#include "core/interfaces/Logger.h"
#include "core/support/Components.h"
#include "core/support/Debug.h"
#include "accounts/AccountManager.h"
#include "Collection.h"
#include "database/Database.h"
#include "database/DatabaseResolver.h"
#include "database/LocalCollection.h"
#include "DropJob.h"
#include "network/Servent.h"
#include "Pipeline.h"
#include "resolvers/QtScriptResolver.h"
#include "resolvers/ScriptResolver.h"
#include "sip/SipHandler.h"
#include "sip/SipInfo.h"
#include "sip/SipPlugin.h"
#include "Source.h"
#include "SourceList.h"
#include "TomahawkSettingsGui.h"
#include "TomahawkSettings.h"
#include "utils/TomahawkCache.h"
#include "utils/TomahawkUtils.h"
#include "TomahawkCollection.h"
#include "TomahawkCollectionLocation.h"
#include "TomahawkMeta.h"
#include "playlist/dynamic/GeneratorFactory.h"
#include "playlist/dynamic/database/DatabaseGenerator.h"
#include "playlist/dynamic/echonest/EchonestGenerator.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>
#include <QtCore/QPluginLoader>
#include <QtCore/QTime>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QWeakPointer>

using namespace Tomahawk;

AMAROK_EXPORT_COLLECTION( TomahawkCollectionFactory, tomahawkcollection )

TomahawkCollectionFactory::TomahawkCollectionFactory( QObject *parent, const QVariantList &args )
    : CollectionFactory( parent, args )
    , m_loaded( true )
{
    m_info = KPluginInfo( "amarok_collection_tomahawkcollection.desktop", "services" );
}

TomahawkCollectionFactory::~TomahawkCollectionFactory()
{
    DEBUG_BLOCK

    tLog() << "Shutting down Tomahawk...";

    Pipeline::instance()->stop();

    if ( !m_servent.isNull() )
        delete m_servent.data();

    delete Tomahawk::Accounts::AccountManager::instance();
    delete TomahawkUtils::Cache::instance();
    delete AtticaManager::instance();

    if ( !m_database.isNull() )
        delete m_database.data();

    delete Pipeline::instance();
    delete InfoSystem::InfoSystem::instance();
}

void
TomahawkCollectionFactory::init()
{
    // MUST register StateHash ****before*** initing TomahawkSettingsGui as constructor of settings does upgrade before Gui subclass registers type
    TomahawkSettings::registerCustomSettingsHandlers();
    TomahawkSettingsGui::registerCustomSettingsHandlers();

    new TomahawkSettingsGui( this );

    registerMetaTypes();

    new Pipeline();
    Pipeline::instance()->addExternalResolverFactory( boost::bind( &QtScriptResolver::factory, _1 ) );
    Pipeline::instance()->addExternalResolverFactory( boost::bind( &ScriptResolver::factory, _1 ) );

    tDebug() << "Init Echonest Factory.";
    GeneratorFactory::registerFactory( "echonest", new EchonestFactory );
    tDebug() << "Init Database Factory.";
    GeneratorFactory::registerFactory( "database", new DatabaseFactory );

    m_servent = QWeakPointer<Servent>( new Servent( this ) );
    connect( m_servent.data(), SIGNAL( ready() ), SLOT( initSIP() ) );

    initDatabase();

    m_accountManager = QWeakPointer< Tomahawk::Accounts::AccountManager >( new Tomahawk::Accounts::AccountManager( this ) );
    connect( m_accountManager.data(), SIGNAL( ready() ), SLOT( accountManagerReady() ) );

    InfoSystem::InfoSystem::instance();

    initLocalCollection();
    initPipeline();

    AtticaManager::instance();

    m_initialized = true;
}

void
TomahawkCollectionFactory::slotCollectionDestroyed( QObject *collection )
{
//    // remove destroyed collection from m_collectionMap
//    QMutableMapIterator<QString, TomahawkCollection *> it( m_collectionMap );
//    while( it.hasNext() )
//    {
//        it.next();
//        if( (QObject *) it.value() == collection )
//            it.remove();
//    }
}

void
TomahawkCollectionFactory::createCollectionForPeer( const QString &udi )
{
//    DEBUG_BLOCK
//    DeviceType type;
//    QString mountPointOrUuid;
//    Solid::Device device( udi );
//    Solid::StorageAccess *ssa = device.as<Solid::StorageAccess>();
//    if( ssa )
//    {
//        type = iPod;
//        if( ssa->isIgnored() )
//        {
//            debug() << "device" << udi << "ignored, ignoring :-)";
//            return;
//        }

//        // we are definitely interested in this device, listen for accessibility changes
//        disconnect( ssa, SIGNAL(accessibilityChanged(bool,QString)), this, 0 );
//        connect( ssa, SIGNAL(accessibilityChanged(bool,QString)),
//                SLOT(slotAccessibilityChanged(bool,QString)) );

//        if( !ssa->isAccessible() )
//        {
//            debug() << "device" << udi << "not accessible, ignoring for now";
//            return;
//        }
//        mountPointOrUuid = ssa->filePath();
//    }
//    else // no ssa
//    {
//        type = iOS;
//        debug() << "device" << udi << "has no StorageAccess interface, treating as iPhone/iPad";
//        // HACK: in order to avoid many false-positives once KDE's solid attaches PMP
//        // interface to class iPods too, check that both PMP and vendor/name matches for
//        // "leaf" iPhone device:
//        if( !device.is<Solid::PortableMediaPlayer>() || !deviceIsRootTomahawkDevice( device ) )
//        {
//            debug() << "Ignoring above device as it either has no PortableMediaPlayer interface or vendor/product doesn't match";
//            return;
//        }
//        // TODO: get device uuid
//    }

//    debug() << "creating iPod collection, mount-point or uuid:" << mountPointOrUuid;
//    TomahawkCollection *collection;
//    switch( type )
//    {
//        case iPod:
//            collection = new TomahawkCollection( QDir( mountPointOrUuid ) ); // QDir to call correct overload
//            break;
//        case iOS:
//            collection = new TomahawkCollection( mountPointOrUuid );
//            break;
//    }
//    m_collectionMap.insert( udi, collection );

//    // when the collection is destroyed by someone else, remove it from m_collectionMap:
//    connect( collection, SIGNAL(destroyed(QObject*)), SLOT(slotCollectionDestroyed(QObject*)) );

//    emit newCollection( collection );  // TODO: emit only when the collection was successfully created
}

void
TomahawkCollectionFactory::initPipeline()
{
    // setup resolvers for local content, and (cached) remote collection content
    Pipeline::instance()->addResolver( new DatabaseResolver( 100 ) );
}

void
TomahawkCollectionFactory::initDatabase()
{
    QString dbpath;
    dbpath = TomahawkUtils::appDataDir().absoluteFilePath( "tomahawk.db" );

    m_database = QWeakPointer<Database>( new Database( dbpath, this ) );
    Pipeline::instance()->databaseReady();
}

void
TomahawkCollectionFactory::initLocalCollection()
{
    connect( SourceList::instance(), SIGNAL( ready() ), SLOT( initServent() ) );

    //HACK: load playlists when sources are added
    onSourcesAdded( SourceList::instance()->sources() );
    connect( SourceList::instance(), SIGNAL( sourceAdded( source_ptr ) ), SLOT( onSourceAdded( Tomahawk::source_ptr ) ) );

    source_ptr src( new Source( 0, tr( "My Collection" ) ) );
    collection_ptr coll( new LocalCollection( src ) );

    src->addCollection( coll );
    SourceList::instance()->setLocal( src );
    SourceList::instance()->loadSources();
}


void
TomahawkCollectionFactory::onSourcesAdded( const QList<source_ptr>& sources )
{
    foreach( const source_ptr& source, sources )
        onSourceAdded( source );
}


void
TomahawkCollectionFactory::onSourceAdded( const source_ptr& source )
{
    source->collection()->playlists();
    source->collection()->autoPlaylists();
    source->collection()->stations();
}

void
TomahawkCollectionFactory::initServent()
{
    tDebug() << "Init Servent.";

    bool upnp = false;
    int port = TomahawkSettings::instance()->externalPort();

    Servent::instance()->startListening( QHostAddress( QHostAddress::Any ), upnp, port );
}

void
TomahawkCollectionFactory::initSIP()
{
    Accounts::AccountManager::instance()->initSIP();

    m_loaded = true;
    emit libTomahawkLoaded();
}

void
TomahawkCollectionFactory::accountManagerReady()
{
    Accounts::AccountManager::instance()->loadFromConfig();
}

void
TomahawkCollectionFactory::registerMetaTypes()
{
    qRegisterMetaType< QSharedPointer<DatabaseCommand> >("QSharedPointer<DatabaseCommand>");
    qRegisterMetaType< DBSyncConnection::State >("DBSyncConnection::State");
    qRegisterMetaType< msg_ptr >("msg_ptr");
    qRegisterMetaType< QList<dbop_ptr> >("QList<dbop_ptr>");
    qRegisterMetaType< QList<QVariantMap> >("QList<QVariantMap>");
    qRegisterMetaType< QList<QString> >("QList<QString>");
    qRegisterMetaType< QList<uint> >("QList<uint>");
    qRegisterMetaType< Connection* >("Connection*");
    qRegisterMetaType< QAbstractSocket::SocketError >("QAbstractSocket::SocketError");
    qRegisterMetaType< QTcpSocket* >("QTcpSocket*");
    qRegisterMetaType< QSharedPointer<QIODevice> >("QSharedPointer<QIODevice>");
    qRegisterMetaType< QFileInfo >("QFileInfo");
    qRegisterMetaType< QDir >("QDir");
    qRegisterMetaType< QHostAddress >("QHostAddress");
    qRegisterMetaType< QMap<QString, unsigned int> >("QMap<QString, unsigned int>");
    qRegisterMetaType< QMap< QString, plentry_ptr > >("QMap< QString, plentry_ptr >");
    qRegisterMetaType< QHash< QString, QMap<quint32, quint16> > >("QHash< QString, QMap<quint32, quint16> >");
    qRegisterMetaType< QMap< QString, QMap< unsigned int, unsigned int > > >("QMap< QString, QMap< unsigned int, unsigned int > >");
    qRegisterMetaType< PairList >("PairList");

    qRegisterMetaType< GeneratorMode>("GeneratorMode");
    qRegisterMetaType<Tomahawk::GeneratorMode>("Tomahawk::GeneratorMode");

    // Extra definition for namespaced-versions of signals/slots required
    qRegisterMetaType< Tomahawk::source_ptr >("Tomahawk::source_ptr");
    qRegisterMetaType< Tomahawk::collection_ptr >("Tomahawk::collection_ptr");
    qRegisterMetaType< Tomahawk::result_ptr >("Tomahawk::result_ptr");
    qRegisterMetaType< Tomahawk::query_ptr >("Tomahawk::query_ptr");
    qRegisterMetaType< Tomahawk::source_ptr >("Tomahawk::source_ptr");
    qRegisterMetaType< Tomahawk::dyncontrol_ptr >("Tomahawk::dyncontrol_ptr");
    qRegisterMetaType< Tomahawk::playlist_ptr >("Tomahawk::playlist_ptr");
    qRegisterMetaType< Tomahawk::playlistinterface_ptr >("Tomahawk::playlistinterface_ptr");
    qRegisterMetaType< Tomahawk::dynplaylist_ptr >("Tomahawk::dynplaylist_ptr");
    qRegisterMetaType< Tomahawk::geninterface_ptr >("Tomahawk::geninterface_ptr");
    qRegisterMetaType< QList<Tomahawk::playlist_ptr> >("QList<Tomahawk::playlist_ptr>");
    qRegisterMetaType< QList<Tomahawk::dynplaylist_ptr> >("QList<Tomahawk::dynplaylist_ptr>");
    qRegisterMetaType< QList<Tomahawk::dyncontrol_ptr> >("QList<Tomahawk::dyncontrol_ptr>");
    qRegisterMetaType< QList<Tomahawk::geninterface_ptr> >("QList<Tomahawk::geninterface_ptr>");
    qRegisterMetaType< QList<Tomahawk::plentry_ptr> >("QList<Tomahawk::plentry_ptr>");
    qRegisterMetaType< QList<Tomahawk::query_ptr> >("QList<Tomahawk::query_ptr>");
    qRegisterMetaType< QList<Tomahawk::result_ptr> >("QList<Tomahawk::result_ptr>");
    qRegisterMetaType< QList<Tomahawk::artist_ptr> >("QList<Tomahawk::artist_ptr>");
    qRegisterMetaType< QList<Tomahawk::album_ptr> >("QList<Tomahawk::album_ptr>");
    qRegisterMetaType< QList<Tomahawk::source_ptr> >("QList<Tomahawk::source_ptr>");
    qRegisterMetaType< QMap< QString, Tomahawk::plentry_ptr > >("QMap< QString, Tomahawk::plentry_ptr >");
    qRegisterMetaType< Tomahawk::PlaylistRevision >("Tomahawk::PlaylistRevision");
    qRegisterMetaType< Tomahawk::DynamicPlaylistRevision >("Tomahawk::DynamicPlaylistRevision");
    qRegisterMetaType< Tomahawk::QID >("Tomahawk::QID");

    qRegisterMetaType< AudioErrorCode >("AudioErrorCode");

    qRegisterMetaType< Tomahawk::InfoSystem::InfoStringHash >( "Tomahawk::InfoSystem::InfoStringHash" );
    qRegisterMetaType< Tomahawk::InfoSystem::InfoType >( "Tomahawk::InfoSystem::InfoType" );
    qRegisterMetaType< Tomahawk::InfoSystem::InfoRequestData >( "Tomahawk::InfoSystem::InfoRequestData" );
    qRegisterMetaType< Tomahawk::InfoSystem::InfoSystemCache* >( "Tomahawk::InfoSystem::InfoSystemCache*" );
    qRegisterMetaType< Tomahawk::InfoSystem::InfoPluginPtr >( "Tomahawk::InfoSystem::InfoPluginPtr" );
    qRegisterMetaType< QList< Tomahawk::InfoSystem::InfoStringHash > >("QList< Tomahawk::InfoSystem::InfoStringHash > ");
    qRegisterMetaTypeStreamOperators< QList< Tomahawk::InfoSystem::InfoStringHash > >("QList< Tomahawk::InfoSystem::InfoStringHash > ");
    qRegisterMetaType< QPersistentModelIndex >( "QPersistentModelIndex" );

    qRegisterMetaType< Tomahawk::PlaylistInterface::LatchMode >( "Tomahawk::PlaylistInterface::LatchMode" );
}

#include "TomahawkCollectionFactory.moc"
