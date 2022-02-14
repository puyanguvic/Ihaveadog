/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/stats-module.h"
#include "priority-tag.h"

namespace ns3 {

//----------------------------------------------------------------------
//-- PriorityTag
//------------------------------------------------------
TypeId
PriorityTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("PriorityTag")
    .SetParent<Tag> ()
    .AddConstructor<PriorityTag> ()
    .AddAttribute ("Priority",
                   "The priority of packet",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&PriorityTag::GetPriority),
                   MakeUintegerChecker <uint32_t> ())
  ;
  return tid;
}

TypeId
PriorityTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
PriorityTag::GetSerializedSize (void) const
{
  return 4;     // 4 bytes
}

void
PriorityTag::Serialize (TagBuffer i) const
{
  uint32_t t = m_priority;
  i.Write ((const uint8_t *)&t, 4);
}

void
PriorityTag::Deserialize (TagBuffer i)
{
  uint32_t t;
  i.Read ((uint8_t *)&t, 4);
  m_priority = t;
}

void
PriorityTag::SetPriority (uint32_t priority)
{
  m_priority = priority;
}

uint32_t
PriorityTag::GetPriority (void) const
{
  return m_priority;
}

void
PriorityTag::Print (std::ostream &os) const
{
  os << "t=" << m_priority;
  // std::cout << "t=" << m_timestamp.GetMicroSeconds() << std::endl;
}

}
