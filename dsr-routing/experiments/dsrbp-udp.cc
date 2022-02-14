/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/dsr-routing-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

std::string dir = "result/";
std::string expName = "dsrbp-tcp";

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  NodeContainer nodes;
  nodes.Create (4);
  NodeContainer n0n1 = NodeContainer (nodes.Get (0), nodes.Get (1));
  NodeContainer n0n2 = NodeContainer (nodes.Get (0), nodes.Get (2));
  NodeContainer n1n3 = NodeContainer (nodes.Get (1), nodes.Get (3));
  NodeContainer n2n3 = NodeContainer (nodes.Get (2), nodes.Get (3));

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));

  NetDeviceContainer dev0, dev1, dev2, dev3;
  dev0 = p2p.Install (n0n1);
  dev1 = p2p.Install (n0n2);
  dev2 = p2p.Install (n1n3);
  dev3 = p2p.Install (n2n3);

  // install DSR routing 
  Ipv4DSRRoutingHelper dsr;
  Ipv4ListRoutingHelper list;
  list.Add (dsr, 10);
  InternetStackHelper internet;
  internet.SetRoutingHelper (list);
  internet.Install (nodes);

  // install dsr virtual queue
  TrafficControlHelper tch;
  tch.SetRootQueueDisc ("ns3::DsrVirtualQueueDisc");
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
  
  QueueDiscContainer qdisc;
  tch.Install (dev1);
  tch.Install (dev2);
  tch.Install (dev3);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1;
  i0i1 = ipv4.Assign (dev0);
  i0i1.SetMetric (0, 10);
  i0i1.SetMetric (1, 10);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i2;
  i0i2 = ipv4.Assign (dev1);
  i0i2.SetMetric (0, 10);
  i0i2.SetMetric (1, 10);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i3;
  i1i3 = ipv4.Assign (dev2);
  i1i3.SetMetric (0, 10);
  i1i3.SetMetric (1, 10);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3;
  i2i3 = ipv4.Assign (dev3);
  i2i3.SetMetric (0, 10);
  i2i3.SetMetric (1, 10);

  Ipv4DSRRoutingHelper::PopulateRoutingTables();
  Packet::EnablePrinting (); 

  // create a sink application
  uint16_t sinkPort = 9;
  Address sinkAddress (InetSocketAddress (i2i3.GetAddress (1), sinkPort));
  DsrSinkHelper dsrSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = dsrSinkHelper.Install (nodes.Get (3));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (20.));

  // create a sender application
  Ptr<Socket> udpSocket = Socket::CreateSocket (nodes.Get (0), UdpSocketFactory::GetTypeId ());
  uint32_t budget = 30;
  Ptr<DsrUdpApplication> app = CreateObject<DsrUdpApplication> ();
  app->Setup (udpSocket, sinkAddress, 200, 5, DataRate ("1Mbps"), budget, false);
  // app->Setup (udpSocket, sinkAddress, 200, 5, DataRate ("1Mbps"), false);
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (20.));

  AnimationInterface anim("dsr-udp.xml");

  Ipv4DSRRoutingHelper d;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>
  (expName + ".routes", std::ios::out);
  d.PrintRoutingTableAllAt (Seconds (0), routingStream);

  Simulator::Stop(Seconds(20));
  Simulator::Run();
  Simulator::Destroy ();

}