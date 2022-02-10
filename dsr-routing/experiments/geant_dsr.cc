/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ctime>

#include <sstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/dsr-routing-module.h"

#include "ns3/topology-read-module.h"
#include <list>

using namespace ns3;

std::string dir = "results/geant_ospf";
std::string ExpName = "Exp1";

NS_LOG_COMPONENT_DEFINE ("TestGeantTopology");

static std::list<unsigned int> data;

int main (int argc, char *argv[])
{

  std::string format ("Inet");
  std::string input ("contrib/dsr-routing/doc/Inet_geant_topo.txt");

  // Set up command line parameters used to control the experiment.
  CommandLine cmd (__FILE__);
  cmd.AddValue ("format", "Format to use for data input [Orbis|Inet|Rocketfuel].",
                format);
  cmd.AddValue ("input", "Name of the input file.",
                input);
  cmd.Parse (argc, argv);


  // ------------------------------------------------------------
  // -- Read topology data.
  // --------------------------------------------

  // Pick a topology reader based in the requested format.
  TopologyReaderHelper topoHelp;
  topoHelp.SetFileName (input);
  topoHelp.SetFileType (format);
  Ptr<TopologyReader> inFile = topoHelp.GetTopologyReader ();

  NodeContainer nodes;

  if (inFile != 0)
    {
      nodes = inFile->Read ();
    }

  if (inFile->LinksSize () == 0)
    {
      NS_LOG_ERROR ("Problems reading the topology file. Failing.");
      return -1;
    }

  // ------------------------------------------------------------
  // -- Create nodes and network stacks
  // --------------------------------------------
  NS_LOG_INFO ("creating internet stack");
  // Setup Routing algorithm
  Ipv4DSRRoutingHelper dsr;
  Ipv4ListRoutingHelper list;
  list.Add (dsr, 10);
  InternetStackHelper internet;
  internet.SetRoutingHelper (list);
  internet.Install (nodes);

  NS_LOG_INFO ("creating ipv4 addresses");
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.252");

  int totlinks = inFile->LinksSize ();

  NS_LOG_INFO ("creating node containers");
  NodeContainer* nc = new NodeContainer[totlinks];
  TopologyReader::ConstLinksIterator iter;
  int i = 0;
  for ( iter = inFile->LinksBegin (); iter != inFile->LinksEnd (); iter++, i++ )
    {
      nc[i] = NodeContainer (iter->GetFromNode (), iter->GetToNode ());
    }

  NS_LOG_INFO ("creating net device containers");
  NetDeviceContainer* ndc = new NetDeviceContainer[totlinks];
  PointToPointHelper p2p;
  for (int i = 0; i < totlinks; i++)
    {
      // p2p.SetChannelAttribute ("Delay", TimeValue(MilliSeconds(weight[i])));
      p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
      p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
      ndc[i] = p2p.Install (nc[i]);
    }
  // install dsr-queue disc
  

  // it crates little subnets, one for each couple of nodes.
  NS_LOG_INFO ("creating ipv4 interfaces");
  Ipv4InterfaceContainer* ipic = new Ipv4InterfaceContainer[totlinks];
  for (int i = 0; i < totlinks; i++)
    {
      ipic[i] = address.Assign (ndc[i]);
      address.NewNetwork ();
    }
  
  // set metric = channel delay
  for (uint32_t i = 0; i < nodes.GetN (); i ++)
    {
      Ptr<Ipv4> ipv4 = nodes.Get (i)->GetObject<Ipv4> ();
      for (uint32_t j = 0; j < ipv4->GetNInterfaces (); j ++)
        {
          Ptr<Channel> channel = ipv4->GetNetDevice (j)->GetChannel ();
          TimeValue delay;
          channel->GetAttribute ("Delay", delay);
          uint32_t matric = delay.Get ().GetMicroSeconds ();
          ipv4->SetMetric (j, matric);
        }
    }

  Ipv4DSRRoutingHelper::PopulateRoutingTables();

  uint32_t totalNodes = nodes.GetN ();
  Ptr<UniformRandomVariable> unifRandom = CreateObject<UniformRandomVariable> ();
  unifRandom->SetAttribute ("Min", DoubleValue (0));
  unifRandom->SetAttribute ("Max", DoubleValue (totalNodes - 1));

  unsigned int randomServerNumber = unifRandom->GetInteger (0, totalNodes - 1);

  Ptr<Node> randomServerNode = nodes.Get (randomServerNumber);
  Ptr<Ipv4> ipv4Server = randomServerNode->GetObject<Ipv4> ();
  Ipv4InterfaceAddress iaddrServer = ipv4Server->GetAddress (1,0);
  Ipv4Address ipv4AddrServer = iaddrServer.GetLocal ();
  
  //Create a dsrSink applications
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress ( ipv4AddrServer, sinkPort));
  DsrSinkHelper dsrSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = dsrSinkHelper.Install (nodes.Get (randomServerNumber));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (20.));

  // create a sender application
  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (0), UdpSocketFactory::GetTypeId ());
  uint32_t budget = 30;
  Ptr<DsrApplication> app = CreateObject<DsrApplication> ();
  app->Setup (ns3TcpSocket, sinkAddress, 200, 1500, DataRate ("1Mbps"), budget, false);
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (20.));

  // ------------------------------------------------------------
  // -- Net anim
  // ---------------------------------------------
  AnimationInterface anim (dir + ExpName + ".xml");
  anim.SetConstantPosition (nodes.Get (0), 16.3729,	48.2091);
  anim.SetConstantPosition (nodes.Get (1), 4.3518, 50.8469);
  anim.SetConstantPosition (nodes.Get (2), 6.1399, 46.2038);
  anim.SetConstantPosition (nodes.Get (3), 14.4423, 50.0785);
  anim.SetConstantPosition (nodes.Get (4), 8.6842, 50.1122);
  anim.SetConstantPosition (nodes.Get (5), -3.7033, 40.4167);
  anim.SetConstantPosition (nodes.Get (6), 2.351, 48.8566);
  anim.SetConstantPosition (nodes.Get (7), 23.5808, 37.9778);
  anim.SetConstantPosition (nodes.Get (8), 15.9644, 45.8071);
  anim.SetConstantPosition (nodes.Get (9), 19.0936, 47.4976);
  anim.SetConstantPosition (nodes.Get (10), -6.2573, 53.3416);
  anim.SetConstantPosition (nodes.Get (11), 34.8097, 32.0714);
  anim.SetConstantPosition (nodes.Get (12), 9.19, 45.4642);
  anim.SetConstantPosition (nodes.Get (13), 6.1296, 49.6112);
  anim.SetConstantPosition (nodes.Get (14), 4.9407, 52.3236);
  anim.SetConstantPosition (nodes.Get (15), -73.94384, 40.6698);
  anim.SetConstantPosition (nodes.Get (16), 16.8874, 52.3963);
  anim.SetConstantPosition (nodes.Get (17), -9.1363, 38.7073);
  anim.SetConstantPosition (nodes.Get (18), 17.8742, 59.3617);
  anim.SetConstantPosition (nodes.Get (19), 14.5148, 46.0574);
  anim.SetConstantPosition (nodes.Get (20), 17.1297, 48.1531);
  anim.SetConstantPosition (nodes.Get (21), -0.1264, 51.5086);

  // ------------------------------------------------------------
  // -- Run the simulation
  // --------------------------------------------
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();

  delete[] ipic;
  delete[] ndc;
  delete[] nc;

  NS_LOG_INFO ("Done.");

  return 0;
}
