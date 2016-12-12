# TNN- MULTI-SOM
Multi-SOM implementation as a part of Technical Neural Networks course at University of Bonn

## Compilation:
```sh
cd build
cmake ..
make
```

To run the program, simply call the executable.
```sh
./TNN-D
```

## Plotting the results
There are two tasks, task1 (using the given data), and task2 (using randomly generated data.

To plot the results using gnuplot:
for the first task
```sh
gnuplot> load "plotSOM.gnuplot"
```
and for the task task
```sh
gnuplot> load "plotSOM2.gnuplot"
```
