//////////////////////////////////////////////////////////////////////
//	Project FLCoreSDK v1.1, modified for use in FLHook Plugin version
//--------------------------
//
//	File:			FLCoreServer.h
//	Module:			FLCoreServer.lib
//	Description:	Interface to Server.dll
//
//	Web: www.skif.be/flcoresdk.php
//
//
//////////////////////////////////////////////////////////////////////
#ifndef _FLCORESERVER_H_
#define _FLCORESERVER_H_

#include "FLCoreCommon.h"
#include "FLCoreDefs.h"

#define POPUPDIALOG_BUTTONS_LEFT_YES    1
#define POPUPDIALOG_BUTTONS_CENTER_NO   2
#define POPUPDIALOG_BUTTONS_RIGHT_LATER 4
#define POPUPDIALOG_BUTTONS_CENTER_OK   8

struct CHAT_ID
{
        uint iID;
};

enum DOCK_HOST_RESPONSE
{
    ACCESS_DENIED = 1,
    DOCK_DENIED = 2,
    DOCK_IN_USE = 3,
    PROCEED_DOCK = 4,
    DOCK = 5,
};

enum DestroyType
{
    VANISH = 0,
    FUSE = 1,
};

struct SSPMunitionCollisionInfo
{
        uint iProjectileArchID;
        DWORD dw2;
        DWORD dwTargetShip;
        ushort s1;
};

struct SSPObjCollisionInfo
{
        uint iColliderObjectID;
        uint iColliderSubObjID;
        uint iDamagedObjectID;
        uint iDamagedSubObjID;
        float fDamage;
};

struct XActivateEquip
{
        uint iSpaceID;
        ushort sID;
        bool bActivate;
};

struct XActivateCruise
{
        uint iShip;
        bool bActivate;
};

struct XActivateThrusters
{
        uint iShip;
        bool bActivate;
};

struct XTractorObjects
{
        int iDunno[3];
        // This points to the start of the array of space IDs
        int *pArraySpaceID;
        // This points to the end of the array of space IDs
        int *pArraySpaceIDEnd;
};

struct SGFGoodSellInfo
{
        long l1;
        uint iArchID;
        int iCount;
};

struct SGFGoodBuyInfo
{
        uint iBaseID;
        ulong lNull;
        uint iGoodID;
        int iCount;
};

struct XFireWeaponInfo
{
        uint iObject;
        Vector vTarget;
        uint iDunno;
        ushort *sHpIdsBegin;
        ushort *sHpIdsEnd;
        ushort *sHpIdsLast;
};

struct XSetManeuver
{
        uint iShipFrom;
        uint IShipTo;
        uint iFlag;
};

struct XSetTarget
{
        uint iShip;
        uint iSlot;
        uint iSpaceID;
        uint iSubObjID;
};

struct SSPObjUpdateInfo
{
        uint iShip;
        Quaternion vDir;
        Vector vPos;
        double fTimestamp;
        float fDunno;
        float throttle;
        char cState;
};

struct XJettisonCargo
{
        uint iShip;
        uint iSlot;
        uint iCount;
};

struct XGoTradelane
{
        uint iShip;
        uint iTradelaneSpaceObj1;
        uint iTradelaneSpaceObj2;
};

struct CAccountListNode
{
        CAccountListNode *next;
        CAccountListNode *prev;
        uint iDunno1;
        wchar_t *wszCharname;
        uint iDunno2[32];
};

enum ConnectionType
{
    JUMPHOLE
};

class IMPORT CAccount
{
    public:
        CAccount(const class CAccount &);
        CAccount(void);
        virtual ~CAccount(void);
        class CAccount &operator=(const class CAccount &);
        void AppendCharacterNames(std::list<std::wstring &> &);
        void DeleteCharacterFromID(std::string &);
        void ForceLogout(void);
        void InitFromFolder(const char *);

    public:
        uint iDunno1;
        wchar_t *wszAccID;
        uint iDunno2[7];
        CAccountListNode *pFirstListNode;
        uint iNumberOfCharacters;
        uint iDunno4[32];
};

namespace BaseGroupMessage
{
    enum Type;
};

class IMPORT CPlayerGroup
{
    public:
        CPlayerGroup(const class CPlayerGroup &);
        CPlayerGroup(void);
        virtual ~CPlayerGroup(void);
        class CPlayerGroup &operator=(const class CPlayerGroup &);
        bool AddInvite(unsigned int);
        bool AddMember(unsigned int);
        bool DelInvite(unsigned int);
        bool DelMember(unsigned int);
        void DeliverChat(unsigned long, const void *);
        static class CPlayerGroup *FromGroupID(unsigned int);
        unsigned int GetID(void);
        unsigned int GetInviteCount(void);
        unsigned int GetMember(int) const;
        unsigned int GetMemberCount(void);
        unsigned int GetMissionID(void);
        unsigned int GetMissionSetBy(void);
        void HandleClientLogout(unsigned int);
        bool IsFull(void);
        bool IsInvited(unsigned int);
        bool IsMember(unsigned int);
        void RewardMembers(int);
        void SendChat(int, const unsigned short *, ...);
        void SendGroup(enum BaseGroupMessage::Type, unsigned int);
        void SetMissionID(unsigned int, unsigned int);
        void SetMissionMessage(struct CSetMissionMessage &);
        void SetMissionObjectives(struct CMissionObjectives &);
        void StoreMemberList(std::vector<unsigned int> &);

    protected:
        static class std::map<const unsigned int, class CPlayerGroup *, struct std::less<const unsigned int>, class std::allocator<class CPlayerGroup *>>
            s_GroupIDToGroupPtrMap;
        static unsigned int s_uiGroupID;

    public:
        unsigned char data[OBJECT_DATA_SIZE];
};

