#!/bin/bash

# Script to test the perfect outcome on different options of the discrete variables

# Recreate file to save output in
logfile="output/run_discrete.log"
printf "Logfile of run_discrete.sh run on %s, at %s \n\n" "$(date +"%D")" "$(date +"%T")" > $logfile
echo "Create logfile in $logfile"
resultfile="output/discrete_results.log"
printf "" > $resultfile
echo "Created Resultfile in $resultfile"

# Declare Variables
varoption="optionfiles/workvar.opt" 
cp optionfiles/variables3.opt $varoption # Enter the chosen variablefile
chmod a+w $varoption
newvaroption="optionfiles/discretevar.opt"
stringoption="optionfiles/workstring.opt"
cp optionfiles/string4.opt $stringoption # Enter the chosen stringgenerationfile
chmod a+w $stringoption
osed=`echo ./work_scripts/oversed.sh`
newstringoption="optionfiles/discretestring.opt"
cp $stringoption $newstringoption

# Regularisation
echo "Regularisation" >> $resultfile
declare values=(OFF REG ARD ASR ALL) tams=0

for reg in ${values[*]}
do
    $osed "/^Regularisation/ c\
Regularisation $reg" $stringoption
# Run sh_run.C and write stdout to logfile
    echo;echo Running sh_run.C for Regularisation $reg, output is written to $logfile
    time { root -l -q sh_run.C >> $logfile; }
# Analyse outcome with TAMS
    echo Calculating Figure of Merit for training data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
    echo Calculating Figure of Merit for test data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
    fom=`tail -n 1 output/tamsout.ascii`
# Write to resultfile
    printf "%9s  %7s\n" $reg $fom >> $resultfile
    if [[ $(echo "$fom > $tams"|bc) == '1' ]]
    then
	tams=$fom
	cp $stringoption $newstringoption
    fi
done
cp $newstringoption $stringoption
echo Regularisation run finished

# Loss Function
echo; echo "Loss Function" >> $resultfile
declare values=(ENTROPY QUADRATIC COMBINED) tams=0

for loss in ${values[*]}
do
    $osed "/^LossFunc/ c\
LossFunc $loss" $stringoption
# Run sh_run.C and write stdout to logfile
    echo;echo Running sh_run.C for LossFunction $loss, output is written to $logfile
    time { root -l -q sh_run.C >> $logfile; }
# Analyse outcome with TAMS
    echo Calculating Figure of Merit for training data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
    echo Calculating Figure of Merit for test data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
    fom=`tail -n 1 output/tamsout.ascii`
# Write to resultfile
    printf "%9s  %7s\n" $loss $fom >> $resultfile
    if [[ $(echo "$fom > $tams"|bc) == '1' ]]
    then
	tams=$fom
	cp $stringoption $newstringoption
    fi
done
cp $newstringoption $stringoption
echo LossFunction run finished

# Shape Treat
echo; echo "Shape Treat" >> $resultfile
declare values=(OFF INCL MARGINAL DIAG TOL) tams=0

for shape in ${values[*]}
do
    $osed "/^ShapeTreat/ c\
ShapeTreat $shape" $stringoption
# Run sh_run.C and write stdout to logfile
    echo;echo Running sh_run.C for Shape Treatment $shape, output is written to $logfile
    time { root -l -q sh_run.C >> $logfile; }
# Analyse outcome with TAMS
    echo Calculating Figure of Merit for training data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
    echo Calculating Figure of Merit for test data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
    fom=`tail -n 1 output/tamsout.ascii`
# Write to resultfile
    printf "%9s  %7s\n" $shape $fom >> $resultfile
    if [[ $(echo "$fom > $tams"|bc) == '1' ]]
    then
	tams=$fom
	cp $stringoption $newstringoption
    fi
done
cp $newstringoption $stringoption
echo Shape Treatment run finished

# Training Method
echo; echo "Training Method" >> $resultfile
declare values=( off BFGS) tams=0

for train in ${values[*]}
do
    if [[ $train == "off" ]]
    then
	$osed "/^TrainingMethod/ c\
TrainingMethod" $stringoption
    else
	$osed "/^TrainingMethod/ c\
TrainingMethod $train" $stringoption	
    fi
# Run sh_run.C and write stdout to logfile
    echo;echo Running sh_run.C for Training Method $train, output is written to $logfile
    time { root -l -q sh_run.C >> $logfile; }
# Analyse outcome with TAMS
    echo Calculating Figure of Merit for training data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
    echo Calculating Figure of Merit for test data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
    fom=`tail -n 1 output/tamsout.ascii`
# Write to resultfile
    printf "%9s  %7s\n" $train $fom >> $resultfile
    if [[ $(echo "$fom > $tams"|bc) == '1' ]]
    then
	tams=$fom
	cp $stringoption $newstringoption
    fi
done
cp $newstringoption $stringoption
echo Training Method run finished

# Variable Transformation
echo; echo "Variable Transformation" >> $resultfile
declare values=(off P D N G) tams=0

for trans in ${values[*]}
do
    if [[ $trans == "off" ]]
    then
	$osed "/^VarTransform/ c\
VarTransform" $stringoption
    else
	$osed "/^VarTransform/ c\
VarTransform $trans" $stringoption
    fi
# Run sh_run.C and write stdout to logfile
    echo;echo Running sh_run.C for Variable Transformation $trans, output is written to $logfile
    time { root -l -q sh_run.C >> $logfile; }
# Analyse outcome with TAMS
    echo Calculating Figure of Merit for training data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_train.root\",0\)
    echo Calculating Figure of Merit for test data
    root -l -q TAMS/sh_tams.C\(\"histos/hist_NeuroBayes_test.root\",1\)
    fom=`tail -n 1 output/tamsout.ascii`
# Write to resultfile
    printf "%9s  %7s\n" $trans $fom >> $resultfile
    if [[ $(echo "$fom > $tams"|bc) == '1' ]]
    then
	tams=$fom
	cp $stringoption $newstringoption
    fi
done
cp $newstringoption $stringoption
echo Variable Transformation run finished

echo; echo "Brief results as saved in $resultfile:"
cat $resultfile; echo
echo "The best optionfile is saved in $newstringoption"

# show that script has finished successfully
echo run_discrete.sh finished