# Prompt-lepton classifier validation

Date: 2026-11-25

## Motivation

The direct hard-W ancestry requirement introduces a
generator-record-dependent selection bias in the GM
H5++ sample at 150 GeV.

Intermediate W propagators are not always explicitly
stored in the event record.

## Implementation

Two selectable lepton-classification modes:

- directW: original hard-W lineage requirement (default).
- prompt: general prompt-lepton ancestry approximation.

Lifetime reconstruction, photon dressing, jet clustering,
b-ghost matching and kinematic cuts remain unchanged.

## Validation results

| Sample | Generated | DirectW leptons | Prompt leptons | DirectW selected | Prompt selected |
|---|---:|---:|---:|---:|---:|
| SM EW+ | 10000 | 20000 | 20000 | 1971 | 1971 |
| GM H5++ 150 GeV | 10000 | 17035 | 20000 | 779 | 1111 |

GM benchmark:
- mH5 = 150 GeV
- sin(theta_H) = 0.1
- Generator cross section: 0.9597 fb
- Previous acceptance: 7.79%
- Updated acceptance: 11.11%
- Previous fiducial cross section: 0.07476063 fb
- Updated fiducial cross section: 0.10662267 fb
- Relative acceptance increase: 42.62%

SM EW+ regression:
- Zero events with different lepton candidates.
- Fiducial acceptance unchanged at 19.71%.

## Conclusion

The general prompt classifier removes the observed
hard-W record dependency in the tested GM sample
while preserving the SM EW+ baseline.

## Remaining validation

- Generator-level cut dependence.
- Remaining SM channels.
- Official ATLAS prompt-object definition.
- Event-by-event GM fiducial selection comparison.
- Additional GM mass benchmarks.

The prompt classifier remains a signal-sample approximation.
Fiducial cross sections are preliminary.
