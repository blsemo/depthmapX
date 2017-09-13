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
#include <string>
#include <map>
#include <vector>
#include <memory>


namespace dXreimpl
{
    class AttributeRow
    {
    public:
        virtual float getValue(const std::string &column) const = 0;
        virtual float getValue(size_t index) const = 0;
        virtual void setValue(const std::string &column, float value ) = 0;
        virtual void setValue(size_t index, float value) = 0;

        virtual ~AttributeRow(){}
    };


    struct AttributeColumnStats
    {


        AttributeColumnStats( double minimum, double maximum, double tot, double vTot, double vMin, double vMax ): min(minimum), max(maximum), total(tot), visibleTotal(vTot), visibleMin(vMin), visibleMax(vMax)
        {}

        AttributeColumnStats() : AttributeColumnStats(-1.0, -1.0, -1.0, -1.0, -1.0, -1.0)
        {
        }

        double min;
        double max;
        double total;
        double visibleTotal;
        double visibleMin;
        double visibleMax;

    };



    class AttributeColumn
    {
    public:
        virtual const std::string& getName() const  = 0;
        virtual bool isLocked() const = 0;
        virtual void setLock(bool lock) = 0;
        virtual bool isHidden() const = 0;
        virtual void setHidden(bool hidden) = 0;
        virtual const std::string& getFormula() const = 0;

        virtual const AttributeColumnStats& getStats() const = 0;

        // stats are mutable - we need to be able to update them all the time,
        // even when not allowed to modify the column settings
        virtual void updateStats(float val, float oldVal = 0.0f) const = 0;

        virtual ~AttributeColumn(){}

    };


    class AttributeColumnManager
    {
    public:
        virtual size_t getNumColumns() const = 0;
        virtual size_t getColumnIndex(const std::string& name) const = 0;
        virtual const AttributeColumn& getColumn(size_t index) const = 0;
        virtual const std::string& getColumnName(size_t index) const = 0;
    };



    // Implementation of AttributeColumn

    class AttributeColumnImpl: public AttributeColumn, AttributeColumnStats
    {


        // AttributeColumn interface
    public:
        AttributeColumnImpl(const std::string &name, const std::string &formula = std::string()) : m_name(name), m_locked(false), m_hidden(false), m_formula(formula)
        {
        }

        virtual const std::string &getName() const;
        virtual bool isLocked() const;
        virtual void setLock(bool lock);
        virtual bool isHidden() const;
        virtual void setHidden(bool hidden);
        virtual const std::string &getFormula() const;
        virtual const AttributeColumnStats& getStats() const;

        virtual void updateStats(float val, float oldVal = 0.0f) const;

    public:
        // stats are mutable - we need to be able to update them all the time,
        // even when not allowed to modify the column settings
        mutable AttributeColumnStats m_stats;

        void setName(const std::string &name);

    private:
        std::string m_name;
        bool m_locked;
        bool m_hidden;
        std::string m_formula;

    };



    class AttributeRowImpl : public AttributeRow
    {
    public:
        AttributeRowImpl(const AttributeColumnManager& colManager) : m_data(colManager.getNumColumns(), -1.0), m_colManager(colManager)
        {
        }

        // AttributeRow interface
    public:
        virtual float getValue(const std::string &column) const;
        virtual float getValue(size_t index) const;
        virtual void setValue(const std::string &column, float value);
        virtual void setValue(size_t index, float value);

        void addColumn();
        void removeColumn(size_t index);

    private:
        std::vector<float> m_data;
        const AttributeColumnManager& m_colManager;

        void checkIndex(size_t index) const;
    };

    ///
    /// \brief Small struct to make a serialised pixel ref distinguishable from an int
    /// PixelRefs are serialised into an int (2 bytes x, 2 bytes y) for historic reason. This seems dangerous
    /// and confusing as these are by no means inices, but look the same to the compiler and the reader.
    /// This struct should disambiguate this...
    ///
    struct SerialisedPixelRef
    {
        SerialisedPixelRef(int val) : value(val)
        {}
        const int value;

        bool operator < (const SerialisedPixelRef& other ) const
        {
            return value < other.value;
        }
    };

    template<class RowKeyType> class AttributeTable : public AttributeColumnManager
    {
    public:
        AttributeTable();
        AttributeRow& getRow(const RowKeyType& key );
        const AttributeRow& getRow(const RowKeyType& key) const;
        AttributeRow &addRow(const RowKeyType& key);
        size_t insertOrResetColumn(const std::string& columnName, const std::string &formula = std::string());
        size_t insertOrResetLockedColumn(const std::string& columnName, const std::string &formula = std::string());
        size_t getOrInsertColumn(const std::string& columnName, const std::string &formula = std::string());
        size_t getOrInsertLockedColumn(const std::string& columnName, const std::string &formula = std::string());
        void removeRow(const RowKeyType& key);
        void removeColumn(size_t colIndex);
        void renameColumn(const std::string& oldName, const std::string& newName);

    private:
        std::map<RowKeyType, std::unique_ptr<AttributeRowImpl>> m_rows;
        std::map<std::string, size_t> m_columnMapping;
        std::vector<AttributeColumnImpl> m_columns;

   // interface AttributeColumnManager
    public:
        virtual size_t getColumnIndex(const std::string& name) const;
        virtual const AttributeColumn& getColumn(size_t index) const;
        virtual const std::string& getColumnName(size_t index) const;
        virtual size_t getNumColumns() const;

    private:
        void checkColumnIndex(size_t index) const;
        size_t addColumnInternal(const std::string &name, const std::string &formula);
    };


