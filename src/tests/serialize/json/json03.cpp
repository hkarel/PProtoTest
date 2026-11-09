#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "pproto/serialize/json.h"

#include "catch2/log_saver.h"
#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_session.hpp"

namespace pproto {
namespace data {

struct A
{
    qint32   p1 = {5};
    QString  p2 = {"string1"};
    qint64   p3 = {10};
    QString  p4 = {"string2"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_OPT ( p2 )
        J_SERIALIZE_ITEM( p3 )
        J_SERIALIZE_OPT ( p4 )
    J_SERIALIZE_END
};

struct Alist
{
    quint32  v1 = {5};
    QString  v2 = {"string1"};
    QList<A> v3;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( v1 )
        J_SERIALIZE_OPT ( v2 )
        J_SERIALIZE_ITEM( v3 )
    J_SERIALIZE_END
};

} // namespace data
} // namespace pproto

TEST_CASE( "Deserialize optional fields", "[json]" )
{
    using namespace pproto::data;

    SECTION( "Filling out all fields" )
    {
        QByteArray json = R"({"p1":10,"p2":"string","p3":20,"p4":"string"})";

        A a;
        pproto::SResult sr = a.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( a.p1 == 10       );
        REQUIRE( a.p2 == "string" );
        REQUIRE( a.p3 == 20       );
        REQUIRE( a.p4 == "string" );
    }
    SECTION( "Filling out mandatory fields" )
    {
        QByteArray json = R"({"p1":10,"p3":20})";

        A a;
        pproto::SResult sr = a.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( a.p1 == 10        );
        REQUIRE( a.p2 == "string1" ); // default value
        REQUIRE( a.p3 == 20        );
        REQUIRE( a.p4 == "string2" ); // default value
    }
    SECTION( "Filling out mandatory and optional fields (skip p2 field)" )
    {
        QByteArray json = R"({"p1":10,"p3":20,"p4":"string"})";

        A a;
        pproto::SResult sr = a.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( a.p1 == 10        );
        REQUIRE( a.p2 == "string1" ); // default value
        REQUIRE( a.p3 == 20        );
        REQUIRE( a.p4 == "string"  );
    }
    SECTION( "Filling out partially mandatory fields (skip p3 field)" )
    {
        QByteArray json = R"({"p1":10,"p2":"string"})";

        A a;
        pproto::SResult sr = a.fromJson(json);
        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }
}

TEST_CASE( "Deserialize optional fields in list", "[json]" )
{
    using namespace pproto::data;

    SECTION( "Filling out all fields" )
    {
        QByteArray json = R"(
            {"v1":10,"v2":"string",
             "v3":[{"p1":11,"p2":"string1","p3":21,"p4":"string1"},
                   {"p1":12,"p2":"string2","p3":22,"p4":"string2"},
                   {"p1":13,"p2":"string3","p3":23,"p4":"string3"}]}
        )";

        Alist al;
        pproto::SResult sr = al.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( al.v3.count() == 3       );

        REQUIRE( al.v1 == 10              );
        REQUIRE( al.v2 == "string"        );
        REQUIRE( al.v3[0].p1 == 11        );
        REQUIRE( al.v3[0].p2 == "string1" );
        REQUIRE( al.v3[0].p3 == 21        );
        REQUIRE( al.v3[0].p4 == "string1" );
        REQUIRE( al.v3[1].p1 == 12        );
        REQUIRE( al.v3[1].p2 == "string2" );
        REQUIRE( al.v3[1].p3 == 22        );
        REQUIRE( al.v3[1].p4 == "string2" );
        REQUIRE( al.v3[2].p1 == 13        );
        REQUIRE( al.v3[2].p2 == "string3" );
        REQUIRE( al.v3[2].p3 == 23        );
        REQUIRE( al.v3[2].p4 == "string3" );
    }
    SECTION( "Filling out mandatory fields (skip v2 field)" )
    {
        QByteArray json = R"(
            {"v1":10,
             "v3":[{"p1":11,"p2":"string","p3":21,"p4":"string"}]}
        )";

        Alist al;
        pproto::SResult sr = al.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( al.v3.count() == 1       );