struct IMPORT IServerImpl
{
        IServerImpl(const struct IServerImpl &);
        IServerImpl(void);
        struct IServerImpl &operator=(const struct IServerImpl &);
        virtual void AbortMission(unsigned int, unsigned int);
        virtual void AcceptTrade(unsigned int, bool);
        virtual void ActivateCruise(unsigned int, const struct XActivateCruise &);
        virtual void ActivateEquip(unsigned int, const struct XActivateEquip &);
        virtual void ActivateThrusters(unsigned int, const struct XActivateThrusters &);
        virtual void AddTradeEquip(unsigned int, const struct EquipDesc &);
        virtual void BaseEnter(unsigned int, unsigned int);
        virtual void BaseExit(unsigned int, unsigned int);
        virtual void BaseInfoRequest(unsigned int, unsigned int, bool);
        virtual void CharacterInfoReq(unsigned int, bool);
        virtual void CharacterSelect(struct CHARACTER_ID const &, unsigned int);
        virtual void CharacterSkipAutosave(unsigned int);
        virtual void CommComplete(unsigned int, unsigned int, unsigned int, enum CommResult);
        virtual void Connect(const char *, unsigned short *);
        virtual void CreateNewCharacter(const struct SCreateCharacterInfo &, unsigned int);
        virtual void DelTradeEquip(unsigned int, const struct EquipDesc &);
        virtual void DestroyCharacter(struct CHARACTER_ID const &, unsigned int);
        virtual void DisConnect(unsigned int, enum EFLConnection);
        virtual void Dock(const unsigned int &, const unsigned int &);
        virtual void DumpPacketStats(const char *);
        virtual void ElapseTime(float);
        virtual void FireWeapon(unsigned int, const struct XFireWeaponInfo &);
        virtual void GFGoodBuy(const struct SGFGoodBuyInfo &, unsigned int);
        virtual void GFGoodSell(const struct SGFGoodSellInfo &, unsigned int);
        virtual void GFGoodVaporized(const struct SGFGoodVaporizedInfo &, unsigned int);
        virtual void GFObjSelect(unsigned int, unsigned int);
        virtual unsigned int GetServerID(void);
        virtual const char *GetServerSig(void);
        void GetServerStats(struct ServerStats &);
        virtual void GoTradelane(unsigned int, const struct XGoTradelane &);
        virtual void Hail(unsigned int, unsigned int, unsigned int);
        virtual void InitiateTrade(unsigned int, unsigned int);
        virtual void InterfaceItemUsed(unsigned int, unsigned int);
        virtual void JettisonCargo(unsigned int, const struct XJettisonCargo &);
        virtual void JumpInComplete(unsigned int, unsigned int);
        virtual void LaunchComplete(unsigned int, unsigned int);
        virtual void LocationEnter(unsigned int, unsigned int);
        virtual void LocationExit(unsigned int, unsigned int);
        virtual void LocationInfoRequest(unsigned int, unsigned int, bool);
        virtual void Login(const struct SLoginInfo &, unsigned int);
        virtual void MineAsteroid(unsigned int, const class Vector &, unsigned int, unsigned int, unsigned int, unsigned int);
        virtual void MissionResponse(unsigned int, unsigned long, bool, unsigned int);
        virtual void MissionSaveB(unsigned int, unsigned long);
        virtual void NewCharacterInfoReq(unsigned int);
        virtual void OnConnect(unsigned int);
        virtual void PlayerLaunch(unsigned int, unsigned int);
        virtual void PopUpDialog(unsigned int, unsigned int);
        virtual void PushToServer(class CDAPacket *);
        virtual void RTCDone(unsigned int, unsigned int);
        virtual void ReqAddItem(unsigned int, const char *, int, float, bool, unsigned int);
        virtual void ReqCargo(const class EquipDescList &, unsigned int);
        virtual void ReqChangeCash(int, unsigned int);
        virtual void ReqCollisionGroups(const class std::list<struct CollisionGroupDesc, class std::allocator<struct CollisionGroupDesc>> &, unsigned int);
        virtual void ReqDifficultyScale(float, unsigned int);
        virtual void ReqEquipment(const class EquipDescList &, unsigned int);
        virtual void ReqHullStatus(float, unsigned int);
        virtual void ReqModifyItem(unsigned short, const char *, int, float, bool, unsigned int);
        virtual void ReqRemoveItem(unsigned short, int, unsigned int);
        virtual void ReqSetCash(int, unsigned int);
        virtual void ReqShipArch(unsigned int, unsigned int);
        virtual void RequestBestPath(unsigned int, unsigned char *, int);
        virtual void RequestCancel(int, unsigned int, unsigned int, unsigned long, unsigned int);
        virtual void RequestCreateShip(unsigned int);
        virtual void RequestEvent(int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned int);
        virtual void RequestGroupPositions(unsigned int, unsigned char *, int);
        virtual void RequestPlayerStats(unsigned int, unsigned char *, int);
        virtual void RequestRankLevel(unsigned int, unsigned char *, int);
        virtual void RequestTrade(unsigned int, unsigned int);
        virtual void SPBadLandsObjCollision(const struct SSPBadLandsObjCollisionInfo &, unsigned int);
        virtual void SPMunitionCollision(const struct SSPMunitionCollisionInfo &, unsigned int);
        virtual void SPObjCollision(const struct SSPObjCollisionInfo &, unsigned int);
        virtual void SPObjUpdate(const struct SSPObjUpdateInfo &, unsigned int);
        virtual void SPRequestInvincibility(unsigned int, bool, enum InvincibilityReason, unsigned int);
        virtual void SPRequestUseItem(const struct SSPUseItem &, unsigned int);
        virtual void SPScanCargo(const unsigned int &, const unsigned int &, unsigned int);
        virtual void SaveGame(struct CHARACTER_ID const &, const unsigned short *, unsigned int);
        virtual void SetActiveConnection(enum EFLConnection);
        virtual void SetInterfaceState(unsigned int, unsigned char *, int);
        virtual void SetManeuver(unsigned int, const struct XSetManeuver &);
        virtual void SetMissionLog(unsigned int, unsigned char *, int);
        virtual void SetTarget(unsigned int, const struct XSetTarget &);
        virtual void SetTradeMoney(unsigned int, unsigned long);
        virtual void SetVisitedState(unsigned int, unsigned char *, int);
        virtual void SetWeaponGroup(unsigned int, unsigned char *, int);
        virtual void Shutdown(void);
        virtual bool Startup(const struct SStartupInfo &);
        virtual void StopTradeRequest(unsigned int);
        virtual void StopTradelane(unsigned int, unsigned int, unsigned int, unsigned int);
        virtual void SubmitChat(struct CHAT_ID, unsigned long, const void *, struct CHAT_ID, int);
        virtual bool SwapConnections(enum EFLConnection, enum EFLConnection);
        virtual void SystemSwitchOutComplete(unsigned int, unsigned int);
        virtual void TerminateTrade(unsigned int, int);
        virtual void TractorObjects(unsigned int, const struct XTractorObjects &);
        virtual void TradeResponse(const unsigned char *, int, unsigned int);
        virtual int Update(void);

