//
//  Geometry.h
//  BuildPatternBank
//
//  Created by Luciano Ristori on 4/24/14.
//


#ifndef BuildPatternBank_Geometry_h
#define BuildPatternBank_Geometry_h


#include <vector>
#include <cmath>
#include <string>


//typedef std::pair<double,double> hit;
typedef std::pair<int,int> bin;
typedef int superStripID;
typedef std::vector<superStripID> pattern;



class Hit {
public:
    int iLayer;
    double x;
    double z;
    
    Hit(int _iLayer, double _x, double _z){
        iLayer = _iLayer;
        x = _x;
        z = _z;
    }
    
};




class Geometry {
    
public:
    
    const double Pi = 3.14159265;
    
    // TRACK GENERATION PARAMETERS
    
    double t_eta = 11./30.;
    double t_phi = Pi*3./8.;
    

    //double t_invPt_max = 1./2.;// Gev/c^(-1)
    //double t_invPt_max = 1./3.;// Gev/c^(-1)
    double t_invPt_max = 1./10.;// Gev/c^(-1)
    
    double t_invPt_min = 0.; // Gev/c^(-1)

    
    double t_x0 = 0.;
    double t_y0 = 0.;
    double t_z0 = 0.;
    
    double t_deltaX0 = 0.;
    double t_deltaY0 = 0.;
    double t_deltaZ0 = 0.05;
    
    double t_deltaEta = 11./30;
    double t_deltaPhi = Pi/8.;
    
    // Jetty
    //double t_deltaEta = 0.1;
    //double t_deltaPhi = 0.1;

    

    // DETECTOR GEOMETRY
    
       
    // measurement errors for different types of modules
    
    double ps_x = 30.e-6;  // PS module, phi direction
    double ps_z = 30.e-6;  // PS module, z direction
    double ss_x = 30.e-6;  // 2S module, phi direction
    double ss_z = 30.e-6;  // 2S module, z direction
    
    // zero  measurement errors
    
/*    double ps_x = 0.; // PS module, phi direction
    double ps_z = 0.; // PS module, z direction
    double ss_x = 0.; // 2S module, phi direction
    double ss_z = 0.; // 2S module, z direction
 */
    
    
    // barrels
    
    static const int nBarrels = 6; // number of barrel layers
    double rBarrel[nBarrels] = {0.2243, 0.3557, 0.5059, 0.6833, 0.9, 1.077}; // radial position (distance from beam axis)
    
    // tower boundaries in eta and phi
    // used in constructor to calculate boundaries in x and z of each layer
    
    //double tow_etaMin = -0.2;
    //double tow_etaMax = +0.8;
    //double tow_phiMin = +0.6;
    //double tow_phiMax = +1.7;
    
    // strip lengths
    
    //double stripLength[nBarrels] = {0.00142, 0.00142, 0.00142, 0.0500, 0.0500, 0.0500};
    
    // superstrip segmentation (barrels)
    
    std::vector<int> nBinsX; // number of superstrips in x for each layer
    std::vector<int> nBinsZ; // number of superstrips in z for each layer
    
    
    // boundaries
    
    std::vector<double> phiMin_b ={0.720, 0.675, 0.640, 0.715, 0.655, 0.615}; // boundaries in phi
    std::vector<double> phiMax_b ={1.665, 1.705, 1.720, 1.640, 1.675, 1.780}; // boundaries in phi
    std::vector<double> zMin_b ={-0.070,-0.070,-0.055,-0.075,-0.075,-0.075};  // boundaries in z
    std::vector<double> zMax_b ={+0.270,+0.370,+0.480,+0.570,+0.765,+0.865};  // boundaries in z
    
    std::vector<double> xMin_b; // boundaries along x direction (calculated in constructor)
    std::vector<double> xMax_b; // boundaries along x direction (calculated in constructor)

    //measurement errors
    
    const double dx_b[nBarrels]= {ps_x,ps_x,ps_x,ss_x,ss_x,ss_x}; // error of x coordinate measurement
    const double dz_b[nBarrels]= {ps_z,ps_z,ps_z,ss_z,ss_z,ss_z} ; // error of z coordinate measurement
    
    
    // discs
    
    static const int nDiscs = 5; // number of disc layers
    const double zDisc[nDiscs] = {1.4, 1.6, 1.9, 2.2, 2.6};
    
    const double rBoundary = 0.6; // boundary between ps and ss modules
    
    std::vector<double> phiMin_d = {-Pi/8., -Pi/8., -Pi/8., -Pi/8., -Pi/8.}; // boundaries along phi direction
    std::vector<double> phiMax_d = {+Pi/8., +Pi/8., +Pi/8., +Pi/8., +Pi/8.}; // boundaries along phi direction
    std::vector<double> rMin_d = {0.300, 0.300, 0.300, 0.300, 0.300}; // boundaries along radial direction
    std::vector<double> rMax_d = {1.100, 1.100, 1.100, 1.100, 1.100}; // boundaries along radial direction

