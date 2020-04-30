
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/psc-module.h"
#include "ns3/stats-module.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/wifi-module.h"
#include "ns3/netanim-module.h"
#include "ns3/lte-module.h"


#include <ns3/callback.h>
#include <ns3/mcptt-call-machine-grp-broadcast.h>
#include <ns3/mcptt-call-machine-grp-broadcast-state.h>
#include <ns3/mcptt-call-msg.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/mcptt-timer.h>
#include <iostream>

using namespace ns3;
//using namespace psc;

//initial environment :initial parameter configurations 
NS_LOG_COMPONENT_DEFINE ("broadcast_call_technique");

//virtual void ReceiveFloorRelease (const McpttFloorMsgRelease& msg);
//virtual void Send (const McpttFloorMsg& msg);

class BroadcastTestCallMachine : public McpttCallMachineGrpBroadcast
{
public:
 static TypeId GetTypeId (void);
 BroadcastTestCallMachine (void);
 virtual ~BroadcastTestCallMachine (void);
 virtual void ChangeState (Ptr<McpttCallMachineGrpBroadcastState>  newState);
 virtual TypeId GetInstanceTypeId (void) const;
 virtual void Receive (const McpttCallMsg& msg);
 virtual void Start (void);
 virtual void Send (const McpttCallMsg& msg);
protected:
 virtual void ExpiryOfTfb1 (void);
 virtual void ExpiryOfTfb2 (void);
 virtual void ExpiryOfTfb3 (void);
private:
 Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> m_postRxCb;
 Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> m_postTimerExpCb;
 Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> m_postTxCb;
 Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> m_preRxCb;
 Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> m_preTimerExpCb;
 Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> m_preTxCb;
 Ptr<McpttCallMachineGrpBroadcastState> m_startState;
 Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState> , Ptr<McpttCallMachineGrpBroadcastState> > m_stateChangeCb;
public:
 virtual Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> GetPostRxCb (void) const;
 virtual Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> GetPostTimerExpCb (void) const;
 virtual Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> GetPostTxCb (void) const;
 virtual Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> GetPreRxCb (void) const;
 virtual Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> GetPreTimerExpCb (void) const;
 virtual Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> GetPreTxCb (void) const;
 virtual Ptr<McpttCallMachineGrpBroadcastState> GetStartState(void) const;
 virtual Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState> , Ptr<McpttCallMachineGrpBroadcastState> > GetStateChangeCb (void) const;
 virtual void SetPostRxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  postRxCb);
 virtual void SetPostTimerExpCb (const Callback<void, const BroadcastTestCallMachine&, const McpttTimer&>  timerExpCb);
 virtual void SetPostTxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  postTxCb);
 virtual void SetPreRxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  preRxCb);
 virtual void SetPreTimerExpCb (const Callback<void, const BroadcastTestCallMachine&, const McpttTimer&>  timerExpCb);
 virtual void SetPreTxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  preTxCb);
 virtual void SetStartState (Ptr<McpttCallMachineGrpBroadcastState>  startState);
 virtual void SetStateChangeTestCb (const Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState> , Ptr<McpttCallMachineGrpBroadcastState> >  stateChangeCb);
};



