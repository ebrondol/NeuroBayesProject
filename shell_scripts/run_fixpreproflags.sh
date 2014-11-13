#!/bin/bash

# Script to find the perfect Preprocessing flags

# Recreate file to save output in
logfile="output/run_fixpreproflags.log"
printf "Logfile of run_fixpreproflags.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
# resultfile="output/fixpreproflags_results.log"
# printf "" > $resultfile
# echo "Created Resultfile in $resultfile"
shortfile="output/fixpreproflags_short.ascii"
printf "" > $shortfile
echo "Created short resultfile in $shortfile"

# Declare Variables
varoption="optionfiles/workvar.opt" 
cp optionfiles/variables1.opt $varoption # Enter the chosen variablefile
chmod a+w $varoption
stringoption="optionfiles/workstring.opt"
cp optionfiles/string2.opt $stringoption # Enter the chosen stringgenerationfile
chmod a+w $stringoption
osed=`echo ./work_scripts/oversed.sh`

declare -i -r NVAR=`sed -n '5 p' $varoption` 
declare -i iarray=(1 2) jarray=(2 3 4 5)

for i in ${iarray[*]}
do
    for j in ${jarray[*]}
    do
        if (( j == 2 && i != 1 )); then continue
        elif (( j ==3 && i != 2 )); then continue; fi
	declare -i it=1
# Switch every preproflag to ij
	while (( $it <= $NVAR )) 
	do
	    $osed "${it}8 c\
${i}${j}" $varoption
	    it+=1
	done
# Run sh_run.C and write stdout to logfile
	echo;echo Running sh_run.C for every preproflag set to $i$j, output is written to $logfile
	time { root -l -q sh_run.C >> $logfile; }
# Analyse outcome with TAMS
	echo Calculating Figure of Merit for training data
	root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
	echo Calculating Figure of Merit for test data
	root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
	fom=`tail -n 1 output/tamsout.ascii`
# Write to resultfile
	printf "%2s  %7s\n" "$i$j" $fom >> $shortfile
    done
done

echo; echo "Summarized results as saved in $shortfile:"
cat $shortfile; echo

# show that script has finished successfully
echo run_fixpreproflags.sh finished