---
layout: single
title: null
sidebar: 
  nav: docs
---
# Rationale
The purpose of this library is to facilitate computations on geographical raster data efficiently using idiomatic C++. 

In particular it achieves that by combining [Expression Template](https://web.archive.org/web/20050210090012/http://osl.iu.edu/~tveldhui/papers/Expression-Templates/exprtmpl.html ) technique and the [Range](https://ericniebler.github.io/std/wg21/D4128.html ) concept. By exposing raster data as ranges, values in a raster can be iterated over using the C++11 range-based for-loop. It hides the complexity of raster data access, such as caching blocks of data, behind the most standard and basic interface.  The Expression Template technique means that operations on raster data can be performed without unnecessarily creating temporary datasets for intermediate results, making operations more efficient and robust than existing approaches. A number of elementary spatial operators provide transformed views of existing raster datasets, that also do not require creating temporary datasets, such as iterating over a subset of the raster, or iterating over cells in a raster at a spatial offset. These elementary spatial transforms can be combined into more complex spatial operation, including highly efficient moving window analysis.

The library is intended to especially useful for environmental modelling, including Map Algebra operations, Cellular Automata modelling, spatial statistics and multi-scale analysis.

Prefer a PowerPoint introduction? Check out the FOSS4G UK 2018 presentation on [Slideshare](https://www.slideshare.net/AlexHagenZanker/pronto-raster-v3).

The library offers the following main functionality:

## Accessing raster data
- open and create permanent or temporary geographical raster data sets
- expose raster data sets as raster views
- access raster views cell-by-cell using standard conforming iterators and range-based for loops
- deal with nodata values gracefully by having `optional<T>` value types.
- apply type erasure on raster views to support runtime polymorphic operations

## Elementary spatial transforms
- create raster views that expose a subset (range of rows and columns) of other raster views
- create raster views that pad other raster views with leading and trailing rows and columns
- create raster views that expose other raster views at a given row and column offset
- create raster views that iterate over pairs of adjacent cells (horizontal and vertical edges)

## Raster algebra
- create raster views that apply any function cell-by-cell on one or more raster views (transform function)
- create raster view that apply a function on a mix of raster data and non-raster (scalar) data
- Overload arithmetic(+,-, *, /, etc.) and logical (>, <, &&, \|\|, etc.) operators on  cell-by-cell operations on raster views.

## Moving windows
- create raster views that iterate over indicators calculated for subrasters centered on each cell
- create raster views that iterate over indicators calculated for circular windows centered on each cell
- create raster views that iterate over subraster views centered on each cell
