/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef FLAGTAG_H
#define FLAGTAG_H

#include "ns3/core-module.h"
#include "ns3/nstime.h"
#include "ns3/tag.h"
#include "ns3/packet.h"

namespace ns3 {

/**
 * author: rxy
 * description: flagTag indicates the current packet is the target packet we
 * hope to inspect.
*/
class FlagTag : public Tag
 {
 public:
  //  static TypeId GetTypeId (void);
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual uint32_t GetSerializedSize (void) const;
   virtual void Serialize (TagBuffer i) const;
   virtual void Deserialize (TagBuffer i);
   virtual void Print (std::ostream &os) const;

   // these are our accessors to our tag structure
   void SetFlagTag (bool flag);
   bool GetFlagTag (void);

 private:
   bool m_flag;  
 };

}

#endif /* FLAGTAG_H */