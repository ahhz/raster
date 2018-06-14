# IndicatorGenerator

## Summary
The `IndicatorGenerator` concept is introduced to be able to work with indicators that do not have a default constructor, or for which the default constructor needs to be bypassed. It is an object can be used to spawn initialized indicators. Objects of `IndicatorGenerator` models are passed as function arguments by value and therefore are required to be trivially copyable.

## Refinement of
`CopyAssignable`, `Destructible` 

## Associated types
A model of `IndicatorGenerator` produces models of `Indicator`. Therefore each is associated with an `Indicator` model

## Notation
`indicator_generator_type` is a model of `IndicatorGenerator`.
`indicator_generator` and `other` are objects of the type `indicator_generator_type`.
`indicator_type` is the associated `Indicator` model.

## Definitions


## Valid expressions
```cpp
indicator_type i = indicator_generator();
indicator_generator_type ig = indicator_generator;
other = indicator_generator;
```
## Expression Semantics

|Expression|Semantic|
|----------|--------|
|`indicator_type i = indicator_generator();|generate an initialized indicator`|
|`indicator_generator_type ig = indicator_generator;`| copy-construct the generator. This must be a small cost operation.|
|`other = indicator_generator;`| copy-assign the generator. This must be a small-cost operation.|

## Complexity guarantees
n.a.

## Invariants
The following two procedures must be equivalent:

```cpp
// First procedure
 auto a = indicator_generator();
 auto b = indicator_generator();
```

```cpp
 // Second procedure
 auto a = indicator_generator();
 auto b = a; // provided that the indicator_type is copy-constructible.
 ```
## Models
Each `Indicator` in Pronto Raster has an associated `IndicatorGenerator`

## Notes
When the associated `Indicator` model has a default constructor that should be used, the `IndicatorGenerator` can be implemented as simple as:

```cpp
struct some_indicator_generator
{
	some_indicator_type operator()() const
	{
		return some_indicator_type{};
	}
};
```

## See also
[Indicator](./Indicator.md)

