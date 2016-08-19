#pragma once
#include "DPGame.hpp"

namespace dplib {

// {B3F2E132-FE6A-11D2-8DEE-00A0C90832B4}
const GUID GUID_AoCPresetData = { 0xB3F2E132, 0xFE6A, 0x11D2, { 0x8D, 0xEE, 0x00, 0xA0, 0xC9, 0x08, 0x32, 0xB4 } };

// {5DE93F3F-FC90-4ee1-AE5A-63DAFA055950}
const GUID GUID_AoC = { 0x5DE93F3F, 0xFC90, 0x4ee1, { 0xAE, 0x5A, 0x63, 0xDA, 0xFA, 0x05, 0x59, 0x50 } };

#pragma pack(1)
struct DPGameAoCPresetData {
  uint8_t civAvail[20];
  uint8_t lockCivAvail;
  uint8_t gameType;
  uint8_t lockGameType;

  union {
    char gameFilename[260];
    // UserPatch has a slightly different preset data format:
    // http://aoczone.net/viewtopic.php?f=68&t=97719&p=321800#p321800
    struct {
      char upGameFilename[240];
      DWORD specIps[4];
      DWORD reservedFlags;
    };
  };

  uint8_t mapTypeAvail[39];
  uint8_t lockMapTypeAvail;
  uint8_t mapSize;
  uint8_t lockMapSize;
  uint8_t difficulty;
  uint8_t lockDifficulty;
  uint8_t resources;
  uint8_t lockResources;
  uint8_t u0[3];
  DWORD population;
  uint8_t lockPopulation;
  uint8_t gameSpeed;
  uint8_t lockGameSpeed;
  uint8_t startingAge;
  uint8_t lockStartingAge;
  uint8_t victory;
  uint8_t lockVictory;
  uint8_t u1;
  DWORD timeScore;
  uint8_t lockTimeScore;
  uint8_t teamTogether;
  uint8_t lockTeamTogether;
  uint8_t teamsLocked;
  uint8_t lockTeamsLocked;
  uint8_t speedLocked;
  uint8_t lockSpeedLocked;
  uint8_t recordGame;
  uint8_t lockRecordGame;
  uint8_t revealMap;
  uint8_t lockRevealMap;
  uint8_t allTechs;
  uint8_t lockAllTechs;
  uint8_t allowCheats;
  uint8_t lockAllowCheats;
  uint8_t u2[6];
  uint8_t teamBonuses;
  uint8_t lockTeamBonuses;
  uint8_t u3[5];
};

class DPGameAoC: public DPGame {
public:
  DPGameAoCPresetData* preset;

  /**
   * Create a new Age of Empires 2 DPGame.
   */
  DPGameAoC ();
  ~DPGameAoC ();

  /**
   * @inherit
   */
  void receiveMessage (DPLobbyMessage* message);
};

}