        REQUIRE( al.v1 == 10              );
        REQUIRE( al.v2 == "string1"       ); // default value
        REQUIRE( al.v3[0].p1 == 11        );
        REQUIRE( al.v3[0].p2 == "string"  );
        REQUIRE( al.v3[0].p3 == 21        );
        REQUIRE( al.v3[0].p4 == "string"  );
    }
    SECTION( "Filling out mandatory and optional fields (skip v3.p2[1], v3.p4[2] fields)" )
    {
        QByteArray json = R"(
            {"v1":10,"v2":"string",
             "v3":[{"p1":11,"p2":"string1","p3":21,"p4":"string1"},
                   {"p1":12,               "p3":22,"p4":"string2"},
                   {"p1":13,"p2":"string3","p3":23               }]}
        )";

        Alist al;
        pproto::SResult sr = al.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( al.v3.count() == 3       );

        REQUIRE( al.v1 == 10              );
        REQUIRE( al.v2 == "string"        );
        REQUIRE( al.v3[0].p1 == 11        );
        REQUIRE( al.v3[0].p2 == "string1" );
        REQUIRE( al.v3[0].p3 == 21        );
        REQUIRE( al.v3[0].p4 == "string1" );
        REQUIRE( al.v3[1].p1 == 12        );
        REQUIRE( al.v3[1].p2 == "string1" ); // default value
        REQUIRE( al.v3[1].p3 == 22        );
        REQUIRE( al.v3[1].p4 == "string2" );
        REQUIRE( al.v3[2].p1 == 13        );
        REQUIRE( al.v3[2].p2 == "string3" );
        REQUIRE( al.v3[2].p3 == 23        );
        REQUIRE( al.v3[2].p4 == "string2" ); // default value
    }
    SECTION( "Filling out partially mandatory fields (skip v3.p3[0] field)" )
    {
        QByteArray json = R"(
            {"v1":10,"v2":"string",
             "v3":[{"p1":11,"p2":"string1",        "p4":"string1"},
                   {"p1":12,"p2":"string2","p3":22,"p4":"string2"},
                   {"p1":13,"p2":"string3","p3":23,"p4":"string3"}]}
        )";

        Alist al;
        pproto::SResult sr = al.fromJson(json);
        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }
}

TEST_CASE( "Deserialize list field from NULL", "[json]" )
{
    using namespace pproto::data;

    SECTION( "Filling out all fields" )
    {
        QByteArray json = R"(
            {"v1":10,"v2":"string",
             "v3":[{"p1":11,"p2":null,     "p3":21,"p4":"string1"},
                   {"p1":0, "p2":"string2","p3":0, "p4":"string2"},
                   {"p1":13,"p2":"string3","p3":23,"p4":null}]}
        )";

        Alist al;
        pproto::SResult sr = al.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( al.v3.count() == 3       );

        REQUIRE( al.v1 == 10              );
        REQUIRE( al.v2 == "string"        );
        REQUIRE( al.v3[0].p1 == 11        );
        REQUIRE( al.v3[0].p2 == ""        );
        REQUIRE( al.v3[0].p3 == 21        );
        REQUIRE( al.v3[0].p4 == "string1" );
        REQUIRE( al.v3[1].p1 == 0         );
        REQUIRE( al.v3[1].p2 == "string2" );
        REQUIRE( al.v3[1].p3 == 0         );
        REQUIRE( al.v3[1].p4 == "string2" );
        REQUIRE( al.v3[2].p1 == 13        );
        REQUIRE( al.v3[2].p2 == "string3" );
        REQUIRE( al.v3[2].p3 == 23        );
        REQUIRE( al.v3[2].p4 == ""        );
    }
    SECTION( "Filling out mandatory fields (v2 is NULL)" )
    {
        QByteArray json = R"(
            {"v1":10,"v2":null,
             "v3":[{"p1":11,"p2":"string","p3":21,"p4":"string"}]}
        )";

        Alist al;
        pproto::SResult sr = al.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( al.v3.count() == 1       );

        REQUIRE( al.v1 == 10              );
        REQUIRE( al.v2 == ""              );
        REQUIRE( al.v3[0].p1 == 11        );
        REQUIRE( al.v3[0].p2 == "string"  );
        REQUIRE( al.v3[0].p3 == 21        );
        REQUIRE( al.v3[0].p4 == "string"  );
    }
    SECTION( "Filling out v3 as NULL)" )
    {
        QByteArray json = R"(
            {"v1":10,"v2":"string","v3":null}
        )";

        Alist al;
        al.v3 = {{1, "123", 2, "345"}, {3, "456", 4, "789"}};

        REQUIRE( al.v3.count() == 2 );

        pproto::SResult sr = al.fromJson(json);
        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( al.v1 == 10        );
        REQUIRE( al.v2 == "string"  );
        REQUIRE( al.v3.count() == 0 );
    }
}

int main(int argc, char* argv[])
{
    alog::logger().start();

    alog::Saver::Ptr saver {new alog::CatchSaver()};
    alog::logger().addSaver(saver);

    int result = Catch::Session().run(argc, argv);

    alog::stop();

    return result;
}
