#ifndef SESGEE_H
#define SESGEE_H

#include <TGClient.h>
#include <TGFrame.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TGTab.h>
#include <TGTextView.h>
#include <TGComboBox.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>
#include <TTimer.h>
#include <vector>

class SGIntegratedGUI : public TGMainFrame {
private:
    TGTab               *fMasterTab;
    TGTab               *fTabsSG;
    TRootEmbeddedCanvas *fLienzoPantalla;
    TRootEmbeddedCanvas *fLienzoHist;
    TRootEmbeddedCanvas *fEcanvas3D;    
    TCanvas             *fCanvas3D; 
    
    TGNumberEntry       *fEntradaAtomos;
    TGNumberEntry       *fEntradaCampo;
    TGComboBox          *fComboEspin;
    TGCheckButton       *fCheckRealista; 
    
    TGTextButton        *fBtnConstruir, *fBtnSimular2D, *fBtnSimular3D, *fBtnLimpiar3D, *fBtnSalir;
    
    TH1D                *fHistDesviacion;
    TGraph              *fGraficoImpactos;
    std::vector<TPolyLine3D*> fPistas3D; 
    std::vector<TPolyMarker3D*> fMarcadores3D;

    TRootEmbeddedCanvas *fEcanvasDipolo;
    TTimer              *fTimer;
    TH2F                *fHeatmap; 
    TRootEmbeddedCanvas *fEcanvasHelp;

    TGNumberEntry *fEntryCurrent;
    TGNumberEntry *fEntryB0;
    TGNumberEntry *fEntryGrad;

    TGLabel *fLblPosZ;
    TGLabel *fLblFieldZ;
    TGLabel *fLblForceZ; 

    TGTextButton *fBtnPlay;
    TGTextButton *fBtnPause;
    TGTextButton *fBtnReset;

    double fZ, fVz, fMass, fArea, fTheta;      
    bool   fIsRunning;

    double fAlpha, fOmega, fGamma;      

public:
    SGIntegratedGUI(const TGWindow *p, UInt_t w, UInt_t h);
    virtual ~SGIntegratedGUI();
    
    void ConstruirGeometria();
    void Simular2D();
    void Simular3D();
    void Limpiar3D();
    void Salir();
    void MostrarPortada(); 
    void MostrarMarcoTeorico(); 

    void PlaySim();
    void PauseSim();
    void ResetPhysics();
    void UpdatePhysics();
    void UpdateCanvas();

    ClassDef(SGIntegratedGUI, 0)
};

#endif
