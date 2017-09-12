// Copyright (C) 2017 Christian Sailer

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "catch.hpp"
#include "Catch/fakeit.hpp"
#include <salalib/attributetable.h>

TEST_CASE("test attribute column")
{
    using namespace dXreimpl;
    AttributeColumnImpl col("colName");
    REQUIRE(col.getName() == "colName");
    REQUIRE(col.getFormula() == "");

    REQUIRE_FALSE(col.isHidden());
    REQUIRE_FALSE(col.isLocked());

    col.setLock(true);
    REQUIRE_FALSE(col.isHidden());
    REQUIRE(col.isLocked());

    col.setHidden(true);
    REQUIRE(col.isHidden());
    REQUIRE(col.isLocked());

    REQUIRE(col.m_stats.max == -1.0);
    REQUIRE(col.m_stats.min == -1.0);
    REQUIRE(col.m_stats.total == -1.0);
    REQUIRE(col.m_stats.visibleMax == -1.0);
    REQUIRE(col.m_stats.visibleMin == -1.0);
    REQUIRE(col.m_stats.visibleTotal == -1.0);

    col.updateStats(1.2);
    REQUIRE(col.m_stats.max == Approx(1.2));
    REQUIRE(col.m_stats.min == Approx(1.2));
    REQUIRE(col.m_stats.total == Approx(1.2));
    REQUIRE(col.m_stats.visibleMax == -1.0);
    REQUIRE(col.m_stats.visibleMin == -1.0);
    REQUIRE(col.m_stats.visibleTotal == -1.0);

    col.updateStats(2);
    REQUIRE(col.m_stats.max == Approx(2.0));
    REQUIRE(col.m_stats.min == Approx(1.2));
    REQUIRE(col.m_stats.total == Approx(3.2));
    REQUIRE(col.m_stats.visibleMax == -1.0);
    REQUIRE(col.m_stats.visibleMin == -1.0);
    REQUIRE(col.m_stats.visibleTotal == -1.0);

    col.updateStats(3,1.2);
    REQUIRE(col.m_stats.max == Approx(3.0));
    REQUIRE(col.m_stats.min == Approx(1.2));
    REQUIRE(col.m_stats.total == Approx(5));
    REQUIRE(col.m_stats.visibleMax == -1.0);
    REQUIRE(col.m_stats.visibleMin == -1.0);
    REQUIRE(col.m_stats.visibleTotal == -1.0);
}

TEST_CASE("test attribute row")
{
    using namespace dXreimpl;
    using namespace fakeit;
    Mock<AttributeColumnManager> colMan;
    When(Method(colMan,getColumnIndex).Using(std::string("col1"))).AlwaysReturn(0);
    When(Method(colMan,getColumnIndex).Using(std::string("col2"))).AlwaysReturn(1);
    When(Method(colMan,getColumnIndex).Using(std::string("colx"))).AlwaysThrow(std::out_of_range("mock out of range"));

    When(Method(colMan,getNumColumns)).Return(2);

    Mock<AttributeColumn> col1;
    Mock<AttributeColumn> col2;
    When(Method(colMan, getColumn).Using(0)).AlwaysReturn(col1.get());
    When(Method(colMan, getColumn).Using(1)).AlwaysReturn(col2.get());
    When(Method(col1,updateStats)).AlwaysReturn();
    When(Method(col2,updateStats)).AlwaysReturn();

    AttributeRowImpl row(colMan.get());
    row.setValue("col1", 1.2);
    REQUIRE(row.getValue("col1") == Approx(1.2));
    REQUIRE(row.getValue(0) == Approx(1.2));

    row.setValue(1, 2.2);
    REQUIRE(row.getValue("col2") == Approx(2.2));
    REQUIRE(row.getValue(1) == Approx(2.2));

    row.setValue(1, 3.2);
    REQUIRE(row.getValue("col2") == Approx(3.2));
    REQUIRE(row.getValue(1) == Approx(3.2));


    Verify(Method(col1,updateStats).Using(1.2,0.0)).Once();
    Verify(Method(col2,updateStats).Using(2.2,0.0)).Once();
    Verify(Method(col2,updateStats).Using(3.2,2.2)).Once();

    REQUIRE_THROWS_AS(row.setValue("colx", 1.1), std::out_of_range);
    REQUIRE_THROWS_AS(row.setValue(2, 1.2), std::out_of_range);
    REQUIRE_THROWS_AS(row.getValue("colx"), std::out_of_range);
    REQUIRE_THROWS_AS(row.getValue(2), std::out_of_range);

    // test attribute row impl only methods
    // note that these do not affect the column manager - that will have to
    // be handled by the caller - that's why these are impl only!
    row.addColumn();
    REQUIRE(row.getValue(2) == -1.0);

    row.removeColumn(1);
    REQUIRE(row.getValue(1) == -1.0);
    REQUIRE(row.getValue(0) == Approx(1.2));
    REQUIRE_THROWS_AS(row.getValue(2), std::out_of_range);
}
