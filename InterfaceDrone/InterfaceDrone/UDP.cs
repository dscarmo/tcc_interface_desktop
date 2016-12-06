﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace InterfaceDrone
{
    class UDP
    {
        UdpClient udpClient;
        UdpClient udpServer;
        IPEndPoint serverEndIP;
        IPEndPoint clientEndIP;
        IPAddress local_ip = IPAddress.Loopback;

        public UDP()
        {
            const int CLIENT_PORT = 11000;
            const int SERVER_PORT = 11001;
            


            serverEndIP = new IPEndPoint(IPAddress.Loopback, CLIENT_PORT);
            clientEndIP = new IPEndPoint(IPAddress.Loopback, SERVER_PORT);


            udpClient = new UdpClient(CLIENT_PORT);
            udpServer = new UdpClient(SERVER_PORT);

        }

        public void sendMsg(string str)
        {

            try
            { 
                // Sends a message to the host to which you have connected.
                Byte[] sendBytes = Encoding.ASCII.GetBytes(str);

                udpClient.Send(sendBytes, sendBytes.Length, clientEndIP);

                // Blocks until a message returns on this socket from a remote host.
                Byte[] receiveBytes = udpClient.Receive(ref clientEndIP);
                string returnData = Encoding.ASCII.GetString(receiveBytes);

                // Uses the IPEndPoint object to determine which of these two hosts responded.
                Console.WriteLine("This is the message you received " +
                                             returnData.ToString());
                Console.WriteLine("This message was sent from " +
                                            serverEndIP.Address.ToString() +
                                            " on their port number " +
                                            serverEndIP.Port.ToString());

                udpClient.Close();
            }  
            catch (Exception e )
            {
                  Console.WriteLine(e.ToString());
            }

        }

        public void server()
        {
            try
            {
                byte[] data = new byte[1024];
                

                Console.WriteLine("Waiting for a client...");
          

                data = udpServer.Receive(ref serverEndIP);

                Console.WriteLine("Message received from {0}:", clientEndIP.ToString());
                Console.WriteLine(Encoding.ASCII.GetString(data, 0, data.Length));

                string welcome = "Welcome to my test server";
                data = Encoding.ASCII.GetBytes(welcome);
                udpServer.Send(data, data.Length, clientEndIP);

                while (true)
                {
                    data = udpServer.Receive(ref serverEndIP);

                    Console.WriteLine(Encoding.ASCII.GetString(data, 0, data.Length));
                    udpServer.Send(data, data.Length, clientEndIP);
                }

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
           
        }

        public void client()
        {
            Console.WriteLine("Cliente inicializado, mandando mensagens de teste.");
            int mensagem = 1;

            Thread.Sleep(1000);
            while(mensagem < 10)
            {
                sendMsg(mensagem++.ToString());
                Thread.Sleep(500);
            }
            
        }
    }
    
}