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

  // create send application
  uint16_t port = 8080;
  DsrTcpAppHelper sourceHelper ("ns3::TcpSocketFactory",
                               InetSocketAddress (i1i3.GetAddress (1), port));
  sourceHelper.SetAttribute ("MaxBytes", UintegerValue (6000000));
  sourceHelper.SetAttribute ("Budget", UintegerValue (30));
  ApplicationContainer sourceApp = sourceHelper.Install (nodes.Get (0));
  sourceApp.Start (Seconds (1.0));
  sourceApp.Stop (Seconds (10.0));

  // create sink application
  DsrSinkHelper sinkHelper ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp = sinkHelper.Install (nodes.Get (3));
  sinkApp.Start (Seconds (0.0));
  sinkApp.Stop (Seconds (10.0));

  AnimationInterface anim("dsrbp-tcp.xml");

  Ipv4DSRRoutingHelper d;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>
  (expName + ".routes", std::ios::out);
  d.PrintRoutingTableAllAt (Seconds (0), routingStream);

  Simulator::Stop(Seconds(20));
  Simulator::Run();
  Simulator::Destroy ();

}