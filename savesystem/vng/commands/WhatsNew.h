/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef WHATSNEW_H
#define WHATSNEW_H

#include "../AbstractCommand.h"
#include "../ExcludeMatcher.h"

class WhatsNew : public AbstractCommand
{
public:
    WhatsNew();

protected:
    virtual ReturnCodes run();
    virtual QString argumentDescription() const;
    virtual QString commandDescription() const;

private:
    ExcludeMatcher m_excludeMatcher;
};

#endif
