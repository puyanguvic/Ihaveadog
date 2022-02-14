/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/stats-module.h"
#include "ns3/nstime.h"


#include "timestamp-tag.h"

namespace ns3 {

//----------------------------------------------------------------------
//-- TimestampTag
//------------------------------------------------------
TypeId
TimestampTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("TimestampTag")
    .SetParent<Tag> ()
    .AddConstructor<TimestampTag> ()
    .AddAttribute ("Timestamp",
                   "Some momentous point in time!",
                   EmptyAttributeValue (),
                   MakeTimeAccessor (&TimestampTag::GetTimestamp),
                   MakeTimeChecker ())
  ;
  return tid;
}

TypeId
TimestampTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
TimestampTag::GetSerializedSize (void) const
{
  return 8;     // 8 bytes
}

void
TimestampTag::Serialize (TagBuffer i) const
{
  int64_t t = m_timestamp.GetNanoSeconds ();
  i.Write ((const uint8_t *)&t, 8);
}

void
TimestampTag::Deserialize (TagBuffer i)
{
  int64_t t;
  i.Read ((uint8_t *)&t, 8);
  m_timestamp = NanoSeconds (t);
}

void
TimestampTag::SetTimestamp (Time time)
{
  m_timestamp = time;
}
Time
TimestampTag::GetTimestamp (void) const
{
  return m_timestamp;
}

double
TimestampTag::GetSeconds (void) const  // why Second here?
{
  double deadline = m_timestamp.GetSeconds();
  return deadline;
  std::cout << "t=" << deadline;
}

uint32_t
TimestampTag::GetMicroSeconds(void) const
{
  int64_t microSeconds = m_timestamp.GetMicroSeconds();
  //std::cout << "microSeconds int64 =" << microSeconds << std::endl;
  uint32_t m = (uint32_t)microSeconds;
  // std::cout << "m uint32_t = " << m << std::endl;
  return m;
}

void
TimestampTag::Print (std::ostream &os) const
{
  os << "t=" << m_timestamp;
  // std::cout << "t=" << m_timestamp.GetMicroSeconds() << std::endl;
}

}
