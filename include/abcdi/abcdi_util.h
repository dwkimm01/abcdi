#pragma once
//
// Created by dwkimm01 on 6/19/24.
//

#if defined(__linux__) || defined(__APPLE__)
#include <cxxabi.h>
#elif _WIN32
#endif
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace abcdi {
// #define DO_DOUT 1

#ifdef DO_DOUT
#define DOUT(X) do { std::cout << X << std::endl; break; } while(true);
#else
#define DOUT(X)
#endif


// Just a helper to make easier to read types in print statements
class abcdi_demangle {
public:
    static std::string demangle(const char* name) {
#if defined(__linux__) || defined(__APPLE__)
        int status = -4;

        const std::unique_ptr<char, void(*)(void*)> res {
                abi::__cxa_demangle(name, nullptr, nullptr, &status),
                std::free
        };

        return 0 == status ? res.get() : name;
#elif
        return name;
#endif
    }

    template <typename T>
    static std::string demangle() { return demangle(typeid(T).name()); }

    template <typename T>
    static std::string demangle_inst(const T &) { return demangle<T>(); }
};

// Purpose: Base class to store in type map, type-erased
class inode {
public:
    virtual ~inode() = default;
    [[nodiscard]] const virtual void* address() const = 0;
    [[nodiscard]] virtual bool already_created() const = 0;
    virtual void create() = 0;
    [[nodiscard]] virtual size_t hash_code() const = 0;
    [[nodiscard]] virtual const std::string & underlying_type_name() const = 0;
    [[nodiscard]] virtual size_t use_count() const = 0;
};

// ---------------------------------------------------------------------------------------------------------------------
// "Normal" Types we can reasonably express as a shared pointer
// Purpose: Type-aware child node provides cast-to point
template <typename T>
class node_container : public inode {
public:
    using underlying_t = T;
    using underlying_raw_ptr_t = T*;
    using smart_ptr_t = std::shared_ptr<T>;
    using smart_raw_ptr_t = std::shared_ptr<T>*;

    ~node_container() override = default;
    [[nodiscard]] const void* address() const override = 0;
    [[nodiscard]] bool already_created() const override = 0;
    void create() override = 0;

//    using ptr_t = std::shared_ptr<T>;
//    virtual ptr_t & get() = 0;
    virtual smart_ptr_t & get() = 0;

//    using raw_ptr_t = T*;
//    virtual raw_ptr_t & get_raw() = 0;
    virtual underlying_raw_ptr_t & get_raw() = 0;

//    using smart_raw_ptr_t = ptr_t*;
//    virtual smart_raw_ptr_t & get_smart_raw() = 0;
    virtual smart_raw_ptr_t & get_smart_raw() = 0;

    [[nodiscard]] size_t hash_code() const override = 0;
    [[nodiscard]] const std::string & underlying_type_name() const override = 0;
    [[nodiscard]] size_t use_count() const override = 0;
};

// Purpose: Child which can create and hold actual instance as a shared_ptr<CHILD_T>
template <typename CHILD_T>
class node_container_child final : public node_container<CHILD_T> {
public:
    using underlying_t = node_container<CHILD_T>::underlying_t;
    using smart_ptr_t = node_container<CHILD_T>::smart_ptr_t;
    using smart_raw_ptr_t = typename node_container<CHILD_T>::smart_raw_ptr_t;

    explicit
    node_container_child(std::function<std::shared_ptr<CHILD_T>()> func)
            : m_func(std::move(func)) {}
    ~node_container_child() override = default;
    [[nodiscard]] const void* address() const override { return static_cast<const void*>(&(*m_data)); }
    [[nodiscard]] bool already_created() const override { return nullptr != m_data; }
    void create() override {
        if (nullptr != m_data) {
            return;
        }
        if (m_func) {
            m_data = m_func();
            m_child_raw = &(*m_data);
            m_smart_child_raw = &(m_data);
            return;
        }

        throw std::runtime_error("node_container_child: m_func failed");
    }

    smart_ptr_t & get() override {
        create();
        return m_data;
    }

    underlying_t* & get_raw() override {
        create();
        return m_child_raw;
    }

    smart_raw_ptr_t & get_smart_raw() override {
        create();
        return m_smart_child_raw;
    }

    [[nodiscard]] size_t hash_code() const override { return typeid(underlying_t).hash_code(); }
    [[nodiscard]] const std::string & underlying_type_name() const override { return m_underlying_type_name; }
    [[nodiscard]] size_t use_count() const override { return m_data.use_count(); }

private:
    std::string m_underlying_type_name {abcdi_demangle::demangle<underlying_t>()};
    std::function<std::shared_ptr<CHILD_T>()> m_func;
    std::shared_ptr<CHILD_T> m_data;
    underlying_t * m_child_raw {nullptr};
    smart_raw_ptr_t m_smart_child_raw {nullptr};
};

// Purpose: Parent will hold pointer to child
template <typename PARENT_NODE_T>
class node_container_parent final : public node_container<PARENT_NODE_T> {
public:
    using underlying_t = PARENT_NODE_T;
    using ptr_t = typename node_container<PARENT_NODE_T>::smart_ptr_t;
    using is_created_fn_t = std::function<bool()>;
    using get_fn_t = std::function<std::shared_ptr<PARENT_NODE_T>()>;
    using smart_raw_ptr_t = typename node_container<PARENT_NODE_T>::smart_raw_ptr_t;

    node_container_parent(is_created_fn_t is_created_fn, get_fn_t get_fn)
            : m_is_created(std::move(is_created_fn))
            , m_get_fn(std::move(get_fn))
    {}

    ~node_container_parent() override = default;

    [[nodiscard]]
    const void* address() const override {
        if (!m_is_created()) {
            return nullptr;
        }
        return static_cast<void*>(&(*m_get_fn()));
    }

    [[nodiscard]]
    bool already_created() const override { return m_is_created(); }

    void create() override {
        if (nullptr != m_data) {
            return;
        }
        if (m_get_fn) {
            m_data = m_get_fn();
            m_child_raw = &(*m_data);
            m_smart_child_raw = &(m_data);
            return;
        }
        throw std::runtime_error("node_container_parent: m_get_fn failed");
    }

    ptr_t & get() override {
        create();
        return m_data;
    }

    underlying_t* & get_raw() override {
        create();
        return m_child_raw;
    }

    smart_raw_ptr_t & get_smart_raw() override {
        create();
        return m_smart_child_raw;
    }

    [[nodiscard]] const std::string & underlying_type_name() const override { return m_underlying_type_name; }
    // Note, this is a little weird, but should work
    [[nodiscard]] size_t use_count() const override { if (!m_is_created()) { return 0; } return m_get_fn().use_count(); }
    [[nodiscard]] size_t hash_code() const override { return typeid(underlying_t).hash_code(); }

private:
    std::string m_underlying_type_name {abcdi_demangle::demangle<underlying_t>()};
    is_created_fn_t m_is_created;
    get_fn_t m_get_fn;
    // Need a copy for returning a reference to this as a parent as both shared_ptr and raw ptr
    std::shared_ptr<PARENT_NODE_T> m_data;
    underlying_t * m_child_raw {nullptr};
    std::shared_ptr<PARENT_NODE_T> * m_smart_child_raw {nullptr};
};


// ---------------------------------------------------------------------------------------------------------------------
// Unique Ptr
template <typename T>
class node_container_unique : public inode {
public:
    ~node_container_unique() override = default;
    [[nodiscard]] const void* address() const override = 0;
    [[nodiscard]] bool already_created() const override = 0;
    void create() override = 0;

    using ptr_t = std::unique_ptr<T>;
    virtual ptr_t & get() = 0;

    using raw_ptr_t = T*;
    virtual raw_ptr_t & get_raw() = 0;

    [[nodiscard]] size_t hash_code() const override = 0;
    [[nodiscard]] const std::string & underlying_type_name() const override = 0;
    [[nodiscard]] size_t use_count() const override = 0;
};


// Purpose: Type-aware child node provides cast-to point
template <typename CHILD_T>
class node_container_unique_child final : public node_container_unique<CHILD_T> {
public:
    using underlying_type = CHILD_T;
    using ptr_t = typename node_container_unique<CHILD_T>::ptr_t;
    using raw_ptr_t = typename node_container_unique_child::raw_ptr_t;

    explicit node_container_unique_child(std::function<std::unique_ptr<underlying_type>()> func)
            : m_func(std::move(func))  {}

    ~node_container_unique_child() override = default;
    [[nodiscard]] const void* address() const override { return static_cast<void*>(&(*m_data)); }
    [[nodiscard]] bool already_created() const override { return nullptr != m_data; }
    void create() override { if (!m_data) { m_data = m_func(); m_raw_ptr = &(*m_data); } }

    ptr_t & get() override { create(); return m_data; }

    raw_ptr_t & get_raw() override { create(); return m_raw_ptr; }

    [[nodiscard]] size_t hash_code() const override { return typeid(raw_ptr_t).hash_code(); }
    [[nodiscard]] const std::string & underlying_type_name() const override { return m_underlying_type_name; }
    [[nodiscard]] size_t use_count() const override { return nullptr != m_data ? 1 : 0; }

private:
    std::string m_underlying_type_name {abcdi_demangle::demangle<underlying_type>()};
    std::function<std::unique_ptr<underlying_type>()> m_func;
    ptr_t m_data;
    raw_ptr_t m_raw_ptr {nullptr};
};


// Purpose: Parent will hold pointer to child
template <typename PARENT_NODE_T>
class node_container_unique_parent final : public node_container_unique<PARENT_NODE_T> {
public:
    using underlying_type = PARENT_NODE_T;
    using ptr_t = typename node_container_unique<PARENT_NODE_T>::ptr_t;
    using is_created_fn_t = std::function<bool()>;
    using get_fn_t = std::function<ptr_t()>;

    node_container_unique_parent(is_created_fn_t is_created_fn, get_fn_t get_fn)
            : m_is_created(std::move(is_created_fn))
            , m_get_fn(std::move(get_fn))
    {}

    ~node_container_unique_parent() override = default;

// TODO, since this is really only used for printing should be intptr_t
    [[nodiscard]]
    const void* address() const override {
        if (!m_is_created()) {
            return nullptr;
        }
        return static_cast<void*>(&(*m_get_fn()));
    }

    [[nodiscard]]
    bool already_created() const override { return m_is_created(); }

    void create() override {
        if (nullptr != m_data) {
            return;
        }
        if (m_get_fn) {
            m_data = m_get_fn();
            m_child_raw = &(*m_data);
            return;
        }
        throw std::runtime_error("node_container_unique_parent: m_get_fn failed");
    }

    ptr_t & get() override {
        create();
        return m_data;
    }

    underlying_type* & get_raw() override {
        create();
        return m_child_raw;
    }

    [[nodiscard]] const std::string & underlying_type_name() const override { return m_underlying_type_name; }
    [[nodiscard]] size_t use_count() const override { if (!m_is_created()) { return 0; } return 1; }
    [[nodiscard]] size_t hash_code() const override { return typeid(underlying_type).hash_code(); }

private:
    std::string m_underlying_type_name {abcdi_demangle::demangle<underlying_type>()};
    is_created_fn_t m_is_created;
    get_fn_t m_get_fn;
    // Need a copy for returning a reference to this as a parent as both shared_ptr and raw ptr
    ptr_t m_data;
    underlying_type * m_child_raw {nullptr};
};




// There is ..
// - Defaultable: can be created by default, like int, double, string
//   may or may not make sense, but it's doable and can use previous reference as source
// - Constructable: may not have default and should be created with most number of CTOR args
//   will want to bind interfaces that can't be instantiated to concrete implementations
// - Really-Requires-Previous-Instance Constructable: things like std::function<> really make
//   no sense ... e.g. std::function<bool(a1, a2)>  ????

// Is String
template <typename T> struct is_string : std::false_type {};
template <typename T, class Traits, class Alloc> struct is_string<std::basic_string<T, Traits, Alloc>> : std::true_type {};

// Vector Detection
template <typename T> struct is_vect : std::false_type {};
template <typename T> struct is_vect<std::vector<T>> : std::true_type {};
template <typename T> concept vect = is_vect<T>::value;

// Vector Removal
template<class T> struct remove_vect { typedef T type; };
template<class T> struct remove_vect<std::vector<T>> { typedef T type; };
template<class T> using remove_vect_t = typename remove_vect<T>::type;

// Shared Pointer Detection
template <typename T> struct is_shared_ptr : std::false_type {};
template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
template <typename T> concept shared_pointer = is_shared_ptr<T>::value;

// Shared Pointer Removal
template<class T> struct remove_shared_pointer { typedef T type; };
template<class T> struct remove_shared_pointer<std::shared_ptr<T>> { typedef T type; };
template<class T> using remove_shared_pointer_t = typename remove_shared_pointer<T>::type;

// Unique Pointer Detection
template <typename T> struct is_unique_ptr : std::false_type {};
template <typename T> struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};
template <typename T> concept unique_pointer = is_unique_ptr<T>::value;

