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
    qint32  v1 = {1};
    QString v2 = {"str 1234"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( v1 )
        J_SERIALIZE_ITEM( v2 )
    J_SERIALIZE_END
};

struct B
{
    qint64  v1 = {1};
    QString v2 = {"str 5678"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( v1 )
        J_SERIALIZE_OPT ( v2 )
    J_SERIALIZE_END
};

struct C
{
    qint32  v1 = {5};
    QString v2 = {"1234"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_OPT( v1 )
        J_SERIALIZE_OPT( v2 )
    J_SERIALIZE_END
};

struct DA
{
    qint32  p1 = {1};
    A       p2;
    QString p3 = {"5678"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_ITEM( p2 )
        J_SERIALIZE_ITEM( p3 )
    J_SERIALIZE_END
};

struct DAopt
{
    qint32  p1 = {1};
    A       p2;
    QString p3 = {"5678"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_OPT ( p2 )
        J_SERIALIZE_ITEM( p3 )
    J_SERIALIZE_END
};

struct DB
{
    qint32  p1 = {0};
    B       p2;
    QString p3 = {"5678"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_ITEM( p2 )
        J_SERIALIZE_ITEM( p3 )
    J_SERIALIZE_END
};

struct DBopt
{
    qint32  p1 = {0};
    B       p2;
    QString p3 = {"5678"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_OPT ( p2 )
        J_SERIALIZE_ITEM( p3 )
    J_SERIALIZE_END
};

struct DC
{
    qint32  p1 = {0};
    C       p2;
    QString p3 = {"5678"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_ITEM( p2 )
        J_SERIALIZE_ITEM( p3 )
    J_SERIALIZE_END
};

struct DCopt
{
    qint32  p1 = {0};
    C       p2;
    QString p3 = {"5678"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_OPT ( p2 )
        J_SERIALIZE_ITEM( p3 )
    J_SERIALIZE_END
};

} // namespace data
} // namespace pproto

TEST_CASE( "Deserialize optional fields with NULL", "[json]" )
{
    using namespace pproto::data;

    SECTION( "Filling out DA all fields" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10,"v2":"string 890"},"p3":"AAA"})";

        DA d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14           );
        REQUIRE( d.p2.v1 == 10           );
        REQUIRE( d.p2.v2 == "string 890" );
        REQUIRE( d.p3    == "AAA"        );
    }

    SECTION( "Filling out DA all fields (p1 is NULL)" )
    {
        QByteArray json = R"({"p1":null,"p2":{"v1":10,"v2":"string 890"},"p3":"AAA"})";

        DA d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );

        // REQUIRE( d.p1    == 0            );
        // REQUIRE( d.p2.v1 == 10           );
        // REQUIRE( d.p2.v2 == "string 890" );
        // REQUIRE( d.p3    == "AAA"        );
    }

    SECTION( "Filling out DA all fields (p1, p2.v1 is NULL)" )
    {
        QByteArray json = R"({"p1":null,"p2":{"v1":null,"v2":"string 890"},"p3":"AAA"})";

        DA d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );

        // REQUIRE( d.p1    == 0            );
        // REQUIRE( d.p2.v1 == 0            );
        // REQUIRE( d.p2.v2 == "string 890" );
        // REQUIRE( d.p3    == "AAA"        );
    }

