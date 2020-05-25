.. title:: clang-tidy - modernize-use-trailing-return-type

modernize-use-trailing-return-type
==================================

Rewrites function signatures to use a trailing return type
(introduced in C++11). This transformation is purely stylistic.
The return type before the function name is replaced by ``auto``
and inserted after the function parameter list (and qualifiers).

Example
-------

======================================== ===============================================
Before                                   After
======================================== ===============================================
.. code-block:: c++                      .. code-block:: c++

  int f1();                                auto f1() -> int;
  inline int f2(int arg) noexcept;         inline auto f2(int arg) -> int noexcept;
  virtual float f3() const && = delete;    virtual auto f3() const && -> float = delete;
======================================== ===============================================

Known Limitations
-----------------

The following categories of return types cannot be rewritten currently:

* function pointers
* member function pointers
* member pointers

Unqualified names in the return type might erroneously refer to different entities after the rewrite.
Preventing such errors requires a full lookup of all unqualified names present in the return type in the scope of the trailing return type location.
This location includes e.g. function parameter names and members of the enclosing class (including all inherited classes).
Such a lookup is currently not implemented.

For example, a careless rewrite would produce the following output:

======================================== ===============================================
Before                                   After
======================================== ===============================================
.. code-block:: c++                      .. code-block:: c++

  struct S { long long value; };           struct S { long long value; };
  S f(unsigned S) { return {S * 2}; }      auto f(unsigned S) -> S { return {S * 2}; } // error
  class CC {                               class CC {
    int S;                                   int S;
    struct S m();                            auto m() -> struct S;
  };                                       };
  S CC::m() { return {0}; }                auto CC::m() -> S { return {0}; } // error
======================================== ===============================================

This code fails to compile because the S in the context of f refers to the equally named function parameter.
Similarly, the S in the context of m refers to the equally named class member.
The check can currently only detect and avoid a clash with a function parameter name.
