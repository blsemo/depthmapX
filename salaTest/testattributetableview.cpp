#include <catch.hpp>
#include <salalib/attributetableview.h>

TEST_CASE("Test Attribute view"){
    using namespace dXreimpl;
    AttributeTable<SerialisedPixelRef> table;

    table.insertOrResetColumn("foo");
    table.insertOrResetColumn("bar");
    table.addRow(0).setValue(0,1.0f).setValue(1, 1.1f);
    table.addRow(7).setValue(0,0.7f).setValue(1,1.7f);

    AttributeTableView<SerialisedPixelRef> view(table);
    view.setDisplayColumn(0);

    REQUIRE(view.getConstIndex().front().key.value == 7);
}


TEST_CASE("Test attribute table handle")
{
    using namespace  dXreimpl;
    AttributeTable<SerialisedPixelRef> table;

    table.insertOrResetColumn("foo");
    table.insertOrResetColumn("bar");
    table.addRow(0).setValue(0,1.0f).setValue(1, 1.1f);
    table.addRow(7).setValue(0,0.7f).setValue(1,1.7f);

    AttributeTableHandle<SerialisedPixelRef> handle(table);
    handle.setDisplayColumn(0);

    REQUIRE(handle.getIndex().front().key.value == 7);
    REQUIRE(handle.getConstIndex().front().key.value == 7);


    handle.getIndex().front().mutable_row->setValue(0, 0.8);

    REQUIRE(table.getRow(7).getValue(0) == Approx(0.8));

}
