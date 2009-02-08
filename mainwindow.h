/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kurl.h>

#include <kmainwindow.h>

class QModelIndex;

namespace Ui {
    class MainWindowClass;
}

class StartPage;

class MainWindow : public KMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

        KUrl::List recentProjects();
        
    public Q_SLOTS:
        void quit();
        
        void changeTab(int tab);
        void loadProject(const KUrl &url);

    private:
        void createMenus();
        void createDockWidgets();
        
        StartPage *m_startPage;
        int oldTab;
};

#endif // MAINWINDOW_H
