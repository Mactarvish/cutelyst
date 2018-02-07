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
#ifndef AUTHENTICATIONUSER_H
#define AUTHENTICATIONUSER_H

#include <QDataStream>
#include <QVariantMap>

#include <Cutelyst/cutelyst_global.h>
#include <Cutelyst/paramsmultimap.h>
#include <Cutelyst/plugin.h>

namespace Cutelyst {

class AuthenticationRealm;
class CUTELYST_PLUGIN_AUTHENTICATION_EXPORT AuthenticationUser : public QVariantMap
{
    Q_GADGET
public:
    /*!
     * Constructs a new AuthenticationUser object
     */
    AuthenticationUser();

    /*!
     * Constructs a new AuthenticationUser object with the given id
     */
    AuthenticationUser(const QString &id);
    virtual ~AuthenticationUser();

    /**
     * A unique ID by which a AuthenticationUser can be retrieved from the store.
     */
    QString id() const;

    /*!
     * Sets the unique user id restored from the store
     */
    void setId(const QString &id);

    /*!
     * Returns true if the object is null
     */
    bool isNull() const;

    /*!
     * Returns the authentication realm from which this user was retrieved
     */
    QString authRealm();

    /*!
     * Sets the authentication realm from which this user was retrieved
     */
    void setAuthRealm(const QString &authRealm);
};

}

Q_DECLARE_METATYPE(Cutelyst::AuthenticationUser)
QDataStream &operator<<(QDataStream &out, const Cutelyst::AuthenticationUser &myObj);
QDataStream &operator>>(QDataStream &in, Cutelyst::AuthenticationUser &myObj);

#endif // AUTHENTICATIONUSER_H