    public:
        unsigned char data[OBJECT_DATA_SIZE];
};

struct CollisionGroupDescList
{
        std::list<CollisionGroupDesc> data;
};

struct EQ_ITEM
{
        EQ_ITEM *next;
        EQ_ITEM *prev;
        USHORT s1;
        USHORT sID;
        UINT iGoodID;
        char *szHardPoint;
        bool bMounted;
        char sz[3];
        float fHealth;
        UINT iCount;
        bool bMission;
};

struct FLString
{
        std::string value;
        UINT iDunno2[12];
};

struct CHARACTER_ID
{
        CHARACTER_ID(void);
        struct CHARACTER_ID const &operator=(struct CHARACTER_ID const &);
        void invalidate(void);
        bool is_valid(void) const;

        char szCharFilename[512]; // Only first 16 bytes are ever used
};

struct PlayerData
{
        wchar_t wszAccID[40];
        long x050, x054, x058, x05C;
        uint iNumberOfCharacters;
        CHARACTER_ID charFile;
        uint iShipArchetype;
        float fRelativeHealth;
        CollisionGroupDescList collisionGroupDesc;
        EquipDescList equipDescList;
        int iRank;
        int iMoneyNeededToNextRank;
        struct structCostume
        {
                UINT iHead;
                UINT iBody;
                UINT iLefthand;
                UINT iRighthand;
                UINT iAccessory[8];
                int iAccessories;
        };
        structCostume costume1;
        long x2C0, x2C4, x2C8, x2CC, x2D0, x2D4, x2D8, x2DC, x2E0;
        structCostume costume2;
        uint iReputation;
        int iInspectCash;
        int iWorth;
        uint iShipArchetypeWhenLanding;
        EquipDescList lShadowEquipDescList;
        int iNumKills;
        int iNumMissionSuccesses;
        int iNumMissionFailures;
        bool bSkipAutosave;
        char __padding0[3];
        uint iSaveCount;
        uint iOnlineID;
        bool bCheated;
        char __padding1[3];
        Vector vPosition;
        Matrix mOrientation;
        FLString weaponGroup; // 0x10 bytes
        uint iSetToZero;
        float fDifficulty;
        ushort sLastEquipID;
        char __padding2[2];
        uint aMenuItem;
        uint iOnlineID2;
        long x3D4, x3D8;
        uint iTradeRequestCount;
        uint iSystemID;
        uint iShipID;
        uint iCreatedShipID;
        uint iBaseID;
        uint iLastBaseID;
        uint iEnteredBase;
        uint iBaseRoomID;
        uint iCharacterID;
        class CAccount *Account;
        class CPlayerGroup *PlayerGroup;
        uint iMissionID;
        uint iMissionSetBy;
        uint iExitedBase;
        uint iUnknownLocID;
};

struct SCreateCharacterInfo
{
        wchar_t wszCharname[24];
        uint iNickName; // From [Faction] section of newcharacter.ini
        uint iBase;     // From [Faction] section of newcharacter.ini
        uint iPackage;  // From [Faction] section of newcharacter.ini
        uint iPilot;    // From [Faction] section of newcharacter.ini
        uint iDunno[96];
};

struct SStartupInfo
{
        uint iDunno[130];
        int iMaxPlayers;
};

struct SLoginInfo
{
        wchar_t wszAccount[36];
};

struct PlayerDBTreeNode
{
        PlayerDBTreeNode *pLeft;
        PlayerDBTreeNode *pParent;
        PlayerDBTreeNode *pRight;
        ulong l1;
        // File name of character
        char *szFLName;
        // Length of file name
        uint iLength;
        // Always seems to be 0x1F. Possibly max length of szFLName
        uint iDunno;
        // Account for this player
        CAccount *acc;
};

