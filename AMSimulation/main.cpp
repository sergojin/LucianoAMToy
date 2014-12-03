//
//  main.cpp
//  AMSimulation
//
//  Created by Luciano Ristori on 7/9/14.
//  Copyright (c) 2014 Luciano Ristori. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <sstream>

#include "TH1D.h"
#include "TFile.h"

#include "HistDivideBinomial.h"

#include "Geometry.h"
#include "AssociativeMemory.h"
#include "Event.h"

string pathToMyDirectory = "/Users/Luciano/Documents/Ccode/AMSimulation/";
TFile* histFile = new TFile((pathToMyDirectory + "hists.root").c_str(),"RECREATE");  // histogram file

// random generator

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.,1.);
std::normal_distribution<double> gauss(0.0,1.0);


int main(int argc, const char * argv[])
{
    cout << "--- Starting..." << endl;
    
    
    // number of events to simulate
    
    long int nEvents1 = 0; // for coverage
    long int nEvents2 = 0; // for AM simulation

    
    // seed random generator 
    // must be different from pattern bank generation
    
    generator.seed(123);
    //generator.seed(132);
    
    // create AM and load patterns from file
    
    string fileName = "patternBank.txt";
    
    long int maxPatterns = 2e6; // maximum number of patterns in AM
    int maxSuperstripID = 10000; // maximum number of superstrips per layer;
    int maxMiss = 0; // maximum number of missing layers for a valid road
    bool cleanUp = false;
    
    Geometry g;
    
    AssociativeMemory AM(maxPatterns,g.nLayers,maxSuperstripID,maxMiss, cleanUp);
    cout << "--- Loading patterns into AM" << endl;
    if(!AM.load(pathToMyDirectory+ fileName)){
        cout << endl << "***** AM.load() failed - abort!" << endl;
        return 0;
    };    
    cout << AM.am.size() << " patterns loaded" << endl;
    
    // Book histograms for coverage
   
    TH1D hRoads("Road Index","Road Index",1000, 0.,maxPatterns);
    TH1D hCoverage("Coverage","Coverage", 1000, 0.,maxPatterns);
    
    // Track parameters
    
    double pt_min = 1./g.t_invPt_max;
    double pt_max = 10;
    if(g.t_invPt_min > 0.1) pt_max = 1./g.t_invPt_min;
    
    // input tracks
    
    TH1D hSingleTrackPhi("SingleTrackPhi","SingleTrackPhi", 100, -g.Pi, +g.Pi);
    TH1D hSingleTrackEta("SingleTrackEta","SingleTrackEta", 100, -2., +2.);
    TH1D hSingleTrackInvPt("SingleTrackInvPt","SingleTrackInvPt", 100, g.t_invPt_min, g.t_invPt_max);
    TH1D hSingleTrackPt("SingleTrackPt","SingleTrackPt", 100, pt_min, pt_max);
    TH1D hSingleTrackZ0("SingleTrackZ0","SingleTrackZ0", 100, -0.200, +0.200);
    
    // found tracks
    
    TH1D hSingleTrackFoundPhi("SingleTrackFoundPhi","SingleTrackFoundPhi", 100, -g.Pi, +g.Pi);
    TH1D hSingleTrackFoundEta("SingleTrackFoundEta","SingleTrackFoundEta", 100, -2., +2.);
    TH1D hSingleTrackFoundInvPt("SingleTrackFoundInvPt","SingleTrackFoundInvPt", 100, g.t_invPt_min, g.t_invPt_max);
    TH1D hSingleTrackFoundPt("SingleTrackFoundPt","SingleTrackFoundPt", 100, pt_min, pt_max);
    TH1D hSingleTrackFoundZ0("SingleTrackFoundZ0","SingleTrackFoundZ0", 100, -0.200, +0.200);
    
    // efficiencies
    
    TH1D hSingleTrackEffPhi("SingleTrackEffPhi","SingleTrackEffPhi", 100, -g.Pi, +g.Pi);
    TH1D hSingleTrackEffEta("SingleTrackEffEta","SingleTrackEffEta", 100, -2., +2.);
    TH1D hSingleTrackEffInvPt("SingleTrackEffInvPt","SingleTrackEffInvPt", 100, g.t_invPt_min, g.t_invPt_max);
    TH1D hSingleTrackEffPt("SingleTrackEffPt","SingleTrackEffPt", 100, pt_min, pt_max);
    TH1D hSingleTrackEffZ0("SingleTrackEffZ0","SingleTrackEffZ0", 100, -0.200, +0.200);

    

    
    // hit coordinates
    
    TH1D hSingleHitX[g.nLayers], hSingleHitZ[g.nLayers];
    for(int iV = 0; iV != g.nLayers; ++ iV){
        ostringstream sx;
        sx << "SingleHitX_" << iV;
        hSingleHitX[iV] = TH1D((sx.str()).c_str(), (sx.str()).c_str(), 100, -1., +1.);
        ostringstream sz;
        sz << "SingleHitZ_" << iV;
        hSingleHitZ[iV] = TH1D((sz.str()).c_str(), (sz.str()).c_str(), 100, -1., +1.);
    }

   
    // Begin first loop on events to calculate coverage with single tracks
    
    cout << "--- Computing coverage" << endl;
    
    for(long int iEvent = 0; iEvent != nEvents1; ++iEvent){
        if(iEvent%100 == 0) cout << iEvent << " events processed" << endl;
        
        long int nTrials;
        Event event(g, 1, 0, nTrials);// single track, no noise
        AM.clear();
        AM.input(event.hitList);
        vector<long int> roads; // list of roads output by AM
        roads = AM.output();        
        if(roads.size() > 0)hRoads.Fill(roads[0]); // fill road indices
        
        // histogram track parameters
        hSingleTrackPhi.Fill(event.trackList[0].phi);
        hSingleTrackEta.Fill(event.trackList[0].eta);
        hSingleTrackInvPt.Fill(fabs(event.trackList[0].invPt));
        hSingleTrackPt.Fill(fabs(1./event.trackList[0].invPt));
        hSingleTrackZ0.Fill(event.trackList[0].z0);
        
        // histogram found track parameters
        if(roads.size() > 0){
            hSingleTrackFoundPhi.Fill(event.trackList[0].phi);
            hSingleTrackFoundEta.Fill(event.trackList[0].eta);
            hSingleTrackFoundInvPt.Fill(fabs(event.trackList[0].invPt));
            hSingleTrackFoundPt.Fill(fabs(1./event.trackList[0].invPt));
            hSingleTrackFoundZ0.Fill(event.trackList[0].z0);
        }
        
        // histogram hit coordinates
        
        for(int iH = 0; iH != event.hitList.size(); ++iH){
            Hit h = event.hitList[iH];
            hSingleHitX[h.iLayer].Fill(h.x);
            hSingleHitZ[h.iLayer].Fill(h.z);
        }
        
        
    } // end first loop on events
    
    // integrate histogram to calculate coverage and errors
    
    double cont = 0.;
    int maxBin = hRoads.GetXaxis()->FindBin(AM.am.size()) + 1;
    for(int iBin = 1; iBin != maxBin; ++iBin){
        cont += hRoads.GetBinContent(iBin); // sum bin contents
        hCoverage.SetBinContent(iBin,cont/(double)nEvents1); // normalize and set bin content
        hCoverage.SetBinError(iBin,sqrt(cont*(nEvents1-cont)/nEvents1)/nEvents1); // set bin error
    }
    
    // calculate coverage as a function of track parameters
    
    HistDivideBinomial(&hSingleTrackFoundPhi, &hSingleTrackPhi, &hSingleTrackEffPhi);
    HistDivideBinomial(&hSingleTrackFoundEta, &hSingleTrackEta, &hSingleTrackEffEta);
    HistDivideBinomial(&hSingleTrackFoundInvPt, &hSingleTrackInvPt, &hSingleTrackEffInvPt);
    HistDivideBinomial(&hSingleTrackFoundPt, &hSingleTrackPt, &hSingleTrackEffPt);
    HistDivideBinomial(&hSingleTrackFoundZ0, &hSingleTrackZ0, &hSingleTrackEffZ0);
    
    
    
    
    // Prepare for second loop on events to calculate occupancy and number of roads
    // and combinations with multiple tracks and noise
    
    
    
    // book histograms for AM simulation
    
    // Track parameters
    
    TH1D hMultiTrackPhi("MultiTrackPhi","MultiTrackPhi", 100, -g.Pi, +g.Pi);
    TH1D hMultiTrackEta("MultiTrackEta","MultiTrackEta", 100, -2., +2.);
    TH1D hMultiTrackInvPt("MultiTrackInvPt","MultiTrackInvPt", 100, 0., g.t_invPt_max);
    TH1D hMultiTrackZ0("MultiTrackZ0","MultiTrackZ0", 100, -0.200, +0.200);
    
    // hit coordinates
    
    TH1D hMultiHitX[g.nLayers], hMultiHitZ[g.nLayers];
    for(int iV = 0; iV != g.nLayers; ++ iV){
        ostringstream sx;
        sx << "MultiHitX_" << iV;
        hMultiHitX[iV] = TH1D((sx.str()).c_str(), (sx.str()).c_str(), 100, -1., +1.);
        ostringstream sz;
        sz << "MultiHitZ_" << iV;
        hMultiHitZ[iV] = TH1D((sz.str()).c_str(), (sz.str()).c_str(), 100, -1., +1.);
    }

    // Roads and combinations
    
    TH1D hNRoads("NRoads","NRoads",2000,0.,2000);
    TH1D hRoadIndexes("Road Indexes","Road Indexes", 100, 0.,1.e6);
    TH1D hMeanRoads("MeanRoads","MeanRoads", 100, 0.,1.e6);
    TH1D hOccupancy("Occupancy","Occupancy", 10, 0.,10.);    
    TH1D hCombinationsPerRoad("CombinationsPerRoad","CombinationsPerRoad", 50, 0.,50.);
    TH1D hCombinationsPerEvent("CombinationsPerEvent","CombinationsPerEvent", 2000, 0.,2000.);
    TH1D hNSingleHitLayers("NSingleHitLayers","NSingleHitLayers", 7, 0.,7.);
    TH1D hNSingleHitLayersFraction("NSingleHitLayersFraction","NSingleHitLayersFraction", 7, 0.,7.);


    
    int nTracks = 5; // number of tracks per event
    int nNoise = 400; // number of noise hits per event

    // Begin second loop on events
    
    cout << "--- Simulating AM" << endl;
    
    for(long int iEvent = 0; iEvent != nEvents2; ++iEvent){
             
        if(iEvent%10 == 0) cout << iEvent << " events processed" << endl;
        long int nTrials;
        Event event(g, nTracks, nNoise, nTrials); // create one event
        
        // histogram all track parameters
        
        for(int iT = 0; iT != event.trackList.size(); ++iT){
            hMultiTrackPhi.Fill(event.trackList[iT].phi);
            hMultiTrackEta.Fill(event.trackList[iT].eta);
            hMultiTrackInvPt.Fill(event.trackList[iT].invPt);
            hMultiTrackZ0.Fill(event.trackList[iT].z0);
        }
        
        // histogram hit coordinates
        
        for(int iH = 0; iH != event.hitList.size(); ++iH){
            Hit h = event.hitList[iH];
            hMultiHitX[h.iLayer].Fill(h.x);
            hMultiHitZ[h.iLayer].Fill(h.z);
        }
        
        AM.clear();
        AM.input(event.hitList);
        vector<long int> roads;
        roads = AM.output(); // list of road indices output by AM
        
        // for each road find the number of hit combinations we need to fit
        // and the total combinations per event
        
        int combPerEvent = 0; // combinations per event
        hNRoads.Fill((int)roads.size()); // histogram number of roads per event
        for(int iR=0; iR!=roads.size(); ++iR) { // loop on all roads
            int combPerRoad = 1;
            long int roadIndex = roads[iR];
            hRoadIndexes.Fill(roadIndex); // histogram road index
            int NSingleHitLayers = 0;// number of layers with a single hit
            for(int iL=0; iL!=g.nLayers; ++iL){ // loop on layers
                int ssID = AM.am[roadIndex][iL]; // superstrip ID
                int mult = (int)AM.hitBuffer[iL][ssID].size(); // number of hits in superstrip
                if(mult == 1) ++NSingleHitLayers; // count single hit superstrip in road
                if(mult != 0) combPerRoad *= mult; // combinations is product of number of hits in all non zero layers
                hOccupancy.Fill((double)mult); // histogram number of hits in superstrip
            }
            hNSingleHitLayers.Fill(NSingleHitLayers); // histogram number of layers with a single hit
            hCombinationsPerRoad.Fill(combPerRoad);// histogram combinations per road
            combPerEvent += combPerRoad; // combinations per event is the sum on all roads
        }
        hCombinationsPerEvent.Fill(combPerEvent); // histogram combinations per event
        
    } // end second loop on events
    
    // integrate histogram to calculate mean number of roads and errors
    
    cont = 0.;
    for(int iBin = 1; iBin != hRoadIndexes.GetNbinsX(); ++iBin){
        cont += hRoadIndexes.GetBinContent(iBin); // sum bin contents
        hMeanRoads.SetBinContent(iBin,cont/(double)nEvents2); // normalize and set bin content
        hMeanRoads.SetBinError(iBin,sqrt(cont)/nEvents2); // set error
    }
    
    // integrate histogram to calculate fraction of roads with at least
    // a certain number of layers with a single hit
    
    cont = 0.;
    double n = hNSingleHitLayers.GetEntries();
    for(int iBin = hNSingleHitLayersFraction.GetNbinsX(); iBin !=0; --iBin){
        cont += hNSingleHitLayers.GetBinContent(iBin); // sum bin contents
        if(n != 0.){
            Double_t p;
            Double_t err_high;
            Double_t err_low;
            // compute p of binomial and asymmetric binomial errors
            BinomialError(cont,n,&p,&err_low,&err_high);
            // transform to simmetrical error bars
            double x3 = p +0.5*(err_high-err_low);// middle point
            hNSingleHitLayersFraction.SetBinContent(iBin,x3);
            hNSingleHitLayersFraction.SetBinError(iBin,0.5*(err_high+err_low));
        }
        else{
            // set p to 1/2 +/- 1/2
            hNSingleHitLayersFraction.SetBinContent(iBin,0.5);
            hNSingleHitLayersFraction.SetBinError(iBin,0.5);
        }
    }
    
    
    
    // write histogram file
    histFile->Write();
    
    cout << "--- Done" << endl;
    
    return 0;
}

