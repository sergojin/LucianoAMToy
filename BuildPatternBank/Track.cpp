//
//  Track.cpp
//  LinearizedTrackFitting
//
//  Created by Luciano Ristori on 4/25/14.
//

#include <iostream>
#include <cmath>
#include <random>
#include "Geometry.h"
#include "Track.h"
double Pi = 3.14159265;

using namespace std;

extern     std::default_random_engine generator;
extern     std::uniform_real_distribution<double> distribution;
extern     std::normal_distribution<double> gauss;


// constructors


Track::Track(){}; // default constructor


Track::Track(Geometry &g){ // constructor with specific geometry
        
    // create random track parameters
    
    double x0_ = 2*g.t_deltaX0 * distribution(generator) + g.t_x0 - g.t_deltaX0; // x at primary vertex
    double y0_ = 2*g.t_deltaY0 * distribution(generator) + g.t_y0 - g.t_deltaY0; // y at primary vertex
    double z0_ = g.t_z0 + g.t_deltaZ0*gauss(generator); // z at primary vertex
    
    double eta_ = 2*g.t_deltaEta * distribution(generator) + g.t_eta - g.t_deltaEta; // eta of track
    double phi_ = 2*g.t_deltaPhi * distribution(generator) + g.t_phi - g.t_deltaPhi; // phi of track
    
    double invPt_ =  g.t_invPt_min + (g.t_invPt_max - g.t_invPt_min) * distribution(generator); // inverse pt of track
    if(distribution(generator) > 0.5) invPt_ = -invPt_;// choose charge
    
    //invPt_ = -invPt_; // only negative tracks are generated

    
    init(x0_, y0_, z0_ ,invPt_, eta_, phi_); // initialize track
    
}; // end constructor


// specific track constructor

Track::Track(double x0_, double y0_, double z0_, double invPt_, double eta_, double phi_){
    
    init(x0_, y0_, z0_ , invPt_, eta_, phi_); // initialize track
    
};

// central track constructor

Track::Track(int special){
    
    Geometry g;
    
    init(g.t_x0, g.t_y0, g.t_z0 ,0. ,g.t_eta, g.t_phi);
    
};


void Track::print(){
    
    cout << "primary vertex: " << x0 << " " << y0 << " " << z0 << endl;
    cout << "pt: " << 1./invPt << " eta: " << eta << " phi: " << phi << endl;
    
};


// common code

void Track::init(double x0_, double y0_, double z0_, double invPt_, double eta_, double phi_){

    
    // coordinates of primary vertex
    
    x0 = x0_;
    y0 = y0_;
    z0 = z0_;
    
    
    // primary parameters from constructor arguments
    
    invPt = invPt_;
    eta = eta_;
    phi = phi_;
    
    // derived parameters
    
    
    c = 1.2*invPt/1.052; // signed curvature (note scale factor to align with CMS official sim)
    charge = 1.;
    if(invPt < 0.) charge = -1.;
    
    cotTheta = sinh(eta); 
    k =sinh(eta); // constant for z evolution = R*pz/pt
    
};


// rigid rotation of track around z axis

void Track::rotate(double phiRot){
    
    double xTemp = x0*cos(phiRot) - y0*sin(phiRot);
    double yTemp = x0*sin(phiRot) + y0*cos(phiRot);
    x0 = xTemp;
    y0 = yTemp;
    phi += phiRot;
    if(phi > +Pi) phi -= 2*Pi;
    if(phi < -Pi) phi += 2*Pi;
    
    return;
};


// calculate exact coordinates of hits for flat barrel and disc layers
// return "false" if no intersection
// measurement errors are NOT included

bool Track::xzBarrel(double yDet, double &x, double &z){
    
    // yDet is the position of the barrel plane (parallel to xz)
    
    double arg = c*(y0-yDet) + cos(phi);
    if(fabs(arg) > 1.)return false;
    
    if(fabs(c) > cMin){
        
        // find two solutions for s (range is (-2*Pi/c, +2*Pi/c)
        
        double sol1 = (+acos(arg) - phi)/c;
        double sol2 = (-acos(arg) - phi)/c;
        
        // bring solutions to positive s
        
        if(sol1 < 0.) sol1 += fabs(2*Pi/c);
        if(sol2 < 0.) sol2 += fabs(2*Pi/c);
        
        double sMax = min(sol1,sol2);
        
        x = x0 + 1./c*(sin(c*sMax + phi)-sin(phi));
        z = z0 + k*sMax;
        
        return true;
    }
    else {
        // very large momentum
        // use first order approximation in c (curvature)
        
        if(fabs(sin(phi)) < 0.001) return false; // no intersection with barrel
        
        double sMax = (yDet - y0)/sin(phi) - (yDet - y0)*(yDet - y0)*cos(phi)/2./sin(phi)/sin(phi)/sin(phi)*c;
        x = x0 + sMax*cos(phi) - 0.5*sMax*sMax*c*sin(phi);
        z = z0 + k*sMax;
        
        return true;        
    }
    
};