class IMPORT PlayerDB
{
    public:
        PlayerDB(const class PlayerDB &);
        PlayerDB(void);
        ~PlayerDB(void);
        class PlayerDB &operator=(const class PlayerDB &);
        struct PlayerData &operator[](const unsigned int &);
        bool BanAccount(std::wstring &, bool);
        void BuildLocalUserDir(void);
        unsigned int CountPlayersInSystem(int);
        bool CreateAccount(std::wstring &);
        void DeleteAccount(std::wstring &);
        void DeleteCharacterFromID(std::wstring &);
        bool DeleteCharacterFromName(std::wstring &);
        class CAccount *FindAccountFromCharacterID(std::string &);
        class CAccount *FindAccountFromCharacterName(std::wstring &);
        class CAccount *FindAccountFromClientID(unsigned int);
        class CAccount *FindAccountFromName(std::wstring &);
        bool GetAccountAdminRights(std::wstring &);
        bool GetAccountBanned(std::wstring &);
        const unsigned short *GetActiveCharacterName(unsigned int) const;
        bool GetCharactersForAccount(std::wstring &, class std::list<std::wstring> &);
        unsigned int GetGroupID(unsigned int);
        std::wstring &GetMOTD(void);
        unsigned int GetMaxPlayerCount(void);
        unsigned int GetServerID(void);
        const char *GetServerSig(void);
        void LockAccountAccess(std::wstring &);
        bool MakeLocalUserPath(char *, const char *);
        void ReadCharacterName(const char *, std::wstring &);
        void SendGroupID(unsigned int, unsigned int);
        void SendSystemID(unsigned int, unsigned int);
        bool SetAccountAdminRights(std::wstring &, bool);
        bool SetAccountPassword(std::wstring &, std::wstring &);
        void SetMOTD(std::wstring &);
        void UnlockAccountAccess(std::wstring &);
        void cleanup(unsigned int);
        bool create_new_character(const struct SCreateCharacterInfo &, unsigned int);
        bool create_restart_file(const char *);
        void free(void);
        void init(unsigned int, bool);
        bool is_valid(const unsigned int &);
        bool is_valid_ship_owner(const unsigned int &, const unsigned int &);
        unsigned char login(const struct SLoginInfo &, unsigned int);
        void logout(unsigned int);
        void logout_all(void);
        struct PlayerData *traverse_active(struct PlayerData *) const;

    private:
        int create_account(const struct SLoginInfo &);
        unsigned char load_user_data(const struct SLoginInfo &, unsigned int);
        unsigned int to_index(unsigned int);

    public:
        char *data;
        uint iDunno1[12];
        PlayerDBTreeNode *pFirstNode;
        PlayerDBTreeNode *pLastNode;
        uint iDunno2;
        uint iNumAccounts;
};

namespace CmnAsteroid // from FLCoreCommon.h
{
    class CAsteroidSystem;
};

namespace SrvAsteroid
{
    class IMPORT SrvAsteroidSystem
    {
        public:
            SrvAsteroidSystem(const SrvAsteroidSystem &);
            SrvAsteroidSystem(void);
            ~SrvAsteroidSystem(void);
            SrvAsteroidSystem &operator=(const SrvAsteroidSystem &);
            int AddRef(void);
            int Release(void);
            void load(const char *);
            void map_asteroid_fields(void);
            void set_cmn_system(class CmnAsteroid::CAsteroidSystem *);
            void set_sys_id(unsigned int);
            void update(void);

        public:
            unsigned char data[OBJECT_DATA_SIZE];
    };

}; // namespace SrvAsteroid

struct IMPORT StarSystem
{
        unsigned int count_players(unsigned int) const;

    public:
        unsigned char data[OBJECT_DATA_SIZE];
};

namespace SysDB
{
    IMPORT std::map<unsigned int, class StarSystem, std::less<unsigned int>, std::allocator<std::pair<const unsigned int, class StarSystem>>> SysMap;
};

namespace Controller
{
    struct TimerExpired;
}

template <class T>
class IMPORT OwnerList
{
    public:
        OwnerList<T>();
        virtual ~OwnerList<T>();
        class OwnerList<T> &operator=(const class OwnerList<T> &);
        void free();

    protected:
        unsigned char data[16];
};

namespace pub
{
    struct CargoEnumerator;

    IMPORT int BuildBaseReader(class INI_Reader &, const unsigned int &);
    IMPORT int BuildSystemReader(class INI_Reader &, const unsigned int &);
    IMPORT struct HINSTANCE__ *DLL_LoadLibrary(const char *);
    IMPORT void DebugPrint(const char *, int);
    IMPORT int FindHardpoint(const char *, unsigned int, class Vector &, class Matrix &);
    IMPORT int GetBaseID(unsigned int &, const char *);
    IMPORT unsigned int GetBaseNickname(char *, unsigned int, const unsigned int &);
    IMPORT int GetBaseStridName(unsigned int &, const unsigned int &);
    IMPORT int GetBases(const unsigned int &, unsigned int *const, unsigned int, unsigned int &);
    IMPORT int GetCargoHoldSize(const unsigned int &, unsigned int &);
    IMPORT int GetCostumeID(int &, const char *);
    IMPORT struct IFileSystem *GetDataPath(void);
    IMPORT int GetEquipmentID(unsigned int &, const char *);
    IMPORT int GetFullHealth(const unsigned int &, unsigned int &);
    IMPORT int GetGoodID(unsigned int &, const char *);
    IMPORT int GetGoodProperties(const unsigned int &, float &, float &);
    IMPORT int GetLoadout(struct EquipDescVector &, const unsigned int &);
    IMPORT int GetLoadoutID(unsigned int &, const char *);
    IMPORT int GetLoadoutName(const unsigned int &, char *, int);
    IMPORT unsigned int GetLocationNickname(char *, unsigned int, const unsigned int &);
    IMPORT int GetLocations(const unsigned int &, unsigned int *const, unsigned int, unsigned int &);
    IMPORT int GetMaxHitPoints(const unsigned int &, int &);
    IMPORT int GetNavMapScale(unsigned int, float &);
    IMPORT unsigned int GetNicknameId(const char *);
    IMPORT int GetRwTime(double &);
    IMPORT int GetShipArchSTRID(const unsigned int &, unsigned int &);
    IMPORT int GetShipID(unsigned int &, const char *);
    IMPORT int GetSolarType(const unsigned int &, unsigned int &);
    IMPORT int GetSystem(unsigned int &, const unsigned int &);
    IMPORT int GetSystemGateConnection(const unsigned int &, unsigned int &);
    IMPORT int GetSystemID(unsigned int &, const char *);
    IMPORT unsigned int GetSystemNickname(char *, unsigned int, const unsigned int &);
    IMPORT int GetTime(double &);
    IMPORT int GetType(const unsigned int &, unsigned int &);
    IMPORT int GetVoiceID(unsigned int &, const char *);
    IMPORT int IsCommodity(const unsigned int &, bool &);
    IMPORT unsigned short MakeId(const char *);
    IMPORT bool NextBaseID(unsigned int &);
    IMPORT bool NextSystemID(unsigned int &);
    IMPORT int ReportFreeTerminal(unsigned int, int);
    IMPORT int Save(unsigned int, unsigned int);
    // IMPORT int SetTimer(const unsigned int &, const struct Controller::TimerExpired &, float);
    IMPORT bool SinglePlayer(void);
    IMPORT int TranslateArchToGood(const unsigned int &, unsigned int &);
    IMPORT int TranslateGoodToMsgIdPrefix(unsigned int, struct TString<64> &);
    IMPORT int TranslateShipToMsgIdPrefix(unsigned int, struct TString<64> &);
    IMPORT int TranslateSystemToMsgIdPrefix(unsigned int, struct TString<64> &);

