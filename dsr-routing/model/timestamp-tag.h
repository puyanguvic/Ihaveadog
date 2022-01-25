/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TIMESTAMPTAG_H
#define TIMESTAMPTAG_H

#include "ns3/core-module.h"
#include "ns3/nstime.h"
#include "ns3/tag.h"
#include "ns3/packet.h"

namespace ns3 {

class TimestampTag : public Tag
 {
 public:
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual uint32_t GetSerializedSize (void) const;
   virtual void Serialize (TagBuffer i) const;
   virtual void Deserialize (TagBuffer i);
   virtual void Print (std::ostream &os) const;
 
   // these are our accessors to our tag structure
   void SetTimestamp (Time time);
   Time GetTimestamp (void) const;
   double GetSeconds (void) const;
   uint32_t GetMicroSeconds (void) const;
 private:
   Time m_timestamp;  
 };

}

#endif /* TIMESTAMPTAG_H */