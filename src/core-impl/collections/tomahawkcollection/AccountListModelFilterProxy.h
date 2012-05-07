/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2012, Leo Franchi <lfranchi@kde.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOMAHAWKACCOUNTMODELFILTERPROXY_H
#define TOMAHAWKACCOUNTMODELFILTERPROXY_H

#include "/home/llg/Programacao/tomahawk/src/libtomahawk/accounts/Account.h"

#include <QSortFilterProxyModel>

namespace Tomahawk {
namespace Accounts {

class AccountListModelFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    AccountListModelFilterProxy( QObject* parent = 0 );

    void setFilterType( Tomahawk::Accounts::AccountType type );

    virtual void setSourceModel( QAbstractItemModel* sourceModel );

signals:
    void scrollTo( const QModelIndex& idx );

    void startInstalling( const QPersistentModelIndex& idx );
    void doneInstalling( const QPersistentModelIndex& idx );

protected:
    virtual bool filterAcceptsRow ( int sourceRow, const QModelIndex& sourceParent ) const;

private slots:
    void onScrollTo( const QModelIndex& idx );

    void onStartInstalling( const QPersistentModelIndex& idx );
    void onDoneInstalling( const QPersistentModelIndex& idx );

private:
    Tomahawk::Accounts::AccountType m_filterType;
};

}

}
#endif // TOMAHAWKACCOUNTMODELFILTERPROXY_H
