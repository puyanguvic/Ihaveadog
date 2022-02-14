/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PRIORITYTAG_H
#define PRIORITYTAG_H

#include "ns3/core-module.h"
#include "ns3/tag.h"
#include "ns3/packet.h"

namespace ns3 {

class PriorityTag : public Tag
 {
 public:
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual uint32_t GetSerializedSize (void) const;
   virtual void Serialize (TagBuffer i) const;
   virtual void Deserialize (TagBuffer i);
   virtual void Print (std::ostream &os) const;
 
   // these are our accessors to our tag structure
   void SetPriority (uint32_t priority);
   uint32_t GetPriority (void) const;
 private:
   uint32_t m_priority; // 0-fast, 1-slow, 2-best effort
 };

}

#endif /* PRIORITYTAG_H */