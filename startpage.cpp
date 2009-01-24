/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QLabel>
#include <QComboBox>
#include <QListView>
#include <QHBoxLayout>

#include "startpage.h"

StartPage::StartPage()
{
    createWidgets();
}

void StartPage::createWidgets()
{
    m_layout = new QHBoxLayout(this);
    
}



