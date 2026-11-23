# Historical LHE study

Moved from analysis/ without changing file contents. Contains lhe_reader.cpp
and seven nonempty generator-level study plots. The source reads LHE from
stdin and writes fixed PNG filenames in the current working directory.
It is not a CMake target or part of the active particle-level pipeline.

The two W plots had zero entries; the rapidity comparison had an empty W
curve and misleading legend and was retired separately. Those PNGs are not
included here. Exact producing input identity and byte-exact regeneration
of the retained plots are unverified. These figures are not canonical SM
particle-level predictions or evidence of agreement with ATLAS.

## Original paths and checksums

| File | Original path | Bytes | SHA-256 |
|---|---|---:|---|
| `lhe_reader.cpp` | `analysis/lhe_reader.cpp` | 12,122 | `a76ffa75268d8dfb76af0aa46112857ad3107fea047fe67d5ba502ea27af72d0` |
| `delta_r_ll.png` | `analysis/delta_r_ll.png` | 14,427 | `d55364bc4b06a0f8e331306944499004650b566904b47e6d7906408714a710a1` |
| `delta_y_qq.png` | `analysis/delta_y_qq.png` | 15,290 | `da75e235a33bc2e23c3713d3ccbe42800d82db3067a91c1910b9d3921d878376` |
| `lepton_pt.png` | `analysis/lepton_pt.png` | 15,780 | `5e13bbb9401d1e3a42d04eb3180360f33e53c62ad50a07604f2df72e9e3386cc` |
| `m_ll.png` | `analysis/m_ll.png` | 14,004 | `25758833abd732085dca9e87e507de3282e7f6fec2f4cc099875efac05822803` |
| `m_qq.png` | `analysis/m_qq.png` | 14,490 | `1f37294348dde65220bec249f0239dce6ae2284b2a4d57d3446f39311e94282f` |
| `missing_pt.png` | `analysis/missing_pt.png` | 15,585 | `94cb499099f0bd26f4cef159e0bdd01a2460806e435f11d6c4c1c6b295fd6f9a` |
| `mqq_vs_delta_y.png` | `analysis/mqq_vs_delta_y.png` | 22,735 | `088decdf7d47b96bfaa39d34e0d5df809491c7a1f2afb05da0a1bc0c674ef573` |