int main (int argc, char *argv[])
{
   
// MCPTT configuration
//variable declarations for using push to talk 
uint32_t appCount;
uint32_t groupcount = 1;
uint32_t usersPerGroup = 5;
DataRate dataRate = DataRate ("24kb/s");
uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
double maxX = 5.0;
double maxY = 5.0;
double pushTimeMean = 5.0; // seconds
double pushTimeVariance = 2.0; // seconds
double releaseTimeMean = 5.0; // seconds
double releaseTimeVariance = 2.0; // seconds
Time startTime = Seconds (1);
Time stopTime = Seconds (15);
TypeId socketFacTid = UdpSocketFactory::GetTypeId ();
//uint32_t groupId = 1;
Ipv4Address peerAddress = Ipv4Address ("255.255.255.255");
appCount = usersPerGroup * groupcount;
  
uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
Ipv4AddressValue grpAddress;


// set config for floor request and call generation
//McpttFloorMsgFieldSsrc ssrc;
McpttCallMsg msg;
McpttCallMsgFieldCallId m_callId; //!< The ID of the call.
McpttCallMsgFieldCallType m_callType; //!< The current call type.
McpttCallMsgFieldGrpId m_grpId; //!< The group ID of the call.
Callback<void, uint16_t> m_newCallCb; //!< The new call callback.
McpttCallMsgFieldUserId m_origId; //!< The originating MCPTT user's ID.
//McpttCall* m_owner; //!< The owner under which the call machine resides.
//uint8_t m_priority; //!< The ProSe per-packet priority.
Ptr<UniformRandomVariable> m_rndCallId; //!< The random number generator used for call ID selection.
McpttCallMsgFieldSdp m_sdp; //!< SDP information.
//bool m_started; //!< The flag that indicates if the state machine has been started.
Ptr<McpttCallMachineGrpBroadcastState> m_state; //!< The current state.
Callback<void, const McpttEntityId&, const McpttEntityId&> m_stateChangeCb; //!< The state changed callback.
TracedCallback<uint32_t, uint32_t, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
//Ptr<McpttTimer> m_tfb1; //!< The timer TFB1.
//Ptr<McpttTimer> m_tfb2; //!< The timer TFB2.
//Ptr<McpttTimer> m_tfb3; //!< The timer TFB3.
//bool m_userAckReq; //!< Indicates if user acknowledgments are required.
Time delayTfb1 = Seconds(5);


//Physical layer 

//creating 3 nodes
NodeContainer nodes;
nodes.Create (appCount);

//positioning nodes
NS_LOG_INFO ("Building physical topology...");
Ptr<RandomBoxPositionAllocator> rndBoxPosAllocator = CreateObject <RandomBoxPositionAllocator> ();
rndBoxPosAllocator->SetX (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (maxX)));
rndBoxPosAllocator->SetY (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (maxY)));
rndBoxPosAllocator->SetZ (CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1.5)));


Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint32_t count = 0; count < appCount; count++)
    {
      Vector position = rndBoxPosAllocator->GetNext ();

      NS_LOG_INFO ("UE " << (count + 1) << " located at " << position << ".");

      positionAlloc->Add (position);
    }
  

//mobility of nodes set to stationary
MobilityHelper mobility;
mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (nodes);

//sidelink pre-configuration
//Configure the UE for UE_SELECTED scenario
Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (5)); //The number of RBs allocated per UE for Sidelink
Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (true)); //use default Trp index of 0

//Set the frequency
uint32_t ulEarfcn = 18100;
uint16_t ulBandwidth = 50;

// Set error models
Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (true));
Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (true));
Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (false));
  
//Set the UEs power in dBm
Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));

//cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
//cmd.AddValue ("enableNsLogs", "Enable ns-3 logging (debug builds)", enableNsLogs);
//cmd.Parse (argc, argv);

//Sidelink bearers activation time
//Time slBearersActivationTime = startTime;

//Create the helpers
Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

//Create and set the EPC helper
//Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
//lteHelper->SetEpcHelper (epcHelper);

//Create Sidelink helper and set lteHelper
Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
proseHelper->SetLteHelper (lteHelper);
Config::SetDefault ("ns3::LteSlBasicUeController::ProseHelper",PointerValue (proseHelper));

//Enable Sidelink
lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

//Set pathloss model
lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));

// channel model initialization
lteHelper->Initialize ();

// Since we are not installing eNB, we need to set the frequency attribute of pathloss model here
double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (ulEarfcn);
NS_LOG_LOGIC ("UL freq: " << ulFreq);
Ptr<Object> uplinkPathlossModel = lteHelper->GetUplinkPathlossModel ();
Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<PropagationLossModel> ();
NS_ABORT_MSG_IF (lossModel == NULL, "No PathLossModel");
bool ulFreqOk = uplinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
  if (!ulFreqOk)
    {
      NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
    }

//NetDeviceContainer devices = lteHelper->InstallUeDevice (nodes);
Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
ueSidelinkConfiguration->SetSlEnabled (true);

LteRrcSap::SlPreconfiguration preconfiguration;

preconfiguration.preconfigGeneral.carrierFreq = ulEarfcn;
preconfiguration.preconfigGeneral.slBandwidth = ulBandwidth;
preconfiguration.preconfigComm.nbPools = 1;

//-Configure preconfigured communication pool
preconfiguration.preconfigComm = proseHelper->GetDefaultSlPreconfigCommPoolList ();

//-Configure preconfigured discovery pool
preconfiguration.preconfigDisc = proseHelper->GetDefaultSlPreconfigDiscPoolList ();

//-Configure preconfigured UE-to-Network Relay parameters
preconfiguration.preconfigRelay = proseHelper->GetDefaultSlPreconfigRelay ();
  
//-Enable discovery
ueSidelinkConfiguration->SetDiscEnabled (true);
//-Set frequency for discovery messages monitoring //this is for another scenario with remote host
//ueSidelinkConfiguration->SetDiscInterFreq (ueDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());
  