// Unique Pointer Removal
template<class T> struct remove_unique_pointer { typedef T type; };
template<class T> struct remove_unique_pointer<std::unique_ptr<T>> { typedef T type; };
template<class T> using remove_unique_pointer_t = typename remove_unique_pointer<T>::type;

// Remove it all ...
template <typename T> struct remove_ref_ptrs { typedef T type; };
template <typename T> struct remove_ref_ptrs<const T> { typedef T type; };
template <typename T> struct remove_ref_ptrs<T&> { typedef T type; };
template <typename T> struct remove_ref_ptrs<T const&> { typedef T type; };
template <typename T> struct remove_ref_ptrs<T*> { typedef T type; };
template <typename T> struct remove_ref_ptrs<T*&> { typedef T type; };
template <typename T> struct remove_ref_ptrs<T* const&> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::shared_ptr<T>> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::shared_ptr<T>&> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::shared_ptr<T>const &> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::shared_ptr<T>*> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::shared_ptr<T>* &> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::shared_ptr<T>* const &> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::unique_ptr<T>> { typedef T type; };
// Don't think it makes sense to have const & unique_ptr ... but we could put it in
template <typename T> struct remove_ref_ptrs<std::unique_ptr<T>&> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::unique_ptr<T>*> { typedef T type; };
template <typename T> struct remove_ref_ptrs<std::unique_ptr<T>* &> { typedef T type; };
template <typename T> using remove_ref_ptrs_t = typename remove_ref_ptrs<T>::type;


