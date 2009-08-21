/****************************************************************************************
 * Copyright (c) 2009 Téo Mrnjavac <teo.mrnjavac@gmail.com>                             *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Pulic License for more details.              *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#ifndef AMAROK_OCSPERSONLISTWIDGET_H
#define AMAROK_OCSPERSONLISTWIDGET_H

#include "OcsAuthorItem.h"



class OcsPersonListWidget : public QWidget
{
    Q_OBJECT
public:
    OcsPersonListWidget( QWidget *parent = 0 );
    void addPerson( const KAboutPerson &person, const Attica::Person &ocsPerson );
    void addPerson( const KAboutPerson &person );

signals:
    void personAdded( int persons );

private:
    void addPersonPrivate( OcsAuthorItem *item );
    QWidget *m_personsArea;
    QVBoxLayout *m_areaLayout;

};

#endif  //AMAROK_OCSPERSONLISTWIDGET_H