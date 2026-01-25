/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef STRING_REF_SSO_H
#define STRING_REF_SSO_H

#include <iostream>
#include <string.h>
#include <string>
#include <string_view>

namespace PJ
{
/**
 * @brief A fully owning string class.
 * Replaces the old optimization logic with safety.
 * Always allocates on the heap and owns its data.
 */
class StringRef
{
private:
  char* _data;
  size_t _size;

public:
  // We return true here to tell consumers (like StringSeries) that
  // this object manages its own memory and shouldn't be de-duplicated
  // into an external storage set.
  bool isSSO() const
  {
    return true;
  }

  // --- Constructors ---

  StringRef() : _data(nullptr), _size(0)
  {
  }

  StringRef(const std::string& str) : StringRef(str.data(), str.size())
  {
  }

  StringRef(const char* str) : StringRef(str, str ? strlen(str) : 0)
  {
  }

  StringRef(const std::string_view& str) : StringRef(str.data(), str.size())
  {
  }

  explicit StringRef(const char* data_ptr, size_t length)
  {
    if (data_ptr && length > 0)
    {
      _size = length;
      _data = new char[_size + 1]; // +1 for null terminator safety
      memcpy(_data, data_ptr, _size);
      _data[_size] = '\0';
    }
    else
    {
      _data = nullptr;
      _size = 0;
    }
  }

  // --- Rule of Five (Required for manual memory management) ---

  ~StringRef()
  {
    delete[] _data;
  }

  // Copy Constructor
  StringRef(const StringRef& other)
  {
    if (other._data)
    {
      _size = other._size;
      _data = new char[_size + 1];
      memcpy(_data, other._data, _size + 1);
    }
    else
    {
      _data = nullptr;
      _size = 0;
    }
  }

  // Copy Assignment
  StringRef& operator=(const StringRef& other)
  {
    if (this != &other)
    {
      delete[] _data;
      if (other._data)
      {
        _size = other._size;
        _data = new char[_size + 1];
        memcpy(_data, other._data, _size + 1);
      }
      else
      {
        _data = nullptr;
        _size = 0;
      }
    }
    return *this;
  }

  // Move Constructor (Efficiency)
  StringRef(StringRef&& other) noexcept : _data(other._data), _size(other._size)
  {
    other._data = nullptr;
    other._size = 0;
  }

  // Move Assignment (Efficiency)
  StringRef& operator=(StringRef&& other) noexcept
  {
    if (this != &other)
    {
      delete[] _data;
      _data = other._data;
      _size = other._size;
      other._data = nullptr;
      other._size = 0;
    }
    return *this;
  }

  // --- Accessors ---

  const char* data() const
  {
    return _data;
  }

  size_t size() const
  {
    return _size;
  }
};

}  // namespace PJ

#endif  // STRING_REF_SSO_H
