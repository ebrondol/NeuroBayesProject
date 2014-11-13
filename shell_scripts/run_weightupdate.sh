#!/bin/bash

# Script to compare the FoM for different numbers before weight update

# Recreate file to save output in
logfile="output/run_weightupdate.log"
printf "Logfile of run_weightupdate.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
resultfile="output/weightupdate.ascii"
printf "" > $resultfile
echo "Created Resultfile in $resultfile"

# Declare Variables
varoption="optionfiles/workvar.opt" 
cp optionfiles/variables1.opt $varoption # Enter the chosen variablefile
chmod a+w $varoption
stringoption="optionfiles/workstring.opt"
cp optionfiles/string2.opt $stringoption # Enter the chosen stringgenerationfile
chmod a+w $stringoption
osed=`echo ./work_scripts/oversed.sh`
declare -i update=(3 6 10 15 22 30 40 50 70 100 150 200 300 500)

for iter in ${update[*]}
do
    $osed "/^WeightUpdate/ c\
WeightUpdate $iter" $stringoption
# Run sh_run.C and write stdout to logfile
    echo; echo Running sh_run.C with weight update every $iter iterations, output is written to $logfile
    time { root -l -q sh_run.C >> $logfile; }

# Analyse outcome with TAMS
    echo Calculating Figure of Merit
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
    fom[$iter]=`tail -n 1 output/tamsout.ascii`
# Write to resultfile
    printf "%3i %7s\n" $iter ${fom[$iter]} >> $resultfile
done

echo "The results for weight update after different numbers of iterations, as saved in $resultfile"
cat $resultfile
echo

# Plot
root -l -q work_scripts/plot_1D_2c.C+\(\"weightupdate\",\"\",\""Weight Update after"\",\""FoM"\"\)

# show that script has finished successfully
echo run_weightupdate.sh finished