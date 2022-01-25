/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ipv4-dsr-routing-table-entry.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#define MAX_UINT32 0xffffffff

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Ipv4DSRRoutingTableEntry");

/*****************************************************
 *     Network Ipv4RoutingTableEntry
 *****************************************************/

Ipv4DSRRoutingTableEntry::Ipv4DSRRoutingTableEntry ()
{
  NS_LOG_FUNCTION (this);
}

/**
 * \author Pu Yang
 * \author change m_distance (MAX_UINT32) to m_distance (route.m_distance)
*/
Ipv4DSRRoutingTableEntry::Ipv4DSRRoutingTableEntry (Ipv4DSRRoutingTableEntry const &route)
  : m_dest (route.m_dest),
    m_destNetworkMask (route.m_destNetworkMask),
    m_gateway (route.m_gateway),
    m_interface (route.m_interface),
    m_distance (route.m_distance)
{
  NS_LOG_FUNCTION (this << route);
}

Ipv4DSRRoutingTableEntry::Ipv4DSRRoutingTableEntry (Ipv4DSRRoutingTableEntry const *route)
  : m_dest (route->m_dest),
    m_destNetworkMask (route->m_destNetworkMask),
    m_gateway (route->m_gateway),
    m_interface (route->m_interface),
    m_distance (route->m_distance)
{
  NS_LOG_FUNCTION (this << route);
}

Ipv4DSRRoutingTableEntry::Ipv4DSRRoutingTableEntry (Ipv4Address dest,
                                              Ipv4Address gateway,
                                              uint32_t interface)
  : m_dest (dest),
    m_destNetworkMask (Ipv4Mask::GetOnes ()),
    m_gateway (gateway),
    m_interface (interface),
    m_distance (MAX_UINT32)
{
}
Ipv4DSRRoutingTableEntry::Ipv4DSRRoutingTableEntry (Ipv4Address dest,
                                              uint32_t interface)
  : m_dest (dest),
    m_destNetworkMask (Ipv4Mask::GetOnes ()),
    m_gateway (Ipv4Address::GetZero ()),
    m_interface (interface),
    m_distance (MAX_UINT32)
{
}
Ipv4DSRRoutingTableEntry::Ipv4DSRRoutingTableEntry (Ipv4Address network,
                                              Ipv4Mask networkMask,
                                              Ipv4Address gateway,
                                              uint32_t interface)
  : m_dest (network),
    m_destNetworkMask (networkMask),
    m_gateway (gateway),
    m_interface (interface),
    m_distance (MAX_UINT32)
{
  NS_LOG_FUNCTION (this << network << networkMask << gateway << interface);
}
Ipv4DSRRoutingTableEntry::Ipv4DSRRoutingTableEntry (Ipv4Address network,
                                              Ipv4Mask networkMask,
                                              uint32_t interface)
  : m_dest (network),
    m_destNetworkMask (networkMask),
    m_gateway (Ipv4Address::GetZero ()),
    m_interface (interface),
    m_distance (MAX_UINT32)
{
  NS_LOG_FUNCTION (this << network << networkMask << interface);
}

  /**
   * \author Pu Yang
   * \brief Constructor.
   * \param dest destination address
   * \param interface the interface index
   * \param distance the distance between root and dest
   */
Ipv4DSRRoutingTableEntry::Ipv4DSRRoutingTableEntry (Ipv4Address dest,
                                              Ipv4Address gateway,
                                              uint32_t interface,
                                              uint32_t distance)
  : m_dest (dest),
    m_destNetworkMask (Ipv4Mask::GetOnes ()),
    m_gateway (gateway),
    m_interface (interface),
    m_distance (distance)
{
    // std::cout << "CreateNetworkRouteTo with distance" << distance << std::endl;
    NS_LOG_FUNCTION (this << dest << gateway << interface << distance);
}




bool
Ipv4DSRRoutingTableEntry::IsHost (void) const
{
  NS_LOG_FUNCTION (this);
  if (m_destNetworkMask == Ipv4Mask::GetOnes ())
    {
      return true;
    }
  else
    {
      return false;
    }
}
Ipv4Address
Ipv4DSRRoutingTableEntry::GetDest (void) const
{
  NS_LOG_FUNCTION (this);
  return m_dest;
}
bool
Ipv4DSRRoutingTableEntry::IsNetwork (void) const
{
  NS_LOG_FUNCTION (this);
  return !IsHost ();
}
bool
Ipv4DSRRoutingTableEntry::IsDefault (void) const
{
  NS_LOG_FUNCTION (this);
  if (m_dest == Ipv4Address::GetZero ())
    {
      return true;
    }
  else
    {
      return false;
    }
}
Ipv4Address
Ipv4DSRRoutingTableEntry::GetDestNetwork (void) const
{
  NS_LOG_FUNCTION (this);
  return m_dest;
}
Ipv4Mask
Ipv4DSRRoutingTableEntry::GetDestNetworkMask (void) const
{
  NS_LOG_FUNCTION (this);
  return m_destNetworkMask;
}
bool
Ipv4DSRRoutingTableEntry::IsGateway (void) const
{
  NS_LOG_FUNCTION (this);
  if (m_gateway == Ipv4Address::GetZero ())
    {
      return false;
    }
  else
    {
      return true;
    }
}
Ipv4Address
Ipv4DSRRoutingTableEntry::GetGateway (void) const
{
  NS_LOG_FUNCTION (this);
  return m_gateway;
}
uint32_t
Ipv4DSRRoutingTableEntry::GetInterface (void) const
{
  NS_LOG_FUNCTION (this);
  return m_interface;
}

