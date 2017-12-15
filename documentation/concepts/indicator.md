# Indicator

## Summary
The `Indicator` concept is for objects that compute running statistics over a stream of incoming and outgoing observations. Incoming and outgoing observations can be presented individually, or as a collective summarized by the same `Indicator` implementation

## Refinement of
`CopyAssignable`, `Destructible` 

## Associated types
The `sample_type` of an `Indicator` is the type of the observed objects. The `weight_type` of the `Indicator` is used to associate weights to incoming our outgoing observations. The `result_type` is the type of the calculated statistc over the samples. 

## Notation

## Definitions
`indicator` is a object of a type that implements `Indicator`, `other` has the same type as `indicator`, `v` is an object of `value_type`, `w` is an object of `weight_type`, `r` is an object of `result_type`.

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
using value_type = indicator_traits<indicator_type>::value_type;
using weight_type = indicator_traits<indicator_type>::weight_type;
using result_type = indicator_traits<indicator_type>::result_type;
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

## Complexity guarantees

## Invariants
In principle operations are retractable (subtracting an earlier added observation is equivalent to not having added the observation at all) and communicative (the order of subtracting and adding does not affect results; as long as subtracted observations have been added before. However, differences may occur due to rounding

## Models

## Notes
In many cases the `result_type` will be some `optional<T>` to allow for the circumstance that there are no observations at all.
 
## See also