LteRrcSap::SlCommTxResourcesSetup pool;
pool.setup = LteRrcSap::SlCommTxResourcesSetup::UE_SELECTED;
pool.ueSelected.havePoolToRelease = false;
pool.ueSelected.havePoolToAdd = true;
pool.ueSelected.poolToAddModList.nbPools = 1;
pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

LteSlPreconfigPoolFactory pfactory;

//Control
pfactory.SetControlPeriod ("sf40");
pfactory.SetControlBitmap (0x00000000FF); //8 subframes for PSCCH
pfactory.SetControlOffset (0);
pfactory.SetControlPrbNum (22);
pfactory.SetControlPrbStart (0);
pfactory.SetControlPrbEnd (49);

//Data
pfactory.SetDataBitmap (0xFFFFFFFFFF);
pfactory.SetDataOffset (8); //After 8 subframes of PSCCH
pfactory.SetDataPrbNum (25);
pfactory.SetDataPrbStart (0);
pfactory.SetDataPrbEnd (49);

//synchronization

preconfiguration.preconfigComm.pools[0] = pfactory.CreatePool ();
pfactory.SetHaveUeSelectedResourceConfig (true);
    
NetDeviceContainer devices;
ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
lteHelper->InstallSidelinkConfiguration (devices, ueSidelinkConfiguration);
  
/*Network layer***************************************************************************************/
//installing wifi interface in nodes
WifiHelper wifi;
wifi.SetStandard (WIFI_PHY_STANDARD_80211g); //2.4Ghz
wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("ErpOfdmRate54Mbps"));
 
WifiMacHelper wifiMac;
YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
YansWifiChannelHelper wifiChannel;
wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel",
                                "Frequency", DoubleValue (2.407e9)); //2.4Ghz

wifiMac.SetType ("ns3::AdhocWifiMac");

YansWifiPhyHelper phy = wifiPhy;
phy.SetChannel (wifiChannel.Create ());

WifiMacHelper mac = wifiMac;
devices = wifi.Install (phy, mac, nodes);

//installing internet access to all nodes
NS_LOG_INFO ("Installing internet stack on all nodes...");
InternetStackHelper internet;
internet.Install (nodes);
  
