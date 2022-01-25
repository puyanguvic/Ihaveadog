/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/stats-module.h"
#include "cost-tag.h"

namespace ns3 {

//----------------------------------------------------------------------
//-- CostTag
//------------------------------------------------------
TypeId
CostTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("CostTag")
    .SetParent<Tag> ()
    .AddConstructor<CostTag> ()
    .AddAttribute ("Cost",
                   "The distance between root and dest!",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&CostTag::GetCost),
                   MakeUintegerChecker <uint32_t> ())
  ;
  return tid;
}

TypeId
CostTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CostTag::GetSerializedSize (void) const
{
  return 4;     // 4 bytes
}

void
CostTag::Serialize (TagBuffer i) const
{
  uint32_t t = m_cost;
  i.Write ((const uint8_t *)&t, 4);
}

void
CostTag::Deserialize (TagBuffer i)
{
  uint32_t t;
  i.Read ((uint8_t *)&t, 4);
  m_cost = t;
}

void
CostTag::SetCost (uint32_t cost)
{
  m_cost = cost;
}

uint32_t
CostTag::GetCost (void) const
{
  return m_cost;
}

void
CostTag::Print (std::ostream &os) const
{
  os << "t=" << m_cost;
  // std::cout << "t=" << m_timestamp.GetMicroSeconds() << std::endl;
}

}