template<class T>
struct is_default_made : std::integral_constant<bool,
    std::is_integral_v<remove_ref_ptrs_t<T>>
    || std::is_floating_point_v<remove_ref_ptrs_t<T>>
    || std::is_enum_v<remove_ref_ptrs_t<T>>
    || is_string<remove_ref_ptrs_t<T>>::value
    > {};
template< class T >
inline constexpr bool is_default_made_v = is_default_made<T>::value;
template <typename T>
concept default_made = is_default_made_v<T>;

template<class T>
struct is_ctor_made : std::integral_constant<bool,
    std::is_class_v<remove_ref_ptrs_t<T>>
    && (! std::is_function_v<remove_ref_ptrs_t<T>>)
    > {};
template< class T >
inline constexpr bool is_ctor_made_v = is_ctor_made<T>::value;
template <typename T>
concept ctor_made = is_ctor_made_v<T>;

// ----------------------------------------------------------------------------
// Prevent conversion operator from being used (and causing unbounded/infinite recursion)
template <typename T, typename U>
concept not_is_same = !std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;



template <typename T>
std::string get_type_name() {
    // Const
    if constexpr (std::is_const_v<T>) {
        using rr_t = std::remove_const_t<T>;
        return std::string("const ") + get_type_name<rr_t>();
    }

    // Reference
    if constexpr (std::is_reference_v<T>) {
        using rr_t = std::remove_reference_t<T>;
        return get_type_name<rr_t>() + std::string(" &");
    }

    // Pointer
    if constexpr (std::is_pointer_v<T>) {
        using rr_t = std::remove_pointer_t<T>;
        return get_type_name<rr_t>() + std::string(" *");
    }

    // Shared Ptr
    if constexpr (is_shared_ptr<T>::value) {
        using rr_t = remove_shared_pointer_t<T>;
        return std::string("std::shared_ptr<") + get_type_name<rr_t>() + std::string(">");
    }

    // Unique Ptr
    if constexpr (is_unique_ptr<T>::value) {
        using rr_t = remove_unique_pointer_t<T>;
        return std::string("std::unique_ptr<") + get_type_name<rr_t>() + std::string(">");
    }

    // Bool
    if constexpr (std::is_same_v<bool, T>) {
        return "bool";
    }

    // Char
    if constexpr (std::is_same_v<char, T>) {
        return "char";
    }
    if constexpr (std::is_same_v<int8_t, T>) {
        return "int8_t";
    }

    // Unsigned Char
    if constexpr (std::is_same_v<unsigned char, T>) {
        return "unsigned char";
    }
    if constexpr (std::is_same_v<uint8_t, T>) {
        return "uint8_t";
    }

    // Short
    if constexpr (std::is_same_v<short, T>) {
        return "short";
    }
    if constexpr (std::is_same_v<int16_t, T>) {
        return "int16_t";
    }

    // Unsigned short
    if constexpr (std::is_same_v<unsigned short, T>) {
        return "unsigned short";
    }
    if constexpr (std::is_same_v<uint16_t, T>) {
        return "uint16_t";
    }

    // Int
    if constexpr (std::is_same_v<int, T>) {
        return "int";
    }
    if constexpr (std::is_same_v<int32_t, T>) {
        return "int32_t";
    }

    // Unsigned Int
    if constexpr (std::is_same_v<unsigned int, T>) {
        return "unsigned int";
    }
    if constexpr (std::is_same_v<uint32_t, T>) {
        return "uint32_t";
    }

    // Int 64
    if constexpr (std::is_same_v<long long, T>) {
        return "long long";
    }
    if constexpr (std::is_same_v<int64_t, T>) {
        return "int64_t";
    }

    // Unsigned Int 64
    if constexpr (std::is_same_v<unsigned long long, T>) {
        return "unsigned long long";
    }
    if constexpr (std::is_same_v<uint64_t, T>) {
        return "uint64_t";
    }

    // Float
    if constexpr (std::is_same_v<float, T>) {
        return "float";
    }

    // Double
    if constexpr (std::is_same_v<double, T>) {
        return "double";
    }

    // std String
    if constexpr (std::is_same_v<std::string, T>) {
        return "std::string";
    }

    // Vector
    if constexpr (is_vect<T>::value) {
        return "std::vector<" + get_type_name<remove_vect_t<T>>() + ">";
    }

    // Class
    if constexpr (std::is_class_v<T> || std::is_abstract_v<T>) {
        // return "class/struct";
        // return typeid(T).name();
        return abcdi_demangle::demangle<T>();
    }

    // Shrug
    return "unknown_type_name";
}

