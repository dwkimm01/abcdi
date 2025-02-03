#pragma once
//
// Created by dwkimm01 on 6/19/24.
//
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "abcdi/abcdi_util.h"

namespace abcdi {
class service_provider {
public:
    template <typename T, std::size_t MaxCtorParams = 16>
    T make_injected() const {
        if constexpr (is_unique_ptr<T>::value
                  || is_unique_ptr<std::remove_reference_t<T>>::value
                  || is_unique_ptr<std::remove_pointer_t<T>>::value
                  || is_unique_ptr<std::remove_pointer_t<std::remove_reference_t<T>>>::value
                  ) {
            return make_injected_unique_ptr<T, MaxCtorParams>();
        }
        else {
            return make_injected_normal<T, MaxCtorParams>();
        }
    }

    bool make_name_pre_bound(const std::string & name) const {
        DOUT("make_name_pre_bound: " << name);

        const bool requested_name_is_for_unique_ptr = (std::string::npos != name.find("std::unique_ptr<")) || (std::string::npos != name.find("std::__1::unique_ptr<"));

        for (auto & [entry_type, entry_node_vec] : m_node_map) {
            if (entry_node_vec.empty()) {
                DOUT("make_name_pre_bound, empty node vec found: " << entry_type << " " << entry_node_vec);
                continue;
            }

            const std::string node_name = abcdi_demangle::demangle(entry_type.get().name());
            DOUT("make name pre bound: " << node_name);

            if (node_name.find(name) == std::string::npos) {
                continue;
            }

            if (requested_name_is_for_unique_ptr != entry_node_vec.at(0)->is_unique_ptr()) {
                DOUT("requested_name_is_for_unique_ptr != entry_node_vec is_unique: " << name);
                continue;
            }

            // // This isn't great, but try to discern between a unique_ptr being requested and everything else
            // if (name.find("std::unique") != std::string::npos) {
            //     if (node_name.find("std::unique") == std::string::npos) {
            //         continue;
            //     }
            // }
            // else {
            //     if (node_name.find("std::unique") != std::string::npos) {
            //         continue;
            //     }
            // }

            DOUT("Found node named: " << node_name << ", will try to find created one first, then create if need be, entry count: " << entry_node_vec.size());
            for (auto const & node : entry_node_vec) {
                if (node->already_created()) {
                    // Good, we've already created one that satisfies the requested name
                    return true;
                }
            }
            // Nothing created already, create first entry if there is one
            if (!entry_node_vec.empty()) {
                entry_node_vec.front()->create();
                return true;
            }
            DOUT("Error, was not able to successfully create node named: " << node_name);
            break;
        }
        DOUT("Could not find node with name: " << name);
        return false;
    }

    // Second template parameter is to force the type used in the TYPE_ID_TYPE for the map lookup
    template <typename T, typename TIDT=typename detected_type<T>::underlying_type>
    void add_node(std::shared_ptr<inode> node) const {
        DOUT("adding node: " << hasher::get(typeid(TIDT)) << " " << abcdi::abcdi_demangle::demangle<TIDT>());
        m_node_map[typeid(TIDT)].push_back(std::move(node));
    }

