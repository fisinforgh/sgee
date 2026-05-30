// =============================================================================
// SGEE SoftEdu - Stern-Gerlach Experiment Experience
// =============================================================================
//
// Copyright (C) 2026
//
// Authors:
//   [1] Julián Salamanca*
//   [2] Diego Julián Rodríguez-Patarroyo**
//
//   [1] jasalamanca@udistrital.edu.co (profesor Universidad Distrital)
//   [2] djrodriguezp@udistrital.edu.co (profesor Universidad Distrital)
//
//  * Grupo de Física e Informática (FISINFOR)
//  ** Grupo de Laboratorio de Fuentes Alternas de Energía (LIFAE)
//  *,** Universidad Distrital Francisco José de Caldas (Bogotá, Colombia)
//  
// Web page: https://github.com/fisinforgh/sgee
//
// LICENSE: GNU General Public License v3.0 (GPLv3) or later
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// =============================================================================
#include <iostream>
#include <string>
#include <fstream>

#include "SESGEE.h"
#include <TEllipse.h>
#include <TArrow.h>
#include <TLatex.h>
#include <TMath.h>
#include <TStyle.h>
#include <TColor.h>
#include <TGaxis.h>
#include <TApplication.h>
#include <TSystem.h>
#include <TRandom.h>
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TGIcon.h>
#include <TImage.h>
#include <TGPicture.h>
#include <iostream>
#include <TGLUtil.h>
#include <TGLAnnotation.h>
#include <TGLViewer.h>

