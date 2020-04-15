# SOM-Iris-dataset
Single layout 2d artificial neural network for Iris-dataset, based on Self-Organizing map model

# Dependencies
This projet does not have any particulary dependencies, Make is highly recommanded in order to use the Makefile, otherwise you will have to compile by yourself.<br>
This projet does run on windows and linux environement, however it's not guaranteed to run on macOS since I don't have an mac, so I haven't made any test on this operating system<br>

# Compile and run
Simply use the make command 
$make  >> to compile
$make run >> to run 
$make clean >> to delete binary files

# Functions
Most function have comments, and a lot of print is left on comment in order to check data at that moment<br>
Every function is independent, which mean you can take it and use them for further projet, few modifications are needed if you data 
is different of the given exemple.<br>

# Sources
Iris data is retrieved from this website <br>
http://archive.ics.uci.edu/ml/datasets/iris

# Exemple of possible result
./exec iris.data<br>
1 1 1 0 2 2 2 2 3 3<br>
1 1 1 0 2 2 0 3 3 3<br>
1 1 1 0 2 2 2 3 3 3<br>
1 1 1 0 2 2 2 3 3 3<br>
1 0 1 0 2 2 2 2 0 3<br>
1 1 1 0 2 2 2 3 3 3<br>
0=9     1=17    2=19    3=15<br>
