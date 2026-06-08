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

#include <atomic>

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

struct C
{
    std::atomic<qint32> p1 = {0};
    std::atomic<bool>   p2 = {false};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_OPT ( p2 )
    J_SERIALIZE_END
};

struct D : clife_base
{
    std::atomic<qint32> p1 = {0};
    std::atomic<bool>   p2 = {false};
    std::atomic<qint16> p3 = {10};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( p1 )
        J_SERIALIZE_OPT ( p2 )
        J_SERIALIZE_OPT ( p3 )
    J_SERIALIZE_END
};

struct F : clife_base
{
    std::atomic<qint32> p1 = {0};

    struct SubAtom
    {
        qint32 s1 = {5};
        qint16 s2 = {15};

        J_SERIALIZE_BEGIN
            J_SERIALIZE_ITEM( s1 )
            J_SERIALIZE_OPT ( s2 )
        J_SERIALIZE_END
    };
    std::atomic<SubAtom> p2;

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

    SECTION( "Deserialize atomic lst::List<C>" )
    {
        QByteArray json = R"({"list":[{"p1":10},{"p1":30,"p2":true}]})";

        LST<lst::List<C>> cl;
        pproto::SResult sr = cl.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( cl.list.count() == 2 );

        if (cl.list.count() == 2)
        {
            C* c1 = cl.list.item(0);
            REQUIRE( c1->p1 == 10 );
            REQUIRE( c1->p2 == false );

            C* c2 = cl.list.item(1);
            REQUIRE( c2->p1 == 30 );
            REQUIRE( c2->p2 == true );
        }
    }

    SECTION( "Deserialize atomic lst::List<D> (D derived from clife_base)" )
    {
        QByteArray json = R"({"list":[{"p1":10,"p2":true},{"p1":30,"p3":20}]})";

        LST<lst::List<D>> dl;
        pproto::SResult sr = dl.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( dl.list.count() == 2 );

        if (dl.list.count() == 2)
        {
            D* d1 = dl.list.item(0);
            REQUIRE( d1->p1 == 10 );
            REQUIRE( d1->p2 == true );
            REQUIRE( d1->p3 == 10 );

            D* d2 = dl.list.item(1);
            REQUIRE( d2->p1 == 30 );
            REQUIRE( d2->p2 == false );
            REQUIRE( d2->p3 == 20 );
        }
    }

    SECTION( "Deserialize atomic struct lst::List<F> (F derived from clife_base)" )
    {
        QByteArray json = R"({"list":[{"p1":10,"p2":{"s1":10,"s2":10}},{"p1":20,"p2":{"s1":20}},{"p1":30}]})";

        LST<lst::List<F>> fl;
        pproto::SResult sr = fl.fromJson(json);

        ALOG_FLUSH();
        REQUIRE( bool(sr) == true );

        REQUIRE( fl.list.count() == 3 );

        if (fl.list.count() == 3)
        {
            F* f1 = fl.list.item(0);
            REQUIRE( f1->p1 == 10 );
            F::SubAtom sa1 = f1->p2.load();
            REQUIRE( sa1.s1 == 10 );
            REQUIRE( sa1.s2 == 10 );

            F* f2 = fl.list.item(1);
            REQUIRE( f2->p1 == 20 );
            F::SubAtom sa2 = f2->p2.load();
            REQUIRE( sa2.s1 == 20 );
            REQUIRE( sa2.s2 == 15 );

            F* f3 = fl.list.item(2);
            REQUIRE( f3->p1 == 30 );
            F::SubAtom sa3 = f3->p2.load();
            REQUIRE( sa3.s1 == 5 );
            REQUIRE( sa3.s2 == 15 );
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
