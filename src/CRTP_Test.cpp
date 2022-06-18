#include <iostream>

template <class Implementation>
class Base {
public:
  void test(int arg) {
    Impl().testImplementation(arg);
  }

private:
  inline Implementation &Impl() {
    return static_cast<Implementation &>(*this);
  }

  friend Implementation;
};

template <class Derived1Impl>
class Derived1 : public Base<Derived1<Derived1Impl>> {
};

template <typename Derived2Impl, typename Origin = Derived2>
class Derived2 : public Base<Origin<Derived2Impl>> {
};

class Parent : public Derived1<Parent>, public Derived2<Parent> {
public:
  using Derived2<Parent>::test;

  void testImplementation(int test) {}
};

int main(int argc, char const *argv[]) {

  Parent test3;
  test3.test(1);

  return 0;
}
