/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QModelIndex>
#include <QAbstractItemModel>

#include <KLocalizedString>
#include <KDebug>
#include <KPushButton>
#include <KSeparator>
#include <KUrlRequester>

#include "startpage.h"
#include "mainwindow.h"
#include "newproject.h"

StartPage::StartPage(MainWindow *parent) // TODO set a palette so it will look identical with any color scheme.
    : QWidget(parent),
    m_parent(parent)
{
    createWidgets();
    refreshRecentProjectsList();
}

void StartPage::createWidgets() // Make this a QGV? Use Plasma::Theme?
{
    m_layout = new QVBoxLayout(this);
    
    QHBoxLayout *continueWorkingLayout = new QHBoxLayout;
    m_continueWorkingLabel = new QLabel(i18n("Continue working on..."), this);
    m_recentProjects = new QListWidget(this);
    continueWorkingLayout->addWidget(m_continueWorkingLabel);
    continueWorkingLayout->addWidget(m_recentProjects);
    connect(m_recentProjects, SIGNAL(clicked(const QModelIndex)),
            this, SLOT(emitProjectSelected(const QModelIndex)));
    
    QHBoxLayout *newProjectLayout = new QHBoxLayout;
    m_createNewLabel = new QLabel(i18n("Create new..."), this);
    m_contentTypes = new QComboBox(this);
    m_contentTypes->addItem(i18n("select project type..."));
    m_contentTypes->addItem(i18n("Plasmoid"));
    m_contentTypes->addItem(i18n("Data engine"));
    m_contentTypes->addItem(i18n("Theme"));
    m_newProjectWizardButton = new KPushButton(i18n("Create!"), this);
    m_newProjectWizardButton->setEnabled(false);
    
    newProjectLayout->addWidget(m_createNewLabel);
    newProjectLayout->addWidget(m_contentTypes);
    newProjectLayout->addWidget(m_newProjectWizardButton);
    
    connect(m_contentTypes, SIGNAL(currentIndexChanged(int)), this, SLOT(conditionallyEnableNewProjectButton()));
    connect(m_newProjectWizardButton, SIGNAL(clicked()), this, SLOT(launchNewProjectWizard()));
    
    QHBoxLayout *openExistingLayout = new QHBoxLayout;
    openExistingLayout->addWidget(new QLabel(i18n("Open existing project..."), this));
    openExistingLayout->addWidget(new KUrlRequester(this));
    
    m_layout->addWidget(new KSeparator(this));
    m_layout->addItem(continueWorkingLayout);
    m_layout->addWidget(new KSeparator(this));
    m_layout->addItem(newProjectLayout);
    m_layout->addWidget(new KSeparator(this));
    m_layout->addItem(openExistingLayout);
    
    setLayout(m_layout);
}

void StartPage::conditionallyEnableNewProjectButton()
{
    m_newProjectWizardButton->setEnabled(m_contentTypes->currentIndex() != 0);
}

void StartPage::launchNewProjectWizard()
{
    NewProjectWizard w;
    switch (m_contentTypes->currentIndex()) {
        case 1:
            w.setProjectType(Plasmoid);
        case 2:
            w.setProjectType(DataEngine);
        case 3:
            w.setProjectType(Theme);
    }
    
    w.exec();
    emit(projectSelected(w.projectFile()));
}

void StartPage::refreshRecentProjectsList()
{
    m_recentProjects->clear();
    QList<KUrl> recentFiles = m_parent->recentProjects();
    
    for (int i = 0; i < recentFiles.size(); i++) {
        QString project = recentFiles.at(i).directory();
        kDebug() << project;
        QListWidgetItem *item = new QListWidgetItem(project);
        item->setData(FullPathRole, project);
        m_recentProjects->addItem(item);
    }
}

void StartPage::emitProjectSelected(const QModelIndex &index)
{
    QAbstractItemModel *m = m_recentProjects->model();
    KUrl url = m->data(index, FullPathRole).value<KUrl>();
    kDebug() << "Loading project file:" << m->data(index, FullPathRole);
    emit projectSelected(url);
}
