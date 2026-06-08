#include "shared/list.h"
#include "shared/clife_base.h"
#include "shared/clife_ptr.h"
#include "shared/clife_alloc.h"
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
    //typedef container_ptr<A> Ptr;

    qint32  p1 = {0};
    QString p2 = {"a"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_OPT ( p2 )
    J_SERIALIZE_END
};

struct B : clife_base
{
    typedef clife_ptr<B> Ptr;

    qint32  p1 = {0};
    QString p2 = {"b"};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_OPT ( p2 )
    J_SERIALIZE_END
};

template<typename ListT>
struct LST
{
    ListT list;
    J_SERIALIZE_ONE( list )
};

} // namespace data
} // namespace pproto

TEST_CASE( "Serialize/Deserialize lst::List", "[json]" )
{
    using namespace pproto::data;

    SECTION( "Serialize lst::List<A>" )
    {
        LST<lst::List<A>> al;
        A* a1 = al.list.add();
        a1->p1 = 10;
        a1->p2 = "a1";

        A* a2 = al.list.add(nullptr);
        (void) a2;

        A* a3 = al.list.add();
        a3->p1 = 30;
        a3->p2 = "a3";

        QByteArray json = al.toJson();
        REQUIRE( json == R"({"list":[{"p1":10,"p2":"a1"},null,{"p1":30,"p2":"a3"}]})" );
    }

    SECTION( "Deserialize lst::List<A>" )
    {
        QByteArray json = R"({"list":[{"p1":10,"p2":"a1"},null,{"p1":30,"p2":"a3"}]})";

        LST<lst::List<A>> al;
        pproto::SResult sr = al.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( al.list.count() == 3 );

        if (al.list.count() == 3)
        {
            A* a1 = al.list.item(0);
            REQUIRE( a1->p1 == 10 );
            REQUIRE( a1->p2 == "a1" );

            A* a2 = al.list.item(1);
            REQUIRE( a2 == nullptr );

            A* a3 = al.list.item(2);
            REQUIRE( a3->p1 == 30 );
            REQUIRE( a3->p2 == "a3" );
        }
    }

    SECTION( "Deserialize optional lst::List<A>" )
    {
        QByteArray json = R"({"list":[{"p1":10},null,{"p1":30,"p2":"a3"}]})";

        LST<lst::List<A>> al;
        pproto::SResult sr = al.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( al.list.count() == 3 );

        if (al.list.count() == 3)
        {
            A* a1 = al.list.item(0);
            REQUIRE( a1->p1 == 10 );
            REQUIRE( a1->p2 == "a" );

            A* a2 = al.list.item(1);
            REQUIRE( a2 == nullptr );

            A* a3 = al.list.item(2);
            REQUIRE( a3->p1 == 30 );
            REQUIRE( a3->p2 == "a3" );
        }
    }

    SECTION( "Serialize lst::List<B> (B derived from clife_base)" )
    {
        LST<lst::List<B, clife_alloc<B>>> bl;
        B* b1 = bl.list.add();
        b1->p1 = 10;
        b1->p2 = "b1";

        B* b2 = bl.list.add(nullptr);
        (void) b2;

        B* b3 = bl.list.add();
        b3->p1 = 30;
        b3->p2 = "b3";

        QByteArray json = bl.toJson();
        REQUIRE( json == R"({"list":[{"p1":10,"p2":"b1"},null,{"p1":30,"p2":"b3"}]})" );
    }

    SECTION( "Deserialize lst::List<B> (B derived from clife_base)" )
    {
        QByteArray json = R"({"list":[{"p1":10,"p2":"b1"},null,{"p1":30,"p2":"b3"}]})";

        LST<lst::List<B, clife_alloc<B>>> bl;
        pproto::SResult sr = bl.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( bl.list.count() == 3 );

        if (bl.list.count() == 3)
        {
            B* b1 = bl.list.item(0);
            REQUIRE( b1->clife_count() == 1 );
            REQUIRE( b1->p1 == 10 );
            REQUIRE( b1->p2 == "b1" );

            B* b2 = bl.list.item(1);
            REQUIRE( b2 == nullptr );

            B* b3 = bl.list.item(2);
            REQUIRE( b3->clife_count() == 1 );
            REQUIRE( b3->p1 == 30 );
            REQUIRE( b3->p2 == "b3" );
        }
    }

    SECTION( "Deserialize optional lst::List<B> (B derived from clife_base)" )
    {
        QByteArray json = R"({"list":[{"p1":10,"p2":"b1"},null,{"p1":30}]})";

        LST<lst::List<B, clife_alloc<B>>> bl;
        pproto::SResult sr = bl.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( bl.list.count() == 3 );

        if (bl.list.count() == 3)
        {
            B* b1 = bl.list.item(0);
            REQUIRE( b1->clife_count() == 1 );
            REQUIRE( b1->p1 == 10 );
            REQUIRE( b1->p2 == "b1" );

            B* b2 = bl.list.item(1);
            REQUIRE( b2 == nullptr );

            B* b3 = bl.list.item(2);
            REQUIRE( b3->clife_count() == 1 );
            REQUIRE( b3->p1 == 30 );
            REQUIRE( b3->p2 == "b" );
        }
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
