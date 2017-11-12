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

#pragma once
#include <vector>
#include <algorithm>
#include "paftl.h"

namespace genshim
{
    /**
     * Convert a std::vector to a pvec (preserving the order of elements)
     * This is expensive as it copies every single element
     */
    template<class T> pvector<T> toPVector(const std::vector<T> &vec)
    {
        pvector<T> pvec;
        std::for_each(vec.begin(), vec.end(), [&pvec](const T& val)->void{pvec.push_back(val);});
        return pvec;
    }

    /**
     * Convert a std::vector to a pqvector (preserving the order of elements)
     * This is expensive as it copies every single element
     */
    template<class T> pqvector<T> toPQVector(const std::vector<T> &vec)
    {
        pqvector<T> pvec;
        std::for_each(vec.begin(), vec.end(), [&pvec](const T& val)->void{pvec.push_back(val);});
        return pvec;
    }
}
