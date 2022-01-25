/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef COSTTAG_H
#define COSTTAG_H

#include "ns3/core-module.h"
#include "ns3/tag.h"
#include "ns3/packet.h"

namespace ns3 {

class CostTag : public Tag
 {
 public:
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual uint32_t GetSerializedSize (void) const;
   virtual void Serialize (TagBuffer i) const;
   virtual void Deserialize (TagBuffer i);
   virtual void Print (std::ostream &os) const;
 
   // these are our accessors to our tag structure
   void SetCost (uint32_t cost);
   uint32_t GetCost (void) const;
 private:
   uint32_t m_cost;  
 };

}

#endif /* COSTTAG_H */