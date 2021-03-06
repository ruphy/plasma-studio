/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include <KUrl>

class QLabel;
class QComboBox;
class QListWidget;
class QVBoxLayout;
class QModelIndex;
class KPushButton;
class KLineEdit;
class MainWindow;

namespace Ui {
    class StartPage;
}

class StartPage : public QWidget
{
    Q_OBJECT
    
    enum Roles { 
        FullPathRole = Qt::UserRole + 1
    };
    
    public:
        StartPage(MainWindow *parent);
        ~StartPage();

        void resetStatus();

        enum ProjectTypes {
            Theme           = 1,
            Plasmoid        = 2,
            DataEngine      = 4
        };

        Q_DECLARE_FLAGS(ProjectType, ProjectTypes);

    signals:
        void projectSelected(const QString &name, const QString &type);

    private Q_SLOTS:
        void emitProjectSelected(const QModelIndex &);
        void changeStackedWidgetPage();
        void createNewProject();

    private:
        void setupWidgets();
        void refreshRecentProjectsList();

        Ui::StartPage *ui;
        QLabel *m_createNewLabel, *m_openExistingLabel, *m_continueWorkingLabel;
        QComboBox *m_contentTypes;
        QListWidget *m_recentProjects;
//         KPushButton *m_createNewProjectButton;
        KLineEdit *m_newProjectName;
        QVBoxLayout *m_layout;
        MainWindow *m_parent;
};

#endif // STARTPAGE_H
