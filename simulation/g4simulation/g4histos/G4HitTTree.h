#ifndef G4HITTTREE_H
#define G4HITTTREE_H

#include <fun4all/SubsysReco.h>
#include <string>

class Fun4AllHistoManager;
class PHCompositeNode;
class TH1;
class TH2;

class G4HitTTree: public SubsysReco
{
 public:
  G4HitTTree(const std::string &name = "HITTTREE");
  virtual ~G4HitTTree(){}

  //! full initialization
  int Init(PHCompositeNode *);

  //! event processing method
  int process_event(PHCompositeNode *);

  int End(PHCompositeNode *);

  void Detector(const std::string &det);
  void BlackHoleName(const std::string &bh);

  void SaveHits(const int i=1) {savehits = i;}

 protected:
 std::string _detector;
 std::string _outnodename;
 std::string _hitnodename;
 std::string _absorbernodename;
 std::string _blackholenodename;
 int savehits;
 int evtno;
 Fun4AllHistoManager *hm;
 TH1 *etot_hist;
 TH2 *eion_etot_hist;
};


#endif
