/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "sidebar.h"

#include <qabstractitemdelegate.h>
#include <qaction.h>
#include <qapplication.h>
#include <qevent.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qsplitter.h>
#include <qstackedwidget.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>

// #include "settings.h"

static const int SidebarItemType = QListWidgetItem::UserType + 1;

/* List item representing a sidebar entry. */
class SidebarItem : public QListWidgetItem
{
    public:
        SidebarItem( QWidget* w, const QIcon &icon, const QString &text )
            : QListWidgetItem( 0, SidebarItemType ),
              m_widget( w )
        {
            setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            setIcon( icon );
            setText( text );
            setToolTip( text );
        }

        QWidget* widget() const
        {
            return m_widget;
        }

    private:
        QWidget *m_widget;
};


/* A simple delegate to paint the icon of each item */
#define ITEM_MARGIN_LEFT 5
#define ITEM_MARGIN_TOP 5
#define ITEM_MARGIN_RIGHT 5
#define ITEM_MARGIN_BOTTOM 5
#define ITEM_PADDING 5

class SidebarDelegate : public QAbstractItemDelegate
{
    public:
        SidebarDelegate( QObject *parent = 0 );
        ~SidebarDelegate();

        void setShowText( bool show );
        bool isTextShown() const;

        // from QAbstractItemDelegate
        void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
        QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    private:
        bool m_showText;
};

SidebarDelegate::SidebarDelegate( QObject *parent )
    : QAbstractItemDelegate( parent ), m_showText( true )
{
}

SidebarDelegate::~SidebarDelegate()
{
}

void SidebarDelegate::setShowText( bool show )
{
    m_showText = show;
}

bool SidebarDelegate::isTextShown() const
{
    return m_showText;
}

void SidebarDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QBrush backBrush;
    QColor foreColor;
    bool disabled = false;
    bool hover = false;
    if ( !( option.state & QStyle::State_Enabled ) )
    {
        backBrush = option.palette.brush( QPalette::Disabled, QPalette::Base );
        foreColor = option.palette.color( QPalette::Disabled, QPalette::Text );
        disabled = true;
    }
    else if ( option.state & ( QStyle::State_HasFocus | QStyle::State_Selected ) )
    {
        backBrush = option.palette.brush( QPalette::Highlight );
        foreColor = option.palette.color( QPalette::HighlightedText );
    }
    else if ( option.state & QStyle::State_MouseOver )
    {
        backBrush = option.palette.color( QPalette::Highlight ).light( 115 );
        foreColor = option.palette.color( QPalette::HighlightedText );
        hover = true;
    }
    else /*if ( option.state & QStyle::State_Enabled )*/
    {
        backBrush = option.palette.brush( QPalette::Base );
        foreColor = option.palette.color( QPalette::Text );
    }
    QStyle *style = QApplication::style();
    QStyleOptionViewItemV4 opt( option );
    // KStyle provides an "hover highlight" effect for free;
    // but we want that for non-KStyle-based styles too
    if ( !style->inherits( "KStyle" ) && hover )
    {
        Qt::BrushStyle bs = opt.backgroundBrush.style();
        if ( bs > Qt::NoBrush && bs < Qt::TexturePattern )
            opt.backgroundBrush = opt.backgroundBrush.color().light( 115 );
        else
            opt.backgroundBrush = backBrush;
    }
    painter->save();
    style->drawPrimitive( QStyle::PE_PanelItemViewItem, &opt, painter, 0 );
    painter->restore();
    QIcon icon = index.data( Qt::DecorationRole ).value< QIcon >();
    if ( !icon.isNull() )
    {
        QPoint iconpos(
            ( option.rect.width() - option.decorationSize.width() ) / 2,
            ITEM_MARGIN_TOP
        );
        iconpos += option.rect.topLeft();
        QIcon::Mode iconmode = disabled ? QIcon::Disabled : QIcon::Normal;
        painter->drawPixmap( iconpos, icon.pixmap( option.decorationSize, iconmode ) );
    }

    if ( m_showText )
    {
        QString text = index.data( Qt::DisplayRole ).toString();
        QRect fontBoundaries = QFontMetrics( option.font ).boundingRect( text );
        QPoint textPos(
            ITEM_MARGIN_LEFT + ( option.rect.width() - ITEM_MARGIN_LEFT - ITEM_MARGIN_RIGHT - fontBoundaries.width() ) / 2,
            ITEM_MARGIN_TOP + option.decorationSize.height() + ITEM_PADDING
        );
        fontBoundaries.translate( -fontBoundaries.topLeft() );
        fontBoundaries.translate( textPos );
        fontBoundaries.translate( option.rect.topLeft() );
        painter->setPen( foreColor );
        painter->drawText( fontBoundaries, Qt::AlignCenter, text );
    }
}