    namespace AI
    {
        class Personality;

        IMPORT enum OP_RTYPE SubmitDirective(unsigned int, class BaseOp *);
        IMPORT enum OP_RTYPE SubmitState(unsigned int, class BaseOp *);
        IMPORT bool enable_all_maneuvers(unsigned int);
        IMPORT bool enable_maneuver(unsigned int, int, bool);
        IMPORT int get_behavior_id(unsigned int);
        IMPORT bool get_personality(unsigned int, class Personality &);
        IMPORT enum ScanResponse get_scan_response(unsigned int, unsigned int, unsigned int);
        IMPORT int get_state_graph_id(unsigned int);
        IMPORT bool lock_maneuvers(unsigned int, bool);
        IMPORT void refresh_state_graph(unsigned int);
        IMPORT int remove_forced_target(unsigned int, unsigned int);
        IMPORT enum OP_RTYPE set_directive_priority(unsigned int, enum DirectivePriority);
        IMPORT bool set_player_enemy_clamp(unsigned int, int, int);
        IMPORT int submit_forced_target(unsigned int, unsigned int);
        IMPORT enum FORMATION_RTYPE update_formation_state(unsigned int, unsigned int, const class Vector &);
    }; // namespace AI

    namespace Audio
    {
        struct Tryptich
        {
                uint iDunno;
                uint iDunno2;
                uint iDunno3;
                uint iMusicID;
        };

        IMPORT int CancelMusic(unsigned int);
        IMPORT int PlaySoundEffect(unsigned int, unsigned int);
        IMPORT int SetMusic(unsigned int, const struct Tryptich &);
    }; // namespace Audio

    namespace Controller
    {
        struct CreateParms
        {
                void *pFreeFunc;
                uint iClientID;
        };

        enum PRIORITY;

        IMPORT unsigned int Create(const char *, const char *, const struct CreateParms *, enum PRIORITY);
        IMPORT void Destroy(unsigned int);
        IMPORT int SetHeartbeatInterval(const unsigned int &, float);
        IMPORT int _SendMessage(const unsigned int &, int, const void *);
    }; // namespace Controller

    namespace GF
    {
        IMPORT unsigned long AmbientScriptCreate(const struct AmbientScriptDescription &);
        IMPORT void AmbientScriptDestroy(unsigned long *);
        IMPORT unsigned long CharacterBehaviorCreate(const struct CharacterBehaviorDescription &);
        IMPORT void CharacterBehaviorDestroy(unsigned long *);
        IMPORT unsigned int CharacterCreate(const struct CharacterDescription &);
        IMPORT void CharacterDestroy(unsigned int *);
        IMPORT void CharacterSetBehavior(unsigned int, unsigned long);
        IMPORT void EnumerateCharacterPlacementIni(unsigned int, void (*)(int, class INI_Reader *, void *), void *);
        IMPORT unsigned int FindBase(const char *);
        IMPORT unsigned int FindLocation(unsigned int, const char *);
        IMPORT const char *FormCharacterPlacementName(const struct SetpointProperties *);
        IMPORT int GetAccessory(const char *);
        IMPORT int GetBasePosition(const unsigned int &, const unsigned int &, class Vector &);
        IMPORT int GetBodyPart(const char *, int);
        IMPORT unsigned int GetCharacterOnPlacement(unsigned int, unsigned int, int);
        IMPORT int GetCharacterPlacementByName(unsigned int, const char *, int &);
        IMPORT const char *GetCharacterPlacementName(unsigned int, int);
        IMPORT unsigned long GetCharacterPlacementOccupancy(unsigned int, int);
        IMPORT int GetCharacterPlacementPosture(unsigned int, int, unsigned long &);
        IMPORT bool GetCharacterPlacementProperties(unsigned int, int, struct SetpointProperties *);
        IMPORT void GetCostumeByID(int, struct Costume &);
        IMPORT int GetCostumeSkeletonGender(const struct Costume &, int &);
        IMPORT int GetMissionVendorOfferCount(unsigned int, unsigned int);
        IMPORT int GetNumCharacterPlacements(unsigned int);
        IMPORT float GetRtcPerformanceSlider(void);
        IMPORT unsigned int GetSpaceflightLocation(void);
        IMPORT bool IsCharacterPlacementNormal(unsigned int, int);
        IMPORT bool IsCharacterPlacementSpecial(unsigned int, int);
        IMPORT void MissionVendorAcceptance(unsigned long, bool, const struct FmtStr &, unsigned int);
        IMPORT unsigned long MissionVendorOfferCreate(const struct MissionVendorOfferDescription &);
        IMPORT void MissionVendorOfferDestroy(unsigned long *);
        IMPORT unsigned long NewsBroadcastCreate(const struct NewsBroadcastDescription &);
        IMPORT void NewsBroadcastDestroy(unsigned long *);
        IMPORT int ReportWhyMissionVendorEmpty(unsigned int, enum MVEmptyReason);
    }; // namespace GF

