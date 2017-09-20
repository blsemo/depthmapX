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

#include "layermanagerimpl.h"

LayerManagerImpl::LayerManagerImpl() : m_visibleLayers(1)
{
    m_layers.push_back("Everything");
    m_layerLookup["Everything"] = 0;
}

size_t LayerManagerImpl::addLayer(const std::string &layerName)
{
    size_t newLayerIndex = m_layers.size();
    if (newLayerIndex > 63)
    {
        throw OutOfLayersException();
    }
    auto result = m_layerLookup.insert(std::make_pair(layerName, newLayerIndex));
    if (!result.second)
    {
        throw DuplicateKeyException(std::string("Trying to insert duplicate key: ") + layerName);
    }
    m_layers.push_back(layerName);
    return newLayerIndex;
}

const std::string& LayerManagerImpl::getLayerName(size_t index) const
{
    checkIndex(index);
    return m_layers[index];
}

size_t LayerManagerImpl::getLayerIndex(const std::string &layerName) const
{
    auto iter = m_layerLookup.find(layerName);
    if ( iter == m_layerLookup.end())
    {
        throw std::out_of_range("Unknown layer name");
    }
    return iter->second;
}

void LayerManagerImpl::setLayerVisible(size_t layerIndex, bool visible)
{
    checkIndex(layerIndex);
    if (layerIndex == 0)
    {
        // this it the everything layer - if switching on just show everything, else switch everything off
        m_visibleLayers = visible ? 1 : 0;
        return;
    }
    int64_t layerValue = ((KeyType)1) << layerIndex;

    // if visible, switch on this layer and switch everything layer off, else just switch off this layer
    if (visible)
    {
        m_visibleLayers = (m_visibleLayers | layerValue) & (~0x1);
    }
    else
    {
        m_visibleLayers &= (~layerValue);
    }
}

bool LayerManagerImpl::isLayerVisible(size_t layerIndex) const
{
    checkIndex(layerIndex);
    return isVisible(getKey(layerIndex));
}

bool LayerManagerImpl::isVisible(const KeyType &key) const
{
    return (m_visibleLayers & key) != 0;
}


LayerManager::KeyType LayerManagerImpl::getKey(size_t layerIndex) const
{
    return ((int64_t)1) << layerIndex;
}

void LayerManagerImpl::checkIndex(size_t index) const
{
    if(index >= m_layers.size())
    {
        throw std::out_of_range("Invalid layer index");
    }
}