// assigning ip addresses to all devices of nodes
NS_LOG_INFO ("Assigning IP addresses to each net device...");
Ipv4AddressHelper ipv4;
ipv4.SetBase ("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer i = ipv4.Assign (devices);

/*Application layer*************************************************************************************/

NS_LOG_INFO ("Creating applications...");
//creating mcptt application for each device 
 
ApplicationContainer clientApps;
McpttHelper mcpttHelper;
McpttTimer mcpttTimer;
  
  //creating mcptt service on each node
  clientApps.Add (mcpttHelper.Install (nodes));
  
  clientApps.Start (startTime);
  clientApps.Stop (stopTime);

  mcpttHelper.SetPttApp ("ns3::McpttPttApp",
                         "PeerAddress", Ipv4AddressValue (peerAddress),
                         "PushOnStart", BooleanValue (true));
  mcpttHelper.SetMediaSrc ("ns3::McpttMediaSrc",
                         "Bytes", UintegerValue (msgSize),
                         "DataRate", DataRateValue (dataRate));
  mcpttHelper.SetPusher ("ns3::McpttPusher",
                         "Automatic", BooleanValue (true));
  mcpttHelper.SetPusherPushVariable ("ns3::NormalRandomVariable",
                         "Mean", DoubleValue (pushTimeMean),
                         "Variance", DoubleValue (pushTimeVariance));
  mcpttHelper.SetPusherReleaseVariable ("ns3::NormalRandomVariable",
                         "Mean", DoubleValue (releaseTimeMean),
                         "Variance", DoubleValue (releaseTimeVariance));


/*Call flow process************************************************************************************/

//set call owner, call id, mcptt id, call type, etc.**************************************

  //create floor and call control machines************************************************ 
ObjectFactory callFac;
callFac.SetTypeId ("ns3::McpttCallMachineGrpBroadcast");

ObjectFactory floorFac;
floorFac.SetTypeId ("ns3::McpttFloorMachineBasic");
  
Ipv4AddressValue grpAddr;

//creating location to store application info of UEs A, B 
Ptr<McpttPttApp> ueAPttApp = DynamicCast<McpttPttApp, Application> (clientApps.Get (0));
Ptr<McpttPttApp> ueBPttApp = DynamicCast<McpttPttApp, Application> (clientApps.Get (1));
  
//push button press schedule
Simulator::Schedule (Seconds (1.1), &McpttPttApp::TakePushNotification, ueAPttApp);

//send floor request*********************************************
 
//floor and call machines generate*******************************
McpttCallMachineGrpBroadcast machines;
 
// generate call 
ueAPttApp->CreateCall (callFac, floorFac);
ueAPttApp->SelectLastCall ();
ueBPttApp->CreateCall (callFac, floorFac);
ueBPttApp->SelectLastCall ();
  
//creating call interfaces and location to store call of UEs A, B 
Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
Ptr<McpttChan> callChan = ueAPttApp->GetCallChan ();
  
Ptr<McpttCallMachine> ueAMachine = ueACall->GetCallMachine ();
Ptr<McpttCallMachine> ueBMachine = ueBCall->GetCallMachine ();

Ptr<McpttPusher> ueAPusher = ueAPttApp->GetPusher ();
Ptr<McpttPusher> ueBPusher = ueBPttApp->GetPusher ();

Ptr<McpttMediaSrc> ueAMediaSrc = ueAPttApp->GetMediaSrc ();
Ptr<McpttMediaSrc> ueBMediaSrc = ueBPttApp->GetMediaSrc ();

McpttCallMachineGrpBroadcast broadcastMachines;
broadcastMachines.SetDelayTfb1(delayTfb1);
broadcastMachines.SetDelayTfb2(delayTfb1);
broadcastMachines.SetDelayTfb3(delayTfb1);
Ptr<McpttTimer> tfb1 =broadcastMachines.GetTfb1();
Ptr<McpttTimer> tfb2 =broadcastMachines.GetTfb2();
Ptr<McpttTimer> tfb3 =broadcastMachines.GetTfb3();

Simulator::Schedule (Seconds (5.15), &McpttCall::OpenFloorChan, ueACall, grpAddress.Get (), floorPort);
//Simulator::Schedule (Seconds (5.15), &McpttCall::OpenFloorChan, ueBCall, grpAddress.Get (), floorPort);

 /*
 McpttFloorMsgRequest::McpttFloorMsgRequest (uint32_t ssrc)
  : McpttFloorMsg (McpttFloorMsgRequest::SUBTYPE, ssrc)
{
  NS_LOG_FUNCTION (this << ssrc);

  uint8_t length = GetLength ();
  McpttFloorMsgFieldIndic indicator;
  McpttFloorMsgFieldPriority priority;
  McpttFloorMsgFieldTrackInfo trackInfo;
  McpttFloorMsgFieldUserId userId;

  length += indicator.GetSerializedSize ();
  length += priority.GetSerializedSize ();
  length += trackInfo.GetSerializedSize ();
  length += userId.GetSerializedSize ();

  SetLength (length);
  SetIndicator (indicator);
  SetPriority (priority);
  SetTrackInfo (trackInfo);
  SetUserId (userId);
}
 */

//receive floor granted******************************* 
       
//send floor control info (SCI) share to all participants**************

//relay : reception of SCI and decide to join the call


//generate message************************************ 
//Ptr<McpttTimer> ueATfb1 = ueAMachine->GetTfb1 ();
Ptr<Packet> pkt = Create<Packet> ();
pkt->AddHeader (msg);
//callChan->Send (pkt);  
 
NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: PttApp sending " << msg << ".");

//establish media session***************************** 
Simulator::Schedule (Seconds (5.15), &McpttCall::OpenMediaChan, ueACall, grpAddress.Get (), speechPort); 
//Simulator::Schedule (Seconds (5.15), &McpttCall::OpenMediaChan, ueBCall, grpAddress.Get (), speechPort);

//release button : send call***************************


//start timer TFB1 and TFB2***************************
Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, tfb1);

// synchronization and call in progress 
  
//end of call
Simulator::Schedule (Seconds (3.0), &McpttPttApp::ReleaseCall, ueAPttApp);




//Result generation*******************************************
NS_LOG_INFO ("Enabling MCPTT traces...");
mcpttHelper.EnableMsgTraces ();
mcpttHelper.EnableStateMachineTraces ();
  
NS_LOG_INFO ("Starting simulation...");
AnimationInterface anim("b20.xml");
anim.SetMaxPktsPerTraceFile(500000);

std::cout << "okay 4" << "\n" ;
//Simulator::Stop (Seconds (stopTime));
//anim.SetConstantPosition(nodes.Get(0),1.0,2.0);
//anim.SetConstantPosition(nodes.Get(1),4.0,5.0);
Simulator::Run ();
Simulator::Destroy();

NS_LOG_UNCOND ("Done Simulator");

NS_LOG_INFO ("Done.");
  
  
}
