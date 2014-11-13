#!/bin/bash

# Script to find the perfect Learning speed (with no limit) and iterations

# Recreate file to save output in
logfile="output/run_speed_iter.log"
printf "Logfile of run_speed_iter.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
# resultfile="output/speed_iter_results.log"
# printf "" > $resultfile
# echo "Created Resultfile in $resultfile"
shortfile="output/speed_iter_short.ascii"
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

speed=( 0.1 0.2 0.4 0.6 0.8 1. 1.2 1.4 1.6 2 2.4 2.8 )
iter=( 10 40 70 100 130 160 190 220 250 )
#$osed "/^LimitLearningSpeed/ c\
#LimitLearningSpeed 1000" $stringoption

# Write first line, which serves as legend for the horizontal variable
printf "%8s " 'iter/spe' > $shortfile # Every column should be eight characters long +1space
for sp in ${speed[*]}
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
# LearningSpeed
    for sp in ${speed[*]}
    do
	$osed "/^LearningSpeed/ c\
LearningSpeed $sp" $stringoption
# Run sh_run.C and write stdout to logfile
	echo; echo "Running sh_run.C for Learn Speed multiplied by $sp with $it iterations, output is written to $logfile"
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
root -l -q work_scripts/plot_2D.C+\(\"speed_iter_short\",\"\",\""Learning Speed"\",\""Iterations"\",0,0\)

# show that script has finished successfully
echo run_speed_iter.sh finished