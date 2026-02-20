#include "DetectorConstruction.hh"
#include "FluxSD.hh"
#include "EnergyDepositSD.hh"

#include "G4Material.hh"
#include "G4BooleanSolid.hh"
#include "G4CSGSolid.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4RotationMatrix.hh"
#include "G4UserLimits.hh"

#include "G4TransportationManager.hh"
#include "G4SDManager.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4VisAttributes.hh"
#include "G4String.hh"
#include "globals.hh"

using namespace CLHEP;

//---------------------------------------------------------------------------

DetectorConstruction::DetectorConstruction()
{

  fNistManager  = G4NistManager::Instance();

}

//---------------------------------------------------------------------------

DetectorConstruction::~DetectorConstruction() 
{
}

//---------------------------------------------------------------------------

G4VPhysicalVolume* DetectorConstruction::Construct()
{ 
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();


  //---------------------------------------------------------------------------
  // materials
  //---------------------------------------------------------------------------

  G4Element*  N   = fNistManager->FindOrBuildElement(7); 
  G4Element*  O   = fNistManager->FindOrBuildElement(8); 
   
  G4Material* Beamline = new G4Material("Beam", 1.e-5*g/cm3, 2, kStateGas, STP_Temperature, 2.e-2*bar ); 
  Beamline->AddElement(N, 0.7); 
  Beamline->AddElement(O, 0.3); 

  G4Element*  Al   = fNistManager->FindOrBuildElement(13); 
  G4Element*  Li   = fNistManager->FindOrBuildElement(3); 

  G4Material* AlLi = new G4Material("AlLi", 2.54*g/cm3, 2 );
  Beamline->AddElement(Al, 0.9); 
  Beamline->AddElement(Li, 0.1); 

  //---------------------------------------------------------------------------
  // Create Experimental Hall
  //---------------------------------------------------------------------------


  G4Sphere* expHall_sphere = new G4Sphere("sphere", 
					  0.0*m, 3.0*m, 0, 360*deg, 0*deg, 360*deg);
  
  G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_sphere,
						     Beamline,
						     "expHall_log", 0, 0, 0);
  
  fExpHall                     = new G4PVPlacement(0, G4ThreeVector(),
						   expHall_log, "expHall", 0, false, 0);

  //---------------------------------------------------------------------------
  // Create Spacecraft Volume
  //---------------------------------------------------------------------------

  G4RotationMatrix* scat_rm  = new G4RotationMatrix(); 
  scat_rm->rotateX(90. *deg); 


  G4Tubs* craft_tube = new G4Tubs("craft_tube", 
				  0.0 *m, 1.5 *m, 2.0 *m, 0.*deg, 360.*deg ); 

  G4LogicalVolume* craft_log = new G4LogicalVolume(craft_tube,
						   //fNistManager->FindOrBuildMaterial("G4_Al"),  
						   AlLi,
						   "craft_log", 0, 0, 0);
  
  fCraftVol                  = new G4PVPlacement(scat_rm, G4ThreeVector(),
						 craft_log, "craft", expHall_log, false, 0);

  //---------------------------------------------------------------------------

  G4Tubs* air_tube = new G4Tubs("air_tube", 
				  0.0 *m, 1.4 *m, 2.0 *m, 0.*deg, 360.*deg ); 

  G4LogicalVolume* air_log = new G4LogicalVolume(air_tube,
						 fNistManager->FindOrBuildMaterial("G4_AIR"),  
						 "air_log", 0, 0, 0);
  
  fAirVol                  = new G4PVPlacement(0, G4ThreeVector(),
						 air_log, "air", craft_log, false, 0);
  
  //---------------------------------------------------------------------------
  // Create Detector Volume
  //---------------------------------------------------------------------------
  
  G4Sphere* det_sphere = new G4Sphere("sphere", 
				      0.0*m, 0.5*m, 0, 360*deg, 0*deg, 360*deg);
  
  G4LogicalVolume* det_log = new G4LogicalVolume(det_sphere,
						 fNistManager->FindOrBuildMaterial("G4_WATER"),  
						 "det_log", 0, 0, 0);
  
  fDetVol                  = new G4PVPlacement(0, G4ThreeVector(),
					       det_log, "det", air_log, false, 0);

  //---------------------------------------------------------------------------
  // Set Step Limits, Sensitive Detector and Visualisation
  //---------------------------------------------------------------------------

  G4double maxStep = 0.5 *mm;;
  G4UserLimits* stepLimit = new G4UserLimits(maxStep);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  fFluxSD = new FluxSD("FluxSD", 4);
  SDman->AddNewDetector( fFluxSD );
  det_log->SetSensitiveDetector( fFluxSD );

  fEdepSD = new EnergyDepositSD("EdepSD", 4);
  SDman->AddNewDetector( fEdepSD );
  det_log->SetSensitiveDetector( fEdepSD );
  
  
  G4VisAttributes* blue    = new G4VisAttributes( G4Colour(0.0,0.0,1.0)   );
  G4VisAttributes* red     = new G4VisAttributes( G4Colour(1.0,0.0,0.0)   );

  craft_log->SetVisAttributes(blue);
  air_log->SetVisAttributes(blue);
  det_log->SetVisAttributes(red);

  return fExpHall;
}

//---------------------------------------------------------------------------

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

//---------------------------------------------------------------------------
