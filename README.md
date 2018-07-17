CSCE 590 Assignment 1
=====================

This software uses OpenCV to do several basic image processing operations for CSCE 590.

To make the code and produce a PDF, cd into a directory `Release` and run `cmake ..` followed by `make An` where `n` is assignment number. The resultant pdf will be in the build directory.

To build the code, make each assignment question `An_Qm` where `n` is assignment number and `m` is question number. The binary should be in the bin directory and must be provided path to the assignment directory (or other directory containing `images`)

To create a gzipped tar for assignment `n`, make `An_compress` which will create `An.tar.gz` with generated binary data in directory `tar_binaries`.