QSize SidebarDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QSize baseSize( option.decorationSize.width(), option.decorationSize.height() );
    if ( m_showText )
    {
        QRect fontBoundaries = QFontMetrics( option.font ).boundingRect( index.data( Qt::DisplayRole ).toString() );
        baseSize.setWidth( qMax( fontBoundaries.width(), baseSize.width() ) );
        baseSize.setHeight( baseSize.height() + fontBoundaries.height() + ITEM_PADDING );
    }
    return baseSize + QSize( ITEM_MARGIN_LEFT + ITEM_MARGIN_RIGHT, ITEM_MARGIN_TOP + ITEM_MARGIN_BOTTOM );
}


/* A custom list widget that ignores the events for disabled items */
class SidebarListWidget : public QListWidget
{
    public:
        SidebarListWidget( QWidget *parent = 0 );
        ~SidebarListWidget();

    protected:
        // from QListWidget
        void mouseDoubleClickEvent( QMouseEvent *event );
        void mouseMoveEvent( QMouseEvent *event );
        void mousePressEvent( QMouseEvent *event );
        void mouseReleaseEvent( QMouseEvent *event );

        QModelIndex moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
};

SidebarListWidget::SidebarListWidget( QWidget *parent )
    : QListWidget( parent )
{
}

SidebarListWidget::~SidebarListWidget()
{
}

void SidebarListWidget::mouseDoubleClickEvent( QMouseEvent *event )
{
    QModelIndex index = indexAt( event->pos() );
    if ( index.isValid() && !( index.flags() & Qt::ItemIsSelectable ) )
        return;

    QListWidget::mouseDoubleClickEvent( event );
}

void SidebarListWidget::mouseMoveEvent( QMouseEvent *event )
{
    QModelIndex index = indexAt( event->pos() );
    if ( index.isValid() && !( index.flags() & Qt::ItemIsSelectable ) )
        return;

    QListWidget::mouseMoveEvent( event );
}

void SidebarListWidget::mousePressEvent( QMouseEvent *event )
{
    QModelIndex index = indexAt( event->pos() );
    if ( index.isValid() && !( index.flags() & Qt::ItemIsSelectable ) )
        return;

    QListWidget::mousePressEvent( event );
}

void SidebarListWidget::mouseReleaseEvent( QMouseEvent *event )
{
    QModelIndex index = indexAt( event->pos() );
    if ( index.isValid() && !( index.flags() & Qt::ItemIsSelectable ) )
        return;

    QListWidget::mouseReleaseEvent( event );
}

QModelIndex SidebarListWidget::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    Q_UNUSED( modifiers )
    QModelIndex oldindex = currentIndex();
    QModelIndex newindex = oldindex;
    switch ( cursorAction )
    {
        case MoveUp:
        case MovePrevious:
        {
            int row = oldindex.row() - 1;
            while ( row > -1 && !( model()->index( row, 0 ).flags() & Qt::ItemIsSelectable ) ) --row;
            if ( row > -1 )
                newindex = model()->index( row, 0 );
            break;
        }
        case MoveDown:
        case MoveNext:
        {
            int row = oldindex.row() + 1;
            int max = model()->rowCount();
            while ( row < max && !( model()->index( row, 0 ).flags() & Qt::ItemIsSelectable ) ) ++row;
            if ( row < max )
                newindex = model()->index( row, 0 );
            break;
        }
        case MoveHome:
        case MovePageUp:
        {
            int row = 0;
            while ( row < oldindex.row() && !( model()->index( row, 0 ).flags() & Qt::ItemIsSelectable ) ) ++row;
            if ( row < oldindex.row() )
                newindex = model()->index( row, 0 );
            break;
        }
        case MoveEnd:
        case MovePageDown:
        {
            int row = model()->rowCount() - 1;
            while ( row > oldindex.row() && !( model()->index( row, 0 ).flags() & Qt::ItemIsSelectable ) ) --row;
            if ( row > oldindex.row() )
                newindex = model()->index( row, 0 );
            break;
        }
        // no navigation possible for these
        case MoveLeft:
        case MoveRight:
            break;
    }

    // dirty hack to change item when the key cursor changes item
    if ( oldindex != newindex )
    {
        emit itemClicked( itemFromIndex( newindex ) );
    }
    return newindex;
}


