#ifndef DISPATCHERTEST_H
#define DISPATCHERTEST_H

#include <QTest>
#include <QObject>
#include <QUrlQuery>

#include "headers.h"
#include "coverageobject.h"

#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/authenticationuser.h>
#include <Cutelyst/Plugins/Authentication/authenticationrealm.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/Plugins/Authentication/credentialhttp.h>
#include <Cutelyst/Plugins/Authentication/minimal.h>
#include <Cutelyst/Plugins/Session/Session>

#include <Cutelyst/application.h>
#include <Cutelyst/controller.h>
#include <Cutelyst/headers.h>

using namespace Cutelyst;

class TestAuthentication : public CoverageObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testController_data();
    void testController() {
        doTest();
    }

    void cleanupTestCase();

private:
    TestEngine *m_engine;

    TestEngine* getEngine();

    void doTest();

};

class AuthenticationTest : public Controller
{
    Q_OBJECT
public:
    AuthenticationTest(QObject *parent) : Controller(parent) {}

    C_ATTR(authenticate, :Local :AutoArgs)
    void authenticate(Context *c) {
        if (Authentication::authenticate(c)) {
            c->response()->setBody(QStringLiteral("ok"));
        } else {
            c->response()->setBody(QStringLiteral("fail"));
        }
    }

    C_ATTR(authenticate_user, :Local :AutoArgs)
    void authenticate_user(Context *c) {
        if (Authentication::authenticate(c, c->request()->queryParameters())) {
            c->response()->setBody(QStringLiteral("ok"));
        } else {
            c->response()->setBody(QStringLiteral("fail"));
        }
    }

    C_ATTR(authenticate_user_realm, :Local :AutoArgs)
    void authenticate_user_realm(Context *c, const QString &realm) {
        qDebug() << "HEADERS" << c->req()->headers().authorizationBasic();
        if (Authentication::authenticate(c, c->request()->queryParameters(), realm)) {
            c->response()->setBody(QStringLiteral("ok"));
        } else {
            c->response()->setBody(QStringLiteral("fail"));
        }
    }
};

void TestAuthentication::initTestCase()
{
    m_engine = getEngine();
    QVERIFY(m_engine);
}

TestEngine* TestAuthentication::getEngine()
{
    auto app = new TestApplication;
    auto engine = new TestEngine(app, QVariantMap());

    auto auth = new Authentication(app);

    auto clearStore = new StoreMinimal;
    AuthenticationUser fooUser(QStringLiteral("foo"));
    fooUser.insert(QStringLiteral("password"), QStringLiteral("123"));
    clearStore->addUser(fooUser);
    AuthenticationUser barUser(QStringLiteral("bar"));
    barUser.insert(QStringLiteral("password"), QStringLiteral("321"));
    clearStore->addUser(barUser);
    auto clearPassword = new CredentialPassword;
    clearPassword->setPasswordField(QStringLiteral("password"));
    clearPassword->setPasswordType(CredentialPassword::Clear);
    auth->addRealm(new AuthenticationRealm(clearStore, clearPassword));


    auto hashedStore = new StoreMinimal;
    fooUser.insert(QStringLiteral("password"), CredentialPassword::createPassword(QByteArrayLiteral("123"), QCryptographicHash::Sha256, 10, 10, 10));
    hashedStore->addUser(fooUser);
    barUser.insert(QStringLiteral("password"), CredentialPassword::createPassword(QByteArrayLiteral("321"), QCryptographicHash::Sha256, 20, 20, 20));
    hashedStore->addUser(barUser);
    auto hashedPassword = new CredentialPassword;
    hashedPassword->setPasswordField(QStringLiteral("password"));
    hashedPassword->setPasswordType(CredentialPassword::Hashed);
    auth->addRealm(new AuthenticationRealm(hashedStore, hashedPassword), QStringLiteral("hashed"));

    auto nonePassword = new CredentialPassword;
    nonePassword->setPasswordField(QStringLiteral("password"));
    nonePassword->setPasswordType(CredentialPassword::None);
    auth->addRealm(new AuthenticationRealm(clearStore, nonePassword), QStringLiteral("none"));

    auto clearHttpCredential = new CredentialHttp;
    clearHttpCredential->setPasswordType(CredentialHttp::Clear);
    auth->addRealm(new AuthenticationRealm(clearStore, clearHttpCredential), QStringLiteral("httpClear"));

    auto hashedHttpCredential = new CredentialHttp;
    hashedHttpCredential->setPasswordType(CredentialHttp::Hashed);
    auth->addRealm(new AuthenticationRealm(hashedStore, hashedHttpCredential), QStringLiteral("httpHashed"));

    auto noneHttpCredential = new CredentialHttp;
    noneHttpCredential->setPasswordType(CredentialHttp::None);
    auth->addRealm(new AuthenticationRealm(clearStore, noneHttpCredential), QStringLiteral("httpNone"));


    new Session(app);

    new AuthenticationTest(app);
    if (!engine->init()) {
        return nullptr;
    }
    return engine;
}


void TestAuthentication::cleanupTestCase()
{
    delete m_engine;
}

void TestAuthentication::doTest()
{
    QFETCH(QString, url);
    QFETCH(Headers, headers);
    QFETCH(QByteArray, output);

    QUrl urlAux(url.mid(1));

    QVariantMap result = m_engine->createRequest(QStringLiteral("GET"),
                                                 urlAux.path(),
                                                 urlAux.query(QUrl::FullyEncoded).toLatin1(),
                                                 headers,
                                                 nullptr);

    QCOMPARE(result.value(QStringLiteral("body")).toByteArray(), output);
}

