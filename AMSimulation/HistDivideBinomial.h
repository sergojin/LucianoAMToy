
//_________________________________________________
//
// function for binomial error
//

void BinomialError(Double_t n,Double_t N,Double_t* p,Double_t* err_low,Double_t* err_high);

//__________________________________________________
//
// function to divide histograms
//
// hist3 = hist1 divided by hist2
//
// hist3 may point to the same histogram as hist1 or hist2

void HistDivideBinomial(TH1D* hist1,TH1D* hist2, TH1D* hist3);

