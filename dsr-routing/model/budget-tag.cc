/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/stats-module.h"
#include "budget-tag.h"

namespace ns3 {

//----------------------------------------------------------------------
//-- BudgetTag
//------------------------------------------------------
TypeId
BudgetTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("BudgetTag")
    .SetParent<Tag> ()
    .AddConstructor<BudgetTag> ()
    .AddAttribute ("Budget",
                   "The budget time in millisecond",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&BudgetTag::GetBudget),
                   MakeUintegerChecker <uint32_t> ())
  ;
  return tid;
}

TypeId
BudgetTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
BudgetTag::GetSerializedSize (void) const
{
  return 4;     // 4 bytes
}

void
BudgetTag::Serialize (TagBuffer i) const
{
  uint32_t t = m_budget;
  i.Write ((const uint8_t *)&t, 4);
}

void
BudgetTag::Deserialize (TagBuffer i)
{
  uint32_t t;
  i.Read ((uint8_t *)&t, 4);
  m_budget = t;
}

void
BudgetTag::SetBudget (uint32_t budget)
{
  m_budget = budget;
}

uint32_t
BudgetTag::GetBudget (void) const
{
  return m_budget;
}

void
BudgetTag::Print (std::ostream &os) const
{
  os << "t=" << m_budget;
  // std::cout << "t=" << m_timestamp.GetMicroSeconds() << std::endl;
}

}
