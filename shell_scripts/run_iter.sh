#!/bin/bash

# Script to compare the FoM for different No of Iterations


# Recreate file to save output in
logfile="output/run_iter.log"
printf "Logfile of run_iter.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
resultfile="output/iter.ascii"
printf "" > $resultfile
echo "Created Resultfile in $resultfile"

# Declare Variables
varoption="optionfiles/workvar.opt" 
cp optionfiles/variables3.opt $varoption # Enter the chosen variablefile
chmod a+w $varoption
stringoption="optionfiles/workstring.opt"
cp optionfiles/string4.opt $stringoption # Enter the chosen stringgenerationfile
chmod a+w $stringoption
osed=`echo ./work_scripts/oversed.sh`
#declare -i iterations=(3 6 10 15 22 30 40 50 70 100 150 200 300 500)
declare -i iterations=(200 400 600 800 1000 1200 1500 2000)

for iter in ${iterations[*]}
do
    $osed "/^NTrainingIter / c\
NTrainingIter $iter" $stringoption
# Run sh_run.C and write stdout to logfile
    echo; echo Running sh_run.C with $iter iterations, output is written to $logfile
    time { root -l -q sh_run.C >> $logfile; }

# Analyse outcome with TAMS
    echo Calculating Figure of Merit
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
    fom[$iter]=`tail -n 1 output/tamsout.ascii`
# Write to resultfile
    printf "%3i %7s\n" $iter ${fom[$iter]} >> $resultfile
done

echo "The results for different number of iterations, as saved in $resultfile"
cat $resultfile
echo

# Plot
root -l -q work_scripts/plot_1D_2c.C+\(\"iter\",\"\",\""Number Iterations"\",\""FoM"\"\)

# show that script has finished successfully
echo run_iter.sh finished