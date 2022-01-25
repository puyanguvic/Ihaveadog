/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "dsr-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DsrHeader");

NS_OBJECT_ENSURE_REGISTERED (DsrHeader);

DsrHeader::DsrHeader ()
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
     << "budget = " << m_budget << std::endl;
}

void
DsrHeader::Serialize (Buffer::Iterator start) const
{
  int64_t t = m_txTime.GetNanoSeconds ();
  start.Write ((const uint8_t *)&t, 8);
  start.Write((const uint8_t *)&m_budget, 4);
}

uint32_t
DsrHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  int64_t t;
  i.Read ((uint8_t *)&t, 8);
  m_txTime = NanoSeconds (t);
  i.Read ((uint8_t *)&m_budget, 4);
  return GetSerializedSize ();
}

uint32_t
DsrHeader::GetSerializedSize (void) const
{
  // headersize = 8 + 4 bytes
  return 12;
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
  m_budget =budget;
}

uint32_t
DsrHeader::GetBudget (void) const
{
  return m_budget;
}

}

