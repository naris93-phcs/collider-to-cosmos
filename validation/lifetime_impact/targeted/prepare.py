from pathlib import Path
s=Path('../comparison.cpp').read_text()
def sub(a,b):
 global s
 assert s.count(a)==1,a
 s=s.replace(a,b)
s=s.replace('../../MG5_aMC','../../../MG5_aMC').replace('../../sm_wpwpjj','../../../sm_wpwpjj')
sub('int main(int argc,char** argv)', '''struct AuditId : fastjet::PseudoJet::UserInfoBase {
    int barcode,pdg;
    AuditId(int b,int p):barcode(b),pdg(p){}
};
int main(int argc,char** argv)''')
sub('    std::ofstream rows', '''    const std::set<int> targets{4619,9905,2673,3298,8879,628,4403};
    int audited=0;
    std::ofstream audit(std::string(argv[1])+".jets.tsv");
    audit << std::setprecision(17);
    std::ofstream rows''')
sub('        ++event_count;', '''        ++event_count;
        if (!targets.count(event_count)) {delete event; continue;}
        ++audited;''')
sub('            ghost.set_user_index(500);','            ghost.set_user_index(500);\n            ghost.set_user_info(new AuditId(hadron->barcode(),hadron->pdg_id()));')
sub('                mom.e()\n            );','                mom.e()\n            );\n            particles.back().set_user_info(new AuditId((*p)->barcode(),(*p)->pdg_id()));')
sub('        // Raw clustered jets', '''        const auto dump = [&](const char* stage,const std::vector<fastjet::PseudoJet>& collection) {
            int rank=0;
            for (const auto& j:collection) {
                audit << event_count << '\\t' << stage << '\\t' << ++rank
                      << '\\t' << j.px() << '\\t' << j.py() << '\\t' << j.pz() << '\\t' << j.e()
                      << '\\t' << j.pt() << '\\t' << j.eta() << '\\t' << j.rap() << '\\t' << j.phi_std() << '\\t';
                for(const auto& c:j.constituents()) {
                    const auto& id=c.user_info<AuditId>();
                    audit << id.barcode << ':' << id.pdg << ':' << c.e() << ':' << (c.user_index()==500) << ';';
                }
                audit << '\\n';
            }
        };
        dump("clustered",jets);
        dump("pre",selected_jets);
        // Raw clustered jets''')
sub('        // Ghost matching AFTER', '        dump("post",selected_jets);\n        // Ghost matching AFTER')
sub('        selected_jets = std::move(vbs_jets);','        selected_jets = std::move(vbs_jets);\n        dump("vbs",selected_jets);')
sub('        delete event;\n\n        // Small progress', '        delete event;\n        if(audited==7) break;\n\n        // Small progress')
sub('require(event_count==10000,"Expected exactly 10000 events");','require(audited==7,"Expected exactly seven audited events");')
Path('targeted.cpp').write_text(s)
