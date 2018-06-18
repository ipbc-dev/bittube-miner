#ifndef DOUBLE_INTEGER_HPP
#define DOUBLE_INTEGER_HPP

/* Double Integer template
// double_integer.hpp
//
// Author: Jan Ringos, http://Tringi.TrimCore.cz, tringi@trimcore.cz
// Version: 2.0
// Description: Implements common functionality to double the precission
//              of an little-endian two-complement integer type, both signed
//              and unsigned.
//
// License:
//   This software is provided 'as-is', without any express or implied warranty.
//   In no event will the author be held liable for any damages arising from the
//   use of this software. 
// 
//   Permission is granted to anyone to use this software for any purpose,
//   including commercial applications, and to alter it and redistribute it
//   freely, subject to the following restrictions: 
//
//       1. The origin of this software must not be misrepresented; you must not
//          claim that you wrote the original software. If you use this software
//          in a product, an acknowledgment in the product documentation would
//          be appreciated but is not required.
//
//       2. Altered source versions must be plainly marked as such, and must not
//          be misrepresented as being the original software.
//
//       3. This notice may not be removed or altered from any source
//          distribution.
//
// Changelog:
//      29.01.2011 - initial version
//      28.05.2013 - now requires C++11 (to enable singed/unsigned conversions)
//                 - fixed conversion from negative floating point
//                 - C style fmod, fmodf, fmodl replaced by std::fmod
//      20.03.2015 - completed C++11 release
*/

#include <type_traits>
#include <cstddef>
#include <cstdio>
#include <limits>

// double_integer <LO, HI>
//  - unlike the intrinsic types, this one is always zero initialized
//  - arguments: LO - always an unsigned integer type
//               HI - defaults to LO, type of the upper half
//  - notes: - use of types with different width for LO and HI is unsupported
//           - to implement a signed type, use a signed integer as HI type

template <typename LO, typename HI = LO>
class double_integer {
    
    // check assumptions
    static_assert (sizeof (LO) == sizeof (HI), "both types must have the same size");
    static_assert (!std::numeric_limits <LO> ::is_signed, "type of lower half must be unsigned");
    
    public:
        // the only two data members
        
        LO lo;
        HI hi;
    
        // constructors
        double_integer() = default;
        constexpr double_integer (const LO & _lo, const HI & _hi) : lo (_lo), hi (_hi) {};
/*
        constexpr double_integer () : lo (LO ()), hi (HI ()) {};        
        constexpr double_integer (const double_integer & a) : lo (a.lo), hi (a.hi) {};
*/

        double_integer (signed char);
        double_integer (signed short);
        double_integer (signed int);
        double_integer (signed long);
        double_integer (signed long long);
        
        double_integer (unsigned char);
        double_integer (unsigned short);
        double_integer (unsigned int);
        double_integer (unsigned long);
        double_integer (unsigned long long);
        
        double_integer (float);
        double_integer (double);
        double_integer (long double);

        // string constructor
        //  - skips std::isspace characters at string beginning
        //  - when radix is zero, is detected by following rules:
        //     "0" -> 8, "0x" -> 16, "0b" -> 2, otherwise 10
        
        // explicit double_integer (const char *, unsigned int radix = 0);
        
        // assignment operators
        //  - only between double_integers
        //    and unsigned version of this one (if this one is signed)
        
        double_integer & operator = (const double_integer &);

        template <class = std::enable_if <!std::is_same <LO, HI>::value>>
        double_integer & operator = (const double_integer <LO, LO> &);
        
        // integer operators
    
        constexpr bool operator ! () const;
        constexpr explicit operator bool () const;
        constexpr operator double_integer <LO, LO> () const;
        
        double_integer operator + () const { return *this; };
        double_integer operator - () const;
        double_integer operator ~ () const;

        double_integer operator ++ (int);
        double_integer operator -- (int);
        double_integer & operator ++ ();
        double_integer & operator -- ();

        double_integer & operator += (const double_integer &);
        double_integer & operator -= (const double_integer &);
        double_integer & operator *= (const double_integer &);
        double_integer & operator /= (const double_integer &);
        double_integer & operator %= (const double_integer &);

        double_integer & operator |= (const double_integer &);
        double_integer & operator &= (const double_integer &);
        double_integer & operator ^= (const double_integer &);

        double_integer & operator >>= (unsigned int);
        double_integer & operator <<= (unsigned int);

        // division
        //  - returns result, second parameters (if provided) receives remainder
        //  - throws std::logic_error on division by zero

        double_integer div (const double_integer &, double_integer * = 0) const;
    
        // rendering
        
        const char * print (char * buffer, std::size_t length,
                            unsigned int radix = 10u) const;
        const char * c_str (unsigned int radix = 10u) const; // thread unsafe!

    // internal implementation details and helpers follow
    private:
        template <typename T> static T mask (std::size_t);
    
};

template <typename LO, typename HI> bool operator < (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> bool operator > (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> bool operator <= (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> bool operator <= (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> bool operator == (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> bool operator != (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> bool operator || (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> bool operator && (const double_integer <LO, HI> &, const double_integer <LO, HI> &);

template <typename LO, typename HI> double_integer <LO, HI> operator + (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> double_integer <LO, HI> operator - (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> double_integer <LO, HI> operator * (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> double_integer <LO, HI> operator / (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> double_integer <LO, HI> operator % (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> double_integer <LO, HI> operator & (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> double_integer <LO, HI> operator | (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> double_integer <LO, HI> operator ^ (const double_integer <LO, HI> &, const double_integer <LO, HI> &);
template <typename LO, typename HI> double_integer <LO, HI> operator << (const double_integer <LO, HI> &, unsigned int);
template <typename LO, typename HI> double_integer <LO, HI> operator >> (const double_integer <LO, HI> &, unsigned int);

#include "xmrstak/misc/double_integer.tcc"
#endif
