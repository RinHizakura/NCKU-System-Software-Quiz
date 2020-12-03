reset
set xlabel 'sqrt(n), n='
set ylabel 'iteration'
set terminal png font " Times_New_Roman,12 "
set xtic 100000
set xtics rotate by 45 right
set style fill transparent solid 0.50 border
set output 'runtime.png'


plot \
"out.txt" using 1:4 with linespoints linewidth 2 lc rgb "#33ff0000" title "ieee + binary search", \
"out.txt" using 1:5 with linespoints linewidth 2 title "newton", \
