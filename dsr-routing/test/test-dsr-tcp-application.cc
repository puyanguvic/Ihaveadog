/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/dsr-routing-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevs;
  p2pDevs = p2p.Install (nodes);
  p2p.EnableAsciiAll("simple-udp");

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifaces;
  ifaces = address.Assign (p2pDevs);
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();
  Packet::EnablePrinting (); 

  //Create a dsrSink applications
  uint16_t sinkPort = 9;
  Address sinkAddress (InetSocketAddress (ifaces.GetAddress (1), sinkPort));
  DsrSinkHelper dsrSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = dsrSinkHelper.Install (nodes.Get (1));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (20.));

  // create a sender application
  uint32_t budget = 30;
  uint32_t maxBytes = 100;
  uint32_t sendSize = 100;
  Ptr<DsrTcpApplication> app = CreateObject<DsrTcpApplication> ();
  app->Setup (sinkAddress, sendSize, maxBytes, budget);
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (20.));

  Ptr<FlowMonitor> flowmon;
  FlowMonitorHelper flowmonHelper;
  flowmon = flowmonHelper.InstallAll();
  flowmon->SerializeToXmlFile("dsr-application.xml", true, true);
  Simulator::Stop(Seconds(20));
  Simulator::Run();
  Simulator::Destroy ();

}