    SECTION( "Filling out DA all fields (p2.v2 is NULL)" )
    {
        QByteArray json = R"({"p1":0,"p2":{"v1":0,"v2":null},"p3":"AAA"})";

        DA d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 0  );
        REQUIRE( d.p2.v1 == 0  );
        REQUIRE( d.p2.v2 == "" );
        REQUIRE( d.p3    == "AAA" );
    }

    SECTION( "Filling out DA all fields (all is NULL)" )
    {
        QByteArray json = R"({"p1":null,"p2":{"v1":null,"v2":null},"p3":null})";

        DA d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );

        // REQUIRE( d.p1    == 0  );
        // REQUIRE( d.p2.v1 == 0  );
        // REQUIRE( d.p2.v2 == "" );
        // REQUIRE( d.p3    == "" );
    }

    SECTION( "Filling out DA mandatory p2 as {} object" )
    {
        QByteArray json = R"({"p1":14,"p2":{},"p3":"AAA"})";

        DA d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }

    SECTION( "Filling out DA mandatory p2 as NULL" )
    {
        QByteArray json = R"({"p1":14,"p2":null,"p3":"AAA"})";

        DA d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }

    SECTION( "Filling out DA optional p2" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10,"v2":"string 890"},"p3":"AAA"})";

        DAopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14           );
        REQUIRE( d.p2.v1 == 10           );
        REQUIRE( d.p2.v2 == "string 890" );
        REQUIRE( d.p3    == "AAA"        );
    }

    SECTION( "Filling out DA optional p2 (skip p2)" )
    {
        QByteArray json = R"({"p1":14,"p3":"AAA"})";

        DAopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14         );
        REQUIRE( d.p2.v1 == 1          ); // default value
        REQUIRE( d.p2.v2 == "str 1234" ); // default value
        REQUIRE( d.p3    == "AAA"      );
    }

    SECTION( "Filling out DA optional p2 as {} object" )
    {
        QByteArray json = R"({"p1":14,"p2":{},"p3":"AAA"})";

        DAopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }

    SECTION( "Filling out DA optional p2 as NULL" )
    {
        QByteArray json = R"({"p1":14,"p2":null,"p3":"AAA"})";

        DAopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14         );
        REQUIRE( d.p2.v1 == 1          ); // default value
        REQUIRE( d.p2.v2 == "str 1234" ); // default value
        REQUIRE( d.p3    == "AAA"      );
    }
    //---

    SECTION( "Filling out DB all fields" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10,"v2":"string 890"},"p3":"AAA"})";

        DB d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14           );
        REQUIRE( d.p2.v1 == 10           );
        REQUIRE( d.p2.v2 == "string 890" );
        REQUIRE( d.p3    == "AAA"        );
    }

    SECTION( "Filling out DB mandatory p2 as {} object" )
    {
        QByteArray json = R"({"p1":14,"p2":{},"p3":"AAA"})";

        DB d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }

    SECTION( "Filling out DB mandatory p2 as NULL" )
    {
        QByteArray json = R"({"p1":14,"p2":null,"p3":"AAA"})";

        DB d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }

    SECTION( "Filling out DB mandatory p2 (skip p2.v2)" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10},"p3":"AAA"})";

        DB d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14         );
        REQUIRE( d.p2.v1 == 10         );
        REQUIRE( d.p2.v2 == "str 5678" ); // default value
        REQUIRE( d.p3    == "AAA"      );
    }

    // SECTION( "Filling out DB mandatory p2 (p2.v1 is NULL; skip p2.v2)" )
    // {
    //     QByteArray json = R"({"p1":14,"p2":{"v1":null},"p3":"AAA"})";

    //     DB d;
    //     pproto::SResult sr = d.fromJson(json);

    //     ALOG_FLUSH();
    //     REQUIRE( bool(sr) == true );

    //     REQUIRE( d.p1    == 14         );
    //     REQUIRE( d.p2.v1 == 0          );
    //     REQUIRE( d.p2.v2 == "str 5678" ); // default value
    //     REQUIRE( d.p3    == "AAA"      );
    // }

    SECTION( "Filling out DB optional p2" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10,"v2":"string 890"},"p3":"AAA"})";

        DBopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14           );
        REQUIRE( d.p2.v1 == 10           );
        REQUIRE( d.p2.v2 == "string 890" );
        REQUIRE( d.p3    == "AAA"        );
    }

    SECTION( "Filling out DB optional p2 (skip p2)" )
    {
        QByteArray json = R"({"p1":14,"p3":"AAA"})";

        DBopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14         );
        REQUIRE( d.p2.v1 == 1          ); // default value
        REQUIRE( d.p2.v2 == "str 5678" ); // default value
        REQUIRE( d.p3    == "AAA"      );
    }

    SECTION( "Filling out DB optional p2 (skip p2.v2)" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10},"p3":"AAA"})";

        DBopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14         );
        REQUIRE( d.p2.v1 == 10         );
        REQUIRE( d.p2.v2 == "str 5678" ); // default value
        REQUIRE( d.p3    == "AAA"      );
    }

    SECTION( "Filling out DB optional p2 as {} object" )
    {
        QByteArray json = R"({"p1":14,"p2":{},"p3":"AAA"})";

        DBopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }

    SECTION( "Filling out DB optional p2 as NULL" )
    {
        QByteArray json = R"({"p1":14,"p2":null,"p3":"AAA"})";

        DBopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14         );
        REQUIRE( d.p2.v1 == 1          ); // default value
        REQUIRE( d.p2.v2 == "str 5678" ); // default value
        REQUIRE( d.p3    == "AAA"      );
    }

    SECTION( "Filling out DB optional p2 (skip p2.v2)" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":0},"p3":"AAA"})";

        DBopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14         );
        REQUIRE( d.p2.v1 == 0          );
        REQUIRE( d.p2.v2 == "str 5678" ); // default value
        REQUIRE( d.p3    == "AAA"      );
    }

    // SECTION( "Filling out DB optional p2 (p1, p2.v1 is NULL; skip p2.v2)" )
    // {
    //     QByteArray json = R"({"p1":null,"p2":{"v1":null},"p3":"AAA"})";

    //     DBopt d;
    //     pproto::SResult sr = d.fromJson(json);

    //     ALOG_FLUSH();
    //     REQUIRE( bool(sr) == true );

    //     REQUIRE( d.p1    == 0          );
    //     REQUIRE( d.p2.v1 == 0          );
    //     REQUIRE( d.p2.v2 == "str 5678" ); // default value
    //     REQUIRE( d.p3    == "AAA"      );
    // }

    SECTION( "Filling out DB optional p2 (p1, p2 is NULL)" )
    {
        QByteArray json = R"({"p1":0,"p2":null,"p3":"AAA"})";

        DBopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 0          );
        REQUIRE( d.p2.v1 == 1          ); // default value
        REQUIRE( d.p2.v2 == "str 5678" ); // default value
        REQUIRE( d.p3    == "AAA"      );
    }
    //---

    SECTION( "Filling out DC all fields" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10,"v2":"string 890"},"p3":"AAA"})";

        DC d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14           );
        REQUIRE( d.p2.v1 == 10           );
        REQUIRE( d.p2.v2 == "string 890" );
        REQUIRE( d.p3    == "AAA"        );
    }

    SECTION( "Filling out DC mandatory p2 as {} object" )
    {
        QByteArray json = R"({"p1":14,"p2":{},"p3":"AAA"})";

        DC d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14     );
        REQUIRE( d.p2.v1 == 5      ); // default value
        REQUIRE( d.p2.v2 == "1234" ); // default value
        REQUIRE( d.p3    == "AAA"  );
    }

    SECTION( "Filling out DC mandatory p2 as NULL" )
    {
        QByteArray json = R"({"p1":14,"p2":null,"p3":"AAA"})";

        DC d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE_FALSE( bool(sr) == true );
    }

    SECTION( "Filling out DC mandatory p2 (skip p2.v2)" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10},"p3":"AAA"})";

        DC d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14     );
        REQUIRE( d.p2.v1 == 10     );
        REQUIRE( d.p2.v2 == "1234" ); // default value
        REQUIRE( d.p3    == "AAA"  );
    }

    // SECTION( "Filling out DC mandatory p2 (p2.v1 is NULL; skip p2.v2)" )
    // {
    //     QByteArray json = R"({"p1":14,"p2":{"v1":null},"p3":"AAA"})";

    //     DC d;
    //     pproto::SResult sr = d.fromJson(json);

    //     ALOG_FLUSH();
    //     REQUIRE( bool(sr) == true );

    //     REQUIRE( d.p1    == 14     );
    //     REQUIRE( d.p2.v1 == 0      );
    //     REQUIRE( d.p2.v2 == "1234" ); // default value
    //     REQUIRE( d.p3    == "AAA"  );
    // }

    SECTION( "Filling out DC optional p2" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10,"v2":"string 890"},"p3":"AAA"})";

        DCopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14           );
        REQUIRE( d.p2.v1 == 10           );
        REQUIRE( d.p2.v2 == "string 890" );
        REQUIRE( d.p3    == "AAA"        );
    }

    SECTION( "Filling out DC optional p2 (skip p2)" )
    {
        QByteArray json = R"({"p1":14,"p3":"AAA"})";

        DCopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14     );
        REQUIRE( d.p2.v1 == 5      ); // default value
        REQUIRE( d.p2.v2 == "1234" ); // default value
        REQUIRE( d.p3    == "AAA"  );
    }

    SECTION( "Filling out DC optional p2 (skip p2.v2)" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":10},"p3":"AAA"})";

        DCopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14     );
        REQUIRE( d.p2.v1 == 10     );
        REQUIRE( d.p2.v2 == "1234" ); // default value
        REQUIRE( d.p3    == "AAA"  );
    }

    SECTION( "Filling out DC optional p2 as {} object" )
    {
        QByteArray json = R"({"p1":14,"p2":{},"p3":"AAA"})";

        DCopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14     );
        REQUIRE( d.p2.v1 == 5      ); // default value
        REQUIRE( d.p2.v2 == "1234" ); // default value
        REQUIRE( d.p3    == "AAA"  );
    }

    SECTION( "Filling out DC optional p2 as NULL" )
    {
        QByteArray json = R"({"p1":14,"p2":null,"p3":"AAA"})";

        DCopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14     );
        REQUIRE( d.p2.v1 == 5      ); // default value
        REQUIRE( d.p2.v2 == "1234" ); // default value
        REQUIRE( d.p3    == "AAA"  );
    }

    SECTION( "Filling out DC optional p2 (skip p2.v2)" )
    {
        QByteArray json = R"({"p1":14,"p2":{"v1":0},"p3":"AAA"})";

        DCopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 14     );
        REQUIRE( d.p2.v1 == 0      );
        REQUIRE( d.p2.v2 == "1234" ); // default value
        REQUIRE( d.p3    == "AAA"  );
    }

    // SECTION( "Filling out DC optional p2 (p1, p2.v1 is NULL; skip p2.v2)" )
    // {
    //     QByteArray json = R"({"p1":null,"p2":{"v1":null},"p3":"AAA"})";

    //     DCopt d;
    //     pproto::SResult sr = d.fromJson(json);

    //     ALOG_FLUSH();
    //     REQUIRE( bool(sr) == true );

    //     REQUIRE( d.p1    == 0      );
    //     REQUIRE( d.p2.v1 == 0      );
    //     REQUIRE( d.p2.v2 == "1234" ); // default value
    //     REQUIRE( d.p3    == "AAA"  );
    // }

    // SECTION( "Filling out DC optional p2 (p1, p2 is NULL)" )
    // {
    //     QByteArray json = R"({"p1":null,"p2":null,"p3":"AAA"})";

    //     DCopt d;
    //     pproto::SResult sr = d.fromJson(json);

    //     ALOG_FLUSH();
    //     REQUIRE( bool(sr) == true );

    //     REQUIRE( d.p1    == 0      );
    //     REQUIRE( d.p2.v1 == 5      ); // default value
    //     REQUIRE( d.p2.v2 == "1234" ); // default value
    //     REQUIRE( d.p3    == "AAA"  );
    // }

    SECTION( "Filling out DC optional p2 as {} object (p1, p3 is NULL)" )
    {
        QByteArray json = R"({"p1":0,"p2":{},"p3":null})";

        DCopt d;
        pproto::SResult sr = d.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( d.p1    == 0      );
        REQUIRE( d.p2.v1 == 5      ); // default value
        REQUIRE( d.p2.v2 == "1234" ); // default value
        REQUIRE( d.p3    == ""     );
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
