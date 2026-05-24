// =============================================================================
// SGEE SoftEdu - Stern-Gerlach Experiment Experience (Installer Script)
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
