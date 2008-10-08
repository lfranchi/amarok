/***************************************************************************
 * copyright         : (C) 2008 Daniel Caleb Jones <danielcjones@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#include "SimpleTrackNavigator.h"

#include "playlist/PlaylistModel.h"
#include "playlist/PlaylistActions.h"

Playlist::SimpleTrackNavigator::SimpleTrackNavigator() { }

int
Playlist::SimpleTrackNavigator::lastRow()
{
    int updateRow = The::playlistModel()->activeRow() - 1;
    return The::playlistModel()->rowExists( updateRow ) ? updateRow : -1;
}

void
Playlist::SimpleTrackNavigator::requestNextTrack()
{
    The::playlistActions()->setNextRow( nextRow() );
}

void
Playlist::SimpleTrackNavigator::requestUserNextTrack()
{
    The::playlistActions()->setUserNextRow( userNextRow() );
}

void
Playlist::SimpleTrackNavigator::requestLastTrack()
{
    The::playlistActions()->setPrevRow( lastRow() );
}

