//
//  AssociativeMemory.cpp
//  AMSimulation
//
//  Created by Luciano Ristori on 7/9/14.
//  Copyright (c) 2014 Luciano Ristori. All rights reserved.
//


#include <fstream>
#include <iomanip>

#include "Geometry.h"

#include "AssociativeMemory.h"




//------------------------------------------------------------------------------------

// constructor

AssociativeMemory::AssociativeMemory(long int _maxPatterns, const int _nLayers,
                                     superStripID _maxSuperStrip, int _maxMiss, bool _cleanUp){
    
    maxPatterns = _maxPatterns;
    nLayers = _nLayers;
    maxSuperStrip = _maxSuperStrip;
    maxMiss = _maxMiss;
    cleanUp = _cleanUp;
    
    // allocate space for amIndex. Fill all with empty lists
    
    vector<long int> lempty;
    vector<vector<long int>> ltemp;
    for(superStripID iS = 0; iS != maxSuperStrip; ++iS) ltemp.push_back(lempty);
    for(int iL = 0; iL != nLayers; ++iL) amIndex.push_back(ltemp);

    
    // allocate space for hitBuffer. Fill all with empty hit lists
    
    vector<Hit> empty;
    vector<vector<Hit>> vhtemp;
    for(superStripID iS = 0; iS != maxSuperStrip; ++iS) vhtemp.push_back(empty);
    for(int iL = 0; iL != nLayers; ++iL) hitBuffer.push_back(vhtemp);
};


//------------------------------------------------------------------------------------

// load pattern bank from file

bool AssociativeMemory::load(string filepath){
    
    
    // open file
    
    cout<< "opening " << filepath << " for reading" << endl;
    
    std::ifstream infile;
    infile.open(filepath);
    if(!infile) {
        cout << "error opening " << filepath << endl;
        return false;
    }
    
    // read patterns and fill am
    
    for(int iP = 0; iP != maxPatterns; ++iP){
        if(iP%100000 == 0) cout << iP << " patterns loaded" << endl;
        pattern temp;
        int iPatt, totPop, nPop, ssID;
        infile >> iPatt >> totPop >> nPop; // skip unused numbers
        if(!infile) break; // stop if end of file
        //cout << setw(10) << iPatt-1 << setw(10) << totPop << setw(6) << nPop;
        
        vector<bool> btemp;
        for(int iL = 0; iL != nLayers; ++iL){
            infile >> ssID; // read one superstrip ID
            
            if(ssID < 0){ // abort if superstrip ID is out of range
                cout << endl << "***** ssID = " << ssID << " for pattern " << iP << " layer " << iL << " ---> abort!" << endl;
                return false;
            }
            if(ssID > maxSuperStrip) { // abort if superstrip ID is out of range
                cout << endl << "***** ssID = " << ssID << " for pattern " << iP << " layer " << iL << " ---> abort!" << endl;
                return false;
            }
            temp.push_back(ssID); // create vector of superstrip IDs (build one pattern)
            btemp.push_back(false); // prepare match vector all false
            amIndex[iL][ssID].push_back(iP); // append pattern index to list of this superstrip
            //cout << setw(6) << ssID;
        }
        am.push_back(temp); // add pattern to am
        match.push_back(btemp); // add corresponding match bit vector
        nMatch.push_back(0); // add counter of matching layers
        temp.clear(); 
        btemp.clear();
        //cout << endl;
    }
    nPatterns = am.size();
    return true;
};


//------------------------------------------------------------------------------------

// input hit to AM

bool AssociativeMemory::input(Hit h){
    
    // get superStrip number
    int iL = h.iLayer;
    superStripID ss = g.hitToSuperStripID(h);
    
    // sanity check
    if(ss < 0) {
        cout << "***** in AM ss= " << ss << endl;
        ss = 0;
    }
    if(ss > maxSuperStrip-1) {
        cout << "***** in AM ss= " << ss << " > " << maxSuperStrip-1 << endl;
        ss = maxSuperStrip-1;
    }

    hitBuffer[iL][ss].push_back(h);
    
    
    for(int iPatt = 0; iPatt != amIndex[iL][ss].size(); ++ iPatt){
        long int indPatt = amIndex[iL][ss][iPatt];
        if(match[indPatt][iL] == false) {
            ++nMatch[indPatt];
            match[indPatt][iL] = true;
            if(nMatch[indPatt] == nLayers - maxMiss) pattList.push_back(indPatt);
        }
    }
    return true;
};