bool Track::xyDisc(Geometry &g, int iDisc, double &X, double &R){
    
    // R is the radial position of the measured hit
    // X is the distance from phi = 0 measured along the circumference
    // measurement errors are included
    
    double x,y; // cartesian coordinates of the hit
    
    // zDet is the position in z of the disc (parallel to xy)

    double zDet = g.zDisc[iDisc];
    
    if(k == 0.) return false; // infinite looper
    
    double sMax = (zDet-z0)/k;
    
    if (sMax <0.) return false; // track going in the wrong direction
    
    if(fabs(c) > cMin){
        x = x0 + 1./c*(sin(c*sMax + phi) - sin(phi));
        y = y0 - 1./c*(cos(c*sMax + phi) - cos(phi));
    }
    else {
        // very large momentum
        // use first order approximation in c (curvature)
        
        x = x0 + sMax*cos(phi) - 0.5*sMax*sMax*c*sin(phi);
        y = y0 + sMax*sin(phi) + 0.5*sMax*sMax*c*cos(phi);
    }
    
    // find polar coordinates
    
    double r = sqrt(x*x + y*y);
    double phi = atan2(x,y);
    
    double errTang; // measurement error along phi
    double errRadial; // measuremen error along r

    if(r <= g.rBoundary){ // use ps modules 
        errTang = g.ps_x*gauss(generator); 
        errRadial = g.ps_z*gauss(generator);// short strips are oriented radially
    }
    else { // use ss modules
        errTang = g.ss_x*gauss(generator); 
        errRadial = g.ss_z*gauss(generator);// strips are oriented radially
    }
    
    R = r + errRadial;
    X = R*phi + errTang;
    
    double newPhi = X/R;
    
    // check for detector boundaries
    
    if(R > g.rMax_d[iDisc]) return false;
    if(R < g.rMin_d[iDisc]) return false;    
    if(newPhi > g.phiMax_d[iDisc]) return false;
    if(newPhi < g.phiMin_d[iDisc]) return false;
    
    return true;
    
    
};

bool Track::phizBarrel(Geometry &g, int iBarrel, double &hitPhi, double &hitZ){
    
    // find intersection with cylindrical barrel with successive approximations using xzBarrel
    // measurement errors are included
    
    
    double r = g.rBarrel[iBarrel];
    
    double rotPhi = Pi/2. - phi;
    rotate(rotPhi);
    double err = 1.;
    while(err > 1.e-8){
        //cout << " X ";
        double x_;
        if(!xzBarrel(r, x_, hitZ)){
            rotate(-rotPhi);
            return false;
        }
        else {
            double deltaPhi = atan(x_/r);
            rotate(deltaPhi);
            rotPhi += deltaPhi;
            err = fabs(deltaPhi);
            //cout << err;
        }
    }
    
    hitPhi = Pi/2. - rotPhi;
    if(hitPhi > +Pi) hitPhi -= 2*Pi;
    if(hitPhi < -Pi) hitPhi += 2*Pi;
    hitPhi *= r; //transform angle into linear coordinate
    
    // add measurement errors
    
    hitPhi += g.dx_b[iBarrel]*gauss(generator);
    hitZ += g.dz_b[iBarrel]*gauss(generator);
    
    rotate(-rotPhi);
    
       
    return true;
};

// find hit in generic detector layer

bool Track::getHit(Geometry &g, int iLayer, double &x, double &z){
    
    if(g.detType[iLayer] == 'B'){
        return phizBarrel(g, g.detInd[iLayer], x, z);
    }
    if(g.detType[iLayer] == 'D'){
        return xyDisc(g, g.detInd[iLayer], x, z);
    }
    return false;
};

// computes multiple scattering and effect of energy loss (to be added to coordinates)

void Track::multipleScattering(Geometry &g, std::vector<double> &phiScatt, std::vector<double> &zScatt){
    double phiPos = 0.;
    double phiAngle = 0.;
    double phiCurv = 0.;
    double zPos = 0.;
    double zAngle = 0.;
    
    phiScatt.clear();
    zScatt.clear();
    
    for (int iL = 0; iL != g.nLayers; ++ iL){
        
        double rint = g.rBarrel[iL];
        if(iL > 0) rint -= g.rBarrel[iL-1];
        
        
        phiCurv += -charge*g.dedx[iL]*invPt*invPt;
        phiAngle += invPt*g.thickness[iL]*gauss(generator) + phiCurv;
        phiPos += phiAngle*rint + phiCurv*rint*rint;
        zAngle += invPt*g.thickness[iL]*gauss(generator);
        zPos += zAngle*rint;
        phiScatt.push_back(phiPos);
        zScatt.push_back(zPos*sqrt(1.+cotTheta*cotTheta));
    }

};
