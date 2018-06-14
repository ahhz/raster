# Indicator

## Summary
The `Indicator` concept is for objects that compute running statistics over a stream of incoming and outgoing samples. Incoming and outgoing samples can be presented individually, or as a collective summarized by an object of the same `Indicator` type.

## Refinement of
`CopyAssignable`, `Destructible` 

## Associated types
The `sample_type` of an `Indicator` is the type of the samples over which the statistic is calculated. The `weight_type` of the `Indicator` is used to associate weights to incoming our outgoing samples. The `result_type` is the type of the calculated statistic over the samples. 

## Notation

## Definitions
`indicator_type` models the `Indicator` concept. `indicator` and `other` are both objects of the type `indicator_type`, `v` is an object of `value_type`, `w` is an object of `weight_type`, `r` is an object of `result_type`.

## Valid expressions
```cpp
indicator.add(v, w);
indicator.add(v);
indicator.subtract(v, w);
indicator.subtract(v);
indicator.add(other);
indicator.add(other, w);
indicator.subtract(other);
indicator.subtract(other, w);
r = indicator.extract();
using value_type = typename indicator_traits<indicator_type>::value_type;
using weight_type = typename indicator_traits<indicator_type>::weight_type;
using result_type = typename indicator_traits<indicator_type>::result_type;
```

## Expression Semantics

|Expression|Semantic|
|----------|--------|
|`indicator.add(v,w)`|use the value `v` and weight `w` to update the indicator|
|`indicator.add(v)`|use the value `v`|
|`indicator.subtract(v,w)`|negate the effect of one observation with value `v` and weight `w` on the indicator|
|`indicator.subtract(v)`|negate the effect of one observation with value `v` on the indicator|
|`indicator.add(other,w)`|update the indicator to include the effect of all observations reflected in `other`, weighted by `w`|
|`indicator.add(other)`|update the indicator to include the effect of all observations reflected in `other`|
|`indicator.subtract(other,w)`|negate the effect of all observations reflected in `other`, weighted by `w`|
|`indicator.subtract(other)`|negate the effect all observations reflected in `other`|
|`indicator.extract()`|get the value of the statistic for the samples currently in the `indicator`|

## Complexity guarantees
n.a.

## Invariants
Conceptually, operations must be retractable (subtracting an earlier added sample is equivalent to not having added the observation at all) and communicative (the order of subtracting and adding does not affect results). However, differences may occur due to rounding when floating point numbers are involved. Samples should not be retracted that have not been added before.

## Models
The Pronto Raster library includes a number of classes that model `Indicator` in the include/pronto/raster/indicator/ directory. However, these have not been documented yet. 
 
## Notes
In many cases the `result_type` will be some `optional<T>` to allow for the circumstance that there are no samples at all.
 
## See also
