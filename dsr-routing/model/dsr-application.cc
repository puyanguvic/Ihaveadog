/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "timestamp-tag.h"
#include "priority-tag.h"
#include "cost-tag.h"
#include "budget-tag.h"
#include "flag-tag.h"
#include "dsr-application.h"


#define MAX_UINT_32 0xffffffff


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DsrApplication");

NS_OBJECT_ENSURE_REGISTERED (DsrApplication);

DsrApplication::DsrApplication ()
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

DsrApplication::~DsrApplication ()
{
    m_socket = 0;
}


void
DsrApplication::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, uint32_t budget, bool flag)
{
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
    m_budget = budget * 1000;
    m_flag = flag;
 }

 void
 DsrApplication::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, bool flag)
 {
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
    m_flag = flag;
 }
 

void
DsrApplication::StartApplication ()
{
    m_running = true;
    m_packetSent = 0;
    m_socket->Bind ();
    m_socket->Connect (m_peer);
    SendPacket ();
}

void
DsrApplication::StopApplication ()
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
DsrApplication::SendPacket()
{
    Ptr<Packet> packet = Create <Packet> (m_packetSize);
    TimestampTag txTime;
    txTime.SetTimestamp(Simulator::Now()); //  Pu need add time shift
    packet->AddPacketTag(txTime);
    // std::cout << "The TimeStamp = " << txTime.GetMicroSeconds() << "\t" << "send time" << Simulator::Now().GetMicroSeconds () << std::endl;
    if (m_budget != MAX_UINT_32)
    {
       BudgetTag budget;
       budget.SetBudget(m_budget);
       // std::cout << "----- the budget time = " << budget.GetBudget() << "\t" << m_budget << std::endl;
       packet->AddPacketTag(budget);
    }
    
    // rxy: Add Flag
    FlagTag flag;
    flag.SetFlagTag (m_flag);
    packet->AddPacketTag(flag);
    // std::cout << "The flag is : " << flag.GetFlagTag() << std::endl;
    
    m_socket->Send (packet);
    if(++ m_packetSent < m_nPackets)
    {
        ScheduleTx ();
    }
}

void
DsrApplication::ScheduleTx ()
{
    if (m_running)
    {

        Time tNext (Seconds (m_packetSize * 8 / static_cast <double> (m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule (tNext, &DsrApplication::SendPacket, this);
        // double tVal = Simulator::Now().GetSeconds();
        // if (tVal - int(tVal) >= 0.99)
        // {
        //     std::cout << "T" << Simulator::Now().GetSeconds () << "\t" << m_packetSent << std::endl;
        // }
    }

    // // rxy: Setup VBR traffic
    // if (m_running)
    // {
    //     double tVal = Simulator::Now().GetSeconds();
    //     uint64_t bVal = m_dataRate.GetBitRate();
    //     uint64_t br;

    //     if (tVal <= 0.5)
    //     {   
    //         // std::cout << "Enter Phase 1" << std::endl;
    //         br = bVal;
    //         Time tNext (Seconds (m_packetSize * 8 / static_cast <double> (br)));
    //         m_sendEvent = Simulator::Schedule (tNext, &DsrApplication::SendPacket, this);
    //     }   
    //     else if (tVal <= 1.0)
    //     {
    //         // std::cout << "Enter Phase 2" << std::endl;
    //         br = bVal*20/15;
    //         Time tNext (Seconds (m_packetSize * 8 / static_cast <double> (br)));
    //         m_sendEvent = Simulator::Schedule (tNext, &DsrApplication::SendPacket, this);
    //     }
    //     else if (tVal <= 1.5)
    //     {
    //         // std::cout << "Enter Phase 3" << std::endl;
    //         br = bVal*20/10;
    //         Time tNext (Seconds (m_packetSize * 8 / static_cast <double> (br)));
    //         m_sendEvent = Simulator::Schedule (tNext, &DsrApplication::SendPacket, this);
    //     }
    //     else if (tVal <= 4.0)
    //     {
    //         br = bVal*25/10;
    //         Time tNext (Seconds (m_packetSize * 8 / static_cast <double> (br)));
    //         m_sendEvent = Simulator::Schedule (tNext, &DsrApplication::SendPacket, this);
    //     }
    //     else
    //     {
    //         br =  bVal;
    //         Time tNext (Seconds (m_packetSize * 8 / static_cast <double> (br)));
    //         m_sendEvent = Simulator::Schedule (tNext, &DsrApplication::SendPacket, this);
    //     }
    //     // std::cout << "br = " << br << std::endl;
    // }
}

void
DsrApplication::ChangeRate (DataRate newDataRate)
{
    m_dataRate = newDataRate;
}

} // namespace ns3
