/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

  // Create a new NS-3 simulation
  NodeContainer nodes;
  nodes.Create(100); // Create n nodes

  // Set up point-to-point connection
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

  // Create point-to-point links between nodes
  NetDeviceContainer devices;
  for (uint32_t i = 0; i < nodes.GetN() - 1; i++) {
    devices.Add(pointToPoint.Install(nodes.Get(i), nodes.Get(i + 1)));
  }

  // Install internet stack
  InternetStackHelper stack;
  stack.Install(nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign(devices);

  // Create a UDP echo server on Node 1
  UdpEchoServerHelper echoServer(9);
  ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(10.0));

  // Create a UDP echo client on Node 0
  UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue(1));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  echoClient.SetAttribute("PacketSize", UintegerValue(1024));

  ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
  clientApps.Start(Seconds(2.0));
  clientApps.Stop(Seconds(10.0));

  AnimationInterface anim("Main2.xml");

  // Run the simulation
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

