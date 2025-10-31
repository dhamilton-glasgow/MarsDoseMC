#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"

//---------------------------------------------------------------------------

class G4VPhysicalVolume;
class FluxSD;
class EnergyDepositSD;

//---------------------------------------------------------------------------

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  DetectorConstruction();
  ~DetectorConstruction();

  G4VPhysicalVolume* Construct();

  void UpdateGeometry();

  inline G4VPhysicalVolume* GetExpHall()    { return fExpHall; };
  inline G4VPhysicalVolume* GetDetVol()     { return fDetVol;  };
  inline FluxSD*            GetFluxSD()     { return fFluxSD;  };

  private:

  G4NistManager*     fNistManager;

  G4VPhysicalVolume* fExpHall;
  G4VPhysicalVolume* fCraftVol;
  G4VPhysicalVolume* fAirVol;
  G4VPhysicalVolume* fDetVol;

  FluxSD*            fFluxSD;
  EnergyDepositSD*   fEdepSD;

};
#endif

//---------------------------------------------------------------------------

