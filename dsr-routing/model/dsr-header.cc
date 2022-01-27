/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "dsr-header.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DsrHeader");

NS_OBJECT_ENSURE_REGISTERED (DsrHeader);

DsrHeader::DsrHeader ()
  : ID (0xff),
    m_txTime (Simulator::Now ()),
    m_budget (0),
    m_priority (99),
    m_flag (false)
{
}

DsrHeader::~DsrHeader ()
{
}

TypeId
DsrHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DsrHeader")
  .SetParent<Header> ()
  .AddConstructor<DsrHeader> ()
  ;
  return tid;
}

TypeId
DsrHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DsrHeader::Print (std::ostream &os) const
{
  os << "txTime =" << m_txTime.GetMicroSeconds () << "ms, "
     << "budget = " << m_budget << "ms, "
     << "priority = " << (uint32_t)m_priority << ", "
     << "flag = " << m_flag << std::endl;
}

void
DsrHeader::Serialize (Buffer::Iterator start) const
{
  start.Write ((const uint8_t *)&ID, 1);
  int64_t t = m_txTime.GetNanoSeconds ();
  start.Write ((const uint8_t *)&t, 8);
  start.Write((const uint8_t *)&m_budget, 4);
  start.Write((const uint8_t *)&m_priority, 1);
  start.Write((const uint8_t *)&m_flag, 1);
}

uint32_t
DsrHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  uint8_t id;
  i.Read ((uint8_t *)&id, 1);
  if (id != 0xff)
  {
    return 0;
  }
  int64_t t;
  i.Read ((uint8_t *)&t, 8);
  m_txTime = NanoSeconds (t);
  i.Read ((uint8_t *)&m_budget, 4);
  i.Read ((uint8_t *)&m_priority, 1);
  i.Read ((uint8_t *)&m_flag, 1);
  return GetSerializedSize ();
}

uint32_t
DsrHeader::GetSerializedSize (void) const
{
  // headersize = 1 + 8 + 4 + 1 + 1 bytes
  return 15;
}

void
DsrHeader::SetTxTime (Time txTime)
{
  m_txTime = txTime;
}

Time
DsrHeader::GetTxTime (void) const
{
  return m_txTime;
}

void
DsrHeader::SetBudget (uint32_t budget)
{
  m_budget = budget;
}

uint32_t
DsrHeader::GetBudget (void) const
{
  return m_budget;
}

void
DsrHeader::SetPriority (uint8_t priroity)
{
  m_priority = priroity;
}

uint8_t
DsrHeader::GetPriority (void) const
{
  return m_priority;
}

void
DsrHeader::SetFlag (bool flag)
{
  m_flag = flag;
}

bool
DsrHeader::GetFlag (void) const
{
  return m_flag;
}

}