SGIntegratedGUI::SGIntegratedGUI(const TGWindow *p, UInt_t w, UInt_t h) : TGMainFrame(p, w, h) {


    std::cout << "[INFO] Building the SGEE SoftEdu graphical interface..." << std::endl;
    SetCleanup(kDeepCleanup);

    const Int_t NRGBs = 3;
    Double_t stops[NRGBs] = { 0.00, 0.50, 1.00 };
    Double_t red[NRGBs]   = { 0.20, 1.00, 0.90 };
    Double_t green[NRGBs] = { 0.20, 1.00, 0.20 };
    Double_t blue[NRGBs]  = { 0.90, 1.00, 0.20 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, 255, 0.40);
    gStyle->SetNumberContours(255);

    fHistDesviacion = new TH1D("fHistDesviacion", "Deflection;Z (mm);Atoms", 150, -3.0, 3.0);
    fHistDesviacion->SetFillColor(kBlue-4);
    fGraficoImpactos = new TGraph();
    fGraficoImpactos->SetTitle("Screen Pattern;Y (mm);Z (mm)");
    fGraficoImpactos->SetMarkerStyle(20);
    fGraficoImpactos->SetMarkerSize(0.3);
    fGraficoImpactos->SetMarkerColor(kBlue+2);

    fIsRunning = false;
    fMass = 1.0;
    fArea = 0.05; 
    
    fTimer = new TTimer();
    fTimer->Connect("Timeout()", "SGIntegratedGUI", this, "UpdatePhysics()");
    fHeatmap = new TH2F("fHeatmap", "", 1, 0, 1, 100, 0, 1);
    fHeatmap->SetStats(0); 
    fHeatmap->GetXaxis()->SetTickLength(0);
    fHeatmap->GetYaxis()->SetTickLength(0);
    fHeatmap->GetXaxis()->SetLabelSize(0);
    fHeatmap->GetYaxis()->SetLabelSize(0);

    Pixel_t bgSG, bgDipolo;
    gClient->GetColorByName("#D9E1F2", bgSG);       
    gClient->GetColorByName("#E2EFDA", bgDipolo);   
    
    TString spathHIDEN_FILE(gSystem->HomeDirectory());
    //cout << "spathHIDEN_FILE: " << spathHIDEN_FILE << endl;
    spathHIDEN_FILE.Append("/.pathDIR_SGEESoftEdu.txt");
    //cout << "spathHIDEN_FILE: " << spathHIDEN_FILE << endl;
    std::ifstream inHidenFile;
    inHidenFile.open(spathHIDEN_FILE);
    
    std::string spathICONS;
    getline(inHidenFile,spathICONS);
    pathDIR_ICONS.Append(spathICONS);
    pathDIR_ICONS.Append("/icons/");
    
    
    //    TString workDir = gSystem->pwd();
    const TGPicture *picSGEE = nullptr;
    const TGPicture *picFisinfor = nullptr;
    const TGPicture *picLIFAE = nullptr;
    const TGPicture *picUD = nullptr;

    auto LoadAndScaleImage = [](TString path, TString name, UInt_t targetHeight) -> const TGPicture* {
        TImage *img = TImage::Open(path.Data());
        if (!img || img->GetHeight() == 0) {
            if (img) delete img; 
            return nullptr;
        }
        UInt_t targetWidth = img->GetWidth() * targetHeight / img->GetHeight();
        img->Scale(targetWidth, targetHeight);
        const TGPicture *pic = gClient->GetPicturePool()->GetPicture(name.Data(), img->GetPixmap(), img->GetMask());
        delete img;
        return pic;
    };

    picSGEE     = LoadAndScaleImage(pathDIR_ICONS + "logoSGEE.png", "picSGEE_scaled", 45);
    picFisinfor = LoadAndScaleImage(pathDIR_ICONS + "logoFisinfor_v2.png", "picFis_scaled", 35);
    picLIFAE    = LoadAndScaleImage(pathDIR_ICONS + "logoLIFAE.png", "picLif_scaled", 35);
    picUD       = LoadAndScaleImage(pathDIR_ICONS + "logoUD.png", "picUD_scaled", 45);

    fMasterTab = new TGTab(this, w, h);

    TGCompositeFrame *tabSG = fMasterTab->AddTab("Stern-Gerlach Exp. (Quantum)");
    TGHorizontalFrame *hFrameSG = new TGHorizontalFrame(tabSG, w, h);

    TGVerticalFrame *panelControlesSG = new TGVerticalFrame(hFrameSG, 240, h, kFixedWidth);
    panelControlesSG->ChangeBackground(bgSG); 
    
    TGLabel *lblTemp; 

    lblTemp = new TGLabel(panelControlesSG, "--- PHYSICAL PARAMETERS ---");
    lblTemp->ChangeBackground(bgSG);
    panelControlesSG->AddFrame(lblTemp, new TGLayoutHints(kLHintsCenterX, 5, 5, 15, 5));
    
    lblTemp = new TGLabel(panelControlesSG, "Particle Spin:");
    lblTemp->ChangeBackground(bgSG);
    panelControlesSG->AddFrame(lblTemp, new TGLayoutHints(kLHintsLeft, 5, 5, 10, 2));
    
    fComboEspin = new TGComboBox(panelControlesSG, 100);
    fComboEspin->AddEntry("S = 1/2 (e.g., Silver, Lithium)", 0);
    fComboEspin->AddEntry("S = 1 (e.g., Carbon, Silicon)", 1);
    fComboEspin->AddEntry("S = 3/2 (e.g., Nitrogen, Phosphorus)", 2);
    fComboEspin->Select(0);
    fComboEspin->Resize(220, 25);
    panelControlesSG->AddFrame(fComboEspin, new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));

    lblTemp = new TGLabel(panelControlesSG, "Magnetic Field Gradient [T/m]");
    lblTemp->ChangeBackground(bgSG);
    panelControlesSG->AddFrame(lblTemp, new TGLayoutHints(kLHintsLeft, 5, 5, 10, 2));
    
    fEntradaCampo = new TGNumberEntry(panelControlesSG, 10.0, 5, -1, TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax, 0.0, 20.0);
    panelControlesSG->AddFrame(fEntradaCampo, new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 10));

    fCheckRealista = new TGCheckButton(panelControlesSG, "Realistic Mode (Lip / Thermal)");
    fCheckRealista->SetState(kButtonUp);
    fCheckRealista->ChangeBackground(bgSG); 
    panelControlesSG->AddFrame(fCheckRealista, new TGLayoutHints(kLHintsLeft, 5, 5, 2, 10));

    lblTemp = new TGLabel(panelControlesSG, "--- 3D VISUALIZATION ---");
    lblTemp->ChangeBackground(bgSG);
    panelControlesSG->AddFrame(lblTemp, new TGLayoutHints(kLHintsCenterX, 5, 5, 15, 5));
    
    fBtnConstruir = new TGTextButton(panelControlesSG, "1. Build 3D Apparatus");
    fBtnConstruir->Connect("Clicked()", "SGIntegratedGUI", this, "ConstruirGeometria()");
    panelControlesSG->AddFrame(fBtnConstruir, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

    fBtnSimular3D = new TGTextButton(panelControlesSG, "2. Fire Beam (3D)");
    fBtnSimular3D->Connect("Clicked()", "SGIntegratedGUI", this, "Simular3D()");
    panelControlesSG->AddFrame(fBtnSimular3D, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

    fBtnLimpiar3D = new TGTextButton(panelControlesSG, "Clear 3D Tracks");
    fBtnLimpiar3D->Connect("Clicked()", "SGIntegratedGUI", this, "Limpiar3D()");
    panelControlesSG->AddFrame(fBtnLimpiar3D, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 15));

    lblTemp = new TGLabel(panelControlesSG, "--- 2D STATISTICS ---");
    lblTemp->ChangeBackground(bgSG);
    panelControlesSG->AddFrame(lblTemp, new TGLayoutHints(kLHintsCenterX, 5, 5, 15, 5));
    
    lblTemp = new TGLabel(panelControlesSG, "Number of Atoms:");
    lblTemp->ChangeBackground(bgSG);
    panelControlesSG->AddFrame(lblTemp, new TGLayoutHints(kLHintsLeft, 5, 5, 5, 2));
    
    fEntradaAtomos = new TGNumberEntry(panelControlesSG, 5000, 7, -1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
    panelControlesSG->AddFrame(fEntradaAtomos, new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));

    fBtnSimular2D = new TGTextButton(panelControlesSG, "3. Generate 2D Plots");
    fBtnSimular2D->Connect("Clicked()", "SGIntegratedGUI", this, "Simular2D()");
    panelControlesSG->AddFrame(fBtnSimular2D, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 15));

    fBtnSalir = new TGTextButton(panelControlesSG, "EXIT");
    fBtnSalir->Connect("Clicked()", "SGIntegratedGUI", this, "Salir()");
    fBtnSalir->SetTextColor(kRed);
    panelControlesSG->AddFrame(fBtnSalir, new TGLayoutHints(kLHintsExpandX, 5, 5, 15, 10));

    TGVerticalFrame *fLogosSG = new TGVerticalFrame(panelControlesSG, 240, 150);
    fLogosSG->ChangeBackground(bgSG);

    TGHorizontalFrame *fRow1SG = new TGHorizontalFrame(fLogosSG);
    fRow1SG->ChangeBackground(bgSG);
    if (picSGEE) {
        TGIcon *iconSGEE1 = new TGIcon(fRow1SG, picSGEE, picSGEE->GetWidth(), picSGEE->GetHeight());
        fRow1SG->AddFrame(iconSGEE1, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    }
    fLogosSG->AddFrame(fRow1SG, new TGLayoutHints(kLHintsCenterX, 0, 0, 10, 5));

    TGHorizontalFrame *fRow2SG = new TGHorizontalFrame(fLogosSG);
    fRow2SG->ChangeBackground(bgSG);
    if (picFisinfor) {
        TGIcon *iconFis1 = new TGIcon(fRow2SG, picFisinfor, picFisinfor->GetWidth(), picFisinfor->GetHeight());
        fRow2SG->AddFrame(iconFis1, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 5, 2, 2));
    }
    if (picLIFAE) {
        TGIcon *iconLif1 = new TGIcon(fRow2SG, picLIFAE, picLIFAE->GetWidth(), picLIFAE->GetHeight());
        fRow2SG->AddFrame(iconLif1, new TGLayoutHints(kLHintsCenterY | kLHintsRight, 5, 2, 2, 2));
    }
    fLogosSG->AddFrame(fRow2SG, new TGLayoutHints(kLHintsCenterX, 0, 0, 2, 2));

    TGHorizontalFrame *fRow3SG = new TGHorizontalFrame(fLogosSG);
    fRow3SG->ChangeBackground(bgSG);
    if (picUD) {
        TGIcon *iconUD1 = new TGIcon(fRow3SG, picUD, picUD->GetWidth(), picUD->GetHeight());
        fRow3SG->AddFrame(iconUD1, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    }
    fLogosSG->AddFrame(fRow3SG, new TGLayoutHints(kLHintsCenterX, 0, 0, 5, 10));

    panelControlesSG->AddFrame(fLogosSG, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5, 5, 15, 5));

    hFrameSG->AddFrame(panelControlesSG, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 5, 5, 5, 5));

    fTabsSG = new TGTab(hFrameSG, 650, 500);
    TGCompositeFrame *subTab1 = fTabsSG->AddTab("1. Detector Screen (2D)");
    fLienzoPantalla = new TRootEmbeddedCanvas("LienzoPantalla", subTab1, 650, 500);
    subTab1->AddFrame(fLienzoPantalla, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

    TGCompositeFrame *subTab2 = fTabsSG->AddTab("2. Quantum Histogram");
    fLienzoHist = new TRootEmbeddedCanvas("LienzoHist", subTab2, 650, 500);
    subTab2->AddFrame(fLienzoHist, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

    TGCompositeFrame *subTab3 = fTabsSG->AddTab("3. 3D View");
    fEcanvas3D = new TRootEmbeddedCanvas("Ecanvas3D", subTab3, 650, 500);
    subTab3->AddFrame(fEcanvas3D, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    fCanvas3D = fEcanvas3D->GetCanvas(); 

    hFrameSG->AddFrame(fTabsSG, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    tabSG->AddFrame(hFrameSG, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    TGCompositeFrame *tabDipolo = fMasterTab->AddTab("Dipole Dynamics (Semiclassical)");
    TGHorizontalFrame *hFrameDipolo = new TGHorizontalFrame(tabDipolo, w, h);
    
    TGVerticalFrame *vControlPanelDipolo = new TGVerticalFrame(hFrameDipolo, 270, h, kFixedWidth);
    vControlPanelDipolo->ChangeBackground(bgDipolo); 
    
    TGGroupFrame *gParams = new TGGroupFrame(vControlPanelDipolo, "Physical Parameters");
    gParams->ChangeBackground(bgDipolo);
    
    lblTemp = new TGLabel(gParams, "Current I (A):");
    lblTemp->ChangeBackground(bgDipolo);
    gParams->AddFrame(lblTemp, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 5, 0));
    
    fEntryCurrent = new TGNumberEntry(gParams, 2.0, 6, -1, TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax, -10.0, 10.0);
    gParams->AddFrame(fEntryCurrent, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 10));

    lblTemp = new TGLabel(gParams, "Base Field B_0 (T):");
    lblTemp->ChangeBackground(bgDipolo);
    gParams->AddFrame(lblTemp, new TGLayoutHints(kLHintsLeft, 2, 2, 5, 0));
    
    fEntryB0 = new TGNumberEntry(gParams, 1.0, 6, -1, TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax, -5.0, 5.0);
    gParams->AddFrame(fEntryB0, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 10));

    lblTemp = new TGLabel(gParams, "Gradient dB_z/dz (T/m):");
    lblTemp->ChangeBackground(bgDipolo);
    gParams->AddFrame(lblTemp, new TGLayoutHints(kLHintsLeft, 2, 2, 5, 0));
    
    fEntryGrad = new TGNumberEntry(gParams, 0.5, 6, -1, TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax, -5.0, 5.0);
    gParams->AddFrame(fEntryGrad, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 10));

    vControlPanelDipolo->AddFrame(gParams, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 5, 5));

    TGGroupFrame *gMonitor = new TGGroupFrame(vControlPanelDipolo, "Real-Time Monitor");
    gMonitor->ChangeBackground(bgDipolo);
    
    fLblPosZ = new TGLabel(gMonitor, "Z Position: +0.0 m");
    fLblPosZ->SetTextFont("-*-courier-bold-r-*-*-14-*-*-*-*-*-*-*");
    fLblPosZ->SetTextColor(kBlue+2);
    fLblPosZ->ChangeBackground(bgDipolo);
    gMonitor->AddFrame(fLblPosZ, new TGLayoutHints(kLHintsLeft, 5, 5, 10, 5));

    fLblFieldZ = new TGLabel(gMonitor, "B_z Field:  +0.0 T");
    fLblFieldZ->SetTextFont("-*-courier-bold-r-*-*-14-*-*-*-*-*-*-*");
    fLblFieldZ->SetTextColor(kBlack);
    fLblFieldZ->ChangeBackground(bgDipolo);
    gMonitor->AddFrame(fLblFieldZ, new TGLayoutHints(kLHintsLeft, 5, 5, 5, 5));

    fLblForceZ = new TGLabel(gMonitor, "F_z Force: +0.000 N");
    fLblForceZ->SetTextFont("-*-courier-bold-r-*-*-14-*-*-*-*-*-*-*");
    fLblForceZ->SetTextColor(kMagenta+1);
    fLblForceZ->ChangeBackground(bgDipolo);
    gMonitor->AddFrame(fLblForceZ, new TGLayoutHints(kLHintsLeft, 5, 5, 5, 10));

    vControlPanelDipolo->AddFrame(gMonitor, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 5, 5));

    TGGroupFrame *gControls = new TGGroupFrame(vControlPanelDipolo, "Controls");
    gControls->ChangeBackground(bgDipolo);
    
    fBtnPlay = new TGTextButton(gControls, " Play / Resume ");
    fBtnPlay->Connect("Clicked()", "SGIntegratedGUI", this, "PlaySim()");
    gControls->AddFrame(fBtnPlay, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 2));

    fBtnPause = new TGTextButton(gControls, " Pause ");
    fBtnPause->Connect("Clicked()", "SGIntegratedGUI", this, "PauseSim()");
    gControls->AddFrame(fBtnPause, new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 2));

    fBtnReset = new TGTextButton(gControls, " Reset ");
    fBtnReset->Connect("Clicked()", "SGIntegratedGUI", this, "ResetPhysics()");
    gControls->AddFrame(fBtnReset, new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));

    vControlPanelDipolo->AddFrame(gControls, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 5, 5));

    TGVerticalFrame *fLogosDipolo = new TGVerticalFrame(vControlPanelDipolo, 270, 150);
    fLogosDipolo->ChangeBackground(bgDipolo);

    TGHorizontalFrame *fRow1Dipolo = new TGHorizontalFrame(fLogosDipolo);
    fRow1Dipolo->ChangeBackground(bgDipolo);
    if (picSGEE) {
        TGIcon *iconSGEE2 = new TGIcon(fRow1Dipolo, picSGEE, picSGEE->GetWidth(), picSGEE->GetHeight());
        fRow1Dipolo->AddFrame(iconSGEE2, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    }
    fLogosDipolo->AddFrame(fRow1Dipolo, new TGLayoutHints(kLHintsCenterX, 0, 0, 10, 5));

    TGHorizontalFrame *fRow2Dipolo = new TGHorizontalFrame(fLogosDipolo);
    fRow2Dipolo->ChangeBackground(bgDipolo);
    if (picFisinfor) {
        TGIcon *iconFis2 = new TGIcon(fRow2Dipolo, picFisinfor, picFisinfor->GetWidth(), picFisinfor->GetHeight());
        fRow2Dipolo->AddFrame(iconFis2, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 5, 2, 2));
    }
    if (picLIFAE) {
        TGIcon *iconLif2 = new TGIcon(fRow2Dipolo, picLIFAE, picLIFAE->GetWidth(), picLIFAE->GetHeight());
        fRow2Dipolo->AddFrame(iconLif2, new TGLayoutHints(kLHintsCenterY | kLHintsRight, 5, 2, 2, 2));
    }
    fLogosDipolo->AddFrame(fRow2Dipolo, new TGLayoutHints(kLHintsCenterX, 0, 0, 2, 2));

    TGHorizontalFrame *fRow3Dipolo = new TGHorizontalFrame(fLogosDipolo);
    fRow3Dipolo->ChangeBackground(bgDipolo);
    if (picUD) {
        TGIcon *iconUD2 = new TGIcon(fRow3Dipolo, picUD, picUD->GetWidth(), picUD->GetHeight());
        fRow3Dipolo->AddFrame(iconUD2, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    }
    fLogosDipolo->AddFrame(fRow3Dipolo, new TGLayoutHints(kLHintsCenterX, 0, 0, 5, 10));

    vControlPanelDipolo->AddFrame(fLogosDipolo, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5, 5, 15, 5));

    fEcanvasDipolo = new TRootEmbeddedCanvas("EcanvasDipolo", hFrameDipolo, 680, 600);
    
    hFrameDipolo->AddFrame(vControlPanelDipolo, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 2, 2, 2, 2));
    hFrameDipolo->AddFrame(fEcanvasDipolo, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
    tabDipolo->AddFrame(hFrameDipolo, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    TGCompositeFrame *tabHelp = fMasterTab->AddTab("Theoretical Framework & Help");
    fEcanvasHelp = new TRootEmbeddedCanvas("EcanvasHelp", tabHelp, w, h);
    tabHelp->AddFrame(fEcanvasHelp, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

    AddFrame(fMasterTab, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    SetWindowName("Integrated Simulator: Stern-Gerlach & Magnetic Dipole");
    DontCallClose();
    Connect("CloseWindow()", "SGIntegratedGUI", this, "Salir()");
    
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();

    ResetPhysics();
    MostrarPortada(); 
    MostrarMarcoTeorico(); 
}

SGIntegratedGUI::~SGIntegratedGUI() {
    Limpiar3D();
    if (fTimer) { fTimer->Stop(); delete fTimer; }
    if (fHeatmap) delete fHeatmap;
    Cleanup();
    delete fHistDesviacion;
    delete fGraficoImpactos;
}



void SGIntegratedGUI::MostrarMarcoTeorico() {
    if (!fEcanvasHelp) return;
    
    TCanvas *c = fEcanvasHelp->GetCanvas();
    c->cd();
    c->Clear();
    
    // Configurar márgenes para maximizar el área de texto
    c->SetMargin(0.06, 0.06, 0.05, 0.05);
    
    TLatex tex;
    tex.SetNDC(); 
    
    // =========================================================================
    // ENCABEZADO PRINCIPAL
    // =========================================================================
    tex.SetTextFont(62); tex.SetTextSize(0.040); tex.SetTextColor(kBlue+2);
    tex.DrawLatex(0.05, 0.94, "SGEE #color[2]{Soft}#color[4]{Edu}: THEORETICAL FRAMEWORK & USER MANUAL");
    
    // =========================================================================
    // SECCIÓN 1: FUNDAMENTACIÓN FÍSICA (STERN-GERLACH)
    // =========================================================================
    tex.SetTextFont(62); tex.SetTextSize(0.026); tex.SetTextColor(kBlack);
    tex.DrawLatex(0.05, 0.89, "QUANTUM MECHANICS: SPACE QUANTIZATION AND REALISTIC EFFECTS");
    
    tex.SetTextFont(42); tex.SetTextSize(0.020);
    tex.DrawLatex(0.05, 0.85, "The Stern-Gerlach experiment (1922) provided definitive proof of the quantization of intrinsic angular momentum (spin) #vec{S}. A neutral atom with a magnetic dipole moment");
    tex.DrawLatex(0.05, 0.82, "#vec{#mu} = g #frac{q}{2m} #vec{S} enters a highly inhomogeneous magnetic field. The potential energy of the dipole inside the field is U = - #vec{#mu} #bullet #vec{B}. The resulting net deflecting force is derived as:");
    //tex.DrawLatex(0.05, 0.79, "");
    
    // Ecuación de la fuerza de deflexión
    tex.SetTextFont(12); tex.SetTextSize(0.024); tex.SetTextColor(kRed+2);
    tex.DrawLatex(0.05, 0.77, "#vec{F} = #nabla (#vec{#mu} #bullet #vec{B}) #approx m_{s} g #mu_{B} #frac{#partial B_{z}}{#partial z} #hat{k}     m_{s}: spin magnetic quantum number.      #mu_{B}: Bohr magneton.    g: g-factor (gyromagnetic ratio).");
    
    // Explicación de efectos realistas
    tex.SetTextFont(42); tex.SetTextSize(0.020); tex.SetTextColor(kBlack);
    tex.DrawLatex(0.05, 0.72, "#bf{The Realistic Lip Shape:} According to Maxwell's equations (#vec{#nabla} #bullet #vec{B} = 0), a unique gradient #partial B_{z}/#partial z cannot exist without a corresponding transverse gradient #partial B_{y}/#partial y.");
    tex.DrawLatex(0.05, 0.69, "This forces the beam to distort into a characteristic 'lip-shaped' pattern on the detector screen instead of forming simple parallel lines.");
    //    tex.DrawLatex(0.05, 0.66, "");
    tex.DrawLatex(0.05, 0.66, "#bf{Thermal Distribution:} Atoms leave the oven following a Maxwell-Boltzmann velocity distribution. Slower atoms spend more time inside the pole pieces, experiencing a");
    tex.DrawLatex(0.05, 0.63, "much greater deflection, which broadens the discrete states.");
    
    // =========================================================================
    // SECCIÓN 2: DINÁMICA DEL DIPOLO SEMICLÁSICO
    // =========================================================================
    tex.SetTextFont(62); tex.SetTextSize(0.026);
    tex.DrawLatex(0.05, 0.57, "SEMICLASSICAL DYNAMICS: ROTATIONAL-TRANSLATIONAL COUPLING");
    
    tex.SetTextFont(42); tex.SetTextSize(0.020);
    tex.DrawLatex(0.05, 0.53, "Semiclassically, the magnetic dipole is simulated as a current loop. The external magnetic field exerts a torque #vec{#tau} = #vec{#mu_{l}} #times #vec{B} that attempts to align the magnetic moment");
    tex.DrawLatex(0.05, 0.50, " with the field vector, while mechanical friction dissipates rotational kinetic energy. The equations governing the angular acceleration and alignment torque are:");
    //tex.DrawLatex(0.05, 0.47, "");
    
    // Ecuación del Torque
    tex.SetTextFont(12); tex.SetTextSize(0.024); tex.SetTextColor(kRed+2);
    tex.DrawLatex(0.35, 0.45, "#tau_{#alpha} = - |#vec{#mu_{l}}| |#vec{B}| sin(#alpha) - #gamma #omega");
    
    tex.SetTextFont(42); tex.SetTextSize(0.020); tex.SetTextColor(kBlack);
    tex.DrawLatex(0.05, 0.40, "As the angle #alpha oscillates and dampens due to the friction coefficient (#gamma), the average effective translational force changes, directly coupling the internal rotational");
    tex.DrawLatex(0.05, 0.37, "orientation with the spatial flight trajectory in the Z-axis. Please note that index 'l' means 'orbital', related to the orbiltal angular momentum of the #vec{#mu_{l}}.");
    
    // =========================================================================
    // SECCIÓN 3: MANUAL DE USUARIO DE LA INTERFAZ
    // =========================================================================
    tex.SetTextFont(62); tex.SetTextSize(0.026);
    tex.DrawLatex(0.05, 0.31, "USER STEPS");
    
    tex.SetTextFont(52); tex.SetTextSize(0.023);
    tex.DrawLatex(0.05, 0.27, "#bf{Step 1 - Setup Quantum Parameters:} In the left panel, select the total spin quantum number (S = 1/2, 1, 3/2). Adjust the field gradient [T/m] ");
    tex.DrawLatex(0.05, 0.23, "and toggle 'Realistic Mode' to activate or deactivate thermal and Maxwell perturbations.");
    tex.DrawLatex(0.05, 0.19, "#bf{Step 2 - 3D Visualization:} Click '1. Build 3D Apparatus' to initialize the TGeoManager geometry. Then, click '2. Fire Beam (3D)' to trace ");
    tex.DrawLatex(0.05, 0.15, "quantum trajectories inside the magnet. Switch to '3. 3D View' tab to read the integrated navigation controls.");
    tex.DrawLatex(0.05, 0.11, "#bf{Step 3 - Statistical Analysis:} Click '3. Generate 2D Plots' to analyze thousands of atoms on the Detector Screen or the Histogram.");
    tex.DrawLatex(0.05, 0.07, "#bf{Step 4 - Semiclassical Setup:} Switch to 'Dipole Dynamics' tab. Define the loop Current (I), Base Field (B_0), and Gradient (dB_z/dz).");
    tex.DrawLatex(0.05, 0.03, "#bf{Step 5 - Run Dynamics:} Use Play/Pause/Reset buttons. Observe how the magnetic torque induces angular oscillations altering the Z-axis force.");
    
    c->Modified();
    c->Update();
}

void SGIntegratedGUI::MostrarPortada() {
    if (!fLienzoPantalla) return;
    
    TCanvas *c = fLienzoPantalla->GetCanvas();
    c->cd();
    c->Clear();
    
    TLatex tex;
    tex.SetNDC(); 
    
    // Título Principal
    tex.SetTextAlign(22); // Alineación centrada
    tex.SetTextFont(62);
    tex.SetTextSize(0.06);
    //tex.SetTextColor(kBlue+2);
    tex.DrawLatex(0.5, 0.85, "SGEE #color[2]{Soft}#color[4]{Edu}");
    
    // Subtítulo
    tex.SetTextFont(52);
    tex.SetTextSize(0.04);
    tex.SetTextColor(kBlack);
    tex.DrawLatex(0.5, 0.75, "Stern Gerlach Experiment Experience #color[2]{Software}#color[4]{Education} ");
    
    // Sección Autores
    tex.SetTextAlign(12); // Alineación a la izquierda
    tex.SetTextFont(62);
    tex.SetTextSize(0.035);
    tex.DrawLatex(0.08, 0.65, "Authors:");
    
    // Autor 1
    tex.SetTextFont(42); tex.SetTextSize(0.03);
    tex.DrawLatex(0.08, 0.58, "#bf{Juli\341n A Salamanca B, PhD}");
    tex.DrawLatex(0.08, 0.52, "Grupo de Investigaci\363n F\355sica e Inform\341tica FISINFOR");
    tex.DrawLatex(0.08, 0.46, "Facultad de Ciencias y Educaci\363n");
    
    // Autor 2
    tex.DrawLatex(0.08, 0.40, "#bf{Diego J Rodr\355guez P}");
    tex.DrawLatex(0.08, 0.34, "Grupo de Investigaci\363n");
    tex.DrawLatex(0.08, 0.28, "Laboratorio de Investigaci\363n en Fuentes Alternas de Energ\355a LIFAE");
    tex.DrawLatex(0.08, 0.22, "Facultad de Ingenier\355a");
    
    // =========================================================
    // NUEVO: CARGAR Y DIBUJAR EL LOGO DE LA UNIVERSIDAD
    // =========================================================
    TString pathLogo = pathDIR_ICONS + "logoUD.png";
    
    TImage *imgUD = TImage::Open(pathLogo.Data());
    
    if (imgUD && imgUD->GetHeight() > 0) {
        // Creamos un sub-lienzo (TPad) en la parte inferior central
        // Coordenadas: x_izq, y_aba, x_der, y_arr (en escala normalizada 0 a 1)
        TPad *padLogo = new TPad("padLogo", "Logo UD", 0.25, 0.02, 0.75, 0.20);
        padLogo->SetFillStyle(4000); // Fondo 100% transparente
        padLogo->SetBorderSize(0);   // Sin bordes
        padLogo->Draw();
        padLogo->cd();
        
        // Dibujar la imagen. La opción "X" la ajusta a los límites del TPad
        imgUD->Draw("X");
        
        c->cd(); // Retornar el control al canvas principal
    } else {
        // Texto de respaldo en caso de que borren la carpeta "icons"
        tex.SetTextFont(62);
	tex.SetTextSize(0.038);
	tex.SetTextColor(kRed+2);
        tex.SetTextAlign(22);
        tex.DrawLatex(0.5, 0.12, "Universidad Distrital Francisco Jose de Caldas");
    }
    
    c->Modified();
    c->Update();
}

void SGIntegratedGUI::Salir() {
    std::cout << "[INFO] Closing integrated simulator..." << std::endl;
    if (fTimer) { fTimer->TurnOff(); fTimer->Stop(); }
    gApplication->Terminate(0); 
}

void SGIntegratedGUI::PlaySim() {
    if (!fIsRunning && fTimer) { fIsRunning = true; fTimer->Start(33); }
}

void SGIntegratedGUI::PauseSim() {
    if (fIsRunning && fTimer) { fIsRunning = false; fTimer->Stop(); }
}

void SGIntegratedGUI::ResetPhysics() {
    PauseSim();
    fZ = 0.5;   
    fVz = 0.0;  
    fTheta = 0.0; 
    fAlpha = TMath::Pi() / 6.0; 
    fOmega = 0.0;
    fGamma = 0.5; 
    UpdateCanvas(); 
}

void SGIntegratedGUI::UpdatePhysics() {
    double current = fEntryCurrent->GetNumber();
    double b0 = fEntryB0->GetNumber();
    double gradB = fEntryGrad->GetNumber();

    fTheta -= current * 0.15; 
    if (fTheta > 2*TMath::Pi()) fTheta -= 2*TMath::Pi();
    if (fTheta < 0) fTheta += 2*TMath::Pi();

    double zFisico = (fZ - 0.5) * 10.0; 
    double currentField = b0 + gradB * zFisico;
    double magMoment = current * fArea;

    double inercia = 0.05; 
    double torque = -magMoment * currentField * std::sin(fAlpha);
    double aceleracionAngular = (torque - fGamma * fOmega) / inercia;

    double dt = 0.033;
    fOmega += aceleracionAngular * dt;
    fAlpha += fOmega * dt;

    double forceZ = magMoment * std::cos(fAlpha) * gradB;
    double acceleration = forceZ / fMass;
    
    fVz += acceleration * dt; 
    fZ += fVz * dt;

    if (fZ > 0.85) { fZ = 0.85; PauseSim(); } 
    else if (fZ < 0.15) { fZ = 0.15; PauseSim(); }

    UpdateCanvas();
}

void SGIntegratedGUI::UpdateCanvas() {
    if (!fEcanvasDipolo || !fEntryCurrent || !fEntryB0 || !fEntryGrad) return;

    double current = fEntryCurrent->GetNumber();
    double b0 = fEntryB0->GetNumber();
    double gradB = fEntryGrad->GetNumber();

    double zFisico = (fZ - 0.5) * 10.0; 
    double currentField = b0 + gradB * zFisico;
    double magMoment = current * fArea;
    double forceZ = magMoment * std::cos(fAlpha) * gradB;

    fLblPosZ->SetText(Form("Z Position: %+.1f m", zFisico));
    fLblFieldZ->SetText(Form("B_z Field:  %+.1f T", currentField));
    fLblForceZ->SetText(Form("F_z Force: %+.3f N", forceZ));

    TCanvas *c1 = fEcanvasDipolo->GetCanvas();
    c1->Clear(); c1->cd();
    c1->SetMargin(0.12, 0.05, 0.05, 0.05); 
    
    double minB_plot = b0 + gradB * ((0.0 - 0.5) * 10.0);
    double maxB_plot = b0 + gradB * ((1.0 - 0.5) * 10.0);
    
    for (int i = 1; i <= 100; ++i) {
        double z_bin_norm = fHeatmap->GetYaxis()->GetBinCenter(i);
        double z_bin_fisico = (z_bin_norm - 0.5) * 10.0;
        fHeatmap->SetBinContent(1, i, b0 + gradB * z_bin_fisico);
    }

    if (std::abs(maxB_plot - minB_plot) < 0.01) {
        fHeatmap->SetMinimum(minB_plot - 0.1); fHeatmap->SetMaximum(minB_plot + 0.1);
    } else {
        if(minB_plot > maxB_plot) std::swap(minB_plot, maxB_plot);
        fHeatmap->SetMinimum(minB_plot); fHeatmap->SetMaximum(maxB_plot);
    }
    fHeatmap->Draw("COL");

    TGaxis *axisZ = new TGaxis(0.0, 0.0, 0.0, 1.0, -5.0, 5.0, 510, "");
    axisZ->SetName("axisZ");
    axisZ->SetTitle("Z Position (m)");
    axisZ->SetTitleSize(0.04);
    axisZ->SetTitleOffset(1.2);
    axisZ->SetLabelSize(0.035);
    axisZ->Draw();

    TLatex tex; tex.SetNDC(false); 

    double rx = 0.15, ry = 0.04; 
    double alphaGrados = fAlpha * 180.0 / TMath::Pi();

    TEllipse *orbitaSombra = new TEllipse(0.5, fZ, rx, ry, 0, 360, alphaGrados);
    orbitaSombra->SetFillStyle(0);
    orbitaSombra->SetLineColor(kBlack);
    orbitaSombra->SetLineWidth(5);
    orbitaSombra->Draw();

    TEllipse *orbita = new TEllipse(0.5, fZ, rx, ry, 0, 360, alphaGrados);
    orbita->SetFillStyle(0);
    orbita->SetLineColor(kYellow);
    orbita->SetLineWidth(2);
    orbita->SetLineStyle(2);
    orbita->Draw();

    double localEx = rx * cos(fTheta);
    double localEy = ry * sin(fTheta);
    double ex = 0.5 + localEx * cos(fAlpha) - localEy * sin(fAlpha);
    double ey = fZ  + localEx * sin(fAlpha) + localEy * cos(fAlpha);
    
    TEllipse *electronSombra = new TEllipse(ex, ey, 0.018, 0.018);
    electronSombra->SetFillColor(kBlack); electronSombra->Draw();

    TEllipse *electron = new TEllipse(ex, ey, 0.014, 0.014);
    electron->SetFillColor(kCyan); electron->Draw();
    
    tex.SetTextColor(kBlack); tex.SetTextSize(0.035);
    tex.DrawLatex(ex + 0.02, ey + 0.02, "e^{-}");

    if (current != 0) {
        double magSize = std::abs(magMoment) * 2.0; 
        double factorDir = (magMoment > 0) ? 1.0 : -1.0;
        double endX = 0.5 + magSize * factorDir * std::sin(fAlpha);
        double endY = fZ  + magSize * factorDir * std::cos(fAlpha);

        TArrow *arrM = new TArrow(0.5, fZ, endX, endY, 0.02, "|>");
        arrM->SetLineColor(kGreen+2);
	arrM->SetLineWidth(4);
	arrM->Draw();
        tex.SetTextColor(kGreen+2);
	tex.DrawLatex(endX + 0.02, endY, "#vec{#mu_{l}}");
    }

    if (std::abs(currentField) > 0.01) {
        double bDisplaySize = std::abs(currentField) * 0.05; 
        double startZB = fZ;
        double endZB = (currentField > 0) ? (fZ + bDisplaySize) : (fZ - bDisplaySize);
        TArrow *arrB = new TArrow(0.2, startZB, 0.2, endZB, 0.02, "|>");
        arrB->SetLineColor(kBlack);
	arrB->SetLineWidth(3);
	arrB->Draw();
        tex.SetTextColor(kBlack);
	tex.DrawLatex(0.12, endZB + ((currentField > 0) ? 0.01 : -0.03), "#vec{B}");
    }

    if (std::abs(forceZ) > 0.001) {
        double fDisplaySize = std::abs(forceZ) * 5.0; 
        double startZF = fZ;
        double endZF = (forceZ > 0) ? (fZ + fDisplaySize) : (fZ - fDisplaySize);
        TArrow *arrF = new TArrow(0.8, startZF, 0.8, endZF, 0.02, "|>");
        arrF->SetLineColor(kMagenta+1);
	arrF->SetLineWidth(4);
	arrF->Draw();
        tex.SetTextColor(kMagenta+1);
	tex.DrawLatex(0.83, endZF + ((forceZ > 0) ? 0.01 : -0.03), "#vec{F_z}");
    }

    c1->Update();
}

void SGIntegratedGUI::ConstruirGeometria() {
    if (gGeoManager) {
        std::cout << "[INFO] Geometry has already been built." << std::endl;
        fTabsSG->SetTab(2); 
        return; 
    }

    TGeoManager *geo = new TGeoManager("GeometriaSG", "Stern-Gerlach 3D");
    TGeoMaterial *matVacio = new TGeoMaterial("Vacio", 0,0,0);
    TGeoMedium *medVacio = new TGeoMedium("Vacio", 1, matVacio);

    TGeoVolume *top = geo->MakeBox("TOP", medVacio, 15.0, 15.0, 15.0);
    geo->SetTopVolume(top);
    top->SetVisibility(kFALSE); 

    TGeoVolume *horno = geo->MakeBox("Horno", medVacio, 0.8, 0.8, 0.8);
    horno->SetLineColor(kOrange-5); horno->SetFillColor(kOrange-5);
    top->AddNode(horno, 1, new TGeoTranslation(-7.55, 0, 0)); 

    TGeoVolume *placaB1 = geo->MakeTube("PlacaB1", medVacio, 0.15, 1.2, 0.05); 
    placaB1->SetLineColor(kGray+1); placaB1->SetFillColor(kGray+1);
    TGeoRotation *rotB1 = new TGeoRotation("rotB1");
    rotB1->RotateY(90); 
    top->AddNode(placaB1, 1, new TGeoCombiTrans(-5.05, 0, 0, rotB1));

    TGeoVolumeAssembly *rendijaB2 = new TGeoVolumeAssembly("RendijaB2");
    TGeoVolume *b2Sup = geo->MakeBox("b2Sup", medVacio, 0.05, 1.2, 0.5); 
    TGeoVolume *b2Inf = geo->MakeBox("b2Inf", medVacio, 0.05, 1.2, 0.5);
    TGeoVolume *b2Izq = geo->MakeBox("b2Izq", medVacio, 0.05, 0.4, 0.1); 
    TGeoVolume *b2Der = geo->MakeBox("b2Der", medVacio, 0.05, 0.4, 0.1);

    b2Sup->SetLineColor(kGray+2);
    b2Sup->SetFillColor(kGray+2);
    b2Inf->SetLineColor(kGray+2);
    b2Inf->SetFillColor(kGray+2);
    b2Izq->SetLineColor(kGray+2);
    b2Izq->SetFillColor(kGray+2);
    b2Der->SetLineColor(kGray+2);
    b2Der->SetFillColor(kGray+2);

    rendijaB2->AddNode(b2Sup, 1, new TGeoTranslation(0, 0, 0.6));  
    rendijaB2->AddNode(b2Inf, 1, new TGeoTranslation(0, 0, -0.6)); 
    rendijaB2->AddNode(b2Izq, 1, new TGeoTranslation(0, 0.8, 0));  
    rendijaB2->AddNode(b2Der, 1, new TGeoTranslation(0, -0.8, 0)); 
    top->AddNode(rendijaB2, 1, new TGeoTranslation(-1.75, 0, 0));

    TGeoVolume *poloSup = geo->MakeTrd1("PoloSup", medVacio, 0.05, 1.0, 1.75, 1.0); 
    poloSup->SetLineColor(kBlue); poloSup->SetFillColor(kBlue); poloSup->SetTransparency(60);
    TGeoRotation *rotSup = new TGeoRotation("rotSup", 90, 0, 0); 
    top->AddNode(poloSup, 1, new TGeoCombiTrans(0, 0, 1.5, rotSup)); 

    TGeoVolume *baseInf = geo->MakeBox("BaseInf", medVacio, 1.75, 1.0, 0.5); 
    baseInf->SetLineColor(kRed);
    baseInf->SetFillColor(kRed);
    baseInf->SetTransparency(60);
    TGeoVolume *muroInf = geo->MakeBox("MuroInf", medVacio, 1.75, 0.25, 0.5);
    muroInf->SetLineColor(kRed);
    muroInf->SetFillColor(kRed);
    muroInf->SetTransparency(60);
    top->AddNode(baseInf, 1, new TGeoTranslation(0, 0, -1.5));
    top->AddNode(muroInf, 1, new TGeoTranslation(0, 0.75, -0.5)); 
    top->AddNode(muroInf, 2, new TGeoTranslation(0, -0.75, -0.5)); 

    TGeoVolume *pantalla = geo->MakeBox("Pantalla", medVacio, 0.02, 1.5, 1.5);
    pantalla->SetLineColor(kCyan-9);
    pantalla->SetTransparency(70);
    top->AddNode(pantalla, 1, new TGeoTranslation(1.77, 0, 0));

    geo->CloseGeometry();

    fTabsSG->SetTab(2);
    fCanvas3D->cd();
    fCanvas3D->Clear();

    top->Draw("ogl");

    TPolyLine3D *letraS = new TPolyLine3D(6);
    letraS->SetPoint(0,  0.2, -1.1, 1.75);
    letraS->SetPoint(1, -0.2, -1.1, 1.75); 
    letraS->SetPoint(2, -0.2, -1.1, 1.55);
    letraS->SetPoint(3,  0.2, -1.1, 1.55); 
    letraS->SetPoint(4,  0.2, -1.1, 1.35);
    letraS->SetPoint(5, -0.2, -1.1, 1.35);
    letraS->SetLineColor(kBlue);
    letraS->SetLineWidth(3); letraS->Draw();

    TPolyLine3D *letraN = new TPolyLine3D(4);
    letraN->SetPoint(0, -0.2, -1.1, -1.65);
    letraN->SetPoint(1, -0.2, -1.1, -1.35); 
    letraN->SetPoint(2,  0.2, -1.1, -1.65);
    letraN->SetPoint(3,  0.2, -1.1, -1.35); 
    letraN->SetLineColor(kRed);
    letraN->SetLineWidth(3);
    letraN->Draw();
    
    fCanvas3D->cd();
    TLatex tex;
    tex.SetNDC();
    
    tex.SetTextFont(62);
    tex.SetTextSize(0.045);
    tex.SetTextColor(kBlue+2);
    tex.DrawLatex(0.05, 0.85, "OPENGL SIMULATOR CONTROL GUIDE:");

    tex.SetTextFont(42);
    tex.SetTextSize(0.038);
    tex.SetTextColor(kBlack);
    tex.DrawLatex(0.05, 0.76, "#bullet Camera Rotation: Left Click + Drag mouse");
    tex.DrawLatex(0.05, 0.70, "#bullet Zoom (In/Out): Mouse wheel (Scroll)");
    tex.DrawLatex(0.05, 0.64, "#bullet Panning: Right Click + Drag mouse");
    tex.DrawLatex(0.05, 0.58, "#bullet Reset View: Press the 'r' key");
    tex.DrawLatex(0.05, 0.52, "#bullet Wireframe/Solid Mode: Press 'w' (Wireframe) or 's' (Solid)");

    tex.SetTextFont(52);
    tex.SetTextSize(0.032);
    tex.SetTextColor(kGray+2);
    tex.DrawLatex(0.05, 0.40, "Note: The apparatus will be rendered interactively in a pop-up");
    tex.DrawLatex(0.05, 0.35, "window, independent of ROOT's default 'GL Viewer'.");
    
    fCanvas3D->Modified();
    fCanvas3D->Update(); 

    TGLViewer *visorGL = (TGLViewer *)fCanvas3D->GetViewer3D();
    if (visorGL) {
        visorGL->SetCurrentCamera(TGLViewer::kCameraPerspXOZ);
        visorGL->SetResetCamerasOnUpdate(kFALSE);
        visorGL->CurrentCamera().RotateRad(1.2, -1.2);
        
        Double_t centroOrigen[3] = {0.0, 0.0, 0.0};
        visorGL->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, centroOrigen);
        
        TGLAnnotation *leyendaX = new TGLAnnotation(visorGL, "X (Red) : Propagation", 0.02, 0.94);
        leyendaX->SetTextSize(0.03);
        TGLAnnotation *leyendaY = new TGLAnnotation(visorGL, "Y (Green): Transverse Axis", 0.02, 0.89);
        leyendaY->SetTextSize(0.03);
        TGLAnnotation *leyendaZ = new TGLAnnotation(visorGL, "Z (Blue) : Deflection", 0.02, 0.84);
        leyendaZ->SetTextSize(0.03);
        
        visorGL->RequestDraw();
    }
}

void SGIntegratedGUI::Simular3D() {
    if (!gGeoManager || !fCanvas3D) return;

    Limpiar3D(); 
    fTabsSG->SetTab(2); 
    fCanvas3D->cd();

    double fuerzaB = fEntradaCampo->GetNumber();
    int seleccion = fComboEspin->GetSelected();
    double S = 0.0; 
    
    switch (seleccion) {
        case 0: S = 0.5; break;
        case 1: S = 1.0; break;
        case 2: S = 1.5; break;
        default: S = 0.5; break;
    }

    int numEstados = static_cast<int>(2 * S + 1);
    int nAtomosTotal = fEntradaAtomos->GetNumber();
    int nAtomosVisuales = nAtomosTotal;
    int maxVisuales = 300; 

    if (nAtomosTotal > maxVisuales) nAtomosVisuales = maxVisuales;

    bool esRealista = fCheckRealista->IsOn();
    TPolyMarker3D *impactos = new TPolyMarker3D(nAtomosVisuales);
    impactos->SetMarkerStyle(20); 
    impactos->SetMarkerSize(0.15); 
    impactos->SetMarkerColor(esRealista ? kTeal+1 : kBlue+1);

    for (int i = 0; i < nAtomosVisuales; ++i) {
        int estado = gRandom->Integer(numEstados); 
        double ms = -S + estado; 
        double posY = gRandom->Uniform(-0.4, 0.4); 
        double zInicial = 0; 
        double desviacionZ = 0;

        if (esRealista) {
            double perfilY = 1.0 - 0.9 * (posY * posY) / (0.4 * 0.4);
            double v = gRandom->Gaus(1.0, 0.07); 
            if (v < 0.8) v = 0.8; 
            if (v > 1.2) v = 1.2; 
            
            double factorTermico = 1.0 / (v * v);
            double asimetriaZ = (ms > 0) ? 1.2 : 0.85; 
            
            desviacionZ = ms * (fuerzaB * 0.1) * perfilY * factorTermico * asimetriaZ;
            double grosorRendija = 0.015 * (1.0 - 0.3 * (posY * posY) / (0.4 * 0.4));
            zInicial = gRandom->Gaus(0, grosorRendija);
        } else {
            desviacionZ = ms * (fuerzaB * 0.1);
            zInicial = gRandom->Uniform(-0.02, 0.02);
        }

        double posZFinal = zInicial + desviacionZ;
        TPolyLine3D *pista = new TPolyLine3D(4);
        pista->SetPoint(0, -7.55, 0, 0);              
        pista->SetPoint(1, -5.05, posY, zInicial);    
        pista->SetPoint(2, -1.75, posY, zInicial);    
        pista->SetPoint(3, 1.77, posY, posZFinal); 
        pista->SetLineColorAlpha(kAzure-4, 0.05); 
        pista->SetLineWidth(1); 
        pista->Draw(); 
        fPistas3D.push_back(pista);

        impactos->SetPoint(i, 1.77, posY, posZFinal);
    }
    
    impactos->Draw();
    fMarcadores3D.push_back(impactos);
    
    if (fCanvas3D) {
        TGLViewer *visorGL = (TGLViewer *)fCanvas3D->GetViewer3D();
        if (visorGL) { visorGL->SetResetCamerasOnUpdate(kFALSE); }
        fCanvas3D->Modified();
	fCanvas3D->Update();
    }
}

void SGIntegratedGUI::Limpiar3D() {
    if (!gGeoManager || !fCanvas3D) return;
    for (auto pista : fPistas3D) { delete pista; }
    fPistas3D.clear();
    for (auto marcador : fMarcadores3D) { delete marcador; }
    fMarcadores3D.clear();
    
    if (fCanvas3D) {
        fCanvas3D->cd();
        TGLViewer *visorGL = (TGLViewer *)fCanvas3D->GetViewer3D();
        if (visorGL) { visorGL->SetResetCamerasOnUpdate(kFALSE); }
        fCanvas3D->Modified();
	fCanvas3D->Update();
    }
}

void SGIntegratedGUI::Simular2D() {
    int nAtomos = fEntradaAtomos->GetNumber();
    double fuerzaB = fEntradaCampo->GetNumber();
    int seleccion = fComboEspin->GetSelected();
    
    double S = 0.0; 
    switch (seleccion) {
        case 0: S = 0.5; break;
        case 1: S = 1.0; break;
        case 2: S = 1.5; break;
        default: S = 0.5; break;
    }
    int numEstados = static_cast<int>(2 * S + 1);
    bool esRealista = fCheckRealista->IsOn();

    double limiteZ = esRealista ? 5.0 : 2.5; 
    fHistDesviacion->SetBins(150, -limiteZ, limiteZ);
    fHistDesviacion->Reset();
    fGraficoImpactos->Set(0); 

    if (esRealista) {
        fGraficoImpactos->SetTitle("Screen Pattern (Realistic);Y (mm);Z (mm)");
        fGraficoImpactos->SetMarkerColor(kTeal+3);
        fHistDesviacion->SetTitle("Impact Distribution (Thermal/Asymmetry);Z Deflection (mm);Atoms");
        fHistDesviacion->SetFillColor(kTeal-4);
    } else {
        fGraficoImpactos->SetTitle("Screen Pattern (Ideal);Y (mm);Z (mm)");
        fGraficoImpactos->SetMarkerColor(kBlue+2);
        fHistDesviacion->SetTitle("Spatial Quantization (Ideal);Z Deflection (mm);Atoms");
        fHistDesviacion->SetFillColor(kBlue-4);
    }

    for (int i = 0; i < nAtomos; ++i) {
        int estado = gRandom->Integer(numEstados); 
        double ms = -S + estado; 
        double posY = gRandom->Uniform(-0.4, 0.4); 
        double posZ = 0;

        if (esRealista) {
            double perfilY = 1.0 - 0.9 * (posY * posY) / (0.4 * 0.4);
            double v = gRandom->Gaus(1.0, 0.07); 
            if (v < 0.8) v = 0.8; 
            if (v > 1.2) v = 1.2; 
            double factorTermico = 1.0 / (v * v);
            double asimetriaZ = (ms > 0) ? 1.2 : 0.85; 
            
            double desviacionZ = ms * (fuerzaB * 0.1) * perfilY * factorTermico * asimetriaZ;
            double grosorRendija = 0.015 * (1.0 - 0.3 * (posY * posY) / (0.4 * 0.4));
            double ensanchamientoHaz = gRandom->Gaus(0, grosorRendija);
            posZ = desviacionZ + ensanchamientoHaz;
        } else {
            double desviacionZ = ms * (fuerzaB * 0.1);
            posZ = desviacionZ + gRandom->Uniform(-0.02, 0.02);
        }

        fHistDesviacion->Fill(posZ);
	fGraficoImpactos->SetPoint(i, posY, posZ);
    }

    TCanvas *c1 = fLienzoPantalla->GetCanvas();
    c1->cd(); c1->Clear(); fGraficoImpactos->Draw("AP");
    fGraficoImpactos->GetXaxis()->SetLimits(-1.5, 1.5); 
    fGraficoImpactos->GetYaxis()->SetRangeUser(-limiteZ, limiteZ);
    c1->Update();

    TCanvas *c2 = fLienzoHist->GetCanvas();
    c2->cd();
    c2->Clear();
    fHistDesviacion->Draw();
    c2->Update();
    fTabsSG->SetTab(0); 
}

#ifndef __CINT__
int main(int argc, char **argv) {
    TApplication app("SESGEE_App", &argc, argv);
    new SGIntegratedGUI(gClient->GetRoot(), 950, 650);
    app.Run();
    return 0;
}
#endif
