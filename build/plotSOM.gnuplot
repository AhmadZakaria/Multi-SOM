set title "M-SOM"
plot "train_PA-D.dat" using 1:2 with points title "input" ,\
        "PA-D.net0"  with linespoints title "SOM:0" , \
        "PA-D.net1" with linespoints title "SOM:1" , \
        "PA-D.net2" with linespoints title "SOM:2" , \
        "PA-D.net3" with linespoints title "SOM:3"
