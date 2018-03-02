# RasterView

## Summary
A `RasterView` is a `Raster` that it is copyable with constant time complexity. A copy of a `RasterView` refers to the same data as the original.

## Refinement of
[`Raster`](./raster.md),  `View` as proposed in the [Working Draft, C++ Extensions for Ranges](http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/n4569.pdf)

## Expression Semantics
A copy of a view refers to the same data as the original. The cost of copying does not depend on the size of the raster.