    namespace Market
    {
        IMPORT int GetCommoditiesForSale(unsigned int, unsigned int *const, int *);
        IMPORT int GetCommoditiesInDemand(unsigned int, unsigned int *const, int *);
        IMPORT int GetGoodJumpDist(unsigned int, unsigned int &);
        IMPORT int GetMinInventory(unsigned int, unsigned int, int &);
        IMPORT int GetNominalPrice(unsigned int, float &);
        IMPORT int GetNumCommoditiesForSale(unsigned int, int *);
        IMPORT int GetNumCommoditiesInDemand(unsigned int, int *);
        IMPORT int GetPrice(unsigned int, unsigned int, float &);
        IMPORT int IsGoodInDemand(unsigned int, unsigned int, bool &);
    }; // namespace Market

    namespace Phantom
    {
        IMPORT int Attach(const unsigned int &, void *);
        IMPORT int Create(unsigned int, const class Vector &, const class Vector &, const class Matrix &, unsigned int, void *&);
        IMPORT int Create(unsigned int, float, const class Vector &, unsigned int, void *&);
        IMPORT int Destroy(void *);
        IMPORT int Detach(void *);
        IMPORT int SetActive(void *, bool);
    }; // namespace Phantom

    namespace Player
    {
        IMPORT int AddCargo(const unsigned int &, const unsigned int &, unsigned int, float, bool);
        IMPORT int AdjustCash(const unsigned int &, int);
        IMPORT int CfgInterfaceNotification(unsigned int, unsigned int, bool, int);
        IMPORT int DisplayMissionMessage(const unsigned int &, const struct FmtStr &, enum MissionMessageType, bool);
        IMPORT int EnumerateCargo(const unsigned int &, struct pub::CargoEnumerator &);
        IMPORT int ForceLand(unsigned int, unsigned int);
        IMPORT int GetAssetValue(const unsigned int &, float &);
        IMPORT int GetBase(const unsigned int &, unsigned int &);
        IMPORT int GetBody(const unsigned int &, unsigned int &);
        IMPORT int GetCharacter(const unsigned int &, unsigned int &);
        IMPORT int GetGender(const unsigned int &, int &);
        IMPORT int GetGroupMembers(unsigned int, std::vector<unsigned int> &);
        IMPORT int GetGroupSize(unsigned int, unsigned int &);
        IMPORT int GetLocation(const unsigned int &, unsigned int &);
        IMPORT int GetMoneyNeededToNextRank(const unsigned int &, int &);
        IMPORT int GetMsnID(unsigned int, unsigned int &);
        IMPORT int GetName(unsigned int, std::wstring &);
        IMPORT int GetNumKills(const unsigned int &, int &);
        IMPORT int GetNumMissionFailures(const unsigned int &, int &);
        IMPORT int GetNumMissionSuccesses(const unsigned int &, int &);
        IMPORT int GetRank(const unsigned int &, int &);
        IMPORT int GetRelativeHealth(const unsigned int &, float &);
        IMPORT int GetRemainingHoldSize(const unsigned int &, float &);
        IMPORT int GetRep(const unsigned int &, int &);
        IMPORT int GetShip(const unsigned int &, unsigned int &);
        IMPORT int GetShipID(const unsigned int &, unsigned int &);
        IMPORT int GetSystem(const unsigned int &, unsigned int &);
        IMPORT int InspectCash(const unsigned int &, int &);
        IMPORT int IsGroupMember(unsigned int, unsigned int, bool &);
        IMPORT int LoadHint(unsigned int, struct BaseHint *);
        IMPORT int MarkObj(unsigned int, unsigned int, int);
        IMPORT int PopUpDialog(unsigned int, const struct FmtStr &, const struct FmtStr &, unsigned int);
        IMPORT int RemoveCargo(const unsigned int &, unsigned short, unsigned int);
        IMPORT int RemoveFromGroup(unsigned int);
        IMPORT int ReplaceMissionObjective(const unsigned int &, const unsigned int &, unsigned int, const struct MissionObjective &);
        IMPORT int ReturnBestPath(unsigned int, unsigned char *, int);
        IMPORT int ReturnPlayerStats(unsigned int, unsigned char *, int);
        IMPORT int RevertCamera(unsigned int);
        IMPORT int RewardGroup(unsigned int, int);
        IMPORT int SendNNMessage(unsigned int, unsigned int);
        IMPORT int SetCamera(unsigned int, const class Transform &, float, float);
        IMPORT int SetCostume(const unsigned int &, int);
        IMPORT int SetInitialOrnt(const unsigned int &, const class Matrix &);
        IMPORT int SetInitialPos(const unsigned int &, const class Vector &);
        IMPORT int SetMissionObjectiveState(const unsigned int &, const unsigned int &, int, unsigned int);
        IMPORT int SetMissionObjectives(const unsigned int &, const unsigned int &, const struct MissionObjective *, unsigned int, const struct FmtStr &,
                                        unsigned char, const struct FmtStr &);
        IMPORT int SetMoneyNeededToNextRank(unsigned int, int);
        IMPORT int SetMonkey(unsigned int);
        IMPORT int SetMsnID(unsigned int, unsigned int, unsigned int, bool, unsigned int);
        IMPORT int SetNumKills(const unsigned int &, int);
        IMPORT int SetNumMissionFailures(const unsigned int &, int);
        IMPORT int SetNumMissionSuccesses(const unsigned int &, int);
        IMPORT int SetRank(unsigned int, int);
        IMPORT int SetRobot(unsigned int);
        IMPORT int SetShipAndLoadout(const unsigned int &, unsigned int, const struct EquipDescVector &);
        IMPORT int SetStoryCue(const unsigned int &, unsigned int);
        IMPORT int SetTrent(unsigned int);
    }; // namespace Player

