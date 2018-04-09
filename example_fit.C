///////////////////////////////////////
//
// Little Fitting Tutorial
// Author - Sam Meehan
// Intention : To help Gang make some mad-awesome smoothness tests
//
// Initially following the work here
// https://pprc.qmul.ac.uk/~bevan/yeti/fitting.pdf
//
///////////////////////////////////////

// Define the functional form that you want to be using to perform the fit
// It takes pointers as arguments to the dependent parameter x
// and the parameters you will fit par

Double_t fitFunc(Double_t *x, Double_t *par){

  Double_t PDF = 0.0;

  PDF = par[0]*pow(2.71,par[1]*x[0]); // define the form of the function in terms of the pointers to the input argument and parameters

  return PDF;

}

// This is the main macro which is run

void example_fit(){

  // Define a one dimensional histogram that you will eventually fit
  TH1F *h = new TH1F("h","h",100,0,100);

  // Be sure to turn on the errors so that they are correct
  h->Sumw2();

  // We will generate toy MC for this example according to the product of two
  // exponentials, but this can be modified
  TF1 *f1 = new TF1("f1","(1-pow(2.71,-0.1*x))*pow(2.71,-0.1*x)",0,100);

  // Fill the histogram with some number of events drawn with respect to that function
  int nevents=100000;
  for(int i=0; i<nevents; i++){
    double r = f1->GetRandom();
    h->Fill(r);
  }


  // Canvas for drawing
  TCanvas *c = new TCanvas("c","c",600,400);
  c->Divide(1,2);
  c->cd(1);

  h->Draw("hist");

  // Define an instance of the function you will use for fitting
  double min_xrange = 20;
  double max_xrange = 100;
  int npar = 2;
  TF1 *myFitFunc = new TF1("fitFunc",fitFunc, min_xrange, max_xrange, npar);

  // specify the details about the parameters of that function
  myFitFunc->SetParName(0,"Normalization");
  myFitFunc->SetParName(1,"Dropoff");

  myFitFunc->SetParameter(0,10);
  myFitFunc->SetParameter(1,-0.2);

  myFitFunc->SetParLimits(0,0,100000);
  myFitFunc->SetParLimits(1,-1.0,1.0);

  // This is where the fitting minimization occurs
  h->Fit("fitFunc","L");

  myFitFunc->Draw("LSame");


  // Evaluate the ratio of the fitting function and the initiql histogram
  TH1F *hRat = (TH1F*)h->Clone("hRat");
  hRat->Reset();

  for(int i=0; i<hRat->GetNbinsX(); i++){

    double content=1;
    double fitval=1;

    double xlow  = hRat->GetBinLowEdge(i);
    double xhigh = xlow+hRat->GetBinWidth(i);

    // compare the bin content of the histogram with the fitting integral in that range
    content = h->GetBinContent(i);
    fitval  = myFitFunc->Integral(xlow,xhigh);

    cout<<"Compare : "<<xlow<<"  "<<content<<"  "<<fitval<<endl;

    double ratio=1.0;
    if(fitval!=0){
      ratio = content/fitval;
    }

    hRat->SetBinContent(i,ratio-1);
  }

  // draw the ratio
  c->cd(2);

  TLine *line = new TLine(0,1,100,1);
  line->SetLineColor(kBlue);
  line->Draw();

  hRat->SetFillColor(2);

  hRat->Draw("F");


}