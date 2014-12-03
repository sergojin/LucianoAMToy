//
//  main.cpp
//  BuildPatternBank
//
//  Created by Luciano Ristori on 3/21/14.
//
//  This is BuildPatternBank_V1
//  Created by Luciano Ristori on 4/24/14.
//
//  Introduce "superStripID": single integer as opposed to "bin": pair of integers
//  A "pattern" is now a vector of SuperStrips.
//  Methods are provided to convert SuperStrip <-> bin
//
//  A "Geometry class" is provided to hold all geometry data.
//


#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include "Geometry.h"
#include "makePattern.h"

std::string prefix = "/Users/Luciano/Documents/Ccode/BuildPatternBank/BuildPatternBank/";


// random generator

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.,1.);
std::normal_distribution<double> gauss(0.0,1.0);


int main(int argc, const char * argv[])
{
    using namespace std;
    
 // print time stamp -----------------------
    
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
    std::string str(buffer);
    
    std::cout << str << endl;
    
  // end time stamp -----------------------   
    
    
    Geometry g;
    
    
    // open file for output of coverage data
    
    cout<< "opening coverage.txt" << endl;
    std::ofstream covfile;
    covfile.open(prefix + "coverage.txt");
    if(!covfile) {
        cout << "error opening coverage.txt" << endl;
        return 0;
    }
    
    // open output file for sequential pattern bank
    
    cout<< "opening patternBankSequential.txt" << endl;    
    std::ofstream seqFile;
    seqFile.open(prefix + "patternBankSequential.txt");
    if(!seqFile) {
        cout << "error opening patternBankSequential.txt" << endl;
        return 0;
    }
    seqFile << setfill(' '); // set fill character for output fields

    
    // associate a counter to each pattern - this is our pattern bank
    
    map<pattern, long int> bank;
    
    // draw tracks and update bank
    
    long int nTracks = 2e6;
    long int nStep = 10000;// frequency of update of coverage file
    
    long int trials = 0; // number of trials to get one track in acceptance
    long int totTrials = 0; // total number of trials (used to calculate acceptance)
    
    long int prevSize = 0; // size of bank at previous step
    
    for(long int itrack = 0; itrack != nTracks; ++itrack) {
        pattern pat = makePattern(g, trials);
        totTrials += trials;
        
        if((itrack%nStep) == 0) { // update progress of pattern bank
            double coverage = 1.-((long int)bank.size()-prevSize)/(double)nStep;
            prevSize = (long int)bank.size();
            cout << itrack << " tracks generated, " << bank.size() << " patterns - coverage: "<< coverage << endl;
            covfile << itrack << ", " << bank.size() << ", " << coverage << endl;
        }
        
        if(pat.size() == 0) return 0; // abort if makePattern fails
        
        // write pattern to sequential file if it's new
        if(bank.count(pat) != 1){
            seqFile << setw(10)  << bank.size() << setw(10) << 0 <<  setw(8)  << 0 << "  ";
            for(auto it = pat.begin(); it != pat.end(); ++it)
                seqFile << setw(5) << *it ;
            seqFile << endl;
        }
        
        // HERE IS THE MONEY!
        ++ bank[pat]; // add pattern to the bank and count number of occurences
        
    }// end loop on tracks
    
    double coverage = 1.-((long int)bank.size()-prevSize)/(double)nStep;
    covfile << nTracks << ", " << bank.size()<< ", " << coverage << endl;
    
    cout    << "acceptance " << nTracks << "/" << totTrials << " = " << ((double)nTracks)/((double)totTrials) << endl;
    covfile << "acceptance " << nTracks << "/" << totTrials << " = " << ((double)nTracks)/((double)totTrials) << endl;
    
    cout<< "closing coverage.txt" << endl;
    covfile.close();
    
    cout<< "closing patternBankSequential.txt" << endl;    
    seqFile.close();

    // sort patterns by frequency
    
    cout << "copying patterns" << endl;
    
    std::vector<pair<pattern, int>> pairs;
    for (auto itr = bank.begin(); itr != bank.end(); ++itr)
        pairs.push_back(*itr);
    
    cout << "sorting patterns" << endl;
    
    sort(pairs.begin(), pairs.end(), [=](pair<pattern, int>& a, pair<pattern, int>& b)
    {
        return a.second > b.second;
    }
         );
    
   
    // write sorted pattern bank to readable text file 
    
    cout<< "opening patternBank.txt" << endl;
    
    std::ofstream outfile;
    outfile.open(prefix + "patternBank.txt");
    if(!outfile) cout << "error opening outfile" << endl;
    
    long int totalHits = 0;
    long int totalPatterns = 0;

    outfile << setfill(' ');
    
    for(auto ipatt = pairs.begin(); ipatt != pairs.end(); ++ ipatt){
        //if(ipatt->second < 2) break;
        
        totalHits += ipatt->second;
        
        outfile << setw(10)  << totalPatterns << setw(10) << totalHits <<  setw(8)  << ipatt->second << "  ";
        for(auto it = ipatt->first.begin(); it != ipatt->first.end(); ++it)
            outfile << setw(5) << *it ;
        outfile << endl;
        ++totalPatterns;
        
        
        //outfile << totalPatterns << ", " << totalHits <<  ", " << ipatt->second;
        //for(auto it = ipatt->first.begin(); it != ipatt->first.end(); ++it)
         //   outfile << ", "<< *it ;
        //outfile << endl;

        
        
    }

    cout<< "closing patternBank.txt" << endl;
    outfile.close();
    
    g.print();
    
    cout << endl << "...Done." << endl;
    
    
    return 0;
}

