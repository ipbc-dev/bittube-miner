#ifndef DOUBLE_INTEGER_TCC
#define DOUBLE_INTEGER_TCC

/* Double Integer template
// double_integer.tcc
//
// Author: Jan Ringos, http://Tringi.TrimCore.cz, tringi@trimcore.cz
// Version: 2.0
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
//      28.05.2013 - fixed conversion from negative floating point
//                 - C style fmod, fmodf, fmodl replaced by std::fmod
//      20.03.2015 - completed C++11 release
*/

#include <climits>
#include <cstring>
#include <cctype>
#include <cmath>

#include <limits>
#include <stdexcept>

template <typename LO, typename HI>
constexpr double_integer <LO, HI> ::operator bool () const {
    return !!this->hi || !!this->lo;
};

template <typename LO, typename HI>
bool constexpr
double_integer <LO, HI> ::operator ! () const {
    return !bool(*this);
};

template <typename LO, typename HI>
double_integer <LO, HI> 
double_integer <LO, HI> ::operator - () const {
    double_integer <LO, HI> result = ~*this;
    ++result;
    return result;
};

template <typename LO, typename HI>
double_integer <LO, HI>
double_integer <LO, HI> ::operator ~ () const {
    return double_integer <LO, HI> (~this->lo, ~this->hi);
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator ++ () {
    ++this->lo;
    if (!this->lo) {
        ++this->hi;
    };
    return *this;
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator -- () {
    if (!this->lo) {
        --this->hi;
    };
    --this->lo;
    return *this;
};

template <typename LO, typename HI>
double_integer <LO, HI>
double_integer <LO, HI> ::operator ++ (int) {
    double_integer <LO, HI> previous = *this;
    ++*this;
    return previous;
};

template <typename LO, typename HI>
double_integer <LO, HI>
double_integer <LO, HI> ::operator -- (int) {
    double_integer <LO, HI> previous = *this;
    --*this;
    return previous;
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator += (const double_integer <LO, HI> & addend) {
    LO previous_lo = this->lo;

    this->lo += addend.lo;
    this->hi += addend.hi;
    this->hi += (this->lo < previous_lo);
    
    return *this;
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator -= (const double_integer <LO, HI> & subtrahend) {
    return *this += -subtrahend;
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator *= (const double_integer <LO, HI> & _multiplier) {
    if (!_multiplier) {
        *this = double_integer <LO, HI> ();
        return *this;
    };

    double_integer <LO, HI> previous = *this;
    double_integer <LO, HI> multiplier = _multiplier;
    
    this->lo = LO ();
    this->hi = HI ();
    
    std::size_t i = 0u;
    std::size_t n = CHAR_BIT * (sizeof (LO) + sizeof (HI));
    
    while (i < n && !!multiplier) {
        if (!!(multiplier.lo & LO (1u))) {
            *this += previous << i;
        };
        
        multiplier >>= 1u;
        ++i;
    };
    
    return *this;
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator /= (const double_integer <LO, HI> & divisor) {
    *this = this->div (divisor);
    return *this;
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator %= (const double_integer <LO, HI> & divisor) {
    this->div (divisor, this);
    return *this;
};

template <typename LO, typename HI>
double_integer <LO, HI>
double_integer <LO, HI> ::div (const double_integer <LO, HI> & _ds,
                               double_integer <LO, HI> * remainder) const {
    
//    if (!_ds)
//        return this->lo / _ds.lo;
    
    if (!_ds)
        throw std::logic_error ("div/0");
    
    // reset sign bits
    
    double_integer <LO, HI> zero(0, 0);
    double_integer <LO, HI> ds = _ds;
    double_integer <LO, HI> dd = *this;
    
    if (ds < zero) {
        ds = -ds;
    };
    if (dd < zero) {
        dd = -dd;
    };

    // remainder only
    //  - shortcut, return zero
    
    if (dd < ds) {
        if (remainder) {
            *remainder = *this;
        };
        return zero;
    };
    
    double_integer <LO, HI> r(0, 0);
    double_integer <LO, HI> q(0, 0);
    double_integer <LO, HI> one(0, 0);
    ++one;
    
    std::size_t b = CHAR_BIT * (sizeof (LO) + sizeof (HI));
    
    while (r < ds) {
        r <<= 1u;
        if (!!(dd & (one << --b))) {
            r.lo |= one.lo;
        };
    };
    
    while (true) {
        if (r < ds) {
            if (!b) break;
            
            --b;
            r <<= 1u;

            if (!!(dd & (one << b))) {
                r.lo |= one.lo;
            };
        } else {
            r -= ds;
            q |= one << b;
        };
    };

    // correct sign bits
    
    if ((_ds < zero) ^ (*this < zero)) {
        q = -q;
    };
    
    // results

    if (remainder) {
        if (*this < zero) {
            r = -r;
        };
        *remainder = r;
    };
    return q;
    
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator <<= (unsigned int bits) {

    // discard upper bits of the shift count, just like shift for ints does
    bits &= (CHAR_BIT * (sizeof (LO) + sizeof (HI))) - 1u;
    
    if (bits >= CHAR_BIT * sizeof (LO)) {
        bits -= CHAR_BIT * sizeof (LO);
        
        this->hi = this->lo;
        this->lo = LO ();
        this->hi <<= bits;
        
    } else {
        const unsigned int rest = CHAR_BIT * sizeof (LO) - bits;
        
        this->hi = (LO (this->hi) << bits)
                 | ((this->lo & (this->mask <LO> (bits) << rest)) >> rest);
        this->lo <<= bits;
    };
    
    return *this;
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator >>= (unsigned int bits) {
    
    // discard upper bits of the shift count, just like shift for ints does
    bits &= (CHAR_BIT * (sizeof (LO) + sizeof (HI))) - 1u;
    
    if (bits >= CHAR_BIT * sizeof (LO)) {
        bits -= CHAR_BIT * sizeof (LO);
        
        this->lo = this->hi;
        this->lo >>= bits;
        
        if (std::numeric_limits <HI> ::is_signed
                && double_integer (LO (), this->hi) < double_integer ()) {
            
            this->hi = ~HI ();
            this->lo |= this->mask <LO> (bits)
                    << (CHAR_BIT * sizeof (LO) - bits);
        } else {
            this->hi = HI ();
        };

    } else {
        this->lo >>= bits;
        this->lo |= (LO (this->hi) & this->mask <LO> (bits))
                 << (CHAR_BIT * sizeof (LO) - bits);
        this->hi >>= bits;
    };
    
    return *this;
};

template <typename LO, typename HI>
template <typename T>
T double_integer <LO, HI> ::mask (std::size_t n) {
    T m = T ();
    
    ++m;
    m <<= n;
    --m;
    
    return m;
};

template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (unsigned char a)
    :   lo (LO (a)),
        hi (HI ()) {};
        
template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (unsigned short a)
    :   lo (LO (a)),
        hi (sizeof (a) > sizeof (LO) ? (a >> (sizeof (LO) * CHAR_BIT)) : HI ()) {};
        
template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (unsigned int a)
    :   lo (LO (a)),
        hi (sizeof (a) > sizeof (LO) ? (a >> (sizeof (LO) * CHAR_BIT)) : HI ()) {};
        
template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (unsigned long a)
    :   lo (LO (a)),
        hi (sizeof (a) > sizeof (LO) ? (a >> (sizeof (LO) * CHAR_BIT)) : HI ()) {};
        
template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (unsigned long long a)
    :   lo (LO (a)),
        hi (sizeof (a) > sizeof (LO) ? (a >> (sizeof (LO) * CHAR_BIT)) : HI ()) {};

template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (signed char a)
    :   lo (LO (a)),
        hi (HI ((a < 0) ? -1 : 0)) {};

template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (signed short a)
    :   lo (LO (a)),
        hi (sizeof (a) > sizeof (LO)
                ? HI (a >> (sizeof (LO) * CHAR_BIT))
                : HI (a < 0 ? -1 : 0)) {};

template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (signed int a)
    :   lo (LO (a)),
        hi (sizeof (a) > sizeof (LO)
                ? HI (a >> (sizeof (LO) * CHAR_BIT))
                : HI (a < 0 ? -1 : 0)) {};

template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (signed long a)
    :   lo (LO (a)),
        hi (sizeof (a) > sizeof (LO)
                ? HI (a >> (sizeof (LO) * CHAR_BIT))
                : HI (a < 0 ? -1 : 0)) {};

template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (signed long long a)
    :   lo (LO (a)),
        hi (sizeof (a) > sizeof (LO)
                ? HI (a >> (sizeof (LO) * CHAR_BIT))
                : HI (a < 0 ? -1 : 0)) {};


template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (float a)
    :   lo (std::fmod (a,      static_cast <float> (LO (~LO ())) + 1.0f)),
        hi ((a < 0.0f) ? (a / (static_cast <float> (LO (~LO ())) + 1.0f) - 1u)
                       : (a / (static_cast <float> (LO (~LO ())) + 1.0f))) {};

template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (double a)
    :   lo (std::fmod (a,     static_cast <double> (LO (~LO ())) + 1.0)),
        hi ((a < 0.0) ? (a / (static_cast <double> (LO (~LO ())) + 1.0) - 1u)
                      : (a / (static_cast <double> (LO (~LO ())) + 1.0))) {};

template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (long double a)
    :   lo (std::fmod (a,      static_cast <long double> (LO (~LO ())) + 1.0l)),
        hi ((a < 0.0l) ? (a / (static_cast <long double> (LO (~LO ())) + 1.0l) - 1u)
                       : (a / (static_cast <long double> (LO (~LO ())) + 1.0l))) {};

/*
template <typename LO, typename HI>
double_integer <LO, HI> ::double_integer (const char * s, unsigned int _radix)
    :   lo (LO ()),
        hi (HI ()) {
    
    if (!s)
        return;
    
    double_integer <LO, HI> digit(0, 0);
    double_integer <LO, HI> radix(0, 0);
    bool minus = false;
    
    // skip blanks to first character
    while (std::isspace (*s)) ++s;

    if (*s == '-') {
        ++s;
        minus = true;
        
        // skip optional spaces after minus
        while (std::isspace (*s)) ++s;
    };
    
    // if no radix was provided, autodetect
    if (_radix) {
        radix.lo = _radix;
    } else {
        if (*s == '0') {
            ++s;
            radix.lo = 8u;
            
            if (*s == 'x' || *s == 'X') {
                ++s;
                radix.lo = 16u;
            };
            if (*s == 'b' || *s == 'B') {
                ++s;
                radix.lo = 2u;
            };
        } else {
            radix.lo = 10u;
        };
    };
    
    while (std::isxdigit (*s)) {
        if (std::isdigit (*s)) {
            digit.lo = static_cast <unsigned int> (*s - '0');
        } else
        if (std::islower (*s)) {
            digit.lo = static_cast <unsigned int> (*s - 'a' + 10u);
        } else {
            digit.lo = static_cast <unsigned int> (*s - 'A' + 10u);
        };
        
        *this *= radix;
        *this += digit;
        ++s;
    };
    
    if (minus) {
        *this = -*this;
    };
    return;
};
*/

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator = (const double_integer <LO, HI> & x) {
    this->lo = x.lo;
    this->hi = x.hi;
    return *this;
};

template <typename LO, typename HI>
template <class>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator = (const double_integer <LO, LO> & x) {
    this->lo = x.lo;
    this->hi = x.hi;
    return *this;
};

template <typename LO, typename HI>
constexpr
double_integer <LO, HI> ::operator double_integer <LO, LO> () const {
    return double_integer <LO, LO> (this->lo, this->hi);
};

template <typename LO, typename HI>
const char * double_integer <LO, HI> ::print (char * buffer, std::size_t length,
                                              unsigned int radix) const {
    if (!*this)
        return "0";

    if (radix < 2 || radix > 37)
        return "(inv radix)";

    if (length) {
        std::size_t i = length;
        buffer [--i] = '\0';

        double_integer <LO, HI> zero(0, 0);
        double_integer <LO, HI> r(0, 0);
        double_integer <LO, HI> x = *this;
        
        if (x < zero) {
            x = -x;
            if (x < zero) {
                
                buffer [0] = '-';
                double_integer <LO, LO>
                    (this->lo, this->hi) .print (buffer + 1, length - 1, radix);
                
                return buffer;
            };
        };
        
        while (i && !!x) {
            x = x.div (double_integer <LO, HI> (radix), &r);
            
            unsigned char c = *reinterpret_cast <unsigned char *> (&r);
            if (c < 10u)
                buffer [--i] = c + '0';
            else
                buffer [--i] = c + 'A' - 10u;
        };

        if (*this < zero) {
            if (i) {
                buffer [--i] = '-';
            } else
                return NULL;
        };
        
        if (!x) {
            std::memmove (buffer, buffer + i, length - i);
            return buffer;
        };
    };
    
    return NULL;
};

template <typename LO, typename HI>
const char * double_integer <LO, HI> ::c_str (unsigned int radix) const {
    static char sz [CHAR_BIT * (sizeof (LO) + sizeof (HI)) + 2u];
    return this->print (sz, sizeof sz, radix);
};

template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator |= (const double_integer <LO, HI> & param) {
    this->lo |= param.lo;
    this->hi |= param.hi;
    return *this;
};
template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator &= (const double_integer <LO, HI> & param) {
    this->lo &= param.lo;
    this->hi &= param.hi;
    return *this;
};
template <typename LO, typename HI>
double_integer <LO, HI> &
double_integer <LO, HI> ::operator ^= (const double_integer <LO, HI> & param) {
    this->lo ^= param.lo;
    this->hi ^= param.hi;
    return *this;
};

template <typename LO, typename HI>
bool operator < (const double_integer <LO, HI> & a,
                 const double_integer <LO, HI> & b) {
    if (a.hi == b.hi)
        return a.lo < b.lo;
    else
        return a.hi < b.hi;
};
template <typename LO, typename HI>
bool operator == (const double_integer <LO, HI> & a,
                  const double_integer <LO, HI> & b) {
    return a.lo == b.lo
        && a.hi == b.hi;
};
template <typename LO, typename HI>
bool operator != (const double_integer <LO, HI> & a,
                  const double_integer <LO, HI> & b) {
    return a.lo != b.lo
        || a.hi != b.hi;
};
template <typename LO, typename HI>
bool operator || (const double_integer <LO, HI> & a,
                  const double_integer <LO, HI> & b) {
    return (a.hi || a.lo)
        || (b.hi || b.lo);
};
template <typename LO, typename HI>
bool operator && (const double_integer <LO, HI> & a,
                  const double_integer <LO, HI> & b) {
    return (a.hi || a.lo)
        && (b.hi || b.lo);
};

// non-shortcut arithmetic operators implemented as global functions

template <typename LO, typename HI>
double_integer <LO, HI> operator + (const double_integer <LO, HI> & a,
                                    const double_integer <LO, HI> & b) {
    return double_integer <LO, HI> (a) += b;
};
template <typename LO, typename HI>
double_integer <LO, HI> operator - (const double_integer <LO, HI> & a,
                                    const double_integer <LO, HI> & b) {
    return double_integer <LO, HI> (a) -= b;
};
template <typename LO, typename HI>
double_integer <LO, HI> operator * (const double_integer <LO, HI> & a,
                                    const double_integer <LO, HI> & b) {
    return double_integer <LO, HI> (a) *= b;
};
template <typename LO, typename HI>
double_integer <LO, HI> operator / (const double_integer <LO, HI> & a,
                                    const double_integer <LO, HI> & b) {
    return double_integer <LO, HI> (a) /= b;
};
template <typename LO, typename HI>
double_integer <LO, HI> operator % (const double_integer <LO, HI> & a,
                                    const double_integer <LO, HI> & b) {
    return double_integer <LO, HI> (a) %= b;
};

template <typename LO, typename HI>
double_integer <LO, HI> operator << (const double_integer <LO, HI> & a,
                                     unsigned int n) {
    return double_integer <LO, HI> (a) <<= n;
};
template <typename LO, typename HI>
double_integer <LO, HI> operator >> (const double_integer <LO, HI> & a,
                                     unsigned int n) {
    return double_integer <LO, HI> (a) >>= n;
};

template <typename LO, typename HI>
double_integer <LO, HI> operator & (const double_integer <LO, HI> & a,
                                    const double_integer <LO, HI> & b) {
    return double_integer <LO, HI> (a) &= b;
};
template <typename LO, typename HI>
double_integer <LO, HI> operator | (const double_integer <LO, HI> & a,
                                    const double_integer <LO, HI> & b) {
    return double_integer <LO, HI> (a) |= b;
};
template <typename LO, typename HI>
double_integer <LO, HI> operator ^ (const double_integer <LO, HI> & a,
                                    const double_integer <LO, HI> & b) {
    return double_integer <LO, HI> (a) ^= b;
};

template <typename LO, typename HI>
bool operator >  (const double_integer <LO, HI> & a,
                  const double_integer <LO, HI> & b) { return  (b < a); };
template <typename LO, typename HI>
bool operator <= (const double_integer <LO, HI> & a,
                  const double_integer <LO, HI> & b) { return !(b < a); };
template <typename LO, typename HI>
bool operator >= (const double_integer <LO, HI> & a,
                  const double_integer <LO, HI> & b) { return !(a < b); };

#endif
