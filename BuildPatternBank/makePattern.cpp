//
//  makePattern.cpp
//  BuildPatternBank
//
//  Created by Luciano Ristori on 3/21/14.
//  Modified for V1 on 4/24/14
//

#include "Geometry.h"
#include "Track.h"
#include "Event.h"
#include <iostream>

pattern makePattern(Geometry g, long int &nTrials){
    
            
    pattern pat;
    
    
    Event event(g, 1, 0, nTrials);// single track, no noise
    
    // find superstrip IDs (1Dim)
    
    for(int iL = 0; iL != g.nLayers; ++iL){
        superStripID s = g.hitToSuperStripID(event.hitList[iL]);
        pat.push_back(s); // append superstrip ID to pattern
    }

    return pat;
}