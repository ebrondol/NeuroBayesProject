#!/bin/bash

# Basic script to compare the impact of the elimination of different variables
# Build to provide the capacity of looping

# Recreate file to save output in
logfile="output/run_var_down.log"
printf "Logfile of run_var_down.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
resultfile="output/var_down_long.log"
printf "Result file of run_var_down.sh on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $resultfile
printf "%18s %6s\n" "Variable name" "FoM" >> $resultfile
echo "Results are written to $resultfile"
shortfile="output/var_down_short.ascii"
printf "" > $shortfile

# Get the initial file into the reading position
optionfile="optionfiles/workvar.opt"
cp optionfiles/initial.opt $optionfile
chmod a+w $optionfile

osed=`echo ~/private/OptionFileGeneration/oversed.sh`
declare -i -r NVAR=`sed -n '5 p' $optionfile`
declare -i uvar=$NVAR

# Calculate FoM with all variables
printf "Using all $uvar variables\n" >> $resultfile
 # Run sh_run.C and write stdout to logfile
echo; echo Running sh_run.C for all $uvar variables, output is written to $logfile
time { root -l -q sh_run.C >> $logfile; }
 # Analyse outcome with TAMS
echo Calculating Figure of Merit
root -l -q TAMS/sh_tams.C 
tams=`tail -n 1 output/tamsout.ascii`
printf "%18s   %7s\n" "Figure of Merit:" $tams >> $resultfile
fom[$uvar]=$tams

# Loop until the last variable
while (( $uvar > 1 ))
do
    uvar=$uvar-1
    fom[$uvar]=0
    declare -i elimvar[$uvar] it=1
    printf "\nAll possible sets of $uvar variables\n" >> $resultfile

 # Loop for testing the Figure of Merit, eliminating everytime one variable
    while (( $it <= $NVAR ))
    do
	if [[ `sed -n "${it}1 p" $optionfile` == '0' ]]; then it=$it+1; continue; fi
 # Disable one variable
	varname=`sed -n "${it}2 p" $optionfile`
	$osed "${it}1 c\
0" $optionfile
 # Run sh_run.C and write stdout to logfile
	echo; echo Running sh_run.C for eliminated variable $varname, output is written to $logfile
	time { root -l -q sh_run.C >> $logfile; }
 # Analyse outcome with TAMS
	echo Calculating Figure of Merit
	root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
	root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
	tams=`tail -n 1 output/tamsout.ascii`
	printf "%18s   %7s\n" $varname $tams >> $resultfile
 # If the FoM is higher then before, save the index and the optionfile
	if [[ $(echo "$tams > ${fom[$uvar]}"|bc) == '1' ]]
	then
	    fom[$uvar]=$tams
	    elimvar[$uvar]=$it
	    cp $optionfile optionfiles/varout/best${uvar}.opt
	fi
 # Prepare for next loop
	$osed "${it}1 c\
1" $optionfile  
	it=$it+1
    done

 # Change the optionfile for the next loop
    cp optionfiles/varout/best${uvar}.opt $optionfile
 # Outputs
    varname[$uvar+1]=`sed -n "${elimvar[$uvar]}2 p" $optionfile`
    echo; echo Eliminating Variable ${elimvar[$uvar]}, ${varname[$uvar]}.
    echo The best Figure of Merit for $uvar Variables is ${fom[$uvar]}; echo
    printf "Variable ${varname[$uvar+1]} eliminated, FoM ${fom[$uvar]}\n" >> $resultfile
done
# Adding last variable to varname
it=0
while (( $it <= $NVAR )); do
    it+=1
    if [[ `sed -n "${it}1 p" $optionfile` == '1' ]]
    then
	varname[$uvar]=`sed -n "${it}2 p" $optionfile`
	echo Addded last variable ${varname[$uvar]} to list of Variables
	continue;
    fi
done

# Creating shortfile
it=1
while (( $it <= $NVAR ))
do
    printf "%2i %7s %18s" $it ${fom[$it]} ${varname[$it]} >> $shortfile
    if (( $it != $NVAR )); then echo >> $shortfile; fi
    it=$it+1
done
echo "The results in short form, as saved in $shortfile:"
cat $shortfile
echo; echo

# Create plot
root -l -q work_scripts/plot_1D_3c.C\(\"var_up_short\",\"\",\""Number Variables"\",\""FoM"\"\) # Check file with $shortfile

# show that script has finished successfully
echo; echo run_var_down.sh finished