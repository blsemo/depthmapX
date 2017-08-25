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
#include "genlib/paftl.h"

TEST_CASE("Searching in pqmap")
{
    pqmap<int, std::string> testmap;
    testmap.add(0, "foo");
    testmap.add(2, "bar");
    testmap.add(5, "baz");

    REQUIRE(testmap.findindex(0) == 0);
    REQUIRE(testmap.findindex(1) == paftl::npos);
    REQUIRE(testmap.findindex(2) == 1);

    REQUIRE(testmap[1] == "bar");


}

