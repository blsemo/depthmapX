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

#include "attributetable.h"
#include "attributetableindex.h"

template<typename TKey>
class AttributeTableView
{
public:
    AttributeTableView(const dXreimpl::AttributeTable<TKey>& table );

    const dXreimpl::AttributeTable<TKey> &m_table;

    // columnIndex < 0 -> not set
    virtual void setDisplayColumn(int columnIndex);
    int getDisplayColumn() const{ return m_displayColumn;}

    float getNormalisedValue(const TKey& key, const dXreimpl::AttributeRow &row) const;
    const DisplayParams& getDisplayParams() const;

    typedef std::vector<dXreimpl::ConstAttributeIndexItem<TKey>> ConstIndex;
    const ConstIndex& getConstIndex() const{return m_index;}

private:
    ConstIndex m_index;
    int m_displayColumn;
};



template<typename TKey>
AttributeTableView<TKey>::AttributeTableView( const dXreimpl::AttributeTable<TKey>& table ) : m_table(table), m_displayColumn(-1)
{}

template<typename TKey> void AttributeTableView<TKey>::setDisplayColumn(int columnIndex){
    if (columnIndex < 0)
    {
        m_displayColumn = -1;
        m_index.clear();
        return;
    }
    // recalculate the index even if it's the same column in case stuff has changed
    m_index = dXreimpl::makeAttributeIndex<dXreimpl::ConstAttributeIndexItem<TKey>>(m_table, columnIndex);
    m_displayColumn = columnIndex;
}

template<typename TKey>
float AttributeTableView<TKey>::getNormalisedValue(const TKey& key, const dXreimpl::AttributeRow &row) const
{
    if ( m_displayColumn < 0)
    {
        auto& endIter = m_table.end();
        --endIter;
        return (float)key.value /(float) endIter->getKey().value;
    }
    return row.getNormalisedValue(m_displayColumn);
}

template<typename TKey>
const DisplayParams &AttributeTableView<TKey>::getDisplayParams() const
{
    if (m_displayColumn < 0)
    {
        return m_table.getDisplayParams();
    }
    return m_table.getColumn(m_displayColumn).getDisplayParams();
}



template <typename TKey>
class AttributeTableHandle : public AttributeTableView<TKey>
{
public:
    AttributeTableHandle(dXreimpl::AttributeTable<TKey> &table) : m_mutableTable(table), AttributeTableView<TKey>(table){}
    typedef std::vector<dXreimpl::AttributeIndexItem<TKey>> Index;
    const Index& getIndex() const {return m_mutableIndex;}
    virtual void setDisplayColumn(int columnIndex);
private:
    dXreimpl::AttributeTable<TKey>& m_mutableTable;
    Index m_mutableIndex;

};

template<typename TKey> void AttributeTableHandle<TKey>::setDisplayColumn(int columnIndex){
    if (columnIndex < 0)
    {
        m_mutableIndex.clear();
    }
    else
    {
        // recalculate the index even if it's the same column in case stuff has changed
        m_mutableIndex = dXreimpl::makeAttributeIndex<dXreimpl::AttributeIndexItem<TKey>>(m_mutableTable, columnIndex);
    }
    AttributeTableView::setDisplayColumn(columnIndex);
}
