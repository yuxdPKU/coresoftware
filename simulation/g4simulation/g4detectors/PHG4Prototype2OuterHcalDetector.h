#ifndef PHG4Prototype2OuterHcalDetector_h
#define PHG4Prototype2OuterHcalDetector_h

#include "PHG4Parameters.h"

#include <g4main/PHG4Detector.h>

#include <Geant4/globals.hh>
#include <Geant4/G4RotationMatrix.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4TwoVector.hh>
#include <Geant4/G4Types.hh>

#include <map>
#include <vector>
#include <set>

class G4AssemblyVolume;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VSolid;

class PHG4Prototype2OuterHcalDetector: public PHG4Detector
{

  public:

  //! constructor
 PHG4Prototype2OuterHcalDetector( PHCompositeNode *Node,  PHG4Parameters *parameters, const std::string &dnam);

  //! destructor
  virtual ~PHG4Prototype2OuterHcalDetector(){}

  //! construct
  virtual void Construct( G4LogicalVolume* world );

  virtual void Print(const std::string &what = "ALL") const;

  //!@name volume accessors
  //@{
  int IsInPrototype2OuterHcal(G4VPhysicalVolume*) const;
  //@}

  void SuperDetector(const std::string &name) {superdetector = name;}
  const std::string SuperDetector() const {return superdetector;}
  int get_Layer() const {return layer;}

  G4LogicalVolume* ConstructSteelPlate(G4LogicalVolume* hcalenvelope);
  G4LogicalVolume* ConstructScintillatorBox(G4LogicalVolume* hcalenvelope);
  G4LogicalVolume* ConstructScintiTileU1(G4LogicalVolume* hcalenvelope);
  G4LogicalVolume* ConstructScintiTileU2(G4LogicalVolume* hcalenvelope);
  double GetScintiAngle();

  protected:
  void AddGeometryNode();
  int ConstructOuterHcal(G4LogicalVolume* sandwich);
  int DisplayVolume(G4VSolid *volume,  G4LogicalVolume* logvol, G4RotationMatrix* rotm=NULL);
  PHG4Parameters *params;
  G4LogicalVolume *outerhcalsteelplate;
  G4AssemblyVolume *outerhcalassembly;
  G4TwoVector steel_plate_corner_upper_left;
  G4TwoVector steel_plate_corner_upper_right;
  G4TwoVector steel_plate_corner_lower_right;
  G4TwoVector steel_plate_corner_lower_left;
  double inner_radius;
  double outer_radius;
  double scinti_x;
  double steel_x;
  double steel_yhi;
  double steel_ylo;
  double steel_z;
  double bottom_xmiddle_steel_tile;
  double bottom_ymiddle_steel_tile;
  double size_z;
  double scinti_tile_z;
  double scinti_tile_thickness;
  double scinti_box_shift;
  double gap_between_tiles;
  double scinti_gap;
  double tilt_angle;
  double envelope_inner_radius;
  double envelope_outer_radius;
  double envelope_z;
  double volume_envelope;
  double volume_steel;
  double volume_scintillator;

  int n_scinti_plates;

  int active;
  int absorberactive;

  int layer;
  std::string detector_type;
  std::string superdetector;
  std::set<G4VPhysicalVolume *>steel_absorber_vec;
  std::string scintilogicnameprefix;
};

#endif