void TestAuthentication::testController_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<Headers>("headers");
    QTest::addColumn<QByteArray>("output");

    // UriFor
    Headers headers;
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("foo"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("123"));
    QTest::newRow("auth-test00") << QStringLiteral("/authentication/test/authenticate?") + query.toString(QUrl::FullyEncoded)
                                 << headers << QByteArrayLiteral("fail");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("foo"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("321"));
    QTest::newRow("auth-test01") << QStringLiteral("/authentication/test/authenticate?") + query.toString(QUrl::FullyEncoded)
                                 << headers << QByteArrayLiteral("fail");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("foo"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("123"));
    QTest::newRow("auth-user-test00") << QStringLiteral("/authentication/test/authenticate_user?") + query.toString(QUrl::FullyEncoded)
                                      << headers << QByteArrayLiteral("ok");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("foo"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("321"));
    QTest::newRow("auth-user-test01") << QStringLiteral("/authentication/test/authenticate_user?") + query.toString(QUrl::FullyEncoded)
                                      << headers << QByteArrayLiteral("fail");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("bar"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("321"));
    QTest::newRow("auth-user-test02") << QStringLiteral("/authentication/test/authenticate_user?") + query.toString(QUrl::FullyEncoded)
                                      << headers << QByteArrayLiteral("ok");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("bar"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("123"));
    QTest::newRow("auth-user-test03") << QStringLiteral("/authentication/test/authenticate_user?") + query.toString(QUrl::FullyEncoded)
                                      << headers << QByteArrayLiteral("fail");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("foo"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("123"));
    QTest::newRow("auth-user-realm-test00") << QStringLiteral("/authentication/test/authenticate_user_realm/hashed?") + query.toString(QUrl::FullyEncoded)
                                            << headers << QByteArrayLiteral("ok");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("foo"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("321"));
    QTest::newRow("auth-user-realm-test01") << QStringLiteral("/authentication/test/authenticate_user_realm/hashed?") + query.toString(QUrl::FullyEncoded)
                                            << headers << QByteArrayLiteral("fail");
    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("bar"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("321"));
    QTest::newRow("auth-user-realm-test02") << QStringLiteral("/authentication/test/authenticate_user_realm/hashed?") + query.toString(QUrl::FullyEncoded)
                                            << headers << QByteArrayLiteral("ok");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("bar"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("123"));
    QTest::newRow("auth-user-realm-test03") << QStringLiteral("/authentication/test/authenticate_user_realm/hashed?") + query.toString(QUrl::FullyEncoded)
                                            << headers << QByteArrayLiteral("fail");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("foo"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("123"));
    QTest::newRow("auth-user-realm-test04") << QStringLiteral("/authentication/test/authenticate_user_realm/none?") + query.toString(QUrl::FullyEncoded)
                                            << headers << QByteArrayLiteral("ok");

    query.clear();
    query.addQueryItem(QStringLiteral("id"), QStringLiteral("foo"));
    query.addQueryItem(QStringLiteral("password"), QStringLiteral("3212134324234324"));
    QTest::newRow("auth-user-realm-test05") << QStringLiteral("/authentication/test/authenticate_user_realm/none?") + query.toString(QUrl::FullyEncoded)
                                            << headers << QByteArrayLiteral("ok");

    // HTTP auth
    headers.clear();
    headers.setAuthorizationBasic(QStringLiteral("foo"), QStringLiteral("123"));
    QTest::newRow("auth-http-user-realm-test00") << QStringLiteral("/authentication/test/authenticate_user_realm/httpHashed")
                                                 << headers << QByteArrayLiteral("ok");
    headers.clear();
    headers.setAuthorizationBasic(QStringLiteral("foo"), QStringLiteral("321"));
    QTest::newRow("auth-http-user-realm-test01") << QStringLiteral("/authentication/test/authenticate_user_realm/httpHashed")
                                                 << headers << QByteArrayLiteral("fail");
    headers.clear();
    headers.setAuthorizationBasic(QStringLiteral("bar"), QStringLiteral("321"));
    QTest::newRow("auth-http-user-realm-test02") << QStringLiteral("/authentication/test/authenticate_user_realm/httpHashed")
                                                 << headers << QByteArrayLiteral("ok");

    headers.clear();
    headers.setAuthorizationBasic(QStringLiteral("bar"), QStringLiteral("123"));
    QTest::newRow("auth-http-user-realm-test03") << QStringLiteral("/authentication/test/authenticate_user_realm/httpHashed")
                                                 << headers << QByteArrayLiteral("fail");

    headers.clear();
    headers.setAuthorizationBasic(QStringLiteral("foo"), QStringLiteral("123"));
    QTest::newRow("auth-http-user-realm-test04") << QStringLiteral("/authentication/test/authenticate_user_realm/httpNone")
                                                 << headers << QByteArrayLiteral("ok");

    headers.clear();
    headers.setAuthorizationBasic(QStringLiteral("foo"), QStringLiteral("3212134324234324"));
    QTest::newRow("auth-http-user-realm-test05") << QStringLiteral("/authentication/test/authenticate_user_realm/httpNone")
                                                 << headers << QByteArrayLiteral("ok");

}

QTEST_MAIN(TestAuthentication)

#include "testauthentication.moc"

#endif