# Filesystems, btrees, 2-3-4 trees, wow!

Currently the project contains a partial implementaion of 2-3-4 trees,
bunch of tests and helper functions for examining the trees and traversals.

To finish the trees I need to implement the delete operation.

# Building and running

Project was only tested on linux so far. To build it, you need gcc with support
for c++11 and cmake.

To run the project:

    $ mkdir build
    $ cd build && cmake ..

From here you can run it or test it.

    $ make test

Build setup was heavily inspired by this [blogpost](http://www.kaizou.org/2014/11/gtest-cmake/).
