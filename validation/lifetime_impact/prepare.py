from pathlib import Path
import hashlib,difflib
p=Path(__file__).resolve().parent;r=p.parents[1]
s=(r/'validation/stable_particles/diagnostic.cpp').read_text()
s=s[:s.index('int main(')]
s=s.replace('void inspect(', 'Set reconstruct(')
a=s.index('  std::cout << "EVENT "'); b=s.index('  // Print one root',a)
# Retain global closure checks; remove printing, not validation.
s=s[:a]+'''  for(int k=0;k<4;++k) require(std::isfinite(newSum[k]) && std::abs(newSum[k]-oldSum[k])<tolerance,"Global closure failed");
  return reconstructed;
}
'''
(p/'reconstruction.h').write_text(s)
s=(r/'analysis/particle_level.cpp').read_text(); original=s
s='#include "reconstruction.h"\n#include <fstream>\n'+s
s=s.replace('int main()', 'int main(int argc,char** argv)')
s=s.replace('    // -------------------------------------------------------------------------\n    // Input HepMC file', '''    require(argc==2,"Use baseline or lifetime");
    const bool lifetime=std::string(argv[1])=="lifetime";
    require(lifetime || std::string(argv[1])=="baseline","Invalid mode");
    Pythia8::Pythia pd("../../MG5_aMC_v3_8_0/HEPTools/pythia8/share/Pythia8/xmldoc",false);
    std::ofstream rows(std::string(argv[1])+".csv");
    rows << std::setprecision(17);
    rows << "index,event,weight,inputs,metx,mety,met,njets,pt1,pt2,mjj,dy,cuts,ids\\n";
    // -------------------------------------------------------------------------
    // Input HepMC file''',1)
s=s.replace('../sm_wpwpjj_leptonic/', '../../sm_wpwpjj_leptonic/')
s=s.replace('        ++event_count;', '''        ++event_count;
        context=nullptr;
        Set stable;
        try { if(lifetime) stable=reconstruct(*event,pd.particleData); }
        catch(const std::exception& e) { std::cerr << "INDEX=" << event_count << " EVENT=" << event->event_number() << " " << e.what(); return 1; }
        std::vector<int> input_ids;''',1)
s=s.replace('if ((*p)->status() != 1)', 'if (lifetime ? !stable.count(*p) : (*p)->status() != 1)')
s=s.replace('            particles.emplace_back(', '            input_ids.push_back((*p)->barcode());\n            particles.emplace_back(')
anchor='        delete event;'
assert s.count(anchor)==1
s=s.replace(anchor,'''        const bool cuts[]={true,pass_two_leptons,pass_two_leptons&&pass_same_sign,pass_mll,pass_veto_cumulative,pass_z_cumulative,pass_met,pass_b_cumulative,pass_b_cumulative&&selected_jets.size()>=2,pass_b_cumulative&&pass_jet_pt,pass_b_cumulative&&pass_mjj,pass_full_baseline};
        double pt1=0,pt2=0,mass=0,sep=0;
        if(!selected_jets.empty()) pt1=selected_jets[0].pt();
        if(selected_jets.size()>=2) {pt2=selected_jets[1].pt();mass=(selected_jets[0]+selected_jets[1]).m();sep=std::abs(selected_jets[0].rap()-selected_jets[1].rap());}
        for(double x:{met_px,met_py,met,pt1,pt2,mass,sep}) require(std::isfinite(x),"Nonfinite observable");
        rows << event_count << ',' << event->event_number() << ',' << (event->weights().empty()?1:event->weights()[0]) << ',' << input_ids.size() << ',' << met_px << ',' << met_py << ',' << met << ',' << selected_jets.size() << ',' << pt1 << ',' << pt2 << ',' << mass << ',' << sep << ',';
        for(bool c:cuts) rows << c;
        rows << ','; for(int id:input_ids) rows << id << ';'; rows << '\\n';
        delete event;''')
s=s.replace('"particle_level.root"','(std::string(argv[1])+".root").c_str()')
s=s.replace('"RECREATE"','"CREATE"')
s=s.replace('    output.Close();','    output.Close();\n    require(event_count==10000,"Expected exactly 10000 events");')
(p/'comparison.cpp').write_text(s)
(p/'source.diff').write_text(''.join(difflib.unified_diff(original.splitlines(True),s.splitlines(True))))
(p/'source.sha256').write_text(hashlib.sha256(original.encode()).hexdigest()+' analysis/particle_level.cpp\n')
