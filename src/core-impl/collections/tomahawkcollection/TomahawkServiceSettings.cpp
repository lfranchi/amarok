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

#define DEBUG_PREFIX "TomahawkServiceSettings"

#include "TomahawkServiceSettings.h"

#include "accounts/AccountDelegate.h"
#include "accounts/AccountFactoryWrapper.h"
#include "accounts/AccountModelFilterProxy.h"
#include "accounts/AccountModel.h"
#include "core/support/Debug.h"
#include "accounts/Account.h"
#include "accounts/AccountManager.h"
#include "database/Database.h"
#include "network/Servent.h"
#include "TomahawkSettings.h"
#include "utils/GuiHelpers.h"

#include <KLocale>
#include <KMessageBox>
#include <KPluginFactory>

using namespace Tomahawk;

K_PLUGIN_FACTORY( TomahawkServiceSettingsFactory, registerPlugin<TomahawkServiceSettings>(); )
K_EXPORT_PLUGIN( TomahawkServiceSettingsFactory( "kcm_amarok_tomahawk" ) )

TomahawkServiceSettings::TomahawkServiceSettings( QWidget *parent, const QVariantList &args )
        : KCModule( TomahawkServiceSettingsFactory::componentData(), parent, args )
{
    debug() << "Creating tomahawk config object";

    QVBoxLayout* l = new QVBoxLayout( this );
    QWidget *w = new QWidget;
    m_configDialog = new Ui::TomahawkConfigWidget;
    m_configDialog->setupUi( w );
    l->addWidget( w );

    // Accounts
    Accounts::AccountDelegate* accountDelegate = new Accounts::AccountDelegate( this );
    m_configDialog->accountsListView->setItemDelegate( accountDelegate );
    m_configDialog->accountsListView->setContextMenuPolicy( Qt::CustomContextMenu );
    m_configDialog->accountsListView->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
    m_configDialog->accountsListView->setMouseTracking( true );

    connect( accountDelegate, SIGNAL( openConfig( Tomahawk::Accounts::Account* ) ), this, SLOT( openAccountConfig( Tomahawk::Accounts::Account* ) ) );
    connect( accountDelegate, SIGNAL( openConfig( Tomahawk::Accounts::AccountFactory* ) ), this, SLOT( openAccountFactoryConfig( Tomahawk::Accounts::AccountFactory* ) ) );
    connect( accountDelegate, SIGNAL( update( QModelIndex ) ), m_configDialog->accountsListView, SLOT( update( QModelIndex ) ) );

    m_accountModel = new Accounts::AccountModel( this );
    m_accountProxy = new Accounts::AccountModelFilterProxy( m_accountModel );
    m_accountProxy->setSourceModel( m_accountModel );

    connect( m_accountProxy, SIGNAL( startInstalling( QPersistentModelIndex ) ), accountDelegate, SLOT( startInstalling(QPersistentModelIndex) ) );
    connect( m_accountProxy, SIGNAL( doneInstalling( QPersistentModelIndex ) ), accountDelegate, SLOT( doneInstalling(QPersistentModelIndex) ) );

    m_configDialog->accountsListView->setModel( m_accountProxy );

    connect( m_accountModel, SIGNAL( createAccount( Tomahawk::Accounts::AccountFactory* ) ), this, SLOT( createAccountFromFactory( Tomahawk::Accounts::AccountFactory* ) ) );

    load();
}


TomahawkServiceSettings::~TomahawkServiceSettings()
{
}


void
TomahawkServiceSettings::save()
{
    KCModule::save();
}

void
TomahawkServiceSettings::load()
{
    KCModule::load();
}

void
TomahawkServiceSettings::defaults()
{

}

void
TomahawkServiceSettings::settingsChanged()
{
    emit changed( true );
}

void
TomahawkServiceSettings::openAccountFactoryConfig( Accounts::AccountFactory* factory )
{
    QList< Accounts::Account* > accts;
    foreach ( Accounts::Account* acct, Accounts::AccountManager::instance()->accounts() )
    {
        if ( Accounts::AccountManager::instance()->factoryForAccount( acct ) == factory )
            accts << acct;
        if ( accts.size() > 1 )
            break;
    }
    Q_ASSERT( accts.size() > 0 ); // Shouldn't have a config wrench if there are no accounts!
    if ( accts.size() == 1 )
    {
        // If there's just one, open the config directly w/ the delete button. Otherwise open the multi dialog
        openAccountConfig( accts.first(), true );
        return;
    }

    AccountFactoryWrapper* dialog = new AccountFactoryWrapper( factory, this );
    dialog->show();
}


void
TomahawkServiceSettings::createAccountFromFactory( Accounts::AccountFactory* factory )
{
    TomahawkUtils::createAccountFromFactory( factory, this );
}


void
TomahawkServiceSettings::openAccountConfig( Accounts::Account* account, bool showDelete )
{
    TomahawkUtils::openAccountConfig( account, this, showDelete );
}