    // Bind interface
    template <typename PARENT_T, typename... PARENTS_T> class interface_binder {
    public:
        explicit interface_binder(service_provider * sp) : m_sp(sp) {}

        template <typename CHILD_T, std::size_t MaxCtorParams = 16>
        void to() {
            using u_t = CHILD_T;
            using i_t = PARENT_T;

            DOUT("CHILD: " << abcdi::abcdi_demangle::demangle<CHILD_T>());
            // "Normal" types
            auto child_node = std::make_shared<node_container_child<u_t>>([sp = m_sp](){
                // this isn't great as the SP now has a pointer to itself buried in there making exporting it all to another SP harder
                std::shared_ptr<u_t> ptr = sp->template make_shared_ptr_injected_with_sequence<u_t>(sp->template injection_parameter_sequence<u_t>(std::make_index_sequence<MaxCtorParams>{}));
                return ptr;
            });
            m_sp->add_node<u_t>(child_node);

            // Parent
            to_parent_impl<u_t, i_t>(child_node);

            // Parents
#ifdef DO_DOUT
            int parent_number = 0;
            DOUT("  P" << ++parent_number << ": " << abcdi::abcdi_demangle::demangle<PARENT_T>());
#endif

            ([&] {
#ifdef DO_DOUT
                ++parent_number;
#endif
                to_parent_impl<u_t, PARENTS_T>(child_node);
              } (), ...);
        }

    private:
        template <typename U_T, typename SPECIFIC_PARENT_T, std::size_t MaxCtorParams = 16>
        void to_parent_impl(std::shared_ptr<node_container_child<U_T>> & child_node) {
            using u_t = U_T;
            using i_t = SPECIFIC_PARENT_T;
            std::function<bool()> is_created = [child_node]{return child_node->already_created();};
            std::function<std::shared_ptr<i_t>()> get_fn = [child_node]{return child_node->get();};

            auto i_parent_node = std::make_shared<node_container_parent<i_t>>(is_created, get_fn);
            m_sp->add_node<i_t>(i_parent_node);

            // --------------------------------------------------------
            // Unique Ptr
            auto child_node_unique = std::make_shared<node_container_unique_child<u_t>>([sp=m_sp]() -> std::unique_ptr<u_t> {
                return sp->template make_unique_injected_with_sequence<u_t>(sp->template injection_parameter_sequence<u_t>(std::make_index_sequence<MaxCtorParams>{}));
            });
            m_sp->add_node<std::unique_ptr<u_t>, std::unique_ptr<u_t> >(child_node_unique);

            std::function<bool()> is_created_unique = [child_node_unique]{return child_node_unique->already_created();};
            std::function<std::unique_ptr<i_t>()> get_fn_unique = [child_node_unique]() -> std::unique_ptr<i_t> {return std::move(child_node_unique->get());};

            auto i_parent_node_unique = std::make_shared<node_container_unique_parent<i_t>>(is_created_unique, get_fn_unique);
            m_sp->add_node<std::unique_ptr<i_t>, std::unique_ptr<i_t>>(i_parent_node_unique);
        }

        service_provider * m_sp;
    };

    template <typename PARENT_T, typename... PARENTS_T>
    interface_binder<PARENT_T, PARENTS_T...> bind() {
        return interface_binder<PARENT_T, PARENTS_T...>(this);
    }

    template <typename FN_T> void bind_to(FN_T fn) {
        using fn_return_t = decltype(std::declval<FN_T>()());
        using d_t = detected_type<fn_return_t>;
        using u_t = typename d_t::underlying_type;

        DOUT("bind_to, fn_return_t: " << abcdi::abcdi_demangle::demangle<fn_return_t>());
        if constexpr (std::is_pointer_v<fn_return_t>) {
            auto child_node = std::make_shared<node_container_child<u_t>>([&, fn_cp = std::move(fn)](){
                u_t* raw_ptr = fn_cp();
                std::shared_ptr<u_t> ptr(raw_ptr);
                return ptr;
            });
            add_node<u_t>(child_node);
        }
        else if constexpr (is_shared_ptr<fn_return_t>::value) {
            auto child_node = std::make_shared<node_container_child<u_t>>([&, fn_cp = std::move(fn)](){
                auto ptr_val = fn_cp();
                std::shared_ptr<u_t> ptr(ptr_val);
                return ptr;
            });
            add_node<u_t>(child_node);
        }
        else {
            auto child_node = std::make_shared<node_container_child<u_t>>([&, fn_cp = std::move(fn)](){
                std::shared_ptr<u_t> ptr = std::make_shared<u_t>(std::move(fn_cp()));
                return ptr;
            });
            add_node<u_t>(child_node);
        }
    }

private:

    mutable std::unordered_map<type_info_ref, std::vector<std::shared_ptr<inode>>, hasher, hasher_is_equal> m_node_map;
    friend std::ostream & operator<<(std::ostream &os, const service_provider& sp) {
        for (const auto & [entry_type, entry_node_vec] : sp.m_node_map) {
            os << "  Node Type:[" << entry_node_vec.size() << "]: "
               << abcdi_demangle::demangle(entry_type.get().name())
               << "\n";
            size_t index = 0;
            for (const auto & node : entry_node_vec) {
                os << "    " << index++
                   << ", already created: " << node->already_created()
                   << ", address: " << node->address()
                   << ", use_count: " << node->use_count()
                   << ", hash_code: " << node->hash_code()
                   << "\n";
            }
        }
        return os;
    }

    template <class T, std::size_t Head, std::size_t... Rest>
    constexpr auto injection_parameter_sequence
            ( std::index_sequence<Head, Rest...>
            , decltype(T{implicit_converter<service_provider, T>{Head }, implicit_converter<service_provider, T>{Rest }... })* = nullptr
            ) const {
        return std::index_sequence<Head, Rest...>{};
    }

    template <class T>
    constexpr auto injection_parameter_sequence(std::index_sequence<>) const {
        return std::index_sequence<>{};
    }

    template <class T, std::size_t... Rest>
    constexpr auto injection_parameter_sequence(std::index_sequence<Rest...>) const {
        return injection_parameter_sequence<T>(std::make_index_sequence<sizeof...(Rest) - 1>{});
    }

    template <class T, std::size_t... Params>
    constexpr auto make_injected_with_sequence(std::index_sequence<Params...>) const {
        return T(implicit_converter<service_provider, T>(*this, Params)...);
    }

     // Pointer (raw)
     template <class T, std::size_t... Params>
     constexpr auto make_pointer_injected_with_sequence(std::index_sequence<Params...>) const {
         return new T(implicit_converter<service_provider, T>(*this, Params)...);
     }

    template <class T, std::size_t... Params>
    constexpr auto
    make_shared_ptr_injected_with_sequence(std::index_sequence<Params...>) const {
        return std::make_shared<T>(implicit_converter<service_provider, T>(*this, Params)...);
    }

    // Unique Ptr
    template <class T, std::size_t... Params>
    constexpr auto make_unique_injected_with_sequence(std::index_sequence<Params...>) const {
        return std::make_unique<T>(implicit_converter<service_provider, T>(*this, Params)...);
    }

    // The Two High Level Cases that we need to handle different, most "normal" types, and then unique_ptr
    template <typename T, std::size_t MaxCtorParams = 16>
    T make_injected_normal() const {
        using d_t = detected_type<T>;
        using u_t = typename d_t::underlying_type;
        DOUT("Looking to make: " << hasher::get(typeid(u_t)) << " " << abcdi_demangle::demangle<u_t>());

        // Look up first to see if we can use something that has already been created
        const auto itr = m_node_map.find(typeid(u_t));

        // Cleanup/simplify
        if constexpr (std::is_abstract_v<u_t>) {
            if (itr == m_node_map.end() || itr->second.empty()) {
                throw std::runtime_error("Cannot create abstract type without binder: " + abcdi_demangle::demangle<u_t>());
            }
            for (auto & n : itr->second) {
                if (n->already_created()) {
                    return d_t::make(n);
                }
            }
            return d_t::make(itr->second.front());
        }
        else {
            if (itr != m_node_map.end() && !itr->second.empty()) {
                for (auto & n : itr->second) {
                    if (n->already_created()) {
                        return d_t::make(n);
                    }
                }
                auto itr_second_front = itr->second.front();
                return d_t::make(itr_second_front);
            }
            else if constexpr (is_default_made_v<u_t>) {
                // Just add a simple default, on the fly
                std::shared_ptr<inode> n = std::make_shared<node_container_child<typename d_t::underlying_type>>([](){
                    auto ptr = std::make_shared<typename d_t::underlying_type>();
                    return ptr;
                });
                add_node<u_t>(n);
                return d_t::make(n);
            }
            else if constexpr (is_vect<u_t>::value) {
                using element_t = remove_vect_t<u_t>;

                std::shared_ptr<inode> n = std::make_shared<node_container_child<typename d_t::underlying_type>>([&](){
                    using element_d_t = detected_type<element_t>;
                    using element_u_t = typename detected_type<element_t>::underlying_type;

                    auto ptr = std::make_shared<typename d_t::underlying_type>();
                    // Collect up all entries that have already been made at this point
                    auto elem_itr = m_node_map.find(typeid(element_u_t));
                    if (elem_itr != m_node_map.end()) {
                        // using element_dt_t = detected_type<element_t>;
                        for (auto & node_of_element : elem_itr->second) {
                            if (node_of_element->already_created()) {
                                ptr->push_back(element_d_t::make(node_of_element));
                            }
                        }
                    }

                    return ptr;
                });
                add_node<u_t>(n);
                return d_t::make(n);
            }
            else if constexpr (std::is_class_v<u_t>) {
                // Just add a simple default, on the fly
                std::shared_ptr<inode> n = std::make_shared<node_container_child<typename d_t::underlying_type>>([&]() -> std::shared_ptr<u_t> {
                    return make_shared_ptr_injected_with_sequence<u_t>(injection_parameter_sequence<u_t>(std::make_index_sequence<MaxCtorParams>{}));
                });
                add_node<u_t>(n);
                return d_t::make(n);
            }
        }

        // throw std::runtime_error("No known way to handle type normal: " + abcdi_demangle::demangle<T>());
    }

    template <typename T, std::size_t MaxCtorParams = 16>
    T make_injected_unique_ptr() const {
        using d_t = detected_type<T>;
        using u_t = typename d_t::underlying_type;
        using smart_ptr_t = std::unique_ptr<u_t>;

        DOUT("Looking to make unique node: " << hasher::get(typeid(std::unique_ptr<u_t>)) << " " << abcdi::abcdi_demangle::demangle<std::unique_ptr<u_t>>());

        // Store into type map with unique_ptr so as not to conflict
        const auto itr = m_node_map.find(typeid(std::unique_ptr<u_t>));
        if (itr != m_node_map.end()) {
            for (auto & n : itr->second) {
                if (n->already_created()) {
                    return d_t::make(n);
                }
            }
            return d_t::make(itr->second.front());
        }

        if constexpr (std::is_abstract_v<u_t>) {
            if (itr == m_node_map.end() || itr->second.empty()) {
                throw std::runtime_error("Cannot create unique_ptr<abstract type> without binder: " + abcdi_demangle::demangle<u_t>());
            }
        }
        else {
            if constexpr (is_default_made_v<u_t>) {
                // Just add a simple default, on the fly
                std::shared_ptr<inode> n = std::make_shared<node_container_unique_child<typename d_t::underlying_type>>([](){
                    std::unique_ptr<typename d_t::underlying_type> ptr = std::make_unique<typename d_t::underlying_type>();
                    return ptr;
                });

                // Force the typeid since we are making the non-normal (unique_ptr) types be of type-id unique_ptr<T>
                add_node<smart_ptr_t, smart_ptr_t>(n);
                return d_t::make(n);
            }
            else if constexpr (std::is_class_v<u_t>) {
                // Just add a simple default, on the fly
                std::shared_ptr<inode> n = std::make_shared<node_container_unique_child<typename d_t::underlying_type>>([&]() -> std::unique_ptr<u_t> {
                    return make_unique_injected_with_sequence<u_t>(injection_parameter_sequence<u_t>(std::make_index_sequence<MaxCtorParams>{}));
                });
                add_node<smart_ptr_t, smart_ptr_t>(n);
                return d_t::make(n);
            }
        }

        throw std::runtime_error("unique_ptr could not create " + abcdi_demangle::demangle<T>());
    }
};
}
