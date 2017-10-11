#ifndef ATTRIBUTETABLEVIEW_H
#define ATTRIBUTETABLEVIEW_H

#include "attributetable.h"
#include "attributetableindex.h"

template<typename TKey>
class AttributeTableView
{
public:
    AttributeTableView(const dXreimpl::AttributeTable<TKey>& table );

    const dXreimpl::AttributeTable<TKey> &m_table;

    // columnIndex < 0 -> not set
    void setDisplayColumn(int columnIndex);
    int getDisplayColumn() const{ return m_displayColumn;}

    typedef std::vector<dXreimpl::ConstAttributeIndexItem<TKey>> ConstIndex;
    const ConstIndex& getIndex(){return m_index;}

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

#endif // ATTRIBUTETABLEVIEW_H
