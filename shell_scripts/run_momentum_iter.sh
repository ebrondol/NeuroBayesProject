#!/bin/bash

# Script to find the perfect Momentum in combination with iterations

# Recreate file to save output in
logfile="output/run_momentum_iter.log"
printf "Logfile of run_momentum_iter.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
# resultfile="output/momentum_iter_results.log"
# printf "" > $resultfile
# echo "Created Resultfile in $resultfile"
shortfile="output/momentum_iter_short.ascii"
printf "" > $shortfile
echo "Created short resultfile in $shortfile"

# Declare Variables
varoption="optionfiles/workvar.opt" 
cp optionfiles/variables3.opt $varoption # Enter the chosen variablefile
chmod a+w $varoption
stringoption="optionfiles/workstring.opt"
cp optionfiles/string4.opt $stringoption # Enter the chosen stringgenerationfile
chmod a+w $stringoption
osed=`echo ./work_scripts/oversed.sh`

momentum=( 0 0.01 0.02 0.04 .08 0.16 0.32 0.58 0.92 )
iter=( 80 100 120 140 160 180 200 220 240)

# Write first line, which serves as legend for the horizontal variable
printf "%8s " 'iter/mom' > $shortfile # Every column should be eight characters long +1space
for mom in ${momentum[*]}
do
    printf "%8s " $mom >> $shortfile
done
printf "\n" >> $shortfile

# Iteration
for it in ${iter[*]}
do
    printf "%-8s " $it >> $shortfile
    $osed "/^NTrainingIter/ c\
NTrainingIter $it" $stringoption
# Momentum
    for mom in ${momentum[*]}
    do
	$osed "/^Momentum/ c\
Momentum $mom" $stringoption
# Run sh_run.C and write stdout to logfile
	echo; echo "Running sh_run.C for a momentum of $mom with $it iterations, output is written to $logfile"
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
root -l -q work_scripts/plot_2D.C+\(\"momentum_iter_short\",\"\",\""Momentum"\",\""Iterations"\",1,0\)

# show that script has finished successfully
echo run_momentum_iter.sh finished