// input event to AM

bool AssociativeMemory::input(vector<Hit> hitList){
    
    for(int iH = 0; iH != hitList.size(); ++iH)
        input(hitList[iH]);
    
    return true;
};


//------------------------------------------------------------------------------------

// clear all match bits in AM and hit lists in HitBuffer

void AssociativeMemory::clear(){
    
    // clear match bits and counters
    
    for(int iP = 0; iP != nPatterns; ++iP){
        nMatch[iP] = 0;
        for(int iL = 0; iL != nLayers; ++iL)
            match[iP][iL] = false;
    }
    
    // clear matched pattern list
    
    pattList.clear();
    
    // clear hitBuffer
    
    for(int iL = 0; iL != nLayers; ++iL)
        for(int iSS = 0; iSS != maxSuperStrip; ++iSS)
            hitBuffer[iL][iSS].clear();
};


//------------------------------------------------------------------------------------

// print road list

void AssociativeMemory::printRoads(vector<long int> pattList){
    cout << endl;
    for(int iP = 0; iP != pattList.size(); ++iP){
        long int pattInd = pattList[iP];
        cout << setw(10) << pattInd << ":" << setw(2) << nMatch[pattList[iP]]<< "/" << setw(1) << nLayers;
        for(int iLayer = 0; iLayer != nLayers; ++iLayer){
            superStripID ss = am[pattInd][iLayer];
            cout <<  " " << setw(4) << ss << " " << setw(2) << hitBuffer[iLayer][ss].size();
        }
        cout << endl;
    }
    
}


//------------------------------------------------------------------------------------




// get vector of matching patterns


vector<long int> AssociativeMemory::output(){
    
    //cout << "-------- new event ---------" << endl;
    //printRoads(pattList);
    
    // sort pattList by nMatch
    if (maxMiss > 0){
    
        sort(pattList.begin(), pattList.end(), [=](long int& iP1, long int& iP2)
             {
                 return nMatch[iP1] > nMatch[iP2];
             }
             );
    }
    
     //printRoads(pattList);
    
    if(!cleanUp)return pattList; // no clean up
       
    
    // eliminate ghost roads
    
    vector<long int> newPattList;
        
    // loop on old road list
    for(long int iP1 = 0; iP1 != pattList.size(); ++ iP1){
        bool keep = true; // should we keep this road?
        // loop on roads already output
        for(long int iP2 = 0; iP2 != newPattList.size(); ++ iP2)
            if(isGhost(newPattList[iP2],pattList[iP1])) {
                keep = false; // if iP1 is a ghost of iP2, do not keep it
                break;
            }
        
        if(keep) newPattList.push_back(pattList[iP1]);
    }
    
    //printRoads(newPattList);
    return newPattList;
   
}

//------------------------------------------------------------------------------------

// is patt2 a ghost of patt1 ?
// pattIndx is the pattern index in the AM

bool AssociativeMemory::isGhost(long pattInd1, long pattInd2){
    
    bool ghost = true; 
    // loop on layers
    for(int iLayer = 0; iLayer != nLayers; ++ iLayer){
        superStripID ss1,ss2;
        // extract corresponding superStrip IDs
        ss1 = am[pattInd1][iLayer];
        ss2 = am[pattInd2][iLayer];
        // look for different superstrips and ss2 is not empty
        if(hitBuffer[iLayer][ss2].size() != 0){
            if(ss1 != ss2){
                ghost = false; // we know this road (pattInd2) is not a ghost
                break; // no need to look at remaining layers
            }
        }
    }
    return ghost;
};
