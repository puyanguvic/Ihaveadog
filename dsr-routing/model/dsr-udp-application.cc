/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "dsr-udp-application.h"
#include "dsr-header.h"


#define MAX_UINT_32 0xffffffff


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DsrUdpApplication");

NS_OBJECT_ENSURE_REGISTERED (DsrUdpApplication);

TypeId
DsrUdpApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DsrUdpApplication")
    .SetParent<Application> ()
    .SetGroupName("dsr-routing") 
    .AddConstructor<DsrUdpApplication> ()
  ;
  return tid;
}

DsrUdpApplication::DsrUdpApplication ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetSent (0),
    m_budget (MAX_UINT_32),
    m_flag (false)
{
}

DsrUdpApplication::~DsrUdpApplication ()
{
    m_socket = 0;
}


void
DsrUdpApplication::Setup (Ptr<Socket> socket, Address sinkAddress, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, uint32_t budget, bool flag)
{
    m_socket = socket;
    m_peer = sinkAddress;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
    m_budget = budget * 1000;
    m_flag = flag;
 }

 void
 DsrUdpApplication::Setup (Ptr<Socket> socket, Address sinkAddress, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, bool flag)
 {
    m_socket = socket;
    m_peer = sinkAddress;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
    m_flag = flag;
 }
 

void
DsrUdpApplication::StartApplication ()
{
    m_running = true;
    m_packetSent = 0;
    m_socket->Bind ();
    m_socket->Connect (m_peer);
    SendPacket ();
}

void
DsrUdpApplication::StopApplication ()
{
    m_running = false;
    if (m_sendEvent.IsRunning ())
    {
        Simulator::Cancel (m_sendEvent);
    }
    if (m_socket)
    {
        m_socket->Close ();
    }
}

void
DsrUdpApplication::SendPacket()
{
    DsrHeader dsrheader;
    
    Ptr<Packet> packet = Create <Packet> (m_packetSize - dsrheader.GetSerializedSize ());
    Time txTime = Simulator::Now ();
    uint32_t budget;
    if (m_budget == MAX_UINT_32)
    {
        budget = 0;
    }
    else
    {
        budget = m_budget;
    }
    bool flag = m_flag;
    dsrheader.SetTxTime (txTime);
    dsrheader.SetBudget (budget);
    dsrheader.SetFlag (flag);
    packet->AddHeader (dsrheader);
    // std::cout << "send size: " << packet->GetSize () << std::endl;
    m_socket->Send (packet);
    if(++ m_packetSent < m_nPackets)
    {
        ScheduleTx ();
    }
}

void
DsrUdpApplication::ScheduleTx ()
{
    if (m_running)
    {

        Time tNext (Seconds (m_packetSize * 8 / static_cast <double> (m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule (tNext, &DsrUdpApplication::SendPacket, this);
    }
}

void
DsrUdpApplication::ChangeRate (DataRate newDataRate)
{
    m_dataRate = newDataRate;
}

} // namespace ns3