    template<class RowKeyType>
    AttributeTable<RowKeyType>::AttributeTable()
    {

    }

    template<class RowKeyType>
    AttributeRow &AttributeTable<RowKeyType>::getRow(const RowKeyType &key)
    {
        auto iter = m_rows.find(key);
        if (iter == m_rows.end())
        {
            throw std::out_of_range("Invalid row key");
        }
        return *iter->second;
    }

    template<class RowKeyType>
    const AttributeRow& AttributeTable<RowKeyType>::getRow(const RowKeyType &key) const
    {
        auto iter = m_rows.find(key);
        if (iter == m_rows.end())
        {
            throw std::out_of_range("Invalid row key");
        }
        return *iter->second;
    }

    template<class RowKeyType>
    AttributeRow &AttributeTable<RowKeyType>::addRow(const RowKeyType &key)
    {
        auto iter = m_rows.find(key);
        if (iter != m_rows.end())
        {
            throw new std::invalid_argument("Duplicate key");
        }
        m_rows[key] = std::unique_ptr<AttributeRowImpl>(new AttributeRowImpl(*this));
        return *m_rows[key];
    }

    template<class RowKeyType>
    size_t AttributeTable<RowKeyType>::insertOrResetColumn(const std::string &columnName, const std::string &formula)
    {
        auto iter = m_columnMapping.find(columnName);
        if (iter == m_columnMapping.end())
        {
            return addColumnInternal(columnName, formula);
        }

        // it exists - we need to reset it
        m_columns[iter->second].m_stats = AttributeColumnStats();
        m_columns[iter->second].setLock(false);
        for (auto& row : m_rows)
        {
            row.second->setValue(iter->second, -1.0f);
        }
        return iter->second;
    }

    template<class RowKeyType>
    size_t AttributeTable<RowKeyType>::insertOrResetLockedColumn(const std::string &columnName, const std::string &formula)
    {
        size_t index = insertOrResetColumn(columnName, formula);
        m_columns[index].setLock(true);
        return index;
    }

    template<class RowKeyType>
    size_t AttributeTable<RowKeyType>::getOrInsertColumn(const std::string &columnName, const std::string &formula)
    {
        auto iter = m_columnMapping.find(columnName);
        if ( iter != m_columnMapping.end())
        {
            return iter->second;
        }
        return addColumnInternal(columnName, formula);
    }

    template<class RowKeyType>
    size_t AttributeTable<RowKeyType>::getOrInsertLockedColumn(const std::string &columnName, const std::string &formula)
    {
        size_t index = getOrInsertColumn(columnName, formula);
        m_columns[index].setLock(true);
        return index;
    }

    template<class RowKeyType>
    void AttributeTable<RowKeyType>::removeColumn(size_t colIndex)
    {
        checkColumnIndex(colIndex);
        const std::string& name = m_columns[colIndex].getName();
        auto iter = m_columnMapping.find(name);
        m_columnMapping.erase(iter);
        for (auto& elem : m_columnMapping)
        {
            if (elem.second > colIndex)
            {
                elem.second--;
            }
        }
        m_columns.erase(m_columns.begin()+colIndex);
        for (auto& row : m_rows)
        {
            row.second->removeColumn(colIndex);
        }
    }

    template<class RowKeyType>
    void AttributeTable<RowKeyType>::renameColumn(const std::string &oldName, const std::string &newName)
    {
        auto iter = m_columnMapping.find(oldName);
        if (iter == m_columnMapping.end())
        {
            throw std::out_of_range("Invalid column name");
        }

        size_t colIndex = iter->second;
        m_columns[colIndex].setName(newName);
        m_columnMapping.erase(iter);
        m_columnMapping[newName] = colIndex;

    }

    template<class RowKeyType>
    size_t AttributeTable<RowKeyType>::getColumnIndex(const std::string &name) const
    {
        auto iter = m_columnMapping.find(name);
        if (iter == m_columnMapping.end())
        {
            std::stringstream message;
            message << "Unknown column name " << name;
            throw std::out_of_range(message.str());
        }
        return iter->second;

    }

    template<class RowKeyType>
    const AttributeColumn &AttributeTable<RowKeyType>::getColumn(size_t index) const
    {
        checkColumnIndex(index);
        return m_columns[index];
    }

    template<class RowKeyType>
    const std::string &AttributeTable<RowKeyType>::getColumnName(size_t index) const
    {
        checkColumnIndex(index);
        return m_columns[index].getName();
    }

    template<class RowKeyType>
    size_t AttributeTable<RowKeyType>::getNumColumns() const
    {
        return m_columns.size();
    }

    template<class RowKeyType>
    void AttributeTable<RowKeyType>::checkColumnIndex(size_t index) const
    {
        if (index >= m_columns.size())
        {
            throw std::out_of_range("ColumnIndex out of range");
        }
    }

    template<class RowKeyType>
    size_t AttributeTable<RowKeyType>::addColumnInternal(const std::string &name, const std::string &formula)
    {
        size_t colIndex = m_columns.size();
        m_columns.push_back(AttributeColumnImpl(name, formula));
        m_columnMapping[name] = colIndex;
        for (auto& elem : m_rows)
        {
            elem.second->addColumn();
        }
        return colIndex;
    }


}

