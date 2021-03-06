/****************************************************************************************
 * Copyright (c) 2008 Daniel Caleb Jones <danielcjones@gmail.com>                       *
 * Copyright (c) 2009 Mark Kretschmann <kretschmann@kde.org>                            *
 * Copyright (c) 2010 Ralf Engels <ralf-engels@gmx.de>                                  *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) version 3 or        *
 * any later version accepted by the membership of KDE e.V. (or its successor approved  *
 * by the membership of KDE e.V.), which shall act as a proxy defined in Section 14 of  *
 * version 3 of the license.                                                            *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#include "core-impl/collections/support/CollectionManager.h"
#include "core/support/Debug.h"
#include "core/collections/MetaQueryMaker.h"
#include "core/collections/QueryMaker.h"
#include "widgets/MetaQueryWidget.h"
#include "widgets/kdatecombo.h"
#include "widgets/kratingwidget.h"
#include "shared/FileType.h"

#include <typeinfo>

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListView>
#include <QTimeEdit>

#include <KComboBox>
#include <KIcon>
// #include <KDateTime> // for local time
#include <klocalizeddate.h>
#include <KNumInput>
#include <klocale.h>

using namespace Amarok;

TimeDistanceWidget::TimeDistanceWidget( QWidget *parent )
    : QWidget( parent )
{
    m_timeEdit = new KIntSpinBox(this);
    m_timeEdit->setMinimum( 0 );
    m_timeEdit->setMaximum( 600 );

    m_unitSelection = new KComboBox(this);
    connect( m_timeEdit, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateComboBoxLabels(int)) );
    for (int i = 0; i < 7; ++i) {
        m_unitSelection->addItem( QString() );
    }
    slotUpdateComboBoxLabels( 0 );

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget( m_timeEdit );
    hLayout->addWidget( m_unitSelection );
}

qint64 TimeDistanceWidget::timeDistance() const
{
    qint64 time = m_timeEdit->value();
    switch( m_unitSelection->currentIndex() )
    {
    case 6:
        time *= 365*24*60*60; // years
        break;
    case 5:
        time *=  30*24*60*60; // months
        break;
    case 4:
        time *=   7*24*60*60; // weeks
        break;
    case 3:
        time *= 24; // days
    case 2:
        time *= 60; // hours
    case 1:
        time *= 60; // minutes
    }

    return time;
}

void TimeDistanceWidget::setTimeDistance( qint64 value )
{
    // as we don't store the time unit we try to reconstuct it
    int unit = 0;
    if( value > 600 || !(value % 60) ) {
        unit = 1;
        value /= 60;

        if( value > 600 || !(value % 60) ) {
            unit = 2;
            value /= 60;

            if( value > 72 || !(value % 24) ) {
                unit = 3;
                value /= 24;

                if( !(value % 365) ) {
                    unit = 6;
                    value /= 365;
                } else if( !(value % 30) ) {
                    unit = 5;
                    value /= 30;
                } else if( !(value % 7) ) {
                    unit = 4;
                    value /= 7;
                }
            }
        }
    }

    m_unitSelection->setCurrentIndex( unit );
    m_timeEdit->setValue( value );
}

void TimeDistanceWidget::connectChanged( QObject *receiver, const char *slot )
{
    connect( m_timeEdit, SIGNAL(valueChanged(const QString&)), receiver, slot );
    connect( m_unitSelection, SIGNAL(currentIndexChanged(int)), receiver, slot );
}


void TimeDistanceWidget::slotUpdateComboBoxLabels( int value )
{
    m_unitSelection->setItemText(0, i18np("second", "seconds", value));
    m_unitSelection->setItemText(1, i18np("minute", "minutes", value));
    m_unitSelection->setItemText(2, i18np("hour", "hours", value));
    m_unitSelection->setItemText(3, i18np("day", "days", value));
    m_unitSelection->setItemText(4, i18np("week", "weeks", value));
    m_unitSelection->setItemText(5, i18np("month", "months", value));
    m_unitSelection->setItemText(6, i18np("year", "years", value));
}


MetaQueryWidget::MetaQueryWidget( QWidget* parent, bool onlyNumeric, bool noCondition )
    : QWidget( parent )
    , m_onlyNumeric( onlyNumeric )
    , m_noCondition( noCondition )
    , m_settingFilter( false )
    , m_andLabel(0)
    , m_compareSelection(0)
    , m_valueSelection1(0)
    , m_valueSelection2(0)
{
    // note: we are using the strange layout structure because the KRatingWidget size depends on the height.
    m_layoutMain = new QVBoxLayout( this );
    m_layoutMain->setContentsMargins(0, 0, 0, 0);

    makeFieldSelection();
    m_layoutMain->addWidget( m_fieldSelection );

    m_layoutValue = new QHBoxLayout();
    m_layoutMain->addLayout(m_layoutValue);

    m_layoutValueLabels = new QVBoxLayout();
    m_layoutValue->addLayout(m_layoutValueLabels, 0);
    m_layoutValueValues = new QVBoxLayout();
    m_layoutValue->addLayout(m_layoutValueValues, 1);

    if( m_onlyNumeric ) {
        m_filter.field = Meta::valYear;
        m_filter.condition = Equals;

    } else {
        m_filter.field = 0;
        m_filter.condition = Contains;
    }

    setFilter(m_filter);
}

MetaQueryWidget::~MetaQueryWidget()
{
}

MetaQueryWidget::Filter
MetaQueryWidget::filter() const
{
    // special handling for between
    if( m_filter.condition == Contains )
    {
        Filter f = m_filter;
        f.numValue  = qMin(m_filter.numValue, m_filter.numValue2) - 1;
        f.numValue2 = qMax(m_filter.numValue, m_filter.numValue2) + 1;
    }
    return m_filter;
}

void
MetaQueryWidget::setFilter( const MetaQueryWidget::Filter &value )
{
    m_settingFilter = true;
    m_filter = value;

    int index = m_fieldSelection->findData( int(m_filter.field) );
    m_fieldSelection->setCurrentIndex( index == -1 ? 0 : index );

    if( !m_noCondition )
        makeCompareSelection();
    makeValueSelection();
    setValueSelection();

    m_settingFilter = false;
    emit changed(m_filter);
}

static void addIconItem( KComboBox *box, qint64 field )
{
    QString icon = Meta::iconForField( field );
    QString text = Meta::i18nForField( field );
    if( icon.isEmpty() )
        box->addItem( text, field );
    else
        box->addItem( KIcon( icon ), text, field );
}

void
MetaQueryWidget::makeFieldSelection()
{
    m_fieldSelection = new KComboBox( this );
    if (!m_onlyNumeric)
    {
        m_fieldSelection->addItem( i18n( "Simple Search" ), 0 );
        addIconItem( m_fieldSelection, Meta::valUrl );
        // note: what about directory?
        addIconItem( m_fieldSelection, Meta::valTitle );
        addIconItem( m_fieldSelection, Meta::valArtist );
        addIconItem( m_fieldSelection, Meta::valAlbumArtist );
        addIconItem( m_fieldSelection, Meta::valAlbum );
        addIconItem( m_fieldSelection, Meta::valGenre );
        addIconItem( m_fieldSelection, Meta::valComposer );
    }
    addIconItem( m_fieldSelection, Meta::valYear );
    if (!m_onlyNumeric)
        addIconItem( m_fieldSelection, Meta::valComment );
    addIconItem( m_fieldSelection, Meta::valTrackNr );
    addIconItem( m_fieldSelection, Meta::valDiscNr );
    addIconItem( m_fieldSelection, Meta::valBpm );
    addIconItem( m_fieldSelection, Meta::valLength );
    addIconItem( m_fieldSelection, Meta::valBitrate );
    addIconItem( m_fieldSelection, Meta::valSamplerate );
    addIconItem( m_fieldSelection, Meta::valFilesize );
    if (!m_onlyNumeric)
        addIconItem( m_fieldSelection, Meta::valFormat );
    addIconItem( m_fieldSelection, Meta::valCreateDate );
    addIconItem( m_fieldSelection, Meta::valScore );
    addIconItem( m_fieldSelection, Meta::valRating );
    addIconItem( m_fieldSelection, Meta::valFirstPlayed );
    addIconItem( m_fieldSelection, Meta::valLastPlayed );
    addIconItem( m_fieldSelection, Meta::valPlaycount );
    if (!m_onlyNumeric)
        addIconItem( m_fieldSelection, Meta::valLabel );
    addIconItem( m_fieldSelection, Meta::valModified );
    connect( m_fieldSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(fieldChanged(int)) );
}

void
MetaQueryWidget::fieldChanged( int i )
{
    if( m_settingFilter )
        return;

    qint64 field = 0;
    if( i<0 || i>=m_fieldSelection->count() )
        field = m_fieldSelection->itemData( 0 ).toInt();
    else
        field = m_fieldSelection->itemData( i ).toInt();

    if( m_filter.field == field )
        return; // nothing to do

    // -- reset the value and the condition if the new filter has another type
    if( isNumeric( m_filter.field ) != isNumeric( field ) )
    {
        m_filter.value.clear();
        if( isNumeric( field ) )
            m_filter.condition = Equals;
        else
            m_filter.condition = Contains;
    }
    if( !isDate( m_filter.field ) && isDate( field ) )
    {
        m_filter.numValue = QDateTime::currentDateTime().toTime_t();
        m_filter.numValue2 = QDateTime::currentDateTime().toTime_t();
    }
    else
    {
        m_filter.numValue = 0;
        m_filter.numValue2 = 0;
    }

    m_filter.field = field;

    // in the fieldChanged slot we assume that the field was really changed,
    // so we don't have a problem with throwing away all the old widgets

    if( !m_noCondition )
        makeCompareSelection();
    makeValueSelection();

    setValueSelection();

    emit changed(m_filter);
}

void
MetaQueryWidget::compareChanged( int index )
{
    FilterCondition condition = FilterCondition( m_compareSelection->itemData( index ).toInt() );

    if( m_filter.condition == condition )
        return; // nothing to do

    if( isDate( m_filter.field) )
    {
        if(  ( condition == OlderThan || condition == NewerThan )
            && m_filter.condition != OlderThan && m_filter.condition != NewerThan
          )
        {
            // fix some inaccuracies caused by the conversion absoulte/relative time specifications
            // this is actually just for visual consistency
            int unit = 0;
            qint64 value = QDateTime::currentDateTime().toTime_t() - m_filter.numValue;
            if( value > 600 || !(value % 60) ) {
                unit = 1;
                value /= 60;

                if( value > 600 || !(value % 60) ) {
                    unit = 2;
                    value /= 60;

                    if( value > 72 || !(value % 24) ) {
                        unit = 3;
                        value /= 24;

                        if( !(value % 365) ) {
                            unit = 6;
                            value /= 365;
                        } else if( !(value % 30) ) {
                            unit = 5;
                            value /= 30;
                        } else if( !(value % 7) ) {
                            unit = 4;
                            value /= 7;
                        }
                    }
                }
            }
            switch( unit )
            {
            case 6:
                value *= 365*24*60*60; // years
                break;
            case 5:
                value *=  30*24*60*60; // months
                break;
            case 4:
                value *=   7*24*60*60; // weeks
                break;
            case 3:
                value *= 24; // days
            case 2:
                value *= 60; // hours
            case 1:
                value *= 60; // minutes
            }
            m_filter.numValue = value;
        }
        else if( condition != OlderThan && condition != NewerThan
            && ( m_filter.condition == OlderThan || m_filter.condition == NewerThan )
          )
        {
            m_filter.numValue = QDateTime::currentDateTime().toTime_t() - m_filter.numValue;
        }
    }

    m_filter.condition = condition;

    // need to re-generate the value selection fields
    makeValueSelection();

    setValueSelection();

    emit changed(m_filter);
}

void
MetaQueryWidget::valueChanged( const QString& value )
{
    m_filter.value = value;

    emit changed(m_filter);
}

void
MetaQueryWidget::numValueChanged( int value )
{
    m_filter.numValue = value;

    emit changed(m_filter);
}

void
MetaQueryWidget::numValue2Changed( int value )
{
    m_filter.numValue2 = value;

    emit changed(m_filter);
}

void
MetaQueryWidget::numValueChanged( qint64 value )
{
    m_filter.numValue = value;

    emit changed(m_filter);
}

void
MetaQueryWidget::numValue2Changed( qint64 value )
{
    m_filter.numValue2 = value;

    emit changed(m_filter);
}

void
MetaQueryWidget::numValueChanged( const QTime& value )
{
    m_filter.numValue = qAbs( value.secsTo( QTime(0,0,0) ) );

    emit changed(m_filter);
}

void
MetaQueryWidget::numValue2Changed( const QTime& value )
{
    m_filter.numValue2 = qAbs( value.secsTo( QTime(0,0,0) ) );

    emit changed(m_filter);
}

void
MetaQueryWidget::numValueDateChanged()
{
    KDateCombo* dateSelection = qobject_cast<KDateCombo*>( sender() );
    if( dateSelection )
    {
        QDate date;
        dateSelection->getDate( &date );
        m_filter.numValue = QDateTime( date ).toTime_t();

        emit changed(m_filter);
    }
}

void
MetaQueryWidget::numValue2DateChanged()
{
    KDateCombo* dateSelection = qobject_cast<KDateCombo*>( sender() );
    if( dateSelection )
    {
        QDate date;
        dateSelection->getDate( &date );
        m_filter.numValue2 = QDateTime( date ).toTime_t();

        emit changed(m_filter);
    }
}

void
MetaQueryWidget::numValueTimeDistanceChanged()
{
    if( !sender() )
        return;

    // static_cast. Remember: the TimeDistanceWidget does not have a Q_OBJECT macro
    TimeDistanceWidget* distanceSelection = static_cast<TimeDistanceWidget*>( sender()->parent() );
    if( distanceSelection )
    {
        m_filter.numValue = distanceSelection->timeDistance();

        emit changed(m_filter);
    }
}

void
MetaQueryWidget::numValueFormatChanged(int index)
{
    KComboBox* combo = static_cast<KComboBox*>(sender());
    if( combo ) {
        m_filter.numValue = combo->itemData( index ).toInt();

        emit changed(m_filter);
    }
}

void
MetaQueryWidget::setValueSelection()
{
    if( m_compareSelection )
        m_layoutValueLabels->addWidget( m_compareSelection );

    if( m_filter.condition == Between )
    {
        delete m_andLabel; // delete the old label
        m_andLabel = new QLabel( i18n( "and" ), this );
        m_layoutValueLabels->addWidget( m_andLabel );
    }
    else
    {
        delete m_andLabel;
        m_andLabel = 0;
    }

    if( m_valueSelection1 )
        m_layoutValueValues->addWidget( m_valueSelection1 );

    if( m_valueSelection2 )
        m_layoutValueValues->addWidget( m_valueSelection2 );
}


void
MetaQueryWidget::makeCompareSelection()
{
    delete m_compareSelection;
    m_compareSelection = 0;

    qint64 field = m_filter.field;

    if( field == Meta::valFormat )
        return; // the field is fixed

    else if( isDate(field) )
    {
        m_compareSelection = new KComboBox();
        m_compareSelection->addItem( conditionToString( Equals, field ), (int)Equals );
        m_compareSelection->addItem( conditionToString( LessThan, field ), (int)LessThan );
        m_compareSelection->addItem( conditionToString( GreaterThan, field ), (int)GreaterThan );
        m_compareSelection->addItem( conditionToString( Between, field ), (int)Between );
        m_compareSelection->addItem( conditionToString( OlderThan, field ), (int)OlderThan );
        m_compareSelection->addItem( conditionToString( NewerThan, field ), (int)NewerThan );
    }
    else if( isNumeric(field) )
    {
        m_compareSelection = new KComboBox();
        m_compareSelection->addItem( conditionToString( Equals, field ), (int)Equals );
        m_compareSelection->addItem( conditionToString( LessThan, field ), (int)LessThan );
        m_compareSelection->addItem( conditionToString( GreaterThan, field ), (int)GreaterThan );
        m_compareSelection->addItem( conditionToString( Between, field ), (int)Between );
    }
    else
    {
        m_compareSelection = new KComboBox();
        m_compareSelection->addItem( conditionToString( Contains, field ), (int)Contains );
        m_compareSelection->addItem( conditionToString( Equals, field ), (int)Equals );
    }

    // -- select the correct entry (even if the condition is not one of the selection)
    int index = m_compareSelection->findData( int(m_filter.condition) );
    if( index == -1 )
    {
        index = 0;
        m_filter.condition = FilterCondition(m_compareSelection->itemData( index ).toInt());
        compareChanged(index);
    }
    m_compareSelection->setCurrentIndex( index == -1 ? 0 : index );

    connect( m_compareSelection, SIGNAL(currentIndexChanged(int)),
            SLOT(compareChanged(int)) );
}

void
MetaQueryWidget::makeValueSelection()
{
    delete m_valueSelection1;
    m_valueSelection1 = 0;
    delete m_valueSelection2;
    m_valueSelection2 = 0;

    qint64 field = m_filter.field;
    if( field == Meta::valUrl )
        makeFilenameSelection();
    else if( field == Meta::valTitle )
        // We,re not going to populate this. There tends to be too many titles.
        makeGenericComboSelection( true, 0 );
    else if( field == Meta::valArtist ||
        field == Meta::valAlbumArtist ||
        field == Meta::valAlbum ||
        field == Meta::valGenre ||
        field == Meta::valComposer )
        makeMetaComboSelection( field );
    else if( field == Meta::valYear )
        makeGenericNumberSelection( 1900, 2300, 1976 );
    else if( field == Meta::valComment )
        makeGenericComboSelection( true, 0 );
    else if( field == Meta::valTrackNr )
        makeGenericNumberSelection( 0, 100, 0 );
    else if( field == Meta::valDiscNr )
        makeGenericNumberSelection( 0, 10, 0 );
    else if( field == Meta::valBpm )
        makeGenericNumberSelection( 60, 200, 80 );
    else if( field == Meta::valLength )
        makeLengthSelection();
    else if( field == Meta::valBitrate )
        makeGenericNumberSelection( 60, 2000, 160, i18nc("Unit for data rate kilo bit per seconds", "kbps") );
    else if( field == Meta::valSamplerate )
        makeGenericNumberSelection( 8000, 48000, 44100, i18nc("Unit for sample rate", "Hz") );
    else if( field == Meta::valFilesize )
        makeGenericNumberSelection( 0, 1000, 10, i18nc("Unit for file size in mega byte", "MiB") );
    else if( field == Meta::valFormat )
        makeFormatComboSelection();
    else if( field == Meta::valCreateDate )
        makeDateTimeSelection();
    else if( field == Meta::valScore )
        makeGenericNumberSelection( 0, 100, 0 );
    else if( field == Meta::valRating )
        makeRatingSelection();
    else if( field == Meta::valFirstPlayed )
        makeDateTimeSelection();
    else if( field == Meta::valLastPlayed )
        makeDateTimeSelection();
    else if( field == Meta::valPlaycount )
        makeGenericNumberSelection( 0, 1000, 0 );
    else if( field == Meta::valLabel )
        makeGenericComboSelection( true, 0 );
    else if( field == Meta::valModified )
        makeDateTimeSelection();
    else // e.g. the simple search
        makeGenericComboSelection( true, 0 );
}

void
MetaQueryWidget::makeGenericComboSelection( bool editable, Collections::QueryMaker* populateQuery )
{
    KComboBox* combo = new KComboBox( this );
    combo->setEditable( editable );

    if( populateQuery != 0 )
    {
        m_runningQueries.insert(populateQuery, QWeakPointer<KComboBox>(combo));
        connect( populateQuery, SIGNAL(newResultReady(QStringList)),
                SLOT(populateComboBox(QStringList)) );
        connect( populateQuery, SIGNAL(queryDone()),
                SLOT(comboBoxPopulated()) );

        populateQuery->run();
    }
    combo->setEditText( m_filter.value );

    connect( combo, SIGNAL(editTextChanged( const QString& ) ),
            SLOT(valueChanged(const QString&)) );

    combo->completionObject()->setIgnoreCase( true );
    combo->setCompletionMode( KGlobalSettings::CompletionPopup );
    combo->setInsertPolicy( QComboBox::InsertAtTop );
    m_valueSelection1 = combo;
}

void
MetaQueryWidget::makeMetaComboSelection( qint64 field )
{
    Collections::QueryMaker* qm = new Collections::MetaQueryMaker( CollectionManager::instance()->queryableCollections() );
    qm->setQueryType( Collections::QueryMaker::Custom );
    qm->addReturnValue( field );
    qm->setAutoDelete( true );
    makeGenericComboSelection( true, qm );
}

void
MetaQueryWidget::populateComboBox( QStringList results )
{
    QObject* query = sender();
    if( !query )
        return;

    QWeakPointer<KComboBox> combo = m_runningQueries.value(query);
    if( combo.isNull() )
        return;

    // note: adding items seems to reset the edit text, so we have
    //   to take care of that.
    disconnect( combo.data(), 0, this, 0 );

    // want the results unique and sorted
    const QSet<QString> dataSet = results.toSet();
    QStringList dataList = dataSet.toList();
    dataList.sort();
    combo.data()->addItems( dataList );

    KCompletion* comp = combo.data()->completionObject();
    comp->setItems( dataList );

    // reset the text and re-enable the signal
    combo.data()->setEditText( m_filter.value );
    connect( combo.data(), SIGNAL(editTextChanged( const QString& ) ),
            SLOT(valueChanged(const QString&)) );
}

void
MetaQueryWidget::makeFormatComboSelection()
{
    KComboBox* combo = new KComboBox( this );
    combo->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
    QStringList filetypes = Amarok::FileTypeSupport::possibleFileTypes();
    for (int listpos=0;listpos<filetypes.size();listpos++)
    {
        combo->addItem(filetypes.at(listpos),listpos);
    }

    int index = m_fieldSelection->findData( (int)m_filter.numValue );
    combo->setCurrentIndex( index == -1 ? 0 : index );

    connect( combo,
             SIGNAL( currentIndexChanged(int) ),
             SLOT( numValueFormatChanged(int) ) );

    m_valueSelection1 = combo;
}

void
MetaQueryWidget::comboBoxPopulated()
{
    QObject* query = sender();
    if( !query )
        return;

    m_runningQueries.remove( query );
}

void
MetaQueryWidget::makeFilenameSelection()
{
    // Don't populate the combobox. Too many urls.
    makeGenericComboSelection( true, 0 );
}


void
MetaQueryWidget::makeRatingSelection()
{
    KRatingWidget* ratingWidget = new KRatingWidget();
    ratingWidget->setRating( (int)m_filter.numValue );
    connect( ratingWidget, SIGNAL(ratingChanged(int)),
             this, SLOT( numValueChanged(int) ) );

    m_valueSelection1 = ratingWidget;

    if( m_filter.condition != Between )
        return;

    // second KRatingWidget for the between selection
    KRatingWidget* ratingWidget2 = new KRatingWidget();
    ratingWidget2->setRating( (int)m_filter.numValue2 );
    connect( ratingWidget2, SIGNAL(ratingChanged(int)),
             this, SLOT(numValue2Changed(int)) );

    m_valueSelection2 = ratingWidget2;
}


void
MetaQueryWidget::makeLengthSelection()
{
    QTimeEdit* timeSpin = new QTimeEdit();
    timeSpin->setDisplayFormat( "m:ss" );
    timeSpin->setMinimumTime( QTime( 0, 0, 0 ) );
    timeSpin->setMaximumTime( QTime( 0, 60, 59) );
    timeSpin->setTime( QTime().addSecs( m_filter.numValue ) );

    connect( timeSpin, SIGNAL(timeChanged(const QTime&)),
            SLOT(numValueChanged(const QTime&)) );

    m_valueSelection1 = timeSpin;

    if( m_filter.condition != Between )
        return;

    QTimeEdit* timeSpin2 = new QTimeEdit();
    timeSpin2->setDisplayFormat( "m:ss" );
    timeSpin2->setMinimumTime( QTime( 0, 0, 0 ) );
    timeSpin2->setMaximumTime( QTime( 0, 60, 59) );
    timeSpin2->setTime( QTime().addSecs( m_filter.numValue2 ) );

    connect( timeSpin2, SIGNAL(timeChanged(const QTime&)),
            SLOT(numValueChanged(const QTime&)) );

    m_valueSelection2 = timeSpin2;
}

void
MetaQueryWidget::makeGenericNumberSelection( int min, int max, int def, const QString& unit )
{
    KIntSpinBox* spin = new KIntSpinBox();
    spin->setMinimum( min );
    spin->setMaximum( max );
    if( !unit.isEmpty() )
        spin->setSuffix( " " + unit );
    if( m_filter.condition == Contains ||
        m_filter.numValue < min || m_filter.numValue > max )
    {
        spin->setValue( def );
        numValueChanged( def );
    }
    else
        spin->setValue( m_filter.numValue );

    connect( spin, SIGNAL(valueChanged(int)),
            this, SLOT(numValueChanged(int)) );

    m_valueSelection1 = spin;

    if( m_filter.condition != Between )
        return;

    // second spin box for the between selection
    KIntSpinBox* spin2 = new KIntSpinBox();
    spin2->setMinimum( min );
    spin2->setMaximum( max );
    if( !unit.isEmpty() )
        spin2->setSuffix( " " + unit );
    if( m_filter.condition == Contains ||
        m_filter.numValue2 < min || m_filter.numValue2 > max )
    {
        spin2->setValue( def );
        numValue2Changed( def );
    }
    else
        spin2->setValue( m_filter.numValue2 );

    connect( spin2, SIGNAL(valueChanged(int)),
            this, SLOT(numValue2Changed(int)) );

    m_valueSelection2 = spin2;
}


void
MetaQueryWidget::makeDateTimeSelection()
{
    if( m_filter.condition == OlderThan || m_filter.condition == NewerThan )
    {
        TimeDistanceWidget* distanceSelection = new TimeDistanceWidget();
        distanceSelection->setTimeDistance( m_filter.numValue );

        distanceSelection->connectChanged( this, SLOT(numValueTimeDistanceChanged()));

        m_valueSelection1 = distanceSelection;
    }
    else
    {
        KDateCombo* dateSelection = new KDateCombo();
        QDateTime dt;
//         if( m_filter.condition == Contains || m_filter.condition == Equals )
//             dt = QDateTime::currentDateTime();
//         else
//             dt.setTime_t( m_filter.numValue );
        dt.setTime_t( m_filter.numValue );
        dateSelection->setDate( dt.date() );

        connect( dateSelection, SIGNAL(currentIndexChanged(int)),
                SLOT( numValueDateChanged() ) );

        m_valueSelection1 = dateSelection;

        if( m_filter.condition != Between )
            return;

        // second KDateCombo for the between selection
        KDateCombo* dateSelection2 = new KDateCombo();
        dt.setTime_t( m_filter.numValue2 );
        dateSelection2->setDate( dt.date() );

        connect( dateSelection2, SIGNAL(currentIndexChanged(int)),
                SLOT( numValue2DateChanged() ) );

        m_valueSelection2 = dateSelection2;
    }
}


bool
MetaQueryWidget::isNumeric( qint64 field )
{
    switch( field )
    {
    case Meta::valYear:
    case Meta::valTrackNr:
    case Meta::valDiscNr:
    case Meta::valBpm:
    case Meta::valLength:
    case Meta::valBitrate:
    case Meta::valSamplerate:
    case Meta::valFilesize:
    case Meta::valFormat:
    case Meta::valCreateDate:
    case Meta::valScore:
    case Meta::valRating:
    case Meta::valFirstPlayed:
    case Meta::valLastPlayed:
    case Meta::valPlaycount:
    case Meta::valModified:
        return true;
    default:
        return false;
    }
}

bool
MetaQueryWidget::isDate( qint64 field )
{
    switch( field )
    {
    case Meta::valCreateDate:
    case Meta::valFirstPlayed:
    case Meta::valLastPlayed:
    case Meta::valModified:
        return true;
    default:
        return false;
    }
}

QString
MetaQueryWidget::conditionToString( FilterCondition condition, qint64 field )
{
    if( isDate(field) )
    {
        switch( condition )
        {
        case LessThan:
            return i18nc( "The date lies before the given fixed date", "before" );
        case Equals:
            return i18nc( "The date is the same as the given fixed date", "on" );
        case GreaterThan:
            return i18nc( "The date is after the given fixed date", "after" );
        case Between:
            return i18nc( "The date is between the given fixed dates", "between" );
        case OlderThan:
            return i18nc( "The date lies before the given time interval", "older than" );
        case NewerThan:
            return i18nc( "The date lies after the given time interval", "newer than" );
        default:
            ; // fall through
        }
    }
    else if( isNumeric(field) )
    {
        switch( condition )
        {
        case LessThan:
            return i18n("less than");
        case Equals:
            return i18nc("a numerical tag (like year or track number) equals a value","equals");
        case GreaterThan:
            return i18n("greater than");
        case Between:
            return i18nc( "a numerical tag (like year or track number) is between two values", "between" );
        default:
            ; // fall through
        }
    }
    else
    {
        switch( condition )
        {
        case Equals:
            return i18nc("an alphabetical tag (like title or artist name) equals some string","equals");
        case Contains:
            return i18nc("an alphabetical tag (like title or artist name) contains some string", "contains");
        default:
            ; // fall through
        }
    }
    return QString( i18n("unknown comparison") );
}

QString
MetaQueryWidget::Filter::fieldToString() const
{
    return Meta::shortI18nForField( field );
}

QString MetaQueryWidget::Filter::toString( bool invert ) const
{
    // this member is called when there is a keyword that needs numeric attributes
    QString strValue1 = value;
    QString strValue2 = value;

    if( field == Meta::valFormat )
    {
        strValue1 = Amarok::FileTypeSupport::toString( Amarok::FileType( numValue ));
    }
    else if( field == Meta::valRating )
    {
        strValue1 = QString::number( (float)numValue / 2 );
        strValue2 = QString::number( (float)numValue2 / 2 );
    }
    else if( MetaQueryWidget::isDate(field) )
    {
        if( condition == OlderThan || condition == NewerThan )
        {
            strValue1 = QString::number( numValue  );
            strValue2 = QString::number( numValue2 );
        }
        else
        {
            KLocalizedDate localizedDate1( QDateTime::fromTime_t(numValue).date() );
            strValue1 = localizedDate1.formatDate( KLocale::ShortDate );
            KLocalizedDate localizedDate2( QDateTime::fromTime_t(numValue2).date() );
            strValue2 = localizedDate2.formatDate( KLocale::ShortDate );
        }
    }
    else if( MetaQueryWidget::isNumeric(field) )
    {
        strValue1 = QString::number( numValue );
        strValue2 = QString::number( numValue2 );
    }

    QString result;
    if( field )
        result = fieldToString() + ':';

    switch( condition )
    {
    case Equals:
        {
            if( MetaQueryWidget::isNumeric(field) )
                result += strValue1;
            else
                result += '=' + QString( "\"%1\"" ).arg( value );
            if( invert )
                result.prepend( QChar('-') );
            break;
        }

    case GreaterThan:
        {
            result += '>' + strValue1;
            if( invert )
                result.prepend( QChar('-') );
            break;
        }

    case LessThan:
        {
            result +='<' + strValue1;
            if( invert )
                result.prepend( QChar('-') );
            break;
        }

    case Between:
        {
            if( invert )
                result = QString( "%1<%2 OR %1>%3" ).arg( result, strValue1, strValue2 );
            else
                result = QString( "%1>%2 AND %1<%3" ).arg( result, strValue1, strValue2 );
            break;
        }

    case OlderThan:
    case NewerThan:
        {
            // a human readable time..
            QChar strUnit = 's';
            qint64 value = numValue;
            if( !(value % 60) ) {
                strUnit = 'M';
                value /= 60;

                if( !(value % 60) ) {
                    strUnit = 'h';
                    value /= 60;

                    if( !(value % 24) ) {
                        strUnit = 'd';
                        value /= 24;

                        if( !(value % 365) ) {
                            strUnit = 'y';
                            value /= 365;
                        } else if( !(value % 30) ) {
                            strUnit = 'm';
                            value /= 30;
                        } else if( !(value % 7) ) {
                            strUnit = 'w';
                            value /= 7;
                        }
                    }
                }
            }

            if( condition == OlderThan )
                result += '>' + QString::number(value) + strUnit;
            else
                result += '<' + QString::number(value) + strUnit;
            if( invert )
                result.prepend( QChar('-') );
            break;
        }

    case Contains:
        {
            result += QString( "\"%1\"" ).arg( value );
            if( invert )
                result.prepend( QChar('-') );
            break;
        }
    }

    return result;
}

bool
MetaQueryWidget::isFieldSelectorHidden() const
{
    return m_fieldSelection->isHidden();
}

void
MetaQueryWidget::setFieldSelectorHidden( const bool hidden )
{
    m_fieldSelection->setVisible( !hidden );
}

void
MetaQueryWidget::setField( const qint64 field )
{
    m_filter = Filter();
    int index = m_fieldSelection->findData( field );
    m_fieldSelection->setCurrentIndex( index == -1 ? 0 : index );
}

#include "MetaQueryWidget.moc"

