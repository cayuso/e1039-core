#ifndef PHG4CEmcTestBeamDetector_h
#define PHG4CEmcTestBeamDetector_h

#include "g4main/PHG4Detector.h"

#include <Geant4/G4Types.hh>
#include <Geant4/globals.hh>

#include <map>
#include <vector>

class G4Material;
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

class PHG4CEmcTestBeamDetector: public PHG4Detector
{

  public:

  //! constructor
  PHG4CEmcTestBeamDetector( PHCompositeNode *Node, const std::string &dnam="BLOCK", const int lyr = 0 );

  //! destructor
  virtual ~PHG4CEmcTestBeamDetector( void )
  {}

  //! construct
  virtual void Construct( G4LogicalVolume* world );

  //!@name volume accessors
  //@{
  int IsInCEmcTestBeam(G4VPhysicalVolume*) const;
  //@}

  void SetPlaceZ(const G4double place_z) {place_in_z = place_z;}
  void SetPlace(const G4double place_x, const G4double place_y, const G4double place_z)
  {
    place_in_x = place_x;
    place_in_y = place_y;
    place_in_z = place_z;
  }
  void SetXRot(const G4double angle) {x_rot = angle;}
  void SetYRot(const G4double angle) {y_rot = angle;}
  void SetZRot(const G4double angle) {z_rot = angle;}
  void SetActive(const int i = 1) {active = i;}
  void SetAbsorberActive(const int i = 1) {absorberactive = i;}
  int IsActive() const {return active;}
  void SuperDetector(const std::string &name) {superdetector = name;}
  const std::string SuperDetector() const {return superdetector;}
  int get_Layer() const {return layer;}

  void BlackHole(const int i=1) {blackhole = i;}
  int IsBlackHole() const {return blackhole;}

  private:

  void CalculateGeometry();
  int ConstructTowerVolume(G4LogicalVolume* tower);
  int ConstructSandwichVolume(G4LogicalVolume* sandwich);


  std::vector<G4VPhysicalVolume *> sandwich_vol;
  G4double w_dimension[3];
  G4double sc_dimension[3];
  G4double gap;
  G4int num_planes_;
  G4double place_in_x;
  G4double place_in_y;
  G4double place_in_z;
  double plate_x;
  double plate_z;
  double sandwich_thickness;
  G4double x_rot;
  G4double y_rot;
  G4double z_rot;
  double alpha;
  double inner_radius;
  double outer_radius;
  double tower_angular_coverage;
  double cemc_angular_coverage;
  double active_scinti_fraction;
  int sandwiches_per_tower;
  int num_towers;

  int active;
  int absorberactive;
  int layer;
  int blackhole;
  std::string detector_type;
  std::string superdetector;
  
};

#endif