    namespace Reputation
    {
        IMPORT int Alloc(int &, const struct FmtStr &, const struct FmtStr &);
        IMPORT int EnumerateGroups(struct Enumerator &);
        IMPORT int Free(const int &);
        IMPORT int GetAffiliation(const int &, unsigned int &);
        IMPORT int GetAttitude(const int &, const int &, float &);
        IMPORT int GetGroupFeelingsTowards(const int &, const unsigned int &, float &);
        IMPORT int GetGroupName(const unsigned int &, unsigned int &);
        IMPORT int GetName(const int &, struct FmtStr &, struct FmtStr &);
        IMPORT int GetRank(const int &, float &);
        IMPORT int GetReputation(int &, const struct ID_String &);
        IMPORT int GetReputation(int &, const char *);
        IMPORT int GetReputation(const int &, const unsigned int &, float &);
        IMPORT int GetReputation(const unsigned int &, const unsigned int &, float &);
        IMPORT int GetReputationGroup(unsigned int &, const char *);
        IMPORT int GetShortGroupName(const unsigned int &, unsigned int &);
        IMPORT int SetAffiliation(const int &, const unsigned int &);
        IMPORT int SetAttitude(const int &, const int &, float);
        IMPORT int SetRank(const int &, float);
        IMPORT int SetReputation(const int &, const unsigned int &, float);
        IMPORT int SetReputation(const unsigned int &, const unsigned int &, float);
    }; // namespace Reputation

    namespace SpaceObj
    {
        struct CargoDesc
        {
                int vTbl;
                int iUnk1;
                int iUnk2;
                int iUnk3;
                int iUnk4;
        };

        struct ShipInfo
        {
                uint iFlag;
                uint iSystem;
                uint iShipArchetype;
                Vector vPos;
                Vector vUnk1; // all 0
                Vector vUnk2; // all 0
                Matrix mOrientation;
                uint iUnk1; // 0
                uint iLoadout;
                OwnerList<pub::SpaceObj::CargoDesc> cargoDesc;
                uint iLook1;
                uint iLook2;
                uint unk4; // 0
                uint unk5; // 0
                uint iComm;
                float fUnk2;
                float fUnk3;
                float fUnk4;
                float fUnk5;
                float fUnk6;
                float fUnk7;
                float fUnk8;
                uint iUnk2;

                int iRep; // increases for each NPC spawned, starts at 0 or 1
                uint iPilotVoice;
                uint unk6;    // 0
                uint iHealth; // -1 = max health
                uint unk7;    // 0
                uint unk8;    // 0
                uint iLevel;
        };

        struct SolarInfo
        {
                int iFlag; // 0x290; ShipInfo has this too, no clue whether actually a flag
                uint iArchID;
                uint iSystemID;
                Vector vPos;
                Matrix mOrientation;
                uint iLoadoutID;
                struct structCostume
                {
                        UINT head;
                        UINT body;
                        UINT lefthand;
                        UINT righthand;
                        UINT accessory[8];
                        int accessories;
                };
                structCostume Costume;
                int iRep;
                int iVoiceID;
                uint iUnk8; // 0
                uint iUnk9; // Boolean, only last byte is used
                int iHitPointsLeft;
                char cNickName[64]; // Has to be unique
                uint iUnk11;        // 0 unused?
                uint iUnk12;        // 1 = flagged as mission solar, 0 = normal
        };

        struct LootInfo
        {
                uint iDunno; // "Flag" like the others?
                uint iArchID;
        };

        struct TerminalInfo
        {
                char szHardPoint[0x20];
                uint iType; // 1=berth 4=moor? 7=jump?
        };

