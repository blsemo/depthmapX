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

        virtual void updateStats(float val, float oldVal = 0.0f) = 0;

        virtual ~AttributeColumn(){}

    };


    class AttributeColumnManager
    {
    public:
        virtual size_t getNumColumns() const = 0;
        virtual size_t getColumnIndex(const std::string& name) const = 0;
        virtual AttributeColumn& getColumn(size_t index) const = 0;
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

        virtual void updateStats(float val, float oldVal = 0.0f);

    public:
        AttributeColumnStats m_stats;

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


    template<class RowKeyType> class AttributeTable : public AttributeColumnManager
    {
    public:
        AttributeTable();
        AttributeRow& getRow(const RowKeyType& key );
        const AttributeRow& getRow(const RowKeyType& key) const;
        size_t insertOrResetColumn(const std::string& columnName);
        size_t insertOrResetLockedColumn(const std::string& columnName);
        size_t getOrInsertColumn(const std::string& columnName);
        size_t getOrInsertLockedColumn(const std::string& columnName);
        void removeRow();
        void removeColumn();
        void renameColumn();

    private:
        std::map<RowKeyType, AttributeRow> m_rows;
        std::map<std::string, size_t> m_columnMapping;
        std::vector<AttributeColumnImpl> m_columns;

   // interface AttributeColumnManager
    public:
        virtual size_t getColumnIndex(const std::string& name) const;
        virtual AttributeColumn& getColumn(size_t index) const;
        virtual const std::string& getColumnName(size_t index) const;
        virtual size_t getNumColumns() const;
    };


    template<class RowKeyType>
    AttributeTable<RowKeyType>::AttributeTable()
    {

    }

    template<class RowKeyType>
    AttributeRow &AttributeTable<RowKeyType>::getRow(const dXreimpl::RowKeyType &key)
    {

    }

    template<class RowKeyType>
    const AttributeRow& AttributeTable<RowKeyType>::getRow(const dXreimpl::RowKeyType &key) const
    {

    }

}

