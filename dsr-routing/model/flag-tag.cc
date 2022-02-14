/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/stats-module.h"

#include "flag-tag.h"

namespace ns3 {

//----------------------------------------------------------------------
//-- TimestampTag
//------------------------------------------------------
TypeId
FlagTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("FlagTag")
    .SetParent<Tag> ()
    .AddConstructor<FlagTag> ()
  ;
  return tid;
}

TypeId
FlagTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
FlagTag::GetSerializedSize (void) const
{
  return 1;     // 1 bytes
}

void
FlagTag::Serialize (TagBuffer i) const
{
  bool flag = m_flag;
  i.Write ((const uint8_t *)&flag, 1);
}

void
FlagTag::Deserialize (TagBuffer i)
{
  bool flag;
  i.Read ((uint8_t *)&flag, 1);
  m_flag = flag;
}

void
FlagTag::SetFlagTag (bool flag)
{
  m_flag = flag;
}

bool
FlagTag::GetFlagTag (void)
{
  return m_flag;
}

void
FlagTag::Print (std::ostream &os) const
{
  os << "flag = " << m_flag;
  // std::cout << "t=" << m_timestamp.GetMicroSeconds() << std::endl;
}

}
