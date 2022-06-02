# raster_iterator

## Summary
An iterator for a `raster` object.

## Refinement of
A C++ iterator (since C++20) is also a `raster_iterator` if it iterates over the elements of a `raster` row-by-row, and within each row column-by-column.

## Associated types

## Notation

## Definitions

## Valid expressions

## Expression semantics
The iterator iterates over the elements of the raster row-by-row and within each row column-by-columns. Otherwise the valid expressions and semantics follow those of the type of `iterator` it models.

## Complexity guarantees

## Invariants

#### Models

## Notes
The reason that a`raster_iterator` is an additional requirement to iterator concepts introduced in C++20 is that most iterator concepts pre-C++20 require`reference` type to be equal to `value_type&` (with very limited exceptions).