/**
 * \author Pu Yang
 * get distance 
*/
uint32_t
Ipv4DSRRoutingTableEntry::GetDistance (void) const
{
  NS_LOG_FUNCTION (this);
  return m_distance;
}

Ipv4DSRRoutingTableEntry 
Ipv4DSRRoutingTableEntry::CreateHostRouteTo (Ipv4Address dest, 
                                          Ipv4Address nextHop,
                                          uint32_t interface)
{
  NS_LOG_FUNCTION (dest << nextHop << interface);
  return Ipv4DSRRoutingTableEntry (dest, nextHop, interface);
}
Ipv4DSRRoutingTableEntry 
Ipv4DSRRoutingTableEntry::CreateHostRouteTo (Ipv4Address dest,
                                          uint32_t interface)
{
  NS_LOG_FUNCTION (dest << interface);
  return Ipv4DSRRoutingTableEntry (dest, interface);
}

/**
 * \author Pu Yang
 * \brief Create a host route with the distance value  
*/
Ipv4DSRRoutingTableEntry
Ipv4DSRRoutingTableEntry::CreateHostRouteTo (Ipv4Address dest, 
                                          Ipv4Address nextHop,
                                          uint32_t interface,
                                          uint32_t distance)
{
  NS_LOG_FUNCTION (dest << nextHop << interface << distance);
  // std::cout << "CreateNetworkRouteTo with distance" << std::endl;
  return Ipv4DSRRoutingTableEntry (dest, nextHop, interface, distance);
}

Ipv4DSRRoutingTableEntry 
Ipv4DSRRoutingTableEntry::CreateNetworkRouteTo (Ipv4Address network, 
                                             Ipv4Mask networkMask,
                                             Ipv4Address nextHop,
                                             uint32_t interface)
{
  NS_LOG_FUNCTION (network << networkMask << nextHop << interface);
  return Ipv4DSRRoutingTableEntry (network, networkMask, 
                                nextHop, interface);
}
Ipv4DSRRoutingTableEntry 
Ipv4DSRRoutingTableEntry::CreateNetworkRouteTo (Ipv4Address network, 
                                             Ipv4Mask networkMask,
                                             uint32_t interface)
{
  NS_LOG_FUNCTION (network << networkMask << interface);
  return Ipv4DSRRoutingTableEntry (network, networkMask, 
                                interface);
}
Ipv4DSRRoutingTableEntry 
Ipv4DSRRoutingTableEntry::CreateDefaultRoute (Ipv4Address nextHop, 
                                           uint32_t interface)
{
  NS_LOG_FUNCTION (nextHop << interface);
  return Ipv4DSRRoutingTableEntry (Ipv4Address::GetZero (), Ipv4Mask::GetZero (), nextHop, interface);
}


std::ostream& operator<< (std::ostream& os, Ipv4DSRRoutingTableEntry const& route)
{
  if (route.IsDefault ())
    {
      NS_ASSERT (route.IsGateway ());
      os << "default out=" << route.GetInterface () << ", next hop=" << route.GetGateway ();
    }
  else if (route.IsHost ())
    {
      if (route.IsGateway ())
        {
          os << "host="<< route.GetDest () << 
          ", out=" << route.GetInterface () <<
          ", next hop=" << route.GetGateway ();
        }
      else
        {
          os << "host="<< route.GetDest () << 
          ", out=" << route.GetInterface ();
        }
    }
  else if (route.IsNetwork ()) 
    {
      if (route.IsGateway ())
        {
          os << "network=" << route.GetDestNetwork () <<
          ", mask=" << route.GetDestNetworkMask () <<
          ",out=" << route.GetInterface () <<
          ", next hop=" << route.GetGateway ();
        }
      else
        {
          os << "network=" << route.GetDestNetwork () <<
          ", mask=" << route.GetDestNetworkMask () <<
          ",out=" << route.GetInterface ();
        }
    }
  else
    {
      NS_ASSERT (false);
    }
  return os;
}

bool operator== (const Ipv4DSRRoutingTableEntry a, const Ipv4DSRRoutingTableEntry b)
{
  return (a.GetDest () == b.GetDest () && 
          a.GetDestNetworkMask () == b.GetDestNetworkMask () &&
          a.GetGateway () == b.GetGateway () &&
          a.GetInterface () == b.GetInterface ());
}

} // namespace ns3