/* Private storage. */
class Sidebar::Private
{
public:
    Private()
        : sideWidget( 0 ), bottomWidget( 0 ), splitterSizesSet( false ),
          itemsHeight( 0 )
    {
    }

    void adjustListSize( bool recalc, bool expand = true );

    SidebarListWidget *list;
    QSplitter *splitter;
    QStackedWidget *stack;
    QWidget *sideContainer;
    QLabel *sideTitle;
    QVBoxLayout *vlay;
    QWidget *sideWidget;
    QWidget *bottomWidget;
    QList< SidebarItem* > pages;
    bool splitterSizesSet;
    int itemsHeight;
    SidebarDelegate *sideDelegate;
};

void Sidebar::Private::adjustListSize( bool recalc, bool expand )
{
    QRect bottomElemRect(
        QPoint( 0, 0 ),
        list->sizeHintForIndex( list->model()->index( list->count() - 1, 0 ) )
    );
    if ( recalc )
    {
        int w = 0;
        for ( int i = 0; i < list->count(); ++i )
        {
            QSize s = list->sizeHintForIndex( list->model()->index( i, 0 ) );
            if ( s.width() > w )
                w = s.width();
        }
        bottomElemRect.setWidth( w );
    }
    bottomElemRect.translate( 0, bottomElemRect.height() * ( list->count() - 1 ) );
    itemsHeight = bottomElemRect.height() * list->count();
    list->setMinimumHeight( itemsHeight + list->frameWidth() * 2 );
    int curWidth = list->minimumWidth();
    int newWidth = expand
                   ? qMax( bottomElemRect.width() + list->frameWidth() * 2, curWidth )
                   : qMin( bottomElemRect.width() + list->frameWidth() * 2, curWidth );
    list->setFixedWidth( newWidth );
}


Sidebar::Sidebar( QWidget *parent )
    : QWidget( parent ), d( new Private )
{
    QHBoxLayout *mainlay = new QHBoxLayout( this );
    mainlay->setMargin( 0 );
    mainlay->setSpacing( 0 );

    d->list = new SidebarListWidget( this );
    mainlay->addWidget( d->list );
    d->list->setMouseTracking( true );
    d->list->viewport()->setAttribute( Qt::WA_Hover );
    d->sideDelegate = new SidebarDelegate( d->list );
//     d->sideDelegate->setShowText( Okular::Settings::sidebarShowText() );
    d->list->setItemDelegate( d->sideDelegate );
    d->list->setUniformItemSizes( true );
    d->list->setSelectionMode( QAbstractItemView::SingleSelection );
    int iconsize = 32;// Okular::Settings::sidebarIconSize();
    d->list->setIconSize( QSize( iconsize, iconsize ) );
    d->list->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    d->list->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    d->list->setContextMenuPolicy( Qt::CustomContextMenu );
    d->list->viewport()->setAutoFillBackground( false );

    d->splitter = new QSplitter( this );
    mainlay->addWidget( d->splitter );
    d->splitter->setOpaqueResize( true );
    d->splitter->setChildrenCollapsible( false );

    d->sideContainer = new QWidget( d->splitter );
    d->sideContainer->setMinimumWidth( 90 );
    d->sideContainer->setMaximumWidth( 600 );
    d->vlay = new QVBoxLayout( d->sideContainer );
    d->vlay->setMargin( 0 );

    d->sideTitle = new QLabel( d->sideContainer );
    d->vlay->addWidget( d->sideTitle );
    QFont tf = d->sideTitle->font();
    tf.setBold( true );
    d->sideTitle->setFont( tf );
    d->sideTitle->setMargin( 3 );
    d->sideTitle->setIndent( 3 );

    d->stack = new QStackedWidget( d->sideContainer );
    d->vlay->addWidget( d->stack );
    d->sideContainer->hide();

    connect( d->list, SIGNAL( itemClicked( QListWidgetItem* ) ), this, SLOT( itemClicked( QListWidgetItem* ) ) );
//     connect( d->list, SIGNAL( customContextMenuRequested( const QPoint & ) ),
//              this, SLOT( listContextMenu( const QPoint & ) ) );
    connect( d->splitter, SIGNAL( splitterMoved( int, int ) ), this, SLOT( splitterMoved( int, int ) ) );
}

Sidebar::~Sidebar()
{
    delete d;
}

int Sidebar::addItem( QWidget *widget, const QIcon &icon, const QString &text )
{
    if ( !widget )
        return -1;

    SidebarItem *newitem = new SidebarItem( widget, icon, text );
    d->list->addItem( newitem );
    d->pages.append( newitem );
    widget->setParent( d->stack );
    d->stack->addWidget( widget );
    // updating the minimum height of the icon view, so all are visible with no scrolling
    d->adjustListSize( false, true );
    return d->pages.count() - 1;
}

void Sidebar::setItemEnabled( int index, bool enabled )
{
    if ( index < 0 || index >= d->pages.count() )
        return;

    Qt::ItemFlags f = d->pages.at( index )->flags();
    if ( enabled )
    {
        f |= Qt::ItemIsEnabled;
        f |= Qt::ItemIsSelectable;
    }
    else
    {
        f &= ~Qt::ItemIsEnabled;
        f &= ~Qt::ItemIsSelectable;
    }
    d->pages.at( index )->setFlags( f );

    if ( !enabled && index == currentIndex() )
        // find an enabled item, and select that one
        for ( int i = 0; i < d->pages.count(); ++i )
            if ( d->pages.at(i)->flags() & Qt::ItemIsEnabled )
            {
                setCurrentIndex( i );
                break;
            }
}

bool Sidebar::isItemEnabled( int index ) const
{
    if ( index < 0 || index >= d->pages.count() )
        return false;

    Qt::ItemFlags f = d->pages.at( index )->flags();
    return ( f & Qt::ItemIsEnabled ) == Qt::ItemIsEnabled;
}

void Sidebar::setCurrentIndex( int index )
{
    if ( index < 0 || index >= d->pages.count() || !isItemEnabled( index ) )
        return;

    itemClicked( d->pages.at( index ) );
    QModelIndex modelindex = d->list->model()->index( index, 0 );
    d->list->setCurrentIndex( modelindex );
    d->list->selectionModel()->select( modelindex, QItemSelectionModel::ClearAndSelect );
}

int Sidebar::currentIndex() const
{
    return d->list->currentRow();
}

void Sidebar::setSidebarVisibility( bool visible )
{
    if ( visible != d->list->isHidden() )
        return;

    static bool sideWasVisible = !d->sideContainer->isHidden();

    d->list->setHidden( !visible );
    if ( visible )
    {
        d->sideContainer->setHidden( !sideWasVisible );
        sideWasVisible = true;
    }
    else
    {
        sideWasVisible = !d->sideContainer->isHidden();
        d->sideContainer->setHidden( true );
    }
}

bool Sidebar::isSidebarVisible() const
{
    return !d->sideContainer->isHidden();
}

void Sidebar::itemClicked( QListWidgetItem *item )
{
    if ( !item )
        return;

    SidebarItem* sbItem = dynamic_cast< SidebarItem* >( item );
    if ( !sbItem )
        return;

    if ( sbItem->widget() == d->stack->currentWidget() )
    {
        if ( d->sideContainer->isVisible() )
        {
            d->list->selectionModel()->clear();
            d->sideContainer->hide();
        }
        else
        {
            d->sideContainer->show();
        }
    }
    else
    {
        if ( d->sideContainer->isHidden() )
            d->sideContainer->show();
        d->stack->setCurrentWidget( sbItem->widget() );
        d->sideTitle->setText( sbItem->toolTip() );
    }
}


#include "sidebar.moc"
