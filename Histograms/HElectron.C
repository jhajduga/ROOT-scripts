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
    const char* labels[9] = {
        "Primary electron (no creator process)",
        "Photoelectric electron",
        "Compton-scattered electron",
        "Pair-production electron",
        "Auger electron (non-radiative transition)",
        "Fluorescent electron (atomic de-excitation)",
        "Electron ionization electron",
        "Ion-induced ionization electron",
        "Beta electron from radioactive decay"
    };

    for (int i = 0; i < 9; ++i) {
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

void HElectron(const char* fileName = "") {
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
    tree->Draw("ProcessTypeId>>hAllElectrons(9,-0.5,8.5)", "", "");
    TH1F* hAllElectrons = (TH1F*)gDirectory->Get("hAllElectrons");
    if (hAllElectrons) {
        hAllElectrons->SetTitle("Electron origin type for all electrons");
        hAllElectrons->SetLineColor(kBlack);
        hAllElectrons->SetLineWidth(2);
        hAllElectrons->Scale(hAllElectrons->GetMaximum()/100000000,"WIDTH");
        SetAxisLabels(hAllElectrons);
    }

    // Draw and configure histogram for all photons
    std::cout << "[INFO] Generating histogram for all photons..." << std::endl;
    tree->Draw("ProcessTypeId>>hFirstBornElectrons(9,-0.5,8.5)", "VoxelTrkId==2", "");
    TH1F* hFirstBornElectrons = (TH1F*)gDirectory->Get("hFirstBornElectrons");
    if (hFirstBornElectrons) {
        hFirstBornElectrons->SetTitle("Electron origin type for first generation of electrons");
        hFirstBornElectrons->SetLineColor(kBlue);
        hFirstBornElectrons->SetLineStyle(2);
        hFirstBornElectrons->SetLineWidth(2);
        hFirstBornElectrons->Scale(hFirstBornElectrons->GetMaximum()/100000000,"WIDTH");

        SetAxisLabels(hFirstBornElectrons);
    }



    std::cout << "[INFO] Drawing combined canvas..." << std::endl;
    TCanvas* cAll = new TCanvas("cAll", "All on one canvas", 2560, 1600);
    cAll->SetLogy();
    cAll->SetBottomMargin(0.12);
    cAll->SetTopMargin(0.09);
    cAll->SetRightMargin(0.05);
    cAll->SetLeftMargin(0.11);
    hAllElectrons->Draw("HIST");
    hFirstBornElectrons->Draw("HIST SAME");

    TLegend* legend = new TLegend(0.80, 0.80, 0.97, 0.97);
    legend->AddEntry(hAllElectrons, "All Electrons", "l");
    legend->AddEntry(hFirstBornElectrons, "First Generation Of Electrons", "l");
    legend->Draw();

    std::cout << "[INFO] Saving: hist_all_combined.pdf" << std::endl;
    cAll->SaveAs("hist_all_combined.pdf");

    std::cout << "[INFO] Drawing and saving individual canvases..." << std::endl;

    TCanvas* c1 = new TCanvas("c1", "All Particles", 2560, 1600);
    c1->SetLogy();
    c1->SetBottomMargin(0.12);
    c1->SetTopMargin(0.09);
    c1->SetRightMargin(0.05);
    c1->SetLeftMargin(0.11);
    hAllElectrons->SetLineColor(kBlack);
    hAllElectrons->SetLineStyle(1);
    hAllElectrons->Draw("HIST");
    c1->SaveAs("hist_all_electrons.pdf");

    TCanvas* c2 = new TCanvas("c2", "All Photons", 2560, 1600);
    c2->SetLogy();
    c2->SetBottomMargin(0.12);
    c2->SetTopMargin(0.09);
    c2->SetRightMargin(0.05);
    c2->SetLeftMargin(0.11);

    hFirstBornElectrons->SetLineColor(kBlack);
    hFirstBornElectrons->SetLineStyle(1);
    hFirstBornElectrons->Draw("HIST");
    c2->SaveAs("hist_generation_1_electrons.pdf");

    std::cout << "[INFO] All plots saved. Done!" << std::endl;
}




