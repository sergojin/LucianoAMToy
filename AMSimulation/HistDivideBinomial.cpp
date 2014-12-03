#include <cmath>
#include "TH1.h"
//_________________________________________________
//
// function for binomial error
//

void BinomialError(Double_t n,Double_t N,Double_t* p,Double_t* err_low,Double_t* err_high){

 *p = n/N;
 *err_high = (sqrt(n + 0.25 - n*n/N) + 0.5 - n/N)/(N + 1.);
 *err_low  = (sqrt(n + 0.25 - n*n/N) - 0.5 + n/N)/(N + 1.);
 return;
}

//__________________________________________________
//
// function to divide histograms
//
// hist3 = hist1 divided by hist2
//
// hist3 may point to the same histogram as hist1 or hist2

void HistDivideBinomial(TH1D* hist1,TH1D* hist2, TH1D* hist3){

 Int_t nbins = hist1->GetNbinsX()+2;

 for(Int_t ibin=0;ibin<nbins;++ibin){

   Double_t x1 = hist1->GetBinContent(ibin);
   Double_t x2 = hist2->GetBinContent(ibin);
   Double_t x3;

   if(x2 != 0.){
     Double_t p;
     Double_t err_high;
     Double_t err_low;
     // compute p of binomial and asymmetric binomial errors
     BinomialError(x1,x2,&p,&err_low,&err_high);
     // transform to simmetrical error bars
     x3 = p +0.5*(err_high-err_low);// middle point
     hist3->SetBinContent(ibin,x3);
     hist3->SetBinError(ibin,0.5*(err_high+err_low));
   }
   else{
     // set p to 1/2 +/- 1/2
     hist3->SetBinContent(ibin,0.5);
     hist3->SetBinError(ibin,0.5);
   }
 }
}

