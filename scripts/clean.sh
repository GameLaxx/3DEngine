#!/bin/bash

find . -name "*.o" -exec rm -rf {} +
echo "Every object files have been deleted !"
