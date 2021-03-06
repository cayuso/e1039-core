#include "PHG4TargetCoilV2Subsystem.h"
#include "PHG4TargetCoilV2Detector.h"
#include "PHG4TargetCoilV2Detector.h"
#include "PHG4CylinderGeomContainer.h"
#include "PHG4CylinderGeomv1.h"
#include "PHG4TargetCoilV2SteppingAction.h"

#include <phparameter/PHParameters.h>

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4PhenixDetector.h>
#include <g4main/PHG4Utils.h>

#include <phool/getClass.h>

#include <Geant4/globals.hh>

#include <sstream>

using namespace std;

//_______________________________________________________________________
PHG4TargetCoilV2Subsystem::PHG4TargetCoilV2Subsystem(const std::string &na, const int lyr)
  : PHG4DetectorSubsystem(na, lyr)
  , detector_(nullptr)
  , steppingAction_(nullptr)
{
  InitializeParameters();
}

//_______________________________________________________________________
int PHG4TargetCoilV2Subsystem::InitRunSubsystem(PHCompositeNode *topNode)
{
  // create hit list only for active layers
  if (GetParams()->get_int_param("lengthviarapidity"))
  {
    GetParams()->set_double_param("length", PHG4Utils::GetLengthForRapidityCoverage(GetParams()->get_double_param("radius") + GetParams()->get_double_param("thickness")) * 2);
  }
  // create detector
  detector_ = new PHG4TargetCoilV2Detector(topNode, GetParams(), Name(), GetLayer());
  G4double detlength = GetParams()->get_double_param("length");
  detector_->SuperDetector(SuperDetector());
  detector_->OverlapCheck(CheckOverlap());
  if (GetParams()->get_int_param("active"))
  {
    PHNodeIterator iter(topNode);
    PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
    PHCompositeNode *runNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "RUN"));

    ostringstream nodename;
    ostringstream geonode;
    if (SuperDetector() != "NONE")
    {
      // create super detector subnodes
      PHNodeIterator iter_dst(dstNode);
      PHCompositeNode *superSubNode = dynamic_cast<PHCompositeNode *>(iter_dst.findFirst("PHCompositeNode", SuperDetector()));
      if (!superSubNode)
      {
        superSubNode = new PHCompositeNode(SuperDetector());
        dstNode->addNode(superSubNode);
      }
      dstNode = superSubNode;
      PHNodeIterator iter_run(runNode);
      superSubNode = dynamic_cast<PHCompositeNode *>(iter_run.findFirst("PHCompositeNode", SuperDetector()));
      if (!superSubNode)
      {
        superSubNode = new PHCompositeNode(SuperDetector());
        runNode->addNode(superSubNode);
      }
      runNode = superSubNode;

      nodename << "G4HIT_" << SuperDetector();
      geonode << "CYLINDERGEOM_" << SuperDetector();
    }

    else
    {
      nodename << "G4HIT_" << Name();
      geonode << "CYLINDERGEOM_" << Name();
    }
    PHG4HitContainer *cylinder_hits = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());
    if (!cylinder_hits)
    {
      dstNode->addNode(new PHIODataNode<PHObject>(cylinder_hits = new PHG4HitContainer(nodename.str()), nodename.str().c_str(), "PHObject"));
    }
    cylinder_hits->AddLayer(GetLayer());
    PHG4CylinderGeomContainer *geo = findNode::getClass<PHG4CylinderGeomContainer>(topNode, geonode.str().c_str());
    if (!geo)
    {
      geo = new PHG4CylinderGeomContainer();
      PHIODataNode<PHObject> *newNode = new PHIODataNode<PHObject>(geo, geonode.str().c_str(), "PHObject");
      runNode->addNode(newNode);
    }
    PHG4CylinderGeom *mygeom = new PHG4CylinderGeomv1(GetParams()->get_double_param("radius"), GetParams()->get_double_param("place_z") - detlength / 2., GetParams()->get_double_param("place_z") + detlength / 2., GetParams()->get_double_param("thickness"));
    geo->AddLayerGeom(GetLayer(), mygeom);
    steppingAction_ = new PHG4TargetCoilV2SteppingAction(detector_, GetParams());
  }
  if (GetParams()->get_int_param("blackhole"))
  {
    steppingAction_ = new PHG4TargetCoilV2SteppingAction(detector_, GetParams());
  }
  return 0;
}

//_______________________________________________________________________
int PHG4TargetCoilV2Subsystem::process_event(PHCompositeNode *topNode)
{
  // pass top node to stepping action so that it gets
  // relevant nodes needed internally
  if (steppingAction_)
  {
    steppingAction_->SetInterfacePointers(topNode);
  }
  return 0;
}

void PHG4TargetCoilV2Subsystem::SetDefaultParameters()
{
  double l = 22.7;
  double ri = 6.0;
  double ro = 22.225;
  double t = 0.3;

  std::vector<double> z_plane = { -l/2, -l/2+t, -l/2+t, l/2-t, l/2-t, l/2 };
  std::vector<double> r_inner = { ri, ri, ro-t, ro-t, ri, ri};
  std::vector<double> r_outer = { ro, ro, ro, ro, ro, ro};

  set_default_int_param("num_z_planes", 6);
  set_default_vdouble_param("z_plane", z_plane);
  set_default_vdouble_param("r_inner", r_inner);
  set_default_vdouble_param("r_outer", r_outer);

  set_default_double_param("rot_x", 0.);
  set_default_double_param("rot_y", 0.);
  set_default_double_param("rot_z", 0.);
  set_default_double_param("place_x", 0.);
  set_default_double_param("place_y", 0.);
  set_default_double_param("place_z", 0.);

  set_default_int_param("use_g4steps", 0);


  set_default_double_param("length", 100);
  set_default_double_param("radius", 100);
  set_default_double_param("steplimits", NAN);
  set_default_double_param("thickness", 100);
  set_default_double_param("tmin", NAN);
  set_default_double_param("tmax", NAN);

  set_default_int_param("lengthviarapidity", 1);
  set_default_int_param("lightyield", 0);

  set_default_string_param("material", "G4_Galactic");
}

PHG4Detector *
PHG4TargetCoilV2Subsystem::GetDetector(void) const
{
  return detector_;
}

void PHG4TargetCoilV2Subsystem::Print(const string &what) const
{
  cout << Name() << " Parameters: " << endl;
  if (!BeginRunExecuted())
  {
    cout << "Need to execute BeginRun() before parameter printout is meaningful" << endl;
    cout << "To do so either run one or more events or on the command line execute: " << endl;
    cout << "Fun4AllServer *se = Fun4AllServer::instance();" << endl;
    cout << "PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco(\"PHG4RECO\");" << endl;
    cout << "g4->InitRun(se->topNode());" << endl;
    cout << "PHG4TargetCoilV2Subsystem *cyl = (PHG4TargetCoilV2Subsystem *) g4->getSubsystem(\"" << Name() << "\");" << endl;
    cout << "cyl->Print()" << endl;
    return;
  }
  GetParams()->Print();
  if (steppingAction_)
  {
    steppingAction_->Print(what);
  }
  return;
}
