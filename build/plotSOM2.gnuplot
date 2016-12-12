set title 'random data'
datafile = 'random_train.dat'
stats datafile
plot for [IDX=1:STATS_blocks] datafile index (IDX-1) u 1:2 t columnheader(1) ,   "PA-D.net0"  with linespoints title "SOM0" ,   "PA-D.net1" with linespoints title 'SOM1',  'PA-D.net2' with linespoints title 'SOM2' , "PA-D.net3" with linespoints title 'SOM3' 
