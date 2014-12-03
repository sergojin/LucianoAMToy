//
//  Geometry.cpp
//  BuildPatternBank
//
//  Created by Luciano Ristori on 4/24/14.
//


#include "Geometry.h"
#include <iostream>
#include <iomanip>

Geometry::Geometry() { // default constructor
    
    // compute barrel boundaries in x from boundaries in phi
    
    for(int iB = 0; iB != nBarrels; ++iB){
        xMax_b.push_back(rBarrel[iB]*phiMax_b[iB]);
        xMin_b.push_back(rBarrel[iB]*phiMin_b[iB]);
    }
    
    // compute number of bins
     
   
    for(int iL = 0; iL != nLayers; ++iL) {
        if(binSizeZ[iL]==0.){
            nBinsZ.push_back(1);
            binSizeZ[iL] = zMax_b[iL]-zMin_b[iL];
        }
        else nBinsZ.push_back(floor((zMax_b[iL]-zMin_b[iL])/binSizeZ[iL]+1.));  // z bins
        
        nBinsX.push_back(floor((xMax_b[iL]-xMin_b[iL])/binSizeX[iL]+1.));  // x bins
    }
    
    // offsets for superstrip boundaries    
    for(int iL = 0; iL != nLayers; ++iL) dZ.push_back(binSizeZ[iL]/nLayers);
    for(int iL = 0; iL != nLayers; ++iL) dX.push_back(binSizeX[iL]/nLayers);
    
    
}; // end default costructor


// convert hit to 2Dim bin

bin Geometry::hitToBin(Hit h){
    
    bin temp;
    int iL = h.iLayer;
    temp.first  = floor((h.x - xMin_b[iL] + dX[iL]*offset[iL])/binSizeX[iL]);
    temp.second = floor((h.z - zMin_b[iL] + dZ[iL]*offset[iL])/binSizeZ[iL]);
    return temp;
};


// convert 2Dim bin to 1Dim superstrip ID

superStripID Geometry::binToSuperStripID(int iL, bin b){
    int ss = (nBinsX[iL]+2)*b.second + b.first;
    //std::cout << "iL: " << iL << " ix: " << b.first << " iz: " << b.second << " ss: " << ss << std::endl;
    return ss;
};

// convert 1Dim superstrip ID to 2Dim bin

bin Geometry::superStripIDToBin(int iL, superStripID ss){
    bin temp;
    temp.second = ss/(nBinsX[iL]+2);
    temp.first = ss % (nBinsX[iL]+2);
    return temp;
};

// convert hit to 1Dim superstrip ID

superStripID Geometry::hitToSuperStripID(Hit h){
    int iL = h.iLayer;
    bin temp = hitToBin(h);
    return binToSuperStripID(iL, temp);
    
};


// PRINT GEOMETRY


void Geometry::print(){
    
    using namespace std;
      
    cout << endl;
    cout << "--- GEOMETRY ---" << endl;
    cout << endl;
    
    cout << "Track Generation Parameters" << endl << endl;
    cout << setw(12) << "t_eta " << t_eta << endl;
    cout << setw(12) << "t_deltaEta " <<  t_deltaEta << endl;
    cout << setw(12) << "t_phi " << t_phi << endl;
    cout << setw(12) << "t_deltaPhi " <<  t_deltaPhi << endl;
    cout << setw(12) << "t_invPt_max " << t_invPt_max << endl;
    cout << setw(12) << "t_invPt_min " << t_invPt_min << endl;
    cout << setw(12) << "t_x0 " << t_x0 << endl;
    cout << setw(12) << "t_deltaX0 " <<  t_deltaX0 << endl;
    cout << setw(12) << "t_y0 " << t_y0 << endl;
    cout << setw(12) << "t_deltaY0 " <<  t_deltaY0 << endl;
    cout << setw(12) << "t_z0 " << t_z0 << endl;
    cout << setw(12) << "t_deltaZ0 " <<  t_deltaZ0 << endl;
    
    cout << setprecision(4);
    
    cout << endl << "Barrels" << endl << endl;
    
    cout << setw(10) << "rBarrel";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << rBarrel[i];
    }
    cout << endl;
    
    cout << setw(10) << "xMin_b";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << xMin_b[i];
    }
    cout << endl;

    cout << setw(10) << "xMax_b";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << xMax_b[i];
    }
    cout << endl;
    
    cout << setw(10) << "nBinsX";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << nBinsX[i];
    }
    cout << endl;    
    
    cout << setw(10) << "binSizeX";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << binSizeX[i];
    }
    cout << endl;
    
    cout << setw(10) << "zMin_b";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << zMin_b[i];
    }
    cout << endl;
    
    cout << setw(10) << "zMax_b";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << zMax_b[i];
    }
    cout << endl;
    
    cout << setw(10) << "nBinsZ";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << nBinsZ[i];
    }
    cout << endl;
    
    cout << setw(10) << "binSizeZ";
    for(int i = 0; i != nBarrels; ++i){
        cout << " " << setw(10) << binSizeZ[i];
    }
    cout << endl;

    
};


