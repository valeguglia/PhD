#!/bin/zsh
#
#
# This is a modified SGE batch script
# run with:   qsub -l h_vmem=8G -l h_rt=48:00:00 -l os=sld6 -cwd b_par_free.sh

# request Bourne shell as shell for job
#$ -S /bin/zsh

# TO USE THIS SCRIPT, IT SUFFICES TO MODIFY THE JOB NAME (RIGHT BELOW)
# AND MAKE SURE XFDIR POINTS TO YOUR XFITTER INSTALLATION
JOBNAME=jets13TeV_NNLO

echo "$QUEUE $JOB $HOST $JOBNAME"

export XFDIR=/afs/desy.de/user/t/tmakela/cms/xfitter-master
export XFVER=xfitter-master
export YAMLDEPSDIR=deps-yaml

# copy the steering and minuit files etc.
# note that these don't need to be changed for the LHAPDF analysis mode
# UPDATE: commented out so the files used for this analysis wont get replaced
# cp $XFDIR/$XFVER/Reactions.txt ./Reactions.txt
# cp $XFDIR/$XFVER/steering.txt ./steering.txt
# cp $XFDIR/$XFVER/minuit.in.txt ./minuit.in.txt

# make a link to the datafiles
ln -s $XFDIR/$XFVER/datafiles datafiles

# make a link to the library 
#ln -s $XFDIR/$XFVER/lib lib

mkdir output
rm *out -f
rm *eps -f
rm *tex -f
rm *pdf -f


#!/bin/zsh
#
# request Bourne shell as shell for job
#$ -S /bin/zsh
# 

# setup
echo "setting up enviroment"
echo

# compiler

. /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9/x86_64-centos7/setup.sh
cd /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.06.08/x86_64-centos7-gcc49-opt/root/
. /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.06.08/x86_64-centos7-gcc49-opt/root/bin/thisroot.sh 
cd -

# xFitter deps and root

#export version=`cat version`
export PATH=$XFDIR/$XFVER/bin:$PATH
export PATH=$XFDIR/deps/hoppet/bin:$PATH
export PATH=$XFDIR/deps/applgrid/bin:$PATH
export PATH=$XFDIR/deps/lhapdf/bin:$PATH
export PATH=$XFDIR/deps/apfel/bin:$PATH
export PATH=$XFDIR/deps/mela/bin:$PATH
export PYTHONPATH=/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.06.08/x86_64-centos7-gcc49-opt/root/lib:/cvmfs/grid.cern.ch/emi-ui-3.17.1-1.el6umd4v5/usr/lib64/python2.6/site-packages:/cvmfs/grid.cern.ch/emi-ui-3.17.1-1.el6umd4v5/usr/lib/python2.6/site-packages:/afs/desy.de/user/t/tmakela/cms/xfitterEL7/deps/lhapdf/lib64/python2.7/site-packages:/afs/desy.de/user/t/tmakela/cms/xfitterEL7/deps/lhapdf/lib64/python2.7/site-packages 
export LD_LIBRARY_PATH=$XFDIR/deps/hoppet/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$XFDIR/deps/lhapdf/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$XFDIR/deps/applgrid/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$XFDIR/deps/apfel/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$XFDIR/deps/mela/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$XFDIR/deps/qcdnum/lib/:$LD_LIBRARY_PATH
export PATH=$XFDIR/deps/qcdnum/bin:$PATH

# yaml

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$XFDIR/$YAMLDEPSDIR/yaml/lib/:$XFDIR/$YAMLDEPSDIR/yaml-cpp/lib
export CFLAGS="$CFLAGS -I$XFDIR/$YAMLDEPSDIR/yaml/include"
export CPPFLAGS="$CPPGLAGS -I$XFDIR/$YAMLDEPSDIR/yaml/include -I$XFDIR/$YAMLDEPSDIR/yaml-cpp/include "
export CXXFLAGS="$CXXFLAGS -I$XFDIR/$YAMLDEPSDIR/yaml/include -I$XFDIR/$YAMLDEPSDIR/yaml-cpp/include "
export LDFLAGS="$LDFLAGS -L$XFDIR/$YAMLDEPSDIR/yaml/lib -L$XFDIR/$YAMLDEPSDIR/yaml-cpp/lib -lyaml-cpp "
export yaml_cpp_DIR="$XFDIR/$YAMLDEPSDIR/yaml-cpp"
export LHAPATH=$XFDIR/deps/lhapdf/share/LHAPDF

# run
xfitter > out.log

# signal that the script has really finished
echo "done" > finished.txt


