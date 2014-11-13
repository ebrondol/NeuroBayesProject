#!/bin/bash

# The intention of this script is to make the influences of Preproflags visible

# Recreate file to save output in
logfile="output/check_preproflags.log"
printf "Logfile of check_preproflags.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
resultfile="output/check_preproflags_results.log"
printf "" > $resultfile
echo "Created Resultfile in $resultfile"
mkdir -p output/preproplots

# Declare Variables
varoption="optionfiles/workvar.opt" 
cp optionfiles/variables1.opt $varoption # Enter the chosen variablefile
chmod a+w $varoption
stringoption="optionfiles/workstring.opt"
cp optionfiles/string2.opt $stringoption # Enter the chosen stringgenerationfile
chmod a+w $stringoption
osed=`echo ./work_scripts/oversed.sh`

declare -i -r NVAR=`sed -n '5 p' $varoption` 
declare -i it=1 iarray=(1 2) jarray=(2 3 4 5)

# Switch every variable off
while (( $it <= $NVAR )) 
do
    $osed "${it}1 c\
0" $varoption
    it+=1
done
# Do zero iteration training
$osed "/^NTrainingIter / c\
NTrainingIter 1" $stringoption

it=1
while (( $it <= $NVAR )) 
do
    $osed "${it}1 c\
0" $varoption
    varname=`sed -n "${it}2 p" $varoption`
    printf "\n${varname}\n" >> $resultfile
    for i in ${iarray[*]}
    do
	for j in ${jarray[*]}
	do
            if (( j == 2 && i != 1 )); then continue
            elif (( j == 3 && i != 2 )); then continue; fi
	    $osed "${it}8 c\
${i}${j}" $varoption
# Run sh_run.C and write stdout to logfile
	    echo "Running sh_run.C for individual preprocessing flag $i$j of ${varname}, output is written to $logfile"
	    time { root -l -q sh_run.C >> $logfile; }
# Analyse and make plots
	    root -l -q -b plot_preprocessing.C\(\"$varname\"\)
	    printf "   ${i}${j}  %7s\n" `tail -n 1 output/tamsout.ascii`
	done
    done
    $osed "${it}1 c\
0" $varoption
    it+=1
done

# show that script has finished successfully
echo check_preproflags.sh finished