#ifndef G4RAWTOWERTTREE_H
#define G4RAWTOWERTTREE_H

#include <fun4all/SubsysReco.h>
#include <string>

class Fun4AllHistoManager;
class PHCompositeNode;
class TH1;
class TH2;

class G4RawTowerTTree: public SubsysReco
{
 public:
  G4RawTowerTTree(const std::string &name = "RAWTOWERTTREE");
  virtual ~G4RawTowerTTree(){}

  //! full initialization
  int Init(PHCompositeNode *);

  //! event processing method
  int process_event(PHCompositeNode *);

  int End(PHCompositeNode *);

  void Detector(const std::string &det);

  void SaveRawTowers(const int i=1) {savetowers = i;}

  void HistoFileName(const std::string &name) {_histofilename = name;}

 protected:
 std::string _detector;
 std::string _outnodename;
 std::string _towernodename;
 std::string  _towergeomnodename;
 std::string _histofilename;
 int savetowers;
 int evtno;
 Fun4AllHistoManager *hm;
 TH1 *etot_hist;
};


#endif
