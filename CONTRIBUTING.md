# Contributing

## How do I submit a bug report?
To submit a bug report, please use the Issues tracker of Github.  

## How do I submit a patch
To submit a patch to fix a bug please fork the repository, make your modifications and submit a Pull Request. If your patch is non-trivial, please add a comment explaining what the bug entails and how your patch fixes it. If you can it would be great if you could add a test to the Test directory that fails without your patch, and passes with your patch. 

If you are interested in submitting a patch for a new feature, please discuss the scope and nature of the new feature first (see below). When submitting new features make sure to include documentation, an example program and one or more tests.

Fixes and additions to the tests, documentation and benchmarks are always welcome. 

## How do I start a discussion
To start a discussion or pose questions, please use the Issues tracker of Github.
  
## Coding style
For the coding style, please use the following guidelines where possible:
- Use descriptive names
- Use two spaces as indents
- All variable names in underscore
- m_ prefix for member variables
- Template parameters capitalized
- Macros all-caps
- Copyright statement (MIT) above each file
- Preferably one class per header file
- Header only 
- Use `#pragma once` instead of include guards
- Use `using` directive rather than `typedef`.
- 80 characters per line

The library should target C++11 and only use C++17 features when there is a Boost alternative available. The library should only use Boost features if there is a C++17 alternative available.   
 
Currently, the library upholds this style pretty well. Where it does not, patches are welcome.  

## Documentation style
The documentation aims to follow the SGI template used for their documentation of the STL. Regrettably, this is no longer at the www.sgi.com website, but there are still mirrors available: 
- [Mirror 1](http://www-f9.ijs.si/~matevz/docs/www.sgi.com/tech/stl/doc_introduction.html)
- [Mirror 2](https://www.lix.polytechnique.fr/~liberti/public/computing/prog/libstdc++/doc_introduction.html)

The essence is that for each concept, function and class there is a separate MarkDown file that follows a fixed format. 

This choice for documentation was inspired by CppCon 2017: Robert Ramey “How to Write Effective Documentation for C++ Libraries...” [youtube](https://www.youtube.com/watch?v=YxmdCxX9dMk).

Currently, the documentation is not complete and does not fully follow this format. Patches are welcome.  If there are particular aspects of the library unclear, please raise an Issue.

 