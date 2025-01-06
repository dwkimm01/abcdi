#pragma once
//
// Created by dwkimm01 on 7/9/24.
//
#include "abcdi/abcdi_util.h"

namespace abcdi {

// ----------------------------------------------------------------------------
// Service Provider - to create new instances
class spv4 {
public:
    // Make Injected Entry Point
    template <class T, std::size_t Max = 16>
    T make_injected() const {
        // Integral values: bool, char, int, int64_t, float, double, enum
        if constexpr (std::is_integral_v<T>) {
            DOUT("make_injected is integral_v: " << get_type_name<T>());
            T t{};
            return t;
        }

        // Integral pointers: bool*, char*, int*, int64_t*, float*, double*, enum*
        if constexpr (std::is_pointer_v<T> && std::is_integral_v<std::remove_pointer_t<T>>) {
            DOUT("make_injected is_pointer_v and is integral_v: " << get_type_name<T>());
            using underlying_t = std::remove_pointer_t<T>;
            return new underlying_t {};
        }

        // Class values: class that don't match shared_ptr or unique_ptr
        if constexpr (std::is_class_v<T> && !is_shared_ptr<T>::value && !is_unique_ptr<T>::value) {
            DOUT("make_injected is_class_v but not shared_ptr and ! unique_ptr: " << get_type_name<T>());
            return make_injected_with_sequence<T>(injection_parameter_sequence<T>(std::make_index_sequence<Max>{}));
        }

        // Reference: recurses into this fn - but this badly copied into place
        if constexpr (std::is_reference_v<T>) {
            DOUT("make_injected is_reference_v: " << get_type_name<T>());
            using underlying_t = std::remove_cvref_t<T>;
            static underlying_t t { make_injected<underlying_t>() };
            return t;
        }

        // Raw Pointer: recurse into this fn
        if constexpr (std::is_pointer_v<T>) {
            DOUT("make_injected is_pointer_v: " << get_type_name<T>());
            using underlying_t = std::remove_pointer_t<T>;
            return make_pointer_injected_with_sequence<underlying_t>(injection_parameter_sequence<underlying_t>(std::make_index_sequence<Max>{}));
        }

        // Shared Pointer
        if constexpr (is_shared_ptr<T>::value) {
            DOUT("make_injected is_shared_ptr: " << get_type_name<T>());
            using underlying_t = remove_shared_pointer_t<T>;
            return make_shared_injected_with_sequence<underlying_t>(injection_parameter_sequence<underlying_t>(std::make_index_sequence<Max>{}));
        }

        // Unique Pointer
        if constexpr (is_unique_ptr<T>::value) {
            DOUT("make_injected is_unique_ptr: " << get_type_name<T>());
            using underlying_t = remove_unique_pointer_t<T>;
            return make_unique_injected_with_sequence<underlying_t>(injection_parameter_sequence<underlying_t>(std::make_index_sequence<Max>{}));
        }

        throw std::runtime_error(std::string("Unknown Type to build: ") +
                get_type_name<T>());
    }

private:
    template <class T, std::size_t Head, std::size_t... Rest>
    constexpr auto
    injection_parameter_sequence
            ( std::index_sequence<Head, Rest...>
                    , decltype(T{implicit_converter<spv4, T>{Head }, implicit_converter<spv4, T>{Rest }... })* = nullptr
            ) const {
        return std::index_sequence<Head, Rest...>{};
    }

    template <class T>
    constexpr auto
    injection_parameter_sequence(std::index_sequence<>) const {
        return std::index_sequence<>{};
    }

    template <class T, std::size_t... Rest>
    constexpr auto
    injection_parameter_sequence(std::index_sequence<Rest...>) const {
        return injection_parameter_sequence<T>(std::make_index_sequence<sizeof...(Rest) - 1>{});
    }

    template <class T, std::size_t... Params>
    constexpr auto
    make_injected_with_sequence(std::index_sequence<Params...>) const {
        return T(implicit_converter<spv4, T>(*this, Params)...);
    }

    // Pointer (raw)
    template <class T, std::size_t... Params>
    constexpr auto
    make_pointer_injected_with_sequence(std::index_sequence<Params...>) const {
        return new T(implicit_converter<spv4, T>(*this, Params)...);
    }

    // Shared Ptr
    template <class T, std::size_t... Params>
    constexpr auto
    make_shared_injected_with_sequence(std::index_sequence<Params...>) const {
        return std::make_shared<T>(implicit_converter<spv4, T>(*this, Params)...);
    }

    // Unique Ptr
    template <class T, std::size_t... Params>
    constexpr auto
    make_unique_injected_with_sequence(std::index_sequence<Params...>) const {
        return std::make_unique<T>(implicit_converter<spv4, T>(*this, Params)...);
    }
};

template <typename T>
T call_sp_make_injected_ref(spv4 const* p) {
    using rr_t = std::remove_cvref_t<T>;
    return p->make_injected<rr_t&>();
}
}
