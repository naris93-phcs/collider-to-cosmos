# Collider → Cosmos

### From same-sign WW scattering at the LHC to extended Higgs sectors and the early Universe

**Collider → Cosmos** is an MSc-level computational high-energy physics project developing a particle-level analysis of electroweak same-sign W-boson scattering at the LHC.

The project starts from the Standard Model process

**pp → W⁺W⁺jj**

and builds the simulation and analysis chain from hard-scattering event generation to experimentally motivated particle-level observables.

The longer-term objective is to use this validated Standard Model baseline to investigate extended Higgs sectors — particularly the **Georgi–Machacek model** — and ultimately connect collider constraints with the physics of the **electroweak phase transition in the early Universe**.

---

## Simulation and Analysis Pipeline

```text
MadGraph5_aMC@NLO
        ↓
Hard-scattering events
        ↓
Pythia 8
        ↓
Parton shower + hadronization
        ↓
HepMC
        ↓
Stable final-state particles
        ↓
FastJet
anti-kT, R = 0.4
        ↓
Particle-level jets
        ↓
C++ / ROOT analysis
        ↓
Dressed leptons + MET + VBS observables
        ↓
ATLAS-inspired particle-level selection
```

The current workflow therefore reaches the **particle-level fiducial-analysis stage**.

---

## Physics Motivation

Same-sign W-boson scattering is an important vector-boson-scattering (VBS) process at the LHC and provides a direct probe of the electroweak sector.

For the leptonic channel considered here,

**pp → W⁺W⁺jj**

with

**W⁺ → ℓ⁺ν, ℓ = e, μ**

the characteristic experimental signature contains

```text
two same-sign charged leptons
              +
missing transverse momentum
              +
two energetic tagging jets
              +
additional QCD radiation
```

The VBS topology is characterized by two energetic jets with large dijet invariant mass and substantial rapidity separation.

The analysis therefore focuses particularly on:

- dijet invariant mass **m<sub>jj</sub>**
- jet rapidity separation **|Δy<sub>jj</sub>|**

These observables help select a VBS-enhanced region of phase space and provide sensitivity to the dynamics of electroweak symmetry breaking.

---

## Current Standard Model Sample

The present baseline uses a pure electroweak same-sign W⁺W⁺jj sample generated with **MadGraph5_aMC@NLO**.

```text
Process:        pp → W+ W+ jj
Decay:          W+ → l+ ν
Leptons:        e+, μ+
Events:         10,000
Collision:      pp
√s:             13 TeV
Cross section:  0.008951 ± 0.00002036 pb
```

The quoted uncertainty is the Monte Carlo integration uncertainty from event generation and is **not** a complete theoretical uncertainty.

The hard events are showered and hadronized using **Pythia 8**, with the resulting particle-level event record stored in **HepMC** format.

---

## Particle-Level Object Reconstruction

### Jets

Stable visible final-state particles are clustered with **FastJet** using the anti-kT algorithm with

**R = 0.4**

The current jet definition uses:

- **p<sub>T</sub> > 25 GeV**
- **|η| < 4.5**

Neutrinos and muons are excluded from jet clustering.

The two highest-p<sub>T</sub> selected jets define the current tagging-jet pair.

---

### Dressed Leptons

Prompt electron and muon candidates are identified from the generated W-boson decay history.

To account for nearby photon radiation, the lepton four-momentum is dressed using stable photons satisfying

**ΔR(ℓ, γ) < 0.1**

The current dressed-lepton acceptance is:

- **p<sub>T</sub> > 27 GeV**
- **|η| < 2.5**

Events are then required to contain exactly two selected same-sign dressed leptons with

**m<sub>ℓℓ</sub> > 20 GeV**

---

### Missing Transverse Momentum

Particle-level missing transverse momentum is currently constructed from stable final-state neutrinos.

The baseline requirement is

**E<sub>T</sub><sup>miss</sup> > 30 GeV**

---

## VBS Selection

The current ATLAS-inspired VBS jet requirements are:

- **p<sub>T,j1</sub> > 65 GeV**
- **p<sub>T,j2</sub> > 35 GeV**
- **m<sub>jj</sub> > 500 GeV**
- **|Δy<sub>jj</sub>| > 2**

These cuts select a VBS-enhanced region of phase space. They should not be interpreted as isolating an individual subset of Feynman diagrams.

---

## First Particle-Level Baseline Result

For the current sample of **10,000 generated events**, the cumulative lepton and missing-momentum selection gives:

| Selection | Events |
|---|---:|
| Two dressed leptons, p<sub>T</sub> > 27 GeV and \|η\| < 2.5 | 3416 |
| + same-sign requirement | 3415 |
| + m<sub>ℓℓ</sub> > 20 GeV | 3380 |
| + E<sub>T</sub><sup>miss</sup> > 30 GeV | 2959 |

The independent jet/VBS cutflow is:

| Selection | Events |
|---|---:|
| At least two selected jets | 9749 |
| + p<sub>T,j1</sub> > 65 GeV, p<sub>T,j2</sub> > 35 GeV | 8459 |
| + m<sub>jj</sub> > 500 GeV | 2783 |
| + \|Δy<sub>jj</sub>\| > 2 | 1384 |

Requiring the current lepton, missing-momentum and VBS selections simultaneously gives:

