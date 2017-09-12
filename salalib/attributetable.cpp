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


#include "attributetable.h"


const std::string &dXreimpl::AttributeColumnImpl::getName() const
{
    return m_name;
}

bool dXreimpl::AttributeColumnImpl::isLocked() const
{
    return m_locked;
}

void dXreimpl::AttributeColumnImpl::setLock(bool lock)
{
    m_locked = lock;
}

bool dXreimpl::AttributeColumnImpl::isHidden() const
{
    return m_hidden;
}

void dXreimpl::AttributeColumnImpl::setHidden(bool hidden)
{
    m_hidden = hidden;
}

const std::string &dXreimpl::AttributeColumnImpl::getFormula() const
{
    return m_formula;
}

void dXreimpl::AttributeColumnImpl::updateStats(float val, float oldVal)
{
    if (m_stats.total < 0)
    {
        m_stats.total = val;
    }
    else
    {
        m_stats.total += val;
        m_stats.total -= oldVal;
    }
    if (val > m_stats.max)
    {
        m_stats.max = val;
    }
    if (m_stats.min < 0 || val < m_stats.min)
    {
        m_stats.min = val;
    }
}


// AttributeRow implementation
float dXreimpl::AttributeRowImpl::getValue(const std::string &column) const
{
    return getValue(m_colManager.getColumnIndex(column));
}

float dXreimpl::AttributeRowImpl::getValue(size_t index) const
{
    checkIndex(index);
    return m_data[index];
}

void dXreimpl::AttributeRowImpl::setValue(const std::string &column, float value)
{
    setValue(m_colManager.getColumnIndex(column), value);
}

void dXreimpl::AttributeRowImpl::setValue(size_t index, float value)
{
    checkIndex(index);
    float oldVal = m_data[index];
    m_data[index] = value;
    if (oldVal < 0.0f)
    {
        oldVal = 0.0f;
    }
    m_colManager.getColumn(index).updateStats(value, oldVal);
}

void dXreimpl::AttributeRowImpl::addColumn()
{
    m_data.push_back(-1);
}

void dXreimpl::AttributeRowImpl::removeColumn(size_t index)
{
    checkIndex(index);
    m_data.erase(m_data.begin() + index);
}

void dXreimpl::AttributeRowImpl::checkIndex(size_t index) const
{
    if( index >= m_data.size())
    {
        throw std::out_of_range("AttributeColumn index out of range");
    }
}
