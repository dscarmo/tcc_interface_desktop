using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;

namespace InterfaceDrone
{
    public class UDP
    {
        UdpClient udpClient;
        UdpClient udpServer;
        IPEndPoint serverEndIP;
        IPEndPoint clientEndIP;

        public string default_ip = "192.168.43.1";
        public static int SERVER_PORT = 11001;

        static int CLIENT_PORT = 11000;

        public UDP()
        {
            
            

            serverEndIP = new IPEndPoint(IPAddress.Loopback, CLIENT_PORT);
            //clientEndIP = new IPEndPoint(IPAddress.Loopback, SERVER_PORT);

            //clientEndIP = new IPEndPoint(IPAddress.Parse("192.168.26.2"), SERVER_PORT);

            //clientEndIP = new IPEndPoint(IPAddress.Parse("192.168.26.2"), SERVER_PORT);
            clientEndIP = new IPEndPoint(IPAddress.Parse(default_ip), SERVER_PORT);


            udpClient = new UdpClient(CLIENT_PORT);
            udpServer = new UdpClient(SERVER_PORT);

            udpClient.Client.SendTimeout = 1000;
            udpClient.Client.ReceiveTimeout = 1000;

        }

        private void sendMsg(string str)
        {

            try
            {
                Console.WriteLine("Cliente: Enviando: " + str);

                // Sends a message to the host to which you have connected.
                Byte[] sendBytes = Encoding.ASCII.GetBytes(str);

                udpClient.Send(sendBytes, sendBytes.Length, clientEndIP);

                // Blocks until a message returns on this socket from a remote host.
                Byte[] receiveBytes = udpClient.Receive(ref clientEndIP);
                string returnData = Encoding.ASCII.GetString(receiveBytes);

                // Uses the IPEndPoint object to determine which of these two hosts responded.
                Console.WriteLine("Cliente: This is the response you received " +
                                             returnData.ToString());
                MainWindow.frontEnd.android.Dispatcher.Invoke(new Action(() => MainWindow.frontEnd.android.AppendText(returnData.ToString())));

            }  
            catch (Exception e )
            {
                Console.WriteLine(e.ToString());
                AutoClosingMessageBox.Show(e.Message, "Erro ao Enviar Comando", 3000);
            }

        }

        public void androidServer()
        {
            try
            {
                byte[] data = new byte[1024];

                Console.WriteLine("Server: Waiting for a client...");
          

                data = udpServer.Receive(ref serverEndIP);

                Console.WriteLine("Server: Message received from {0}:", serverEndIP.ToString());
                Console.WriteLine(Encoding.ASCII.GetString(data, 0, data.Length));

                string welcome = "Welcome to my test server";
                data = Encoding.ASCII.GetBytes(welcome);
                udpServer.Send(data, data.Length, serverEndIP);

                while (MainWindow.server_running)
                {
                    data = udpServer.Receive(ref serverEndIP);
                    string data_string = Encoding.ASCII.GetString(data, 0, data.Length);

                    string resposta = "recebi: " + data_string;
                    byte[] resposta_data = Encoding.ASCII.GetBytes(resposta);

                    Console.WriteLine("Server: Enviando devolta pro cliente: " + Encoding.ASCII.GetString(resposta_data, 0, resposta_data.Length));
                    udpServer.Send(resposta_data, resposta.Length, serverEndIP);
                    
                    //Puts received stuff on frontend
                    MainWindow.frontEnd.android.Dispatcher.Invoke(new Action(() => MainWindow.frontEnd.android.AppendText(data_string)));
                 
                }

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
           
        }

        public void clientTest()
        {
            Console.WriteLine("Cliente: Cliente inicializado, mandando mensagens de teste.");
            int mensagem = 1;

            Thread.Sleep(1000);

            while(MainWindow.running)
            {
                sendMsg(mensagem++.ToString());
                Thread.Sleep(500);
            }
            
        }

        //Sends one of the string commands. 
        public void sendCommand(string command)
        {
            sendMsg(command);
        }

        public void changeTargetIP(string ip, int port)
        {
            clientEndIP = new IPEndPoint(IPAddress.Parse(ip), port);
        }

        public void closeSockets()
        {
            udpClient.Close();
            udpServer.Close();
        }
    }
    
}
