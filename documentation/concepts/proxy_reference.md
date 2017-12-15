# ProxyReference

## Summary
The `ProxyReference` concept is used for the `reference` type of iterators that cannot return a `value_type&`. The `ProxyReference` for a `value_type` behaves in several ways just like `value_type&`. 

## Refinement of
`CopyAssignable`, `Destructible` 

## Associated types
`ProxyReference` objects are used as the type of dereferenced `InputIterator` and `OutputIterators`, the valid expressions of the `ProxyReference` depend on the mutability of the Iterator.

## Notation
`I` is the associated `Iterator` type
`it` is and object of `I`
`pr` is a `ProxyReference` object.
`v` is an object of `std::iterator_traits<I>::value_type`.
`w` can be any type. 
`os` and `is` are standard input and output streams.

## Definitions
## Valid expressions
For all InputIterators:
```cpp
auto pr = *it;
value_type v = pr;
auto v = static_cast<value_type>(pr);
os << pr;
```
For Mutable iterators only:
```cpp
pr = v; 
pr++; 
++pr; 
--pr;
pr--;
pr -= w; 
pr += w;
pr *= w;
pr /= w;
pr %= w;
pr &= w;
pr |= w;
pr >>= w;
pr <<= w;
pr >> is;
```

## Expression Semantics
|Expression|Semantic|
|----------|--------|
|`v = pr`| Equivalent to `v = static_cast<value_type>(pr)`|
|`auto v = static_cast<value_type>(pr)`| v will hold the value referred to by pr|
|`os << pr`|Equivalent to `os << static_cast<value_type>(pr)`|
|`pr = v`|Assigning the value of v to the subject of the ProxyReference|
|`pr++`| Equivalent to `pr = static_cast<value_type>(pr) + 1`|
|`pr--`| Equivalent to `pr = static_cast<value_type>(pr) - 1`|
|`++pr`| Equivalent to `{auto temp = pr; pr = static_cast<value_type>(pr) + 1}`|
|`--pr`| Equivalent to `{auto temp = pr; pr = static_cast<value_type>(pr) - 1}`|
|`pr -= w`|Equivalent to `pr = static_cast<value_type>(pr) - w`|
|`pr += w`|Equivalent to `pr = static_cast<value_type>(pr) + w`|
|`pr *= w`|Equivalent to `pr = static_cast<value_type>(pr)* w`|
|`pr /= w`|Equivalent to `pr = static_cast<value_type>(pr) / w`|
|`pr %= w`|Equivalent to `pr = static_cast<value_type>(pr) % w`|
|`pr &= w`|Equivalent to `pr = static_cast<value_type>(pr) & w`|
|`pr != w`|Equivalent to `pr = !static_cast<value_type>(pr)`|
|`pr >>= w`|Equivalent to `pr = static_cast<value_type>(pr) >> w`|
|`pr <<= w`|Equivalent to `pr = static_cast<value_type>(pr) << w`|
|`pr >> is`|Equivalent to `static_cast<value_type>(pr) >> is`|
 
## Complexity guarantees
Not applicable

## Invariants
Not applicable

## Models
`ProxyReferences` are implemented as the `reference` type of most iterators in the Blink Raster library. The actual class used is considered an implementation detail. 

## Notes
One limitation is that the `ProxyReference` is not required to be castable to a plain reference. Hence, the following will not compile:

```cpp
#include <blink/raster/raster.h>
#include <iostream>

auto ras = blink::raster::open<int>("demo.tif");
for(int& i : raster) // will not compile as the reference type of raster is not int& 
{
  std::cout << i * 2 << std::endl; 
}
```

even though the following will:

```c++
#include <blink/raster/raster.h>
#include <iostream>

auto ras = blink::raster::open<int>("demo.tif");
for(auto&& i : raster) // i will be a a ProxyReference
{
  std::cout << i * 2 << std::endl; // i will be cast to int.
}
```

A second limitation becomes apparent when the context does not make clear that the `ProxyReference` must be cast to the `value_type`:

```c++
#include <blink/raster/raster.h>

template<class T>
T square(const T& value){ return value * value };

int main()
{
  auto ras = blink::raster::open<int>("demo.tif");
  for(auto&& i : ras) 
  {
    // i = square(i);   // would not compile as the template 
                        // argument T would resolve to the
                        // reference type and not the value type.

    i = square(static_cast<int>(i)); // does compile 
  }
  return 0;
}
```

## See also
Not applicable