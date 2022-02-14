/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/object-factory.h"
#include "ns3/queue.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "dsr-virtual-queue-disc.h"
#include "priority-tag.h"
#include "timestamp-tag.h"

#define FAST_LANE 0
#define SLOW_LANE 1
#define NORMAL_LANE 2

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DsrVirtualQueueDisc");

NS_OBJECT_ENSURE_REGISTERED (DsrVirtualQueueDisc);

TypeId DsrVirtualQueueDisc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DsrVirtualQueueDisc")
    .SetParent<QueueDisc> ()
    .SetGroupName ("DsrRouting")
    .AddConstructor<DsrVirtualQueueDisc> ()
    .AddAttribute ("MaxSize",
                   "The maximum number of packets accepted by this queue disc.",
                   QueueSizeValue (QueueSize ("1000p")),
                   MakeQueueSizeAccessor (&QueueDisc::SetMaxSize,
                                          &QueueDisc::GetMaxSize),
                   MakeQueueSizeChecker ())
  ;
  return tid;
}

DsrVirtualQueueDisc::DsrVirtualQueueDisc ()
  : QueueDisc (QueueDiscSizePolicy::MULTIPLE_QUEUES, QueueSizeUnit::PACKETS)
{
  NS_LOG_FUNCTION (this);
}

DsrVirtualQueueDisc::~DsrVirtualQueueDisc ()
{
  NS_LOG_FUNCTION (this);
}


bool
DsrVirtualQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << item);
  uint32_t lane = EnqueueClassify (item);
  if (GetInternalQueue(lane)->GetCurrentSize ().GetValue() >= LinesSize[lane])
  {
    DropBeforeEnqueue (item, LIMIT_EXCEEDED_DROP);
  }
  bool retval = GetInternalQueue (lane)->Enqueue (item);
  return retval;
}

Ptr<QueueDiscItem>
DsrVirtualQueueDisc::DoDequeue (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<QueueDiscItem> item;
  uint32_t prio = Classify ();
  if (prio == 88)
  {
    return 0;
  }
  if (item = GetInternalQueue (prio)->Dequeue ())
    {
      NS_LOG_LOGIC ("Popped from band " << prio << ": " << item);
      NS_LOG_LOGIC ("Number packets band " << prio << ": " << GetInternalQueue (prio)->GetNPackets ());
      // std::cout << "++++++ Current Queue length: " << GetInternalQueue (prio)->GetNPackets () << " at band: " << item <<  std::endl;
      return item;
    }
  NS_LOG_LOGIC ("Queue empty");
  return item;
}

Ptr<const QueueDiscItem>
DsrVirtualQueueDisc::DoPeek (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<const QueueDiscItem> item;

  for (uint32_t i = 0; i < GetNInternalQueues (); i++)
    {
      if ((item = GetInternalQueue (i)->Peek ()) != 0)
        {
          NS_LOG_LOGIC ("Peeked from band " << i << ": " << item);
          NS_LOG_LOGIC ("Number packets band " << i << ": " << GetInternalQueue (i)->GetNPackets ());
          return item;
        }
    }

  NS_LOG_LOGIC ("Queue empty");
  return item;
}

bool
DsrVirtualQueueDisc::CheckConfig (void)
{
  // std::cout << "queue line Number = " << GetNInternalQueues () << std::endl;
  NS_LOG_FUNCTION (this);
  if (GetNQueueDiscClasses () > 0)
    {
      NS_LOG_ERROR ("DsrVirtualQueueDisc cannot have classes");
      return false;
    }

  if (GetNPacketFilters () != 0)
    {
      NS_LOG_ERROR ("DsrVirtualQueueDisc needs no packet filter");
      return false;
    }
  
  if (GetNInternalQueues () == 0)
    {
      // create 3 DropTail queues with GetLimit() packets each
      ObjectFactory factory;
      factory.SetTypeId ("ns3::DropTailQueue<QueueDiscItem>");
      factory.Set ("MaxSize", QueueSizeValue (GetMaxSize ()));
      AddInternalQueue (factory.Create<InternalQueue> ());
      AddInternalQueue (factory.Create<InternalQueue> ());
      AddInternalQueue (factory.Create<InternalQueue> ());
    }

  if (GetNInternalQueues () != 3)
    {
      NS_LOG_ERROR ("DsrVirtualQueueDisc needs 3 internal queues");
      return false;
    }

  if (GetInternalQueue (0)-> GetMaxSize ().GetUnit () != QueueSizeUnit::PACKETS ||
      GetInternalQueue (1)-> GetMaxSize ().GetUnit () != QueueSizeUnit::PACKETS ||
      GetInternalQueue (2)-> GetMaxSize ().GetUnit () != QueueSizeUnit::PACKETS)
    {
      NS_LOG_ERROR ("PfifoFastQueueDisc needs 3 internal queues operating in packet mode");
      return false;
    }

  for (uint8_t i = 0; i < 2; i++)
    {
      if (GetInternalQueue (i)->GetMaxSize () < GetMaxSize ())
        {
          NS_LOG_ERROR ("The capacity of some internal queue(s) is less than the queue disc capacity");
          return false;
        }
    }
  // std::cout << "The number of Internal Queues = "<< GetNInternalQueues () << std::endl;
  return true;
}

void
DsrVirtualQueueDisc::InitializeParams (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
DsrVirtualQueueDisc::Classify ()
{
  if (currentFastWeight > 0)
    {
      if (!GetInternalQueue (0)->IsEmpty ())
        {
          currentFastWeight--;
          return 0;
        }
      else
        {
          currentFastWeight = 0;
        }
    }
  if (currentSlowWeight > 0)
    {
      if (!GetInternalQueue (1)->IsEmpty ())
        {
          currentSlowWeight--;
          return 1;
        }
      else
        {
          currentSlowWeight = 0;
        }
    }
  if (currentNormalWeight > 0)
    {
      if (!GetInternalQueue (2)->IsEmpty ())
        {
          currentNormalWeight--;
          return 2;
        }
      else
        {
          currentNormalWeight = 0;
        }
    }
  currentFastWeight = m_fastWeight;
  currentSlowWeight = m_slowWeight;
  currentNormalWeight = m_normalWeight;
  
   if (currentFastWeight > 0)
    {
      if (!GetInternalQueue (0)->IsEmpty ())
        {
          currentFastWeight--;
          return 0;
        }
      else
        {
          currentFastWeight = 0;
        }
    }
  if (currentSlowWeight > 0)
    {
      if (!GetInternalQueue (1)->IsEmpty ())
        {
          currentSlowWeight--;
          return 1;
        }
      else
        {
          currentSlowWeight = 0;
        }
    }
  if (currentNormalWeight > 0)
    {
      if (!GetInternalQueue (2)->IsEmpty ())
        {
          currentNormalWeight--;
          return 2;
        }
      else
        {
          currentNormalWeight = 0;
        }
    }
  return 88;
}   

uint32_t
DsrVirtualQueueDisc::EnqueueClassify (Ptr<QueueDiscItem> item)
{
  PriorityTag priorityTag;
  if (item->GetPacket ()->FindFirstMatchingByteTag (priorityTag))
    {
      uint32_t priority = priorityTag.GetPriority ();
      switch (priority)
      {
      case 0x00:
        return FAST_LANE;
      case 0x01:
        return SLOW_LANE;
      default:
        return NORMAL_LANE;
      }
    }
  else
    {
      return NORMAL_LANE;
    }

}
} // namespace ns3