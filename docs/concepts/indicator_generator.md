# IndicatorGenerator
The IndicatorGenerator concept is introduced to be able to work with indicators that do not have a default constructor, or for which the default constructor needs to be bypassed. It is an object can be used to spawn initialized indicators.
The following operation is supported:
```cpp
indicator = indicator_generator();
```
where `indicator` is an object of some `indicator_type`  which is an `Indicator` implementation, `indicator_generator` is an object of an `IndicatorGenerator` implementation specifically for `indicator_type`.  
