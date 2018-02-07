/*
 * Copyright (C) 2013-2017 Daniel Nicoletti <dantti12@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "authenticationuser.h"

#include <QtCore/QDataStream>

using namespace Cutelyst;

AuthenticationUser::AuthenticationUser()
{

}

AuthenticationUser::AuthenticationUser(const QString &id)
{
    setId(id);
}

AuthenticationUser::~AuthenticationUser()
{
}

QString AuthenticationUser::id() const
{
    return value(QStringLiteral("id")).toString();
}

void AuthenticationUser::setId(const QString &id)
{
    insert(QStringLiteral("id"), id);
}

bool AuthenticationUser::isNull() const
{
    return id().isEmpty();
}

QString AuthenticationUser::authRealm()
{
    return value(QStringLiteral("authRealm")).toString();
}

void AuthenticationUser::setAuthRealm(const QString &authRealm)
{
    insert(QStringLiteral("authRealm"), authRealm);
}

QDataStream &operator<<(QDataStream &out, const AuthenticationUser &user)
{
    out << user.id() << static_cast<QVariantMap>(user);
    return out;
}

QDataStream &operator>>(QDataStream &in, AuthenticationUser &user)
{
    QString id;
    QVariantMap map;
    in >> id >> map;
    user.setId(id);
    user.swap(map);
    return in;
}
