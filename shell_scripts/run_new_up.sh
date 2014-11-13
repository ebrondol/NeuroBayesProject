#!/bin/bash

# Basic script to compare the impact of adding different variables
# Build to provide the capacity of looping

# Recreate file to save output in
logfile="output/run_new_up.log"
printf "Logfile of run_var_up.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
resultfile="output/new_up_long.log"
printf "Result file of new_var_up.sh on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $resultfile
printf "%18s %6s\n" "Variable name" "FoM" >> $resultfile
echo "Results are written to $resultfile"
shortfile="output/new_up_short.ascii"
printf "" > $shortfile

# Get optionfiles
mkdir -p optionfiles/varout
optionfile="optionfiles/workvar.opt"
cp optionfiles/newprepro.opt $optionfile # Take here the variablefile from run_new_down.sh to have a good start of preproflags
chmod a+w $optionfile
stringoption="optionfiles/workstring.opt"
cp optionfiles/string2.opt $stringoption # Enter the chosen stringgenerationfile
chmod a+w $stringoption

osed=`echo ~/private/OptionFileGeneration/oversed.sh`
declare -i -r NVAR=`sed -n '5 p' $optionfile`
declare -i uvar=0 it=1

while (( $it <= $NVAR )) # set every variable to off
do
    $osed "${it}1 c\
0" $optionfile
    it+=1
done

# Loop until the last variable
while (( $uvar < 8 ))  # Maximum of variables wanted for the analysis, it would take too long to test until 12
do
    uvar=$uvar+1
    fom[$uvar]=0
    declare -i elimvar[$uvar] it=1
    printf "\nAll possible sets of $uvar variables\n" >> $resultfile

# Loop for testing the Figure of Merit, adding everytime one variable
    while (( $it <= $NVAR ))
    do
	if [[ `sed -n "${it}1 p" $optionfile` == '1' ]]; then it=$it+1; continue; fi
 # Disable one variable
	varname=`sed -n "${it}2 p" $optionfile`
	$osed "${it}1 c\
1" $optionfile
 # Run sh_run.C and write stdout to logfile
	echo; echo Running sh_run.C for adding variable $varname, output is written to $logfile
	time { root -l -q sh_run.C >> $logfile 2>&1; }
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
0" $optionfile  
	it=$it+1
    done

 # Change the optionfile for the next loop
    cp optionfiles/varout/best${uvar}.opt $optionfile
 # Outputs
    varname[$uvar]=`sed -n "${elimvar[$uvar]}2 p" $optionfile`
    echo; echo Adding Variable ${elimvar[$uvar]}, ${varname[$uvar]}.
    echo The best Figure of Merit for $uvar Variables is ${fom[$uvar]}; echo
    printf "Variable ${varname[$uvar]} added, FoM ${fom[$uvar]}\n" >> $resultfile
 # Preproflags
    echo; echo Running Preproflags
    shell_scripts/run_preproflags.sh -in
    printf "\nResults Preproflags\n" >> $resultfile
    cat output/preproflags_short.ascii >> $resultfile
    printf "\n" >> $resultfile
    echo Added results to $resultfile
done

# Creating shortfile
for (( it=1; it < ${#fom[@]}; it++ ))
do
    printf "%2i %7s %18s" $it ${fom[$it]} ${varname[$it]} >> $shortfile
    if (( $it != $NVAR )); then echo >> $shortfile; fi
done
echo "The results in short form, as saved in $shortfile:"
cat $shortfile
echo; echo

# Create plot
root -l -q work_scripts/plot_1D_3c.C\(\"new_up_short\",\"\",\""Number Variables"\",\""FoM"\"\) # Check file with $shortfile

# show that script has finished successfully
echo; echo run_new_up.sh finished