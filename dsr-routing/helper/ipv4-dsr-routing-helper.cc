/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "ipv4-dsr-routing-helper.h"
#include "ns3/dsr-router-interface.h"
#include "ns3/ipv4-dsr-routing.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DSRRoutingHelper");

Ipv4DSRRoutingHelper::Ipv4DSRRoutingHelper ()
{
}

Ipv4DSRRoutingHelper::Ipv4DSRRoutingHelper (const Ipv4DSRRoutingHelper &o)
{
}

Ipv4DSRRoutingHelper*
Ipv4DSRRoutingHelper::Copy (void) const
{
  return new Ipv4DSRRoutingHelper (*this);
}

Ptr<Ipv4RoutingProtocol>
Ipv4DSRRoutingHelper::Create (Ptr<Node> node) const
{
  NS_LOG_LOGIC ("Adding DSRRouter interface to node " <<
                node->GetId ());

  Ptr<DSRRouter> dsrRouter = CreateObject<DSRRouter> ();
  node->AggregateObject (dsrRouter);

  NS_LOG_LOGIC ("Adding DSRRouting Protocol to node " << node->GetId ());
  Ptr<Ipv4DSRRouting> dsrRouting = CreateObject<Ipv4DSRRouting> ();
  dsrRouter->SetRoutingProtocol (dsrRouting);

  return dsrRouting;
}

void 
Ipv4DSRRoutingHelper::PopulateRoutingTables (void)
{
  DSRRouteManager::BuildDSRRoutingDatabase ();
  DSRRouteManager::InitializeRoutes ();
}
void 
Ipv4DSRRoutingHelper::RecomputeRoutingTables (void)
{
  DSRRouteManager::DeleteDSRRoutes ();
  DSRRouteManager::BuildDSRRoutingDatabase ();
  DSRRouteManager::InitializeRoutes ();
}


} // namespace ns3
