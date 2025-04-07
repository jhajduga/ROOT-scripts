# Histogram.C - Interaction Type Histogram Generator

This ROOT macro generates histograms of interaction types from a Geant4 simulation ntuple. It is designed to visualize the types of physical processes occurring during particle transport.

## 🔧 Usage

You can run the macro using ROOT in two modes:

### 1. With a specified input file:
```bash
root -l 'Histogram.C("relative/path/to/data.root")'
```
- The macro will load the specified `.root` file.

### 2. Without specifying a file:
```bash
root -l Histogram.C
```
- The macro will automatically search the current directory for the first `.root` file it finds and use that as input.

## 📊 Output
The script generates four canvases:

1. **Combined Canvas**: All interaction histograms overlaid with different line colors.
2. **Individual Canvas: All Particles**
3. **Individual Canvas: All Photons**
4. **Individual Canvas: Primary Photons Only**

Each canvas is saved as a PDF file:
- `hist_all_combined.pdf`
- `hist_all_particles.pdf`
- `hist_all_photons.pdf`
- `hist_primary_photons.pdf`

## 📁 Required Tree Structure
The macro expects the `.root` file to contain a TTree named:
```
WaterTankTTree
```
with the following branches:

| Branch Name          | Type    | Description                                      |
|----------------------|---------|--------------------------------------------------|
| `VoxelProcessTypeId` | `Int_t` | Encoded process type ID (e.g., 0–15)             |
| `VoxelTrkTypeId`     | `Int_t` | Particle type (1 = photon, others = optional)    |
| `VoxelTrkId`         | `Int_t` | Track ID (1 = primary particle, >1 = secondary)  |

These variables are used for filtering and histogramming interaction processes.

## ✨ Features
- Automatically detects `.root` files in the current directory if no argument is provided.
- Reads TTree named `WaterTankTTree`.
- Histograms show interaction types (e.g., Compton, Rayleigh, Photoelectric, etc.).
- Custom X-axis bin labels for interaction type names.
- Vertical X-axis labels with reduced size for improved readability.
- Y-axis uses logarithmic scale.
- Clean formatting with frame margins adjusted so axis labels fit inside the canvas.
- Suppresses default `TPaveStats` boxes.
- Console log output (`[INFO]`, `[ERROR]`) to follow progress.

---
**Author**: Jakub Hajduga
**License**: MIT