// ----------------------------------------------------------------------------
// Detected Type Helpers
// Detect type to do conversion from known shared_ptr<T> back to other possibly types user wants to represent data as
template <typename T> struct detected_type {
    using underlying_type = T;
    using sp_type = std::shared_ptr<underlying_type>;

    static underlying_type & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<T>");
        }
        return *(ptr->get());
    }
};

template <typename T> struct detected_type<T&> {
    using underlying_type = typename detected_type<std::remove_cvref_t<T>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static underlying_type & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<T&>");
        }
        return *(ptr->get());
    }
};

template <typename T> struct detected_type<T*> {
    using underlying_type = typename detected_type<std::remove_pointer_t<T>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static underlying_type* make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<T*>");
        }
        return &(*(ptr->get()));
    }
};

template <typename T> struct detected_type<T* &> {
    using underlying_type = typename detected_type<std::remove_pointer_t<std::remove_cvref_t<T>>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static underlying_type* & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<T* &>");
        }
        return ptr->get_raw();
    }
};

template <typename T> struct detected_type<std::shared_ptr<T>> {
    using underlying_type = typename detected_type<remove_shared_pointer_t<T>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static std::shared_ptr<underlying_type> & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<std::shared_ptr<T>>");
        }
        return ptr->get();
    }
};

