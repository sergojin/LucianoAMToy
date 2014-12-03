//
//  AssociativeMemory.h
//  AMSimulation
//
//  Created by Luciano Ristori on 7/9/14.
//  Copyright (c) 2014 Luciano Ristori. All rights reserved.
//

#ifndef __AMSimulation__AssociativeMemory__
#define __AMSimulation__AssociativeMemory__
#endif /* defined(__AMSimulation__AssociativeMemory__) */


#include <iostream>
#include <string>

#include "Geometry.h"

using namespace std;

class AssociativeMemory {

public:
    
    Geometry g;
    
    long int maxPatterns, nPatterns;
    int nLayers, maxSuperStrip, maxMiss;
    bool cleanUp;
    
    vector<pattern> am; // associative memory words
    
    // pointers from superstrips to patterns
    // amIndex[iLayer][superStripID] is a list of pattern addresses
    
    vector<vector<vector<long int>>> amIndex;
    
    // match bits
    // match[iPattern, iLayer]
    
    vector<vector<bool>> match;
    
    vector<int> nMatch; // number of matching layers for each pattern
    
    // Hit Buffer: one hit list per superstrip ID per layer
    // hitBuffer[iLayer][superStripID][iHit]
    
    vector<vector<vector<Hit>>> hitBuffer;
    
    // list of matching patterns
    
    vector<long int> pattList;
    
    // constructor
    
    AssociativeMemory(long int _nPatterns, int _nLayers, superStripID _maxSuperStrip, int _maxMiss, bool _cleanUp = false);
    //
    // _nPatterns = maximum number of patterns to be loaded in the AM
    // _nLayers = number of detector layers
    // _maxSuperStrips = maximum superstrip ID
    // _maxMiss = maximumnumber of missing layers in a matching pattern
    //
    bool load(string filepath); // load pattern bank from file
    bool input(Hit h); // input hit to AM
    bool input(vector<Hit> event); // input event to AM
    void clear(); // clear all match bits
    vector<long int> output(); // get vector of matching patterns
    bool isGhost(long int pattInd1, long int pattInd2);// is patt2 a ghost of patt1 ?
    void printRoads(vector<long int> pattList); // print road list
    
};