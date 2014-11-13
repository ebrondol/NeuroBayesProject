#!/bin/bash

# Script to find the perfect Learning Speed Limit in combination with iterations

# Recreate file to save output in
logfile="output/run_limit_iter.log"
printf "Logfile of run_limit_iter.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
# resultfile="output/limit_iter_results.log"
# printf "" > $resultfile
# echo "Created Resultfile in $resultfile"
shortfile="output/limit_iter_short.ascii"
printf "" > $shortfile
echo "Created short resultfile in $shortfile"

# Declare Variables
varoption="optionfiles/workvar.opt" 
cp optionfiles/variables3.opt $varoption # Enter the chosen variablefile
chmod a+w $varoption
stringoption="optionfiles/workstring.opt"
cp optionfiles/string3.opt $stringoption # Enter the chosen stringgenerationfile
chmod a+w $stringoption
osed=`echo ./work_scripts/oversed.sh`

limit=( 0.01 0.0316 0.1 0.316 1 3.16 10 31.6 100 316 1000 )
iter=( 10 40 70 100 130 160 190 220 250 )
#$osed "/^LimitLearningSpeed/ c\
#LimitLearningSpeed 1000" $stringoption

# Write first line, which serves as legend for the horizontal variable
printf "%8s " 'lim /spe' > $shortfile # Every column should be eight characters long +1space
for sp in ${limit[*]}
do
    printf "%8s " $sp >> $shortfile
done
printf "\n" >> $shortfile

# Iteration
for it in ${iter[*]}
do
    printf "%-8s " $it >> $shortfile
    $osed "/^NTrainingIter/ c\
NTrainingIter $it" $stringoption
# LearningLimit
    for sp in ${limit[*]}
    do
	$osed "/^LimitLearningSpeed/ c\
LimitLearningSpeed $sp" $stringoption
# Run sh_run.C and write stdout to logfile
	echo; echo "Running sh_run.C for Learn Speed Limit of $sp with $it iterations, output is written to $logfile"
	time { root -l -q sh_run.C >> $logfile; }
# Analyse outcome with TAMS
	echo "Calculating Figure of Merit"
	root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
	root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
	fom=`tail -n 1 output/tamsout.ascii`
# Write to shortfile
	printf "%8s " $fom >> $shortfile
    done
    printf "\n" >> $shortfile
done

echo; echo "Analysis finished, results as shown in $shortfile:"
cat $shortfile; echo

# Make plot
root -l -q work_scripts/plot_2D.C+\(\"limit_iter_short\",\"\",\""Speed Limit"\",\""Iterations"\",1,0\)

# show that script has finished successfully
echo run_limit_iter.sh finished