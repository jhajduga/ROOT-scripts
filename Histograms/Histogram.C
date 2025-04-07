#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TAxis.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <TSystemFile.h>
#include <TCanvas.h>
#include <TLegend.h>

// Helper function to set X axis bin labels and log-scale Y axis
void SetAxisLabels(TH1F* hist) {
    const char* labels[16] = {
        "UserStepMax/Transportation",
        "Photoelectric (phot)",
        "Compton (compt)",
        "Pair production (conv)",
        "Rayleigh (Rayl)",
        "Electron Ionization (eIoni)",
        "Multiple Coulomb Scattering (msc)",
        "e+ e- Annihilation (annihil)",
        "Bremsstrahlung (eBrem)",
        "Radioactive Decay",
        "Auger Effect",
        "X-ray Fluorescence (phot_fluo)",
        "Hadronic Elastic (hadElastic)",
        "Neutron Inelastic (neutronInelastic)",
        "Neutron Capture (nCapture)",
        "Ion Ionization (ionIoni)"
    };

    for (int i = 0; i < 16; ++i) {
        hist->GetXaxis()->SetBinLabel(i + 1, labels[i]);
    }

    hist->SetMinimum(1);
    hist->SetMaximum(hist->GetMaximum() * 1.2);
    hist->SetStats(0); // Disable TPaveStats

    // Set axis labels and formatting
    hist->GetYaxis()->SetTitle("Number of interactions");
    hist->GetXaxis()->SetLabelSize(0.025);
    hist->GetXaxis()->LabelsOption("u"); // vertical orientation
    hist->GetXaxis()->SetTitleOffset(1.8);
    hist->GetYaxis()->SetTitleOffset(1.6);
    hist->GetXaxis()->SetTickLength(0.02);
    hist->GetYaxis()->SetTickLength(0.02);

}

void Histogram(const char* fileName = "") {
    std::cout << "[INFO] Starting histogram generation..." << std::endl;

    // Automatically find the first .root file if none is specified
    TString rootFile;
    if (strlen(fileName) == 0) {
        std::cout << "[INFO] No input file specified. Scanning current directory for .root files..." << std::endl;
        TSystemDirectory dir(".", ".");
        TList *files = dir.GetListOfFiles();
        if (!files) {
            std::cerr << "[ERROR] Failed to read current directory." << std::endl;
            return;
        }
        TIter next(files);
        TSystemFile *file;
        while ((file = (TSystemFile*)next())) {
            if (file->IsDirectory()) continue;
            TString fname = file->GetName();
            if (fname.EndsWith(".root")) {
                rootFile = fname;
                break;
            }
        }
        if (rootFile.Length() == 0) {
            std::cerr << "[ERROR] No .root file found in the current directory!" << std::endl;
            return;
        }
        fileName = rootFile.Data();
    }
    std::cout << "[INFO] Using file: " << fileName << std::endl;

    // Open the ROOT file and extract the tree
    TFile *f = TFile::Open(fileName);
    if (!f || f->IsZombie()) {
        std::cerr << "[ERROR] Failed to open ROOT file: " << fileName << std::endl;
        return;
    }

    TTree *tree = (TTree*)f->Get("WaterTankTTree");
    if (!tree) {
        std::cerr << "[ERROR] Tree 'WaterTankTTree' not found in file " << fileName << std::endl;
        return;
    }
    std::cout << "[INFO] Successfully loaded TTree: WaterTankTTree" << std::endl;

    // Draw and configure histogram for all particles
    std::cout << "[INFO] Generating histogram for all particles..." << std::endl;
    tree->Draw("VoxelProcessTypeId>>hAllParticles(16,-0.5,15.5)", "", "");
    TH1F* hAllParticles = (TH1F*)gDirectory->Get("hAllParticles");
    if (hAllParticles) {
        hAllParticles->SetTitle("All Particles: Interaction Type Distribution");
        hAllParticles->SetLineColor(kBlack);
        hAllParticles->SetLineWidth(2);
        SetAxisLabels(hAllParticles);
    }

    // Draw and configure histogram for all photons
    std::cout << "[INFO] Generating histogram for all photons..." << std::endl;
    tree->Draw("VoxelProcessTypeId>>hAllPhotons(16,-0.5,15.5)", "VoxelTrkTypeId==1", "");
    TH1F* hAllPhotons = (TH1F*)gDirectory->Get("hAllPhotons");
    if (hAllPhotons) {
        hAllPhotons->SetTitle("All Photons: Interaction Type Distribution");
        hAllPhotons->SetLineColor(kBlue);
        hAllPhotons->SetLineStyle(2);
        hAllPhotons->SetLineWidth(2);
        SetAxisLabels(hAllPhotons);
    }

    // Draw and configure histogram for primary photons only
    std::cout << "[INFO] Generating histogram for primary photons..." << std::endl;
    tree->Draw("VoxelProcessTypeId>>hPrimaryPhotons(16,-0.5,15.5)", "VoxelTrkTypeId==1&&VoxelTrkId==1", "");
    TH1F* hPrimaryPhotons = (TH1F*)gDirectory->Get("hPrimaryPhotons");
    if (hPrimaryPhotons) {
        hPrimaryPhotons->SetTitle("Primary Photons Only: Interaction Type Distribution");
        hPrimaryPhotons->SetLineColor(kRed);
        hPrimaryPhotons->SetLineStyle(3);
        hPrimaryPhotons->SetLineWidth(2);
        SetAxisLabels(hPrimaryPhotons);
    }

    std::cout << "[INFO] Drawing combined canvas..." << std::endl;
    TCanvas* cAll = new TCanvas("cAll", "All on one canvas", 2560, 1600);
    cAll->SetLogy();
    cAll->SetRightMargin(0.03);
    hAllParticles->Draw("HIST");
    hAllPhotons->Draw("HIST SAME");
    hPrimaryPhotons->Draw("HIST SAME");

    TLegend* legend = new TLegend(0.7, 0.75, 0.9, 0.9);
    legend->AddEntry(hAllParticles, "All Particles", "l");
    legend->AddEntry(hAllPhotons, "All Photons", "l");
    legend->AddEntry(hPrimaryPhotons, "Primary Photons", "l");
    legend->Draw();

    std::cout << "[INFO] Saving: hist_all_combined.pdf" << std::endl;
    cAll->SaveAs("hist_all_combined.pdf");

    std::cout << "[INFO] Drawing and saving individual canvases..." << std::endl;

    TCanvas* c1 = new TCanvas("c1", "All Particles", 2560, 1600);
    c1->SetLogy();
    c1->SetRightMargin(0.03);
    hAllParticles->SetLineColor(kBlack);
    hAllParticles->SetLineStyle(1);
    hAllParticles->Draw("HIST");
    c1->SaveAs("hist_all_particles.pdf");

    TCanvas* c2 = new TCanvas("c2", "All Photons", 2560, 1600);
    c2->SetLogy();
    c2->SetRightMargin(0.03);
    hAllPhotons->SetLineColor(kBlack);
    hAllPhotons->SetLineStyle(1);
    hAllPhotons->Draw("HIST");
    c2->SaveAs("hist_all_photons.pdf");

    TCanvas* c3 = new TCanvas("c3", "Primary Photons", 2560, 1600);
    c3->SetLogy();
    c3->SetRightMargin(0.03);
    hPrimaryPhotons->SetLineColor(kBlack);
    hPrimaryPhotons->SetLineStyle(1);
    hPrimaryPhotons->Draw("HIST");
    c3->SaveAs("hist_primary_photons.pdf");

    std::cout << "[INFO] All plots saved. Done!" << std::endl;
}
