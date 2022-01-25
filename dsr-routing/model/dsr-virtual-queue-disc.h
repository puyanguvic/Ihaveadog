/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DSR_VIRTUAL_QUEUE_DISC_H
#define DSR_VIRTUAL_QUEUE_DISC_H

#include "ns3/queue-disc.h"

namespace ns3 {

class DsrVirtualQueueDisc : public QueueDisc {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * \brief PfifoFastQueueDisc constructor
   *
   * Creates a queue with a depth of 1000 packets per band by default
   */
  DsrVirtualQueueDisc ();

  virtual ~DsrVirtualQueueDisc();

  // Reasons for dropping packets
  static constexpr const char* LIMIT_EXCEEDED_DROP = "Queue disc limit exceeded";  //!< Packet dropped due to queue disc limit exceeded
  static constexpr const char* TIMEOUT_DROP = "time out !!!!!!!!";
  static constexpr const char* BUFFERBLOAT_DROP = "Buffer bloat !!!!!!!!";

private:
  // packet size = 1kB
  // packet size for test = 52B
  uint32_t LinesSize[3] = {12,36,100};
  uint32_t m_fastWeight = 10;
  uint32_t m_slowWeight = 3;
  uint32_t m_normalWeight = 2;

  uint32_t currentFastWeight = 0;
  uint32_t currentSlowWeight = 0;
  uint32_t currentNormalWeight = 0;
  virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
  virtual Ptr<QueueDiscItem> DoDequeue (void);
  // virtual void DoPrioDequeue (void);
  virtual Ptr<const QueueDiscItem> DoPeek (void);
  virtual bool CheckConfig (void);
  virtual void InitializeParams (void);
  virtual uint32_t Classify (); 
};

}

#endif /* DSR_VIRTUAL_QUEUE_DISC_H */
