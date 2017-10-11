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

    REQUIRE(view.getIndex().front().key.value == 7);
}
