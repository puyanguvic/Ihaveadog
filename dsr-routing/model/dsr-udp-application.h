/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DSR_UDP_APPLICATION_H
#define DSR_UDP_APPLICATION_H


#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"

namespace ns3 {

class DsrUdpApplication : public Application
{

public:

  DsrUdpApplication ();
  static TypeId GetTypeId (void);
  virtual ~DsrUdpApplication();
  // application with udp
  void Setup (Ptr<Socket> socket, Address sinkAddress, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, uint32_t budget, bool flag);
  void Setup (Ptr<Socket> socket, Address sinkAddress, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, bool flag);
  void ChangeRate (DataRate newDataRate);
  void recv (int numBytesRcvd);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket> m_socket;
  Address m_peer;
  uint32_t m_packetSize;
  uint32_t m_nPackets;
  DataRate m_dataRate;
  EventId m_sendEvent;
  bool m_running;
  uint32_t m_packetSent;
  uint32_t m_budget;
  bool m_flag;
};
}

#endif /* DSR_APPLICATION_H */

