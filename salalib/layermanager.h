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

#pragma once;
#include <string>

class LayerManager
{
public:
    typedef int64_t KeyType;
    virtual size_t addLayer(const std::string &layerName) = 0;
    virtual const std::string& getLayerName(size_t index) const = 0;
    virtual size_t getLayerIndex(const std::string &layerName) const = 0;
    virtual void setLayerVisible( size_t layerIndex, bool visible = true ) = 0;
    virtual bool isLayerVisible( size_t layerIndex ) const = 0;

    virtual KeyType getKey(size_t layerIndex) const = 0;
    virtual bool isVisible( const KeyType &key ) const = 0;

    virtual ~LayerManager(){}

public:
    class OutOfLayersException : std::exception
    {
    public:
        OutOfLayersException()
        {}
        OutOfLayersException(const std::string &message) : std::exception(message.c_str())
        {
        }
    };

    class DuplicateKeyException : std::exception
    {
    public:
        DuplicateKeyException()
        {}
        DuplicateKeyException(const std::string &message) : std::exception(message.c_str())
        {
        }
    };

};

