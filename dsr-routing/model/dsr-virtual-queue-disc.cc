/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/object-factory.h"
#include "ns3/queue.h"
#include "ns3/socket.h"
#include "dsr-virtual-queue-disc.h"
#include "priority-tag.h"
#include "budget-tag.h"
#include "timestamp-tag.h"
#include "flag-tag.h"

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
  FlagTag flagTag;
  item->GetPacket ()->PeekPacketTag (flagTag);
  PriorityTag priorityTag;
  uint32_t priority;
  BudgetTag budgetTag;
  TimestampTag timestampTag;
  item->GetPacket ()->PeekPacketTag (timestampTag);
  
  // Enqueue Best-Effort to best effort lane
  if (!item->GetPacket ()->PeekPacketTag (budgetTag))
  {
    bool retval = GetInternalQueue (2)->Enqueue (item);
    return retval;
  }

  item->GetPacket ()->PeekPacketTag (budgetTag);
  int32_t budget = budgetTag.GetBudget () + timestampTag.GetMicroSeconds () - Simulator::Now().GetMicroSeconds ();
  // std::cout << "***** current budget = " << budget << std::endl;
  
  if (budget < 0)
    {
      NS_LOG_LOGIC ("Timeout dropping");
      if (flagTag.GetFlagTag () == true)
        {
          std::cout << "DROP PACKETS BEFORE ENQUEUE: TIME OUT" << std::endl;
        }
      DropBeforeEnqueue (item, TIMEOUT_DROP); // BUG: This did not work
      return false;
    }
  if(item->GetPacket ()->PeekPacketTag (priorityTag))
    {
      priority = priorityTag.GetPriority ();
      // if (flagTag.GetFlagTag () == true)
      //   {
      //     std::cout<< "Enqueue to lane: "<< priority  << "queue size: "<< GetInternalQueue(priority)->GetCurrentSize ().GetValue()<< std::endl; // Check queue length
      //   }
      
      if(GetInternalQueue(priority)->GetCurrentSize ().GetValue() >= LinesSize[priority])
        {
          NS_LOG_LOGIC ("The Internal Queue limit exceeded -- dropping packet");
          DropBeforeEnqueue (item, LIMIT_EXCEEDED_DROP);
          std::cout << "DROP PACKETS: BUFFERBLOAT" << std::endl;
          if (flagTag.GetFlagTag () == true)
            {
              std::cout << "DROP PACKETS: BUFFERBLOAT" << std::endl;
            }
          return false;
        }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
      
      bool retval = GetInternalQueue (priority)->Enqueue (item);
      if (!retval)
        {
          NS_LOG_WARN ("Packet enqueue failed. Check the size of the internal queues");
        }

      NS_LOG_LOGIC ("Number packets band" << priority << ": " <<GetInternalQueue (priority)->GetNPackets ());
      return retval;
    }
  
  if (GetInternalQueue (2)->GetCurrentSize ().GetValue () >= LinesSize[2])
    {
      NS_LOG_LOGIC ("The Normal line Queue limit exceeded -- dropping packet");
      DropBeforeEnqueue (item, LIMIT_EXCEEDED_DROP);
      return false;
    }
  // std::cout << "The current Internal queue size =" << GetInternalQueue (2) ->GetCurrentSize ().GetValue () << std::endl;
  bool retval = GetInternalQueue (2)->Enqueue (item);
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

// Ptr<QueueDiscItem>
// DsrVirtualQueueDisc::DoDequeue (void)
// {
//   NS_LOG_FUNCTION (this);

//   Ptr<QueueDiscItem> item;
//   /* strict priority queue discipline
//      dequeue slow lane when the fast lane is empty
//      dequeue normal lane when the slow lane is empty
//      shortcoming: may starve the fast lane when there are plenty of packets in the slow/normal lane 
//      WRR shortcoming: will significant decrease the router effort
//                       It's not very easy to find a appropriate parameter
//   */
//   uint32_t weight[3]= {5, 3, 2};  
//   for (uint32_t i = 0; i < GetNQueueDiscClasses (); i++)
//     {
//       uint32_t account = weight[i];
//       if (-- account >= 0 && (item = GetQueueDiscClass (i)->GetQueueDisc ()->Dequeue ()) != 0)
//         {
//           NS_LOG_LOGIC ("Popped from band " << i << ": " << item);
//           NS_LOG_LOGIC ("Number packets band " << i << ": " << GetQueueDiscClass (i)->GetQueueDisc ()->GetNPackets ());
//           QueueSize Q = GetQueueDiscClass (i)->GetQueueDisc ()->GetCurrentSize();
//           QueueSize B = GetQueueDiscClass (i)->GetQueueDisc ()->GetMaxSize();
//           if (Q == B)
//           {
//             NS_LOG_ERROR ("Buffer bloat at band "<< i << "!!!");
//             // std::cout << "Buffer bloat at band " << i << "!!!" << std::endl;
//           }

//           // std::cout << "Buffer size of the lane "<< i << ": " << GetQueueDiscClass (i)->GetQueueDisc ()->GetMaxSize() << std::endl;
//           // std::cout << "Number of packets in lane "<< i << ": " << GetQueueDiscClass (i)->GetQueueDisc ()->GetCurrentSize() << std::endl;         
//           return item;
//         }
//     }
  
//   NS_LOG_LOGIC ("Queue empty");
//   return item;
// }


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

} // namespace ns3