To run a full NNLO fit (not profiling) the list of commands given to MINUIT in 
the beginning of parameters.yaml is a little different with respect to the 
profiling example.

Because this is a full PDF fit, also the HERA data need to be 
included in the steering file, because they give the most important constraints 
on quark PDFs. The CMS jet data then again gives information on the gluon. If 
the fit does not have proper data to constrain a particular PDF, the fit results
would likely end up unphysical.

The order NNLO needs to be given also in constants.yaml, not only in steering.

Modify the cond.sh script so that XFDIR and the datafiles link creation command 
point to the right places in your installation. Then make sure it is an 
executable and submit to HTCondor:
    chmod +x cond.sh
    condor_submit condor.submit

Once the fit is done, the file "finished.txt" should appear.
Then the program plotCorr.cxx can be used for plotting correlations between 
parameters:
    g++ plotCorr.cxx $(root-config --cflags --glibs) -std=gnu++0x  -o plotCorrEXE
    ./plotCorrEXE
And you can produce the standard xfitter plots using xfitter-draw. I recommend 
you see xfitter-draw --help and play around with the different options to 
familiarize yourself with it.
