# Preliminary WWjj physics gallery

Four validated lifetime-mode particle-level predictions for same-sign WWjj
at 13 TeV. The [results index](../../docs/RESULTS_INDEX.md) identifies the
four source ROOT files. The script obtains their paths from
[the sample manifest](../samples.json) and opens them read-only.

| Directory | Plots |
|---|---:|
| [01_EW_WpWp](01_EW_WpWp/) | 9 |
| [02_EW_WmWm](02_EW_WmWm/) | 9 |
| [03_QCD_WpWp](03_QCD_WpWp/) | 9 |
| [04_QCD_WmWm](04_QCD_WmWm/) | 9 |
| [05_Comparisons](05_Comparisons/) | 11 |

## Individual samples

Each sample has eight count distributions from the `fiducial_events` TTree.
Only events passing the complete fiducial selection enter these plots.
The PNG names use the branch names `mjj`, `dy_jj`, `mll`,
`lepton1_pt`, `lepton2_pt`, `jet1_pt`, `jet2_pt` and `met`.
Individual distributions show events per bin with square-root counting
errors. The ninth PNG shows cumulative event counts from the stored
`h_cutflow`. Its stage uncertainties are binomial at fixed analyzed count.

Each observable uses common bin edges across all four samples. The upper
edge is extended when needed to cover the largest entry. Non-finite values
and any underflow or overflow cause validation to fail. All counts are
included in the plotted range.

## Comparisons

| Comparison | Dijet mass | Dijet rapidity separation |
|---|---|---|
| EW versus QCD, positive charge | [mjj](05_Comparisons/ew_vs_qcd_WpWp_mjj_shape.png) | [dy_jj](05_Comparisons/ew_vs_qcd_WpWp_dy_jj_shape.png) |
| EW versus QCD, negative charge | [mjj](05_Comparisons/ew_vs_qcd_WmWm_mjj_shape.png) | [dy_jj](05_Comparisons/ew_vs_qcd_WmWm_dy_jj_shape.png) |
| EW charge comparison | [mjj](05_Comparisons/ew_charge_comparison_mjj_shape.png) | [dy_jj](05_Comparisons/ew_charge_comparison_dy_jj_shape.png) |
| QCD-induced charge comparison | [mjj](05_Comparisons/qcd_charge_comparison_mjj_shape.png) | [dy_jj](05_Comparisons/qcd_charge_comparison_dy_jj_shape.png) |
| Four samples | [mjj](05_Comparisons/four_sample_comparison_mjj_shape.png) | [dy_jj](05_Comparisons/four_sample_comparison_dy_jj_shape.png) |

For each observable the comparison panels share the same binning and x/y
ranges. They show unit-area densities, calculated as the bin fraction
among fiducial events divided by the bin width. The error bars are
per-bin binomial MC statistical errors. This approximation gives zero
error for empty bins and leaves out correlations between shape bins.

The [combined cutflow](05_Comparisons/combined_cross_section_cutflow_fb.png)
uses absolute cross-section normalization. The four stored cutflows have
the same 12 labels in the same order. At each stage, the sample count is
multiplied by its generator cross section divided by its analyzed event
count. Generator cross sections come from [cross_sections.csv](../cross_sections.csv).
The sum is **EW + QCD with interference excluded**, with binomial MC
statistical errors added in quadrature across the independent samples.
Errors between stages are correlated. Integration and theory uncertainties
are outside the plotted bars.

The figure labels specify preliminary particle-level predictions. Absolute
normalization inherits the unresolved 6.05% EW positive pilot-to-parent
discrepancy. The ATLAS fiducial audit is incomplete, and PDF/scale
systematics are absent. [Validation status](../../docs/VALIDATION_STATUS.md)

## Regeneration and validation

Run the [PyROOT plotting script](../../scripts/plot_physics_gallery.py)
with the existing hep interpreter. Choose a fresh output directory to
keep these exports intact.

```bash
cd /home/naris/projects/collider_to_cosmos
"$HOME/miniforge3/envs/hep/bin/python" -B scripts/plot_physics_gallery.py --output results/physics_gallery_replot
```

The script requires a nonexistent destination and creates no new event
data. It checks tree and cutflow counts, per-plot bin coverage, normalization
closure and the order of selection labels. All 47 PNGs were decoded and
verified at 1800 × 1120 pixels.
