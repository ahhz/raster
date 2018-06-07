# Indicator

## Summary
The `Indicator` concept is for objects that compute running statistics over a stream of incoming and outgoing samples. Incoming and outgoing samples can be presented individually, or collectively in the form of an object of the same `Indicator` type.

## Refinement of
`CopyAssignable`, `Destructible` 

## Associated types
The `sample_type` of an `Indicator` is the type of the samples over which the statistic is calculated. 
The `weight_type` of an `Indicator` is the type by which samples are weighted. 
The `result_type` is the type of the calculated statistic over the samples. 

## Notation
`indicator_type` is a class that models the `Indicator` concept. 
`indicator` and `other` are both objects of the type `indicator_type`. 
`v` is an object of `value_type`, 
`w` is an object of `weight_type`, 
`r` is an object of `result_type`.

## Definitions
n.a.

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
|`indicator.add(v,w)`|update the indicator with an additional sample of value `v` and weight `w`|
|`indicator.add(v)`|update the indicator with an additional sample of value `v` and unity weight|
|`indicator.subtract(v,w)`|update the indicator by undoing the effect of `indicator.add(v,w)`|
|`indicator.subtract(v)`|update the indicator by undoing the effect of `indicator.add(v)`|
|`indicator.add(other,w)`|update the indicator to include the effect of all samples reflected in `other`, applying a further weight `w`|
|`indicator.add(other)`|update the indicator to include the effect of all samples reflected in `other`|
|`indicator.subtract(other,w)`|update the indicator by undoing the effect of `indicator.add(other,w)`|
|`indicator.subtract(other)`|update the indicator by undoing the effect of `indicator.add(other)`|
|`r = indicator.extract()`|get the value of the statistic for the samples currently in the `indicator`|

## Complexity guarantees
n.a.

## Invariants
Conceptually, operations must be retractable (subtracting an earlier added sample is equivalent to not having added the observation at all) and communicative (the order of subtracting and adding does not affect results). However, differences may occur due to rounding. Samples should not be retracted that have not been added before.

## Models
The Pronto Raster library includes a number of classes that model `Indicator` in the include/pronto/raster/indicator/ directory. However, these have not been documented yet. 
 
## Notes
In many cases the `result_type` will be some `optional<T>` to allow for the circumstance that there are insufficient samples to extract the indicator.
 
## See also