        IMPORT int Activate(const unsigned int &, bool, int);
        IMPORT enum EQUIPMENT_RTYPE ActivateEquipment(const unsigned int &, struct EQInfo *);
        IMPORT int AddImpulse(const unsigned int &, const class Vector &, const class Vector &);
        IMPORT int Create(unsigned int &, const struct ShipInfo &);
        IMPORT int CreateLoot(unsigned int &, const struct LootInfo &);
        IMPORT int CreateSolar(unsigned int &, const struct SolarInfo &);
        IMPORT int Destroy(unsigned int, enum DestroyType);
        IMPORT int Dock(const unsigned int &, const unsigned int &, int, enum DOCK_HOST_RESPONSE);
        IMPORT int DockRequest(const unsigned int &, const unsigned int &);
        IMPORT int DrainShields(unsigned int);
        IMPORT int EnumerateCargo(const unsigned int &, struct pub::CargoEnumerator &);
        IMPORT int ExistsAndAlive(unsigned int);
        IMPORT int FormationResponse(const unsigned int &, enum FORMATION_RTYPE);
        IMPORT int GetArchetypeID(const unsigned int &, unsigned int &);
        IMPORT int GetAtmosphereRange(const unsigned int &, float &);
        IMPORT int GetBurnRadius(const unsigned int &, float &, class Vector &);
        IMPORT int GetCargoSpaceOccupied(const unsigned int &, unsigned int &);
        IMPORT int GetCenterOfMass(const unsigned int &, class Vector &);
        IMPORT int GetDockingTarget(const unsigned int &, unsigned int &);
        IMPORT int GetEmptyPos(const unsigned int &, const class Transform &, const float &, const float &, const enum PosSelectionType &, class Vector &);
        IMPORT int GetGoodID(const unsigned int &, unsigned int &);
        IMPORT int GetHardpoint(const unsigned int &, const char *, class Vector *, class Matrix *);
        IMPORT int GetHealth(const unsigned int &iSpaceObj, float &fCurrentHealth, float &fMaxHealth);
        IMPORT int GetInvincible(unsigned int, bool &, bool &, float &);
        IMPORT int GetJumpTarget(const unsigned int &, unsigned int &, unsigned int &);
        IMPORT int GetLocation(unsigned int, class Vector &, class Matrix &);
        IMPORT int GetMass(const unsigned int &, float &);
        IMPORT int GetMotion(unsigned int, class Vector &, class Vector &);
        IMPORT int GetRadius(const unsigned int &, float &, class Vector &);
        IMPORT int GetRelativeHealth(const unsigned int &, float &);
        IMPORT int GetRep(unsigned int, int &);
        IMPORT int GetScannerRange(unsigned int, int &, int &);
        IMPORT int GetShieldHealth(const unsigned int &iSpaceObj, float &fCurrentShields, float &fMaxShields, bool &bShieldsUp);
        IMPORT int GetSolarArchetypeID(unsigned int, unsigned int &);
        IMPORT int GetSolarArchetypeNickname(char *, int, unsigned int);
        IMPORT int GetSolarParent(const unsigned int &, unsigned int &);
        IMPORT int GetSolarRep(unsigned int, int &);
        IMPORT int GetSystem(unsigned int, unsigned int &);
        IMPORT int GetTarget(const unsigned int &, unsigned int &);
        IMPORT int GetTerminalInfo(const unsigned int &, int, struct TerminalInfo &);
        IMPORT int GetTradelaneNextAndPrev(const unsigned int &, unsigned int &, unsigned int &);
        IMPORT int GetType(unsigned int, unsigned int &);
        IMPORT int GetVoiceID(const unsigned int &, unsigned int &);
        IMPORT int InstantDock(const unsigned int &, const unsigned int &, int);
        IMPORT int IsPosEmpty(const unsigned int &, const class Vector &, const float &, bool &);
        IMPORT int JettisonEquipment(const unsigned int &, const unsigned short &, const int &);
        IMPORT int JumpIn(const unsigned int &, const unsigned int &);
        IMPORT int LaneResponse(const unsigned int &, int);
        IMPORT int Launch(const unsigned int &, const unsigned int &, int);
        IMPORT int LightFuse(const unsigned int &, const char *, float);
        IMPORT int Relocate(const unsigned int &, const unsigned int &, const class Vector &, const class Matrix &);
        IMPORT int RequestSpaceScript(const unsigned int &, const class Vector &, const int &, unsigned int, const char *);
        IMPORT int SendComm(unsigned int, unsigned int, unsigned int, const struct Costume *, unsigned int, unsigned int *, int, unsigned int, float, bool);
        IMPORT int SetInvincible2(unsigned int, bool, bool, float);
        IMPORT int SetInvincible(unsigned int, bool, bool, float);
        IMPORT int SetRelativeHealth(const unsigned int &, float);
    }; // namespace SpaceObj

    namespace System
    {
        struct SysObj
        {
                char nick[32]; // NOT NUL-terminated if longer
                Vector pos;
                UINT archid;
                UINT ids_name;
                UINT ids_info;
                char reputation[32];
                UINT dock_with;
                UINT goto_system;
                UINT system;
                // -------------------
                // Some nicknames are longer than 32 characters, so take advantage of the
                // fact that where it gets the nickname from immediately follows.
                size_t len; // TString<64>
                char nickname[64];
        };

        struct SysObjEnumerator
        {
                virtual bool operator()(const SysObj &) = 0;
        };

        struct Connection
        {
                UINT stuff[10];
        };

        struct ConnectionEnumerator
        {
                virtual bool operator()(const Connection &) = 0;
        };

        IMPORT int EnumerateConnections(const unsigned int &, struct pub::System::ConnectionEnumerator &, enum ConnectionType);
        IMPORT int EnumerateObjects(const unsigned int &, struct SysObjEnumerator &);
        IMPORT int EnumerateZones(const unsigned int &, struct ZoneEnumerator &);
        IMPORT int Find(const unsigned int &, const char *, unsigned int &);
        IMPORT int GetName(unsigned int, unsigned int &);
        IMPORT int GetNestedProperties(const unsigned int &, const class Vector &, unsigned long &);
        IMPORT int InZones(unsigned int, const class Transform &, float, float, float, unsigned int *const, unsigned int, unsigned int &);
        IMPORT int InZones(unsigned int, const class Vector &, float, unsigned int *const, unsigned int, unsigned int &);
        IMPORT int LoadSystem(const unsigned int &);
        IMPORT int ScanObjects(const unsigned int &, unsigned int *const, unsigned int, const class Vector &, float, unsigned int, unsigned int &);
    }; // namespace System

    namespace Zone
    {
        IMPORT float ClassifyPoint(unsigned int, const class Vector &);
        IMPORT float GetDistance(unsigned int, const class Vector &);
        IMPORT unsigned int GetId(unsigned int, const char *);
        IMPORT int GetLootableInfo(unsigned int, struct ID_String &, struct ID_String &, int &, int &, int &);
        IMPORT int GetName(unsigned int, unsigned int &);
        IMPORT int GetOrientation(const unsigned int &, class Matrix &);
        // IMPORT  int GetPopulation(unsigned int,class weighted_vector<unsigned int> const * &);
        IMPORT class Vector GetPos(unsigned int);
        IMPORT int GetProperties(unsigned int, unsigned long &);
        IMPORT float GetRadius(unsigned int);
        IMPORT int GetShape(unsigned int, enum ZoneShape &);
        IMPORT int GetSize(unsigned int, class Vector &);
        IMPORT unsigned int GetSystem(unsigned int);
        IMPORT bool InZone(unsigned int, const class Vector &, float);
        IMPORT bool Intersect(unsigned int, const class Vector &, const class Vector &, class Vector &);
        IMPORT bool VerifyId(unsigned int);
    }; // namespace Zone

}; // namespace pub

IMPORT void (*g_pPrivateChatHook)(unsigned short *, int);
IMPORT void (*g_pSystemChatHook)(unsigned short *, int);
IMPORT void (*g_pUniverseChatHook)(unsigned short *, int);

IMPORT PlayerDB Players;
extern "C" IMPORT IServerImpl Server;

#endif // _FLCORESERVER_H_
