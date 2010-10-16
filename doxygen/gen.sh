#!/bin/bash

echo This should be run from the project root.

rm -Rf docs/html
doxygen
cp -v doxygen/* docs/html/
rm docs/html/gen.sh
