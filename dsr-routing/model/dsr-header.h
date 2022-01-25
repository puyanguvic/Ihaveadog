/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DSR_HEADER_H
#define DSR_HEADER_H

#include "ns3/header.h"
#include "ns3/nstime.h"

namespace ns3 {

class DsrHeader : public Header
 {
 public:
   DsrHeader ();
   ~DsrHeader ();
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual void Print (std::ostream &os) const;
   virtual void Serialize (Buffer::Iterator start) const;
   virtual uint32_t Deserialize (Buffer::Iterator start);
   virtual uint32_t GetSerializedSize (void) const;

   void SetTxTime (Time txTime);
   Time GetTxTime (void) const;
   void SetBudget (uint32_t budget);
   uint32_t GetBudget (void) const;

 private:
   Time m_txTime;
   uint32_t m_budget;
 };

}

#endif /* DSR_HEADER_H */

