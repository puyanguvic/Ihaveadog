#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/dsr-header.h"
#include "ns3/simulator.h"
#include "ns3/applications-module.h"
#include <iostream>
#include "ns3/dsr-routing-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  Packet::EnablePrinting ();
  DsrHeader dsrHeader;
  dsrHeader.SetTxTime (Simulator::Now ());
  uint32_t budget = 100;
  dsrHeader.SetBudget (budget);
  bool flag = true;
  dsrHeader.SetFlag (flag);
  dsrHeader.SetPriority (1);

  SeqTsHeader seqHeader;
  seqHeader.SetSeq (1);

  
  BudgetTag budgetTag, b, c;
  budgetTag.SetBudget (50);
  TimestampTag txtag;
  txtag.SetTimestamp (Simulator::Now ());

  Ptr<Packet> p = Create<Packet> (200);
  p->AddHeader (dsrHeader);
  p->AddHeader (seqHeader);
  p->AddPacketTag (budgetTag);
  p->AddPacketTag (txtag);
  p->Print (std::cout);

  Ptr<Packet> sent = p->CreateFragment (30, 40);
  std::cout << std::endl;
  std::cout << "packet size1 = " << sent->GetSize () << std::endl;
  // std::cout << "packet size2 = " << sent->GetSize () << std::endl;

  sent->PeekPacketTag (b);
  p->PeekPacketTag (c);
  std::cout << b.GetBudget () << "  " << c.GetBudget ();
  // DsrHeader peekHeader;
  // p->PeekHeader(peekHeader);
  // peekHeader.Print (std::cout);
}