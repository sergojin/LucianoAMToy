//
//  Event.cpp
//  AMSimulation
//
//  Created by Luciano Ristori on 7/15/14.
//  Copyright (c) 2014 Luciano Ristori. All rights reserved.
//

#include "Event.h"

extern     std::default_random_engine generator;
extern     std::uniform_real_distribution<double> distribution;
extern     std::normal_distribution<double> gauss;


Event::Event(Geometry &g, int nTracks, int nNoise, long int &trials){
    
    
    for(int iTrack = 0; iTrack != nTracks; ++iTrack){ // begin loop on tracks
                
        // extract a random track
        
        std::vector<Hit> hits; // vector of hit coordinates
        trials = 0; // number of trials
    
        Track goodTrack;
        
        do{
            ++trials;
            //if(trials == 10) std::cout << "more than 10 trials" << std::endl;
            if(trials == 100) std::cout << "more than 100 trials" << std::endl;
            if(trials == 1000) {
                std::cout << "more than 1000 trials - give up" << std::endl;
                return;
            }
            
            // make track
            
            Track t(g);
            
            
            // simulate multiple scattering for this track
            std::vector<double> zScatt, phiScatt;
            t.multipleScattering(g, phiScatt, zScatt);
            
            // find intersections with detector layers
            
            hits.clear();
            
            double x_, z_;
            for(int iL = 0; iL != g.nLayers; ++ iL){
                if(t.getHit(g, iL, x_, z_)) {
                    if(g.useMultipleScattering) {
                        x_ += phiScatt[iL];
                        z_ += zScatt[iL];
                    }
                    
                    // check for detector boundaries
                    // and skip hits out of bounds
                    
                    if(x_ > g.xMax_b[iL]) break;
                    if(x_ < g.xMin_b[iL]) break;
                    if(z_ > g.zMax_b[iL]) break;
                    if(z_ < g.zMin_b[iL]) break;

                    Hit temp(iL, x_, z_);
                    hits.push_back(temp);
                }
            }
            
            
            goodTrack = t; // memorize track before possible exit from loop scope
            
            // validate track - confirm all hits within detector boundaries
            
        } while(hits.size() != g.nLayers);// try again if track is not valid
        
        // add this track to trackList
        
        trackList.push_back(goodTrack);
        
        // add all hits from this track to event
        
        for(int iH = 0; iH != hits.size(); ++iH) hitList.push_back(hits[iH]);
        
    } // end loop on tracks
    
    // begin loop on noise hits
    
    for(int iN = 0; iN != nNoise; ++iN){
        
        int iL = (int)(g.nLayers*distribution(generator));
        
        
        if(g.detType[iL] == 'B') {
            double lowBoundX = g.xMin_b[g.detInd[iL]];
            double hiBoundX  = g.xMax_b[g.detInd[iL]];
            double lowBoundZ = g.zMin_b[g.detInd[iL]];
            double hiBoundZ =  g.zMax_b[g.detInd[iL]];
            double x_, z_;
            x_ = lowBoundX + (hiBoundX - lowBoundX)*distribution(generator);
            z_ = lowBoundZ + (hiBoundZ - lowBoundZ)*distribution(generator);
            Hit temp(iL, x_, z_);
            hitList.push_back(temp);
        }
        
        if(g.detType[iL] == 'D') {
            double lowBoundPhi = g.phiMin_d[g.detInd[iL]];
            double hiBoundPhi  = g.phiMax_d[g.detInd[iL]];
            double lowBoundR = g.rMin_d[g.detInd[iL]];
            double hiBoundR =  g.rMax_d[g.detInd[iL]];
            double phi_, R_;
            R_ = lowBoundR + (hiBoundR - lowBoundR)*distribution(generator);
            phi_ = lowBoundPhi + (hiBoundPhi - lowBoundPhi)*distribution(generator);
            Hit temp(iL, phi_*R_, R_);
            hitList.push_back(temp);
        }
        
    }

    
};