### **371 / 10,000 selected events**

corresponding to a baseline Monte Carlo efficiency of:

### **3.71%**

This is the first complete **Standard Model particle-level baseline selection** in the project.

---

## Selected Particle-Level Distributions

### Dijet Invariant Mass

The reconstructed particle-level dijet invariant-mass distribution shows a broad falling spectrum extending into the multi-TeV region.

![Particle-level dijet invariant mass](plots/mjj.png)

---

### Dijet Rapidity Separation

The rapidity separation of the two leading selected jets provides a second characteristic VBS observable.

![Particle-level dijet rapidity separation](plots/dy_jj.png)

---

### Leading Dressed-Lepton Transverse Momentum

![Leading dressed-lepton transverse momentum](plots/lepton1_pt.png)

---

### Dilepton Invariant Mass

![Particle-level dilepton invariant mass](plots/mll.png)

---

## From Partons to Particle-Level Jets

An earlier stage of the project studied the outgoing matrix-element quarks directly.

This provided a useful first view of the VBS topology through quantities such as:

- **m<sub>qq</sub>**
- **|Δy<sub>qq</sub>|**

Those quantities are useful for understanding the hard process but are **not experimentally reconstructed jets**.

The current analysis has therefore progressed through:

```text
matrix-element quarks
        ↓
parton shower
        ↓
hadronization
        ↓
stable final-state particles
        ↓
anti-kT clustering
        ↓
particle-level jets
```

This allows the analysis to work with experimentally meaningful particle-level jet observables.

---

## Current Status and Validation

The present analysis should be interpreted as an **ATLAS-inspired Standard Model particle-level baseline**, not yet as an exact reproduction of the final ATLAS fiducial definition.

The next validation steps include:

- refining the prompt-photon definition used for lepton dressing
- implementing the particle-level electron–jet overlap-removal prescription
- implementing the b-jet veto
- validating event weights and absolute normalization
- including the corresponding W⁻W⁻jj contribution where required
- comparing particle-level distributions with unfolded ATLAS / HEPData results
- documenting a fully reproducible Pythia/HepMC runtime configuration

Scientific correctness and validation take priority over agreement with any particular reference distribution.

---

## Toward Experimental Measurements

The analysis is designed to meet collider measurements at the **particle level**:

```text
THEORY / SIMULATION

Matrix element
      ↓
Parton shower
      ↓
Hadronization
      ↓
Particle-level objects
      ↓
Fiducial observables

              ↕ comparison

EXPERIMENT

Detector data
      ↓
Reconstruction
      ↓
Background subtraction
      ↓
Unfolding
      ↓
Particle-level observables
```

A full detector simulation is therefore not required for the current baseline comparison with unfolded measurements.

---

## Long-Term Research Direction

Once the Standard Model same-sign WW baseline is validated, the project will move toward physics beyond the Standard Model.

The primary model of interest is the **Georgi–Machacek (GM) model**, which extends the Standard Model Higgs sector while allowing custodial symmetry to be preserved at tree level.

Of particular interest is the custodial-fiveplet doubly charged scalar

**H₅⁺⁺**

which can couple directly to

**W⁺W⁺**

Same-sign WW scattering therefore provides a natural collider channel for investigating this extended scalar sector.

The intended research chain is:

```text
SM same-sign WW VBS
        ↓
Validated particle-level baseline
        ↓
Georgi–Machacek benchmarks
        ↓
Collider constraints
        ↓
Viable scalar-sector parameter space
        ↓
Finite-temperature electroweak physics
        ↓
Electroweak phase transition
        ↓
Early-Universe interpretation
```

Possible gravitational-wave phenomenology is considered a later extension rather than part of the current baseline.

---

## Repository Structure

```text
collider_to_cosmos/
│
├── analysis/
│   ├── particle_level.cpp
│   ├── lhe_reader.cpp
│   └── supporting analysis code
│
├── plots/
│   ├── mjj.png
│   ├── dy_jj.png
│   ├── lepton1_pt.png
│   └── mll.png
│
├── ROOT_examples.txt
├── workflow.txt
├── README.md
└── .gitignore
```

Large generated event samples, local generator installations, HepMC files, ROOT output files and compiled binaries are intentionally excluded from version control.

---

## Tools

The current workflow uses:

- **C++17**
- **ROOT**
- **MadGraph5_aMC@NLO**
- **Pythia 8**
- **HepMC**
- **FastJet**
- **Linux / WSL2**
- **Git / GitHub**

---

## Project Philosophy

The project follows the workflow:

> **Physical principle → research question → observable → simulation/code → validation → physical interpretation**

The objective is not simply to generate Monte Carlo events, but to understand how theoretical quantities become experimentally meaningful collider observables — and how those observables can ultimately constrain physics beyond the Standard Model.

---

## Current Milestone

**Completed:**  
MadGraph → Pythia → HepMC → FastJet → ROOT particle-level analysis

**Current result:**  
First ATLAS-inspired same-sign WWjj baseline selection, with **371 / 10,000 events** passing the combined selection.

**Next milestone:**  
Complete the fiducial-definition refinements and validate the Standard Model baseline against unfolded ATLAS measurements before introducing controlled Georgi–Machacek benchmarks.

---

*Collider → Cosmos is an ongoing MSc-level high-energy physics research project.*