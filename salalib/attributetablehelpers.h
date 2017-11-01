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
#include "attributetableview.h"
#include "vertex.h"

namespace dXreimpl{
    template<typename RowKeyType>
    inline void pushSelectionToLayer(AttributeTable<RowKeyType> &table, LayerManager& layerManager, const std::string &layerName)
    {
        size_t layerIndex = layerManager.addLayer(layerName);
        LayerManager::KeyType layerKey = layerManager.getKey(layerIndex);
        for (auto & item: table)
        {
            auto& row = item.getRow();
            if (isObjectVisible(layerManager, row) && row.isSelected())
            {
                addLayerToObject(item.getRow(), layerKey);
            }
        }

        layerManager.setLayerVisible(layerIndex);
    }

    class PrecisionGuard
    {
    public:
        PrecisionGuard( std::ostream& stream, int prec)  : m_stream(stream), m_oldPrec(stream.precision(prec))
        {
        }
        ~PrecisionGuard(){
            m_stream.precision(m_oldPrec);
        }
    private:
        std::ostream &m_stream;
        int m_oldPrec;
    };


    template<typename TKey>
    inline void outputHeader( std::ostream& stream, const AttributeTable<TKey>& table, char delim)
    {
        for (size_t i = 0; i < table.getNumColumns(); ++i)
        {
            stream << delim << table.getColumnName(i);
        }
        stream << std::endl;
    }

    inline void outputRow( std::ostream& stream, const AttributeRow& row, char delim, size_t numCol)
    {
        PrecisionGuard pg(stream, 8);
        for (size_t i = 0; i < numCol; ++i)
        {
            stream << delim << row.getValue(i);
        }
        stream << std::endl;
    }

    inline PafColor getDisplayColor( const SerialisedPixelRef& key, const AttributeRow& row, const AttributeTableView& tableView, bool checkSelectionStatus = false)
    {
        if ( checkSelectionStatus && row.isSelected())
        {
            return PafColor(SALA_SELECTED_COLOR);
        }

        PafColor color;
        return color.makeColor(tableView.getNormalisedValue(key, row), tableView.getDisplayParams());

    }

}