    const double dxIn_d = ps_x; // error of x coordinate measurement internal
    const double drIn_d = ps_z; // error of r coordinate measurement internal
    const double dxEx_d = ss_x; // error of x coordinate measurement external
    const double drEx_d = ss_z; // error of r coordinate measurement external
    
    // layer configuration
    
    static const int nLayers = 6;
    
    char detType[nLayers] = {'B','B','B','B','B','B'}; // barrel (B) or disc (D)
    int detInd[nLayers] = {0, 1, 2, 3, 4, 5};

       
    // size of superstrips
    
    // CMS 512
    std::vector<double> binSizeZ = {0.0250,0.0250,0.0250,0.0500,0.0500,0.0500};// z bins
    std::vector<double> binSizeX = {0.0512,0.0512,0.0512,0.0512*0.9,0.0512*0.9,0.0512*0.9};// x bins

    // CMS 256
    //std::vector<double> binSizeZ = {0.0250,0.0250,0.0250,0.0500,0.0500,0.0500};// z bins
    //std::vector<double> binSizeX = {0.0256,0.0256,0.0256,0.0256*0.9,0.0256*0.9,0.0256*0.9};// x bins
    
    // CMS 128
    //std::vector<double> binSizeZ = {0.0250,0.0250,0.0250,0.0500,0.0500,0.0500};// z bins
    //std::vector<double> binSizeX = {0.0128,0.0128,0.0128,0.0128*0.9,0.0128*0.9,0.0128*0.9};// x bins
    
    // CMS 128
    //std::vector<double> binSizeZ = {0.0250,0.0250,0.0250,0.0500,0.0500,0.0500};// z bins
    //std::vector<double> binSizeX = {0.0128,0.0128,0.0128,0.0128*0.9,0.0128*0.9,0.0128*0.9};// x bins
    
    // 200x1
    //std::vector<double> binSizeZ = {0.,0.,0.,0.,0.,0.};// z bins
    //std::vector<double> binSizeX = {rBarrel[0]*Pi/4./200.,rBarrel[1]*Pi/4./200.,rBarrel[2]*Pi/4./200.,
     //  rBarrel[3]*Pi/4./200.,rBarrel[4]*Pi/4./200.,rBarrel[5]*Pi/4./200.};// x bins
    
    // 400x1
    //std::vector<double> binSizeZ = {0.,0.,0.,0.,0.,0.};// z bins
    //std::vector<double> binSizeX = {rBarrel[0]*Pi/4./400.,rBarrel[1]*Pi/4./400.,rBarrel[2]*Pi/4./400.,
    //                             rBarrel[3]*Pi/4./400.,rBarrel[4]*Pi/4./400.,rBarrel[5]*Pi/4./400.};// x bins
    
    // 600x1
    //std::vector<double> binSizeZ = {0.,0.,0.,0.,0.,0.};// z bins
    //std::vector<double> binSizeX = {rBarrel[0]*Pi/4./600.,rBarrel[1]*Pi/4./600.,rBarrel[2]*Pi/4./600.,
    //   rBarrel[3]*Pi/4./600.,rBarrel[4]*Pi/4./600.,rBarrel[5]*Pi/4./600.};// x bins

    
    // offsets for superstrip boundaries
    // computed by constructor
    
    std::vector<double> dZ;
    std::vector<double> dX;

    // bin offsets are dZ*offset and dX*offset
    
    //std::vector<double> offset = { 1., 3., 6., 2., 4., 5.};
    std::vector<double> offset = {0.,0.,0.,0.,0.,0.};
    
    
    // constants for multiple scattering
    // angle = thickness/Pt
    
    bool useMultipleScattering = true;
    std::vector<double> thickness = {0.0027, 0.0027, 0.0027, 0.0027, 0.0027, 0.0027};
    //std::vector<double> thickness = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    // constants for energy loss
    // displacement = dedx*r^2/(Pt^2)
    
    std::vector<double> dedx = {0.0005, 0.0005, 0.0005, 0.0005, 0.0005, 0.0005};
    
    // Default constructor
    
    Geometry();
    
    // convert hit to 2Dim bin
    
    bin hitToBin(Hit h);
    
    // convert 2Dim bin to 1Dim superstrip ID
    
    superStripID binToSuperStripID(int iL, bin b);
    
    // convert 1Dim superstrip ID to 2Dim bin
    
    bin superStripIDToBin(int iL, superStripID ss);
    
    // convert hit to 1Dim superstrip ID
    
    superStripID hitToSuperStripID(Hit h);
    
    void print();
};


#endif
