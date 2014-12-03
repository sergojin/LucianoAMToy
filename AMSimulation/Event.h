//
//  Event.h
//  AMSimulation
//
//  Created by Luciano Ristori on 7/15/14.
//  Copyright (c) 2014 Luciano Ristori. All rights reserved.
//

#ifndef __AMSimulation__Event__
#define __AMSimulation__Event__

#include <iostream>
#include <random>
#include "Geometry.h"
#include "Track.h"

using namespace std;

class Event {
    
public:
    
    vector<Track> trackList;    
    vector<Hit> hitList;
    
    Event(Geometry &g, int nTracks, int nNoise, long int &trials);
    
};

#endif /* defined(__AMSimulation__Event__) */
