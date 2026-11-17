# ATLAS 2024 WWjj: particle-level MET

**Συμπέρασμα:** Το §7.1 περιγράφει MET από επιλεγμένα **ορατά
final-state objects** και απαιτεί (E_T^{\rm miss}>30) GeV. Δεν δίνει
πλήρη αλγόριθμο για το ποια objects συνεισφέρουν. Δεν επιβεβαιώθηκε
analysis-specific υλοποίηση που να κλείνει αυτό το κενό. Η ισοδυναμία
με το δικό μας neutrino sum δεν έχει αποδειχθεί.

## Τι λέει η δημοσίευση

Στο [τοπικό PDF, §7.1, σ. 17](../../references/papers/ATLAS_2024_SSWWjj.pdf#page=17)
η κρίσιμη διατύπωση είναι «visible final-state objects that meet these
criteria». Ελέγχθηκε το κείμενο απευθείας από το PDF (content stream 595).
Η διατύπωση δείχνει συλλογή ορατών objects που υπόκεινται σε κριτήρια:
δεν δηλώνει άθροισμα neutrinos, όλων των invisible particles ή ανεξαίρετα
όλων των stable visible particles. Ούτε δηλώνει ότι αθροίζονται
**αποκλειστικά** τα δύο signal leptons και τα δύο tagging jets.

Το §7.1 δίνει τους fiducial object κανόνες: stable lifetime
>3×10⁻¹¹ s (υποσημείωση 6), dressed prompt e/μ με pT>27 GeV και abs(η)<2,5,
prompt-photon cone ΔR<0,1, anti-kT R=0,4 jets με abs(η)<4,5 και tagging
pT>65/35 GeV (25 GeV για τρίτο jet όπου απαιτείται), και e–jet overlap
ΔR<0,4 με απόφαση από το pT ratio. Η εξαίρεση μ/ν αφορά **τα jet inputs**.
Αυτά είναι γνωστά object/selection thresholds, όχι πλήρης κατάλογος των
**MET-specific** inputs και thresholds.

Δεν διευκρινίζονται στο §7.1:

- Ποια leptons, jets ή standalone photons συνεισφέρουν και αν
  συμπεριλαμβάνονται objects εκτός των signal/tagging cuts.
- Αν υπάρχει visible soft/unclustered όρος και ποια είναι τα δικά του
  όρια pT και η.
- Σε ποιο στάδιο εφαρμόζεται το overlap removal για τη MET συλλογή και πώς
  αποφεύγεται διπλή καταμέτρηση dressed e/γ έναντι jet constituents.

Το §4, σ. 11, περιγράφει **detector-level** MET από baseline objects μετά
το overlap removal και soft track term, παραπέμποντας στη Ref. [83].
Δεν μεταφέρει αυτή τη συνταγή στο particle level. Το §7.2, σ. 18, χρησιμοποιεί
το MET vector στον ορισμό του mT χωρίς να εξειδικεύει τα inputs.

## Τι επιβεβαιώνει το επίσημο υποστηρικτικό υλικό

- Η [Ref. [83], ATLAS MET performance, EPJC 78 (2018) 903](https://doi.org/10.1140/epjc/s10052-018-6288-9)
  περιγράφει detector reconstruction και τον track-based soft term
  (§3.3.6), και χρησιμοποιεί neutrino momentum ως truth reference
  σε συγκεκριμένα performance samples ([§4 και §5.2](https://arxiv.org/pdf/1802.08168#page=13)).
  Αυτό δεν καθορίζει τη truth object συλλογή του WWjj §7.1.
- Η [επίσημη έκδοση arXiv v2](https://arxiv.org/html/2312.00420v2) ταυτοποιεί
  τη μέτρηση ως STDM-2018-32, CERN-EP-2023-221, HEPData 141650,
  JHEP 04 (2024) 026. Ελέγχθηκαν οι συνδέσεις προς
  [ATLAS public results](https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/PAPERS/STDM-2018-32/),
  [HEPData/INSPIRE 2729396](https://www.hepdata.net/record/ins2729396)
  και [Rivet](https://heprivet.org/analyses/ATLAS_2024_I2729396/).
  Οι συγκεκριμένες σελίδες δεν ήταν προσβάσιμες με τα διαθέσιμα εργαλεία
  (HEPData 403, μη προσβάσιμη σελίδα ATLAS/Rivet). Το δοκιμαστικό Rivet
  όνομα **δεν είναι επιβεβαιωμένο analysis ID**. Η αναζήτηση δεν εντόπισε
  επαληθεύσιμη official 2024 routine ή έκδοση· αυτό δεν αποδεικνύει ότι δεν
  υπάρχει. Η [routine του 2019](https://heprivet.org/analyses/ATLAS_2019_I1738841/)
  αφορά άλλη δημοσίευση και δεν χρησιμοποιήθηκε ως υποκατάστατο.
- Ο [κατάλογος αναφορών του project](../../references/README.md) παρέχει
  τα papers του 2019 και 2024, χωρίς πρόσθετο analysis-specific MET recipe.

## Σύγκριση με τον κώδικα και τι συνάγεται

Ο [κώδικας, γραμμές 572 και 603–615](../../analysis/particle_level.cpp#L603)
αθροίζει τα px, py όλων των ν/αντι-ν με abs(PDG ID)=12,14,16 **στην
ανακατασκευασμένη lifetime collection**. Δεν εφαρμόζει neutrino ancestry,
pT ή η cut και δεν αφαιρεί συνεισφορές βάσει e–jet overlap. Το MET cut είναι
[αυστηρό >30 GeV, γραμμή 940](../../analysis/particle_level.cpp#L940).
Το unrestricted non-neutrino sum είναι μόνο
[closure diagnostic, γραμμές 918–930](../../analysis/particle_level.cpp#L918).

Ως **έλεγχος φυσικής**, όχι ως συνταγή ATLAS: έστω N το neutrino vector,
S το άθροισμα μιας υποθετικής ορατής συλλογής και U η υπόλοιπη ορατή
ορμή. Αν οι συλλογές είναι ξένες, εξαντλητικές και η ολική εγκάρσια
ορμή του record είναι δ, τότε −S = N + U − δ. Ακόμη και τέλειο closure
(δ≈0) δεν επιβάλλει U≈0. Η συνήθης ισοδυναμία neutrino sum με αρνητικό
visible sum απαιτεί πλήρη visible κάλυψη (ή ακύρωση της εξαιρούμενης
ορμής), χωρίς διπλομέτρηση, και neutrinos ως μόνα invisible particles.
Ο έλεγχος closure της lifetime ανακατασκευής δεν ελέγχει αυτές τις
συνθήκες για τα ATLAS MET objects.

**Επίδραση στο cut:** μια διαφορά στο μέτρο του MET μπορεί να μετακινήσει
events εκατέρωθεν των 30 GeV, προς οποιαδήποτε κατεύθυνση. Η ύπαρξη ή
το πλήθος τέτοιων migrations στο run_04 δεν έχει μετρηθεί για την ATLAS
συνταγή, επειδή η συνταγή δεν έχει επιβεβαιωθεί. Η υπάρχουσα σύγκριση
status1/lifetime εφαρμόζει neutrino sum και στα δύο σκέλη.

**Σύσταση:** διατηρούμε τον τρέχοντα MET ως ρητά δηλωμένη baseline
προσέγγιση. Ζητούμε από την ομάδα STDM-2018-32 την ακριβή truth object
λίστα, τα thresholds, τη σειρά overlap/dressing και τυχόν soft term, ή
τον αντίστοιχο επίσημο κώδικα. Μόνο μετά την επιβεβαίωση έχει νόημα
paired έλεγχος των MET vectors και των migrations στα 30 GeV. Δεν έγινε
αλλαγή κώδικα, generation ή analysis run.
