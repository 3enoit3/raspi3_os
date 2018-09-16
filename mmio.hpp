/*
 * Copyright (C) 2018
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef MMIO_20180916
#define MMIO_20180916

#include "tools.hpp"

// Registers
template <mystd::size_t Offset, typename Base = mystd::uint32_t>
struct Register {
    using base = Base;

    static decltype(auto) doc() { return "None"; }
    static constexpr decltype(auto) offset() { return Offset; }

    enum Type {
        RW,
        R,
        W
    };

    struct Field {
        constexpr Field(mystd::size_t pos, mystd::size_t size, Type type = RW, const char* doc = 0) : pos_(pos), size_(size), type_(type), doc_(doc) {}
        constexpr Field(mystd::size_t pos, Type type = RW, const char* doc = 0) : pos_(pos), size_(1), type_(type), doc_(doc) {}

        constexpr decltype(auto) pos() const { return pos_; }
        constexpr decltype(auto) size() const { return size_; }
        constexpr decltype(auto) type() const { return type_; }
        static constexpr decltype(auto) offset() { return Offset; }
        using base = Base;

        const mystd::size_t pos_;
        const mystd::size_t size_;
        const Type type_;
        const char* doc_;
    };
};

// Bit manipulation
template <typename T>
struct Bits {
    // Tools
    static constexpr mystd::size_t defsize(mystd::size_t size) {
        return size ? size : sizeof(T);
    }

    static constexpr T max(mystd::size_t size = 0) {
        return (T(1) << defsize(size)) - 1;
    }

    static constexpr T mask(mystd::size_t pos, mystd::size_t size) {
        return max(defsize(size)) << pos;
    }

    static constexpr T value(mystd::size_t pos, T value) {
        return value << pos;
    }

    // Access
    static T read(volatile T* at, mystd::size_t pos = 0, mystd::size_t size = 0) {
        return (*at >> pos) & max(defsize(size));
    }

    static void write(T val, volatile T* at, mystd::size_t pos = 0, mystd::size_t size = 0) {
        write(mask(pos, defsize(size)), value(pos, val), at);
    }

    static void write(T val, T mask, volatile T* at) {
        *at = (*at & ~mask) | val;
    }
};

// Fields
template <typename F> constexpr typename F::base value(const F& f, typename F::base v) {
    return Bits<typename F::base>::value(f.pos(), v);
};

template <typename F> constexpr typename F::base mask(const F& f) {
    return Bits<typename F::base>::value(f.pos(), f.size());
};

template <typename F> constexpr typename F::base read(const F& f) {
    return Bits<typename F::base>::read(reinterpret_cast<volatile typename F::base*>(f.offset()), f.pos(), f.size());
};

template <typename F> void write(const F& f, typename F::base v) {
    Bits<typename F::base>::write(v, reinterpret_cast<volatile typename F::base*>(f.offset()), f.pos(), f.size());
};

template <typename F> void set(const F& f) {
    write(f, Bits<typename F::base>::max(f.size()));
};

template <typename F> void clear(const F& f) {
    write(f, 0);
};

// Registers
template <typename R> void rset(typename R::base v) {
    *(reinterpret_cast<volatile typename R::base*>(R::offset())) = v;
};

template <typename R> void reset() {
    rset<R>(0);
};

template <typename F> void reset(const F& f, typename F::base v) {
    *(reinterpret_cast<volatile typename F::base*>(f.offset())) = v;
};

template <typename F> void reset(const F& f) {
    reset(f, mask(f));
};

// Combinations
template <typename T>
struct FieldUpdate {
    FieldUpdate(mystd::size_t offset, T val, T mask) : offset_(offset), val_(val), mask_(mask) {
    }

    constexpr decltype(auto) offset() const { return offset_; }
    constexpr decltype(auto) val() const { return val_; }
    constexpr decltype(auto) mask() const { return mask_; }

    T offset_;
    T val_;
    T mask_;
};

template <typename F1, typename F2> constexpr FieldUpdate<typename F1::base> operator+(const F1& f1, const F2& f2) {
    return FieldUpdate<typename F1::base>(f1.offset(),
                                          mask(f1) | mask(f2),
                                          mask(f1) | mask(f2));
}

template <typename T, typename F> constexpr FieldUpdate<T> operator+(const FieldUpdate<T>& fu, const F& f) {
    return FieldUpdate<T>(fu.offset(),
                          fu.mask() | mask(f),
                          fu.mask() | mask(f));
}

template <typename T> void set(const FieldUpdate<T>& fu) {
    Bits<T>::write(fu.val(), fu.mask(), reinterpret_cast<volatile T*>(fu.offset()));
}

template <typename T> void clear(const FieldUpdate<T>& fu) {
    Bits<T>::write(0, fu.mask(), reinterpret_cast<volatile T*>(fu.offset()));
}

template <typename T> void reset(const FieldUpdate<T>& fu) {
    *(reinterpret_cast<volatile T*>(fu.offset())) = fu.val();
}

// Integration
/*
template <typename F, typename B>
struct FieldWrapper {
    F& f_;

    FieldWrapper(F& f) : f_(f) {
    }

    operator B(){
        return read(f_);
    }

    FieldWrapper& operator=(B v) {
        write(f_, v);
    }
};

template <typename F> decltype(auto) wrap(F& f) {
    return FieldWrapper<F, typename F::base>(f);
}
*/

#endif

