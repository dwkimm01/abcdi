<a href="https://mit-license.org/" target="_blank">![MIT License](https://img.shields.io/badge/license-mit-blue.svg)</a>

---------------------------------------

# abcdi

A Basic C++ Dependency Injection library, one/minimal header(s), no dependencies.  As easy as ABC-DI 

Header only mostly because of template use and no dependencies unless you want to build unit tests.

There are many dependency inject libraries/frameworks.  For C# there is [Ninject](https://github.com/ninject/Ninject)
and even C++ has a few. 

1) [[Boost::ext].DI](https://github.com/boost-ext/di)
1) [7bitDI](https://github.com/7bitcoder/7bitDI)
1) [Cinject](https://github.com/mjirous/cinject)
1) [Fruit](https://github.com/google/fruit)
1) [Kangaru](https://github.com/gracicot/kangaru)
1) [CPPServiceLocator](https://github.com/stevef51/CPPServiceLocator)
1) [Injecttor](https://github.com/Fabrizio86/Injecttor)
1) [inject](https://github.com/z9u2k/inject)
1) [dicpp](https://bitbucket.org/cheez/dicpp/wiki/Home)
1) [Infector++](https://github.com/Darelbi/Infectorpp2)

For a more complete set of comparisons see [test ioc c++](https://github.com/d-led/test-ioc-cpp)

Any of those options above are great and if they suit your needs they should be preferred.  The motivation for abcdi was
a library that had:

* No additional dependencies
* No dependency on macros
* A non-intrusive mindset
* An easy way to add to an existing project and was easy to use
* Simplified and more readadable code using modern language features
* An automated construction and wiring of objects
* A default use of &, *, shared and unique pointers which decided on lifetime where appropriate
* An easy way to override creation of different objects

---

### Quick start

#### Download
> abcdi requires only one header file, the latest header is [here](http://gitea.ubnt.local:3000/dwkimm01/abcdi/raw/branch/master/include/abcdi/abcdi.h)

#### Quick guide - Create object graph

```cpp
#include <abcdi/abdci.hpp>

class single_arg {
public:
  explicit single_arg(int i) : i(i) {}
  int i;
};

class two_args {
public:
  two_args(std::shared_ptr<single_arg> sa, int i) : m_sa(std::move(sa)), m_i(i) {}
  std::shared_ptr<single_arg> m_sa;
  int m_i;
};

int main() {
  abcdi::service_provider sp;
  
  auto t = sp.make_injected<two_args>();

  return 0;
}
```

#### Compile

* **GCC/Clang**
  ```sh
  $CXX -std=c++23 -O2 -fno-exceptions -Wall -Werror -pedantic-errors main.cpp
  ```
* **MSVC**
  ```sh
  cl /std:c++23 /Ox /W3 main.cpp
  ```


#### Quick guide - Bind interfaces

```cpp
struct interface {
  virtual ~interface() noexcept = default;
  virtual int num() = 0;
};

struct implementation : public interface {
  int num() override { return 42; }
};

struct example {
  example(std::shared_ptr<interface> i) {
    assert(42 == i->get());
  }
};

int main() {
  abcdi::service_provider sp;
  sp.bind<interface>().to<implementation>();
  auto obj = sp.make_injected<std::unique_ptr<example>>();
  return 0;
}
```

---------------------------------------

### Documentation
* [Introduction](https://blah.org)
  * [Quick Start](https://blah.org)
    * [Building with cmake](docs/building_with_cmake.md)
    * [Motivating Example](docs/motivating_example.md)
  * [Tutorial]
    * Value
    * Reference
    * Pointer
    * Shared Pointer
    * Unique Pointer
    * Vector
    * Classes/Structs - Picks ctor with most params
    * Interface Binding(s)
    * Value Binding
    * Lifetime Notes

