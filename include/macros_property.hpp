// Copyright 2023 David SPORN
// ---
// This file is part of 'the clock by sporniket -- ESP32/IDF firmware'.
// ---
// 'the clock by sporniket -- ESP32/IDF firmware' is free software: you can
// redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// 'the clock by sporniket -- ESP32/IDF firmware' is distributed in the hope
// that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'the clock by sporniket -- ESP32/IDF firmware'. If not, see
// <https://www.gnu.org/licenses/>. 
#ifndef MACROS_PROPERTY_HPP
#define MACROS_PROPERTY_HPP

// ========[ Code generator macros ]========
// generate property (pointer to given type) with private storage and fluent
// setter. accName should be capitalized, e.g. property 'foo' will have a 'Foo'
// as accessor name to get 'withFoo(...)' Call this macro for each property
// BEFORE your 'private:' section
#define PROPERTY(of, type, accName)                                            \
private:                                                                       \
  type *my##accName = nullptr;                                                 \
                                                                               \
public:                                                                        \
  of *with##accName(type *a##accName) {                                        \
    my##accName = a##accName;                                                  \
    return this;                                                               \
  }                                                                            \
                                                                               \
  bool has##accName() { return nullptr != my##accName; }

#endif