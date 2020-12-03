reset
set ylabel 'time(nsec)'
set xlabel 'experiment'
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot [2:][:500]'out.txt' using 1:2 with points title 'virtual ring buf', \
'out.txt' using 1:3 with points title 'normal ring buf' , 
