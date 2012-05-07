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

#ifndef GPODDERSERVICESETTINGS_H
#define GPODDERSERVICESETTINGS_H

#include "ui_TomahawkConfigWidget.h"

#include <kcmodule.h>

#include <QNetworkReply>

namespace Ui { class TomahawkConfigWidget; }

namespace Tomahawk
{
    namespace Accounts
    {
        class AccountListModelFilterProxy;
        class AccountListModel;
        class AccountFactory;
        class Account;
    }
}

class QListWidgetItem;

class TomahawkServiceSettings : public KCModule
{
    Q_OBJECT

public:
    explicit TomahawkServiceSettings( QWidget *parent = 0, const QVariantList &args = QVariantList() );

    virtual ~TomahawkServiceSettings();

    virtual void save();
    virtual void load();
    virtual void defaults();

private slots:
    void settingsChanged();

    void createAccountFromFactory( Tomahawk::Accounts::AccountFactory* );
    void openAccountConfig( Tomahawk::Accounts::Account*, bool showDelete = false );
    void openAccountFactoryConfig( Tomahawk::Accounts::AccountFactory* );

private:
    Ui::TomahawkConfigWidget *m_configDialog;

    Tomahawk::Accounts::AccountListModel* m_accountModel;
    Tomahawk::Accounts::AccountListModelFilterProxy* m_accountProxy;
};

#endif // GPODDERSERVICESETTINGS_H
