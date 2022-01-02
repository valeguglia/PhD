//  g++ plotCorr.cxx $(root-config --cflags --glibs) -std=gnu++0x  -o plotCorrEXE
//  ./plotCorrEXE
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include "TCanvas.h"
#include "TH2.h"
#include "TLatex.h"

using namespace std;

//AUX functions to open file I/O streams
bool inStreamOpen(ifstream &in, string filename) {
    in.open(filename.c_str());
    if (!in.is_open()) {
        cout << "Can't open " << filename << endl;
        return false;        
    } else return true;
}
bool outStreamOpen(fstream &out, string filename) {
    out.open(filename.c_str(), std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        cout << "Can't open " << filename << endl;
        return false;        
    } else return true;
}

//AUX function to check if an element exists in a vector
template <typename F>
bool vecFind(vector<F> &v, F x) {
    for (int i=0; i!=v.size(); ++i) if (v[i]==x) return true;
    return false;
}

/* AUX function to read column "iRead" into "arr" from datafile "input" */
bool readCols(string input, int iRead, vector<string> &arr) {

    ifstream in;
    if (!inStreamOpen(in,input)) return false;        

    //Read the data from input file
    string line, stmp;
    stringstream sstream;
    while (getline(in,line)) {
        sstream.str("");    sstream.clear();    //Reinit
        sstream << line;
        for (int i=0; i!=iRead+1; ++i)  sstream >> stmp;
        arr.push_back(stmp);
    }
        
    in.close();    
    return true;

} //readCols

/* The main function */
int main() {
    
    //Map parameter indices and human-readable names
    vector<string> ipar;     readCols("output/parsout_1", 0, ipar);
    vector<string> name;     readCols("output/parsout_1", 1, name);
    map<string, string> pars;
    for (int i=0; i!= ipar.size(); ++i) {
        pars[ipar[i]] = name[i].substr(1,name[i].size()-2);
    }
    
    //Init
    ifstream in;
    double dtmp;
    string line, stmp, stmp2, ipartmp;
    stringstream sstream, sstream2;
    bool corrFound = false;
    bool HesseFound = false;
    int row=0, col=0;  //Row and column in correlation matrix
    vector<double> corrline;  //Temp to store one line of correlation matrix
    vector< vector<double> > corrmat;
    vector<string> corrpars;
    
    if (!inStreamOpen(in,"output/minuit.out.txt")) return -1;
    while (getline(in,line)) {
        
        //Take the correlation matrix given by Hesse
        if (line.find("**HESSE")!=string::npos) HesseFound = true;
        if (!HesseFound) continue;
        
        //Get to the part of file w/ correlation matrix, skip others
        if (line.find("PARAMETER  CORRELATION COEFFICIENTS")!=string::npos) {
            corrFound = true;
            getline(in,line);  //Skip "NO.  GLOBAL     1     6" etc banner
            getline(in,line);  //Correlation matrix starts
        }

        //Read correlation matrix
        sstream.str("");  sstream.clear();
        sstream << line;
        sstream >> ipartmp;  //ipar

        //Check if table has ended (new sections start w/ *s, or ipar not found
        if (corrFound) {
            if (line.find("***")!=string::npos
                || !vecFind(ipar,ipartmp)      ) corrFound = false;
        }
        if (!corrFound) continue;

        corrpars.push_back(ipartmp);
        sstream >> stmp;     //skip "GLOBAL"
        col=0;
        corrline.clear();
        while (sstream >> stmp) {  //Split strings possibly merged by "-"
            stmp2 = "";  //Reinit
            for (char c : stmp) { //Don't split at 1st "-"
                if (c=='-') stmp2 += " ";
                stmp2 += c;
            }
            sstream2.str("");  sstream2.clear();
            sstream2 << stmp2;
            while (sstream2 >> dtmp) {
                corrline.push_back(dtmp);
                ++col;
            }
        }
        corrmat.push_back(corrline);
        ++row;
        
    }
    in.close();
    
    //Canvas setup
    TCanvas canv("","",800,800);
    canv.SetTopMargin(0.06);
    canv.SetLeftMargin(0.17);
    canv.SetRightMargin(0.12);
    canv.SetBottomMargin(0.17);

    //Check dimensions. xFitter doesn't always print all columns, deduce them
    int N = corrmat.size();
    if (N < corrmat[0].size()) {
        corrline.clear();
        for (int i=0; i!=N; ++i) corrline.push_back(corrmat[i].back());
		corrline.push_back(1);
		corrmat.push_back(corrline);
    } else if (N > corrmat[0].size()) {
        for (int i=0; i!=N-1; ++i) corrmat[i].push_back(corrmat[N-1][i]);
        corrmat[N-1].push_back(1);
	}
    if (N != corrpars.size()) {
        cout << "ERROR correlation matrix size != #correlated pars" << endl;
        return 1;
    }

    //ROOT 2D histogram for plotting
    TH2D h("","",N, 0, N, N, 0, N);
    h.SetStats(false);
    h.GetXaxis()->SetLabelSize(0);
    h.GetYaxis()->SetLabelSize(0);
    for (int row=0; row!=N; ++row) {
        for (int col=0; col!=N; ++col) {
            //TH2 indexing starts from 1, hence +1
            h.SetBinContent(row+1, col+1, corrmat[row][col]);
        }
    }
            
    //Draw    
    h.Draw("colz");
    
    //Write human-readable axis labels
    TLatex xlabels, ylabels;
    ylabels.SetTextAngle(90);
    for (int i=0; i!=corrpars.size(); ++i) {
        sstream.str("");  sstream.clear();
        sstream << pars[corrpars[i]];
        xlabels.DrawLatex(-3.7,i+0.1,sstream.str().c_str());
        ylabels.DrawLatex(i+0.9,-3.4,sstream.str().c_str());        
    }

    //Write CMS label
    TLatex latex;
    double ycms = corrpars.size()+0.25;
    latex.DrawLatex(0,ycms,"CMS");

    //Check from filepath which theory is used (SM or CI, order, which CI case)
    system("pwd > MCpwdCheck.txt");
    inStreamOpen(in,"MCpwdCheck.txt");
	getline(in,line);
	in.close();
	string savename;
	string Lstr = line;
	if (Lstr.find("Lambda")!=string::npos) {
	    Lstr.erase(0,line.find("Lambda")+6);
        Lstr.erase(Lstr.find("/"));
    }
	double xTh = 0.75*N;  //Works for "SM NLO"
	stringstream thTag;
	if (line.find("CI1" )!=string::npos) {
		savename = "CI1_NLO_correlations";
		thTag << "LL CI NLO, #Lambda=" << Lstr << " TeV";
        xTh = 0.35*N;
    } else if (line.find("CI2" )!=string::npos) {
		savename = "CI2_NLO_correlations, #Lambda=";
		thTag << "VL CI NLO, #Lambda=" << Lstr << " TeV";
        xTh = 0.35*N;
    } else if (line.find("CI3" )!=string::npos) {
		savename = "CI3_NLO_correlations, #Lambda=";
		thTag << "AV CI NLO, #Lambda=" << Lstr << " TeV";
        xTh = 0.35*N;
    } else if (line.find("NNLO")!=string::npos) {
		savename = "SM_NNLO_correlations";
		thTag << "SM NNLO";
        xTh = 0.7*N;
    } else if (line.find("NLO" )!=string::npos) {
		savename = "SM_NLO_correlations";
		thTag << "SM NLO";
        xTh = 0.75*N;
    }
    latex.DrawLatex(xTh,ycms,thTag.str().c_str());
    
    canv.Print((savename+".pdf").c_str());  //Save plot

    return 1;
    
}
