/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _SIDEBAR_H_
#define _SIDEBAR_H_

#include <qwidget.h>

class QIcon;
class QListWidgetItem;

class Sidebar : public QWidget
{
    Q_OBJECT
    public:
        Sidebar( QWidget *parent = 0 );
        ~Sidebar();

        int addItem( const QIcon &icon, const QString &text );

//         void setMainWidget( QWidget *widget );
//         void setBottomWidget( QWidget *widget );

        void setItemEnabled( int index, bool enabled );
        bool isItemEnabled( int index ) const;

        void setCurrentIndex( int index );
        int currentIndex() const;

        void setSidebarVisibility( bool visible );
        bool isSidebarVisible() const;

    signals:
        void currentIndexChanged(int);
        
//     private slots:
//         void itemClicked( QListWidgetItem *item );
//         void splitterMoved( int pos, int index );
//         void showTextToggled( bool );
//         void iconSizeChanged( QAction *action );

    private:
        void saveSplitterSize() const;

        // private storage
        class Private;
        Private *d;
};

#endif