template <typename T> struct detected_type<std::shared_ptr<T>&> {
    using underlying_type = typename detected_type<remove_shared_pointer_t<std::remove_reference_t<T>>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static std::shared_ptr<underlying_type> & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<std::shared_ptr<T>&>");
        }
        return ptr->get();
    }
};

template <typename T> struct detected_type<std::shared_ptr<T> const&> {
    using underlying_type = typename detected_type<remove_shared_pointer_t<std::remove_cvref_t<T>>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static std::shared_ptr<underlying_type> & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<std::shared_ptr<T>&>");
        }
        return ptr->get();
    }
};

template <typename T> struct detected_type<std::shared_ptr<T>*> {
    using underlying_type = typename detected_type<remove_shared_pointer_t<std::remove_pointer_t<T>>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static std::shared_ptr<underlying_type>* & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<std::shared_ptr<T>&>");
        }
        return ptr->get_smart_raw();
    }
};

template <typename T> struct detected_type<std::shared_ptr<T>* &> {
    using underlying_type = typename detected_type<remove_shared_pointer_t<std::remove_pointer_t<std::remove_reference_t<T>>>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static std::shared_ptr<underlying_type>* & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<std::shared_ptr<T>&>");
        }
        return ptr->get_smart_raw();
    }
};

template <typename T> struct detected_type<std::shared_ptr<T>* const&> {
    using underlying_type = typename detected_type<remove_shared_pointer_t<std::remove_pointer_t<std::remove_cvref_t<T>>>>::underlying_type;
    using sp_type = std::shared_ptr<underlying_type>;

    static std::shared_ptr<underlying_type>* & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<std::shared_ptr<T>&>");
        }
        return ptr->get_smart_raw();
    }
};

