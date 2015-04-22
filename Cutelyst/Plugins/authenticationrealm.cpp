/*
 * Copyright (C) 2013-2015 Daniel Nicoletti <dantti12@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "authenticationrealm.h"

#include "authenticationstore.h"
#include "session.h"
#include "context.h"
#include "common.h"

using namespace Cutelyst;

Q_LOGGING_CATEGORY(C_AUTH_REALM, "cutelyst.plugin.authentication.realm")

AuthenticationRealm::AuthenticationRealm(AuthenticationStore *store, AuthenticationCredential *credential, QObject *parent)
    : QObject(parent)
    , m_store(store)
    , m_credential(credential)
{

}

AuthenticationRealm::~AuthenticationRealm()
{

}

AuthenticationStore *AuthenticationRealm::store() const
{
    return m_store;
}

AuthenticationCredential *AuthenticationRealm::credential() const
{
    return m_credential;
}

AuthenticationUser AuthenticationRealm::findUser(Context *c, const CStringHash &userinfo)
{
    AuthenticationUser ret = m_store->findUser(c, userinfo);

    if (ret.isNull()) {
        if (m_store->canAutoCreateUser()) {
            ret = m_store->autoCreateUser(c, userinfo);
        }
    } else if (m_store->canAutoUpdateUser()) {
        ret = m_store->autoUpdateUser(c, userinfo);
    }

    return ret;
}

AuthenticationUser AuthenticationRealm::authenticate(Context *c, const CStringHash &authinfo)
{
    return m_credential->authenticate(c, this, authinfo);
}

void AuthenticationRealm::removePersistedUser(Context *c)
{
    Session *session = c->plugin<Session*>();
    if (session && session->isValid(c)) {
        session->deleteValue(c, QStringLiteral("Authentication::user"));
        session->deleteValue(c, QStringLiteral("Authentication::userRealm"));
    }
}

AuthenticationUser AuthenticationRealm::persistUser(Context *c, const AuthenticationUser &user)
{
    Session *session = c->plugin<Session*>();
    if (session && session->isValid(c)) {
        session->setValue(c,
                          QStringLiteral("Authentication::user"),
                          m_store->forSession(c, user));
    }

    return user;
}

AuthenticationUser AuthenticationRealm::restoreUser(Context *c, const QVariant &frozenUser)
{
    QVariant _frozenUser = frozenUser;
    if (_frozenUser.isNull()) {
        _frozenUser = userIsRestorable(c);
    }

    if (_frozenUser.isNull()) {
        return AuthenticationUser();
    }

    AuthenticationUser user = m_store->fromSession(c, _frozenUser);

    if (!user.isNull()) {
        c->plugin<Authentication*>()->setUser(c, user);

        // Sets the realm the user originated in
        user.setAuthRealm(this);
    } else {
        qCWarning(C_AUTH_REALM) << "Store claimed to have a restorable user, but restoration failed. Did you change the user's id_field?";
    }

    return user;
}

QVariant AuthenticationRealm::userIsRestorable(Context *c)
{
    Session *session = c->plugin<Session*>();
    if (session && session->isValid(c)) {
        return session->value(c, QStringLiteral("Authentication::user"));
    }
    return QVariant();
}