template <typename T> struct detected_type<std::unique_ptr<T>> {
    using underlying_type = typename detected_type<remove_unique_pointer_t<T>>::underlying_type;
    using sp_type = std::unique_ptr<underlying_type>;

    static std::unique_ptr<underlying_type> make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container_unique<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<std::unique_ptr<T>>");
        }
        return std::move(ptr->get());
    }
};

template <typename T> struct detected_type<std::unique_ptr<T>&> {
    using underlying_type = typename detected_type<remove_unique_pointer_t<std::remove_cvref_t<T>>>::underlying_type;
    using sp_type = std::unique_ptr<underlying_type>;

    static std::unique_ptr<underlying_type> & make(std::shared_ptr<inode> const & n) {
        auto ptr = std::dynamic_pointer_cast<node_container_unique<underlying_type>>(n);
        if (!ptr) {
            throw std::runtime_error("bad type: detected_type<std::unique_ptr<T>>");
        }
        return ptr->get();
    }
};
// The problem with detected type is that if we try to go too many levels deep,
// it won't get created in a way that can be automatically exported.  For example
// "foo* * *" needs more pointers (or for example foo * * &)

// ----------------------------------------------------------------------------
// Mimic type being created helper
template <typename SERVICE_PROVIDER, typename TYPE_BEING_CONSTRUCTED_INTO>
struct implicit_converter {
    SERVICE_PROVIDER const* m_sp {nullptr};

    explicit implicit_converter(std::size_t) {}
    implicit_converter(SERVICE_PROVIDER const & p, std::size_t) : m_sp(&p) {}

    // If type being constructed != conversion being requested, progress is being made
    template <not_is_same<TYPE_BEING_CONSTRUCTED_INTO> T>
    operator T & () const {
        if (!m_sp) {
            throw std::runtime_error("Cannot call sp_make_injected because sp is null");
        }

        return m_sp->template make_injected<T&>();
    }
};

// ----------------------------------------------------------------------------
// Modified from https://en.cppreference.com/w/cpp/types/type_info/hash_code
using type_info_ref = std::reference_wrapper<const std::type_info>;
struct hasher {
    std::size_t operator()(const type_info_ref code) const {
        return code.get().hash_code();
    }

    // Example use: get(typeid(TIDT));
    template <typename TIDT> static std::size_t get(TIDT const& tidt) {
        constexpr hasher h;
        return h(tidt);
    }
};
struct hasher_is_equal { bool operator()(const type_info_ref lhs, const type_info_ref rhs) const { return lhs.get() == rhs.get(); } };
}
