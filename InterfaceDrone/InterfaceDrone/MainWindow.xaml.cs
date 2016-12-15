using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace InterfaceDrone
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static string up = "up";
        static string down = "down";

        static string rollEsquerda = "rollEsquerda";
        static string rollDireita = "rollDireita";

        static string pitchUp = "pitchUp";
        static string pitchDown = "pitchDown";

        static string yawEsquerda = "yawEsquerda";
        static string yawDireita = "yawDireita";

        internal static MainWindow frontEnd;
        public static volatile bool running = false;
        public static volatile bool server_running = false;
        public static UDP udp = new UDP();

        Thread client = null;
        Thread server = null;

        public MainWindow()
        {
            InitializeComponent();
            frontEnd = this;
        }

        private void windowLoaded(object sender, RoutedEventArgs e)
        {
            //MessageBox.Show("Bem-vindo ao sistema de controle de drone do iVision. Inicie a aplicação ponte android.");

            server_running = true;
            server = new Thread(udp.androidServer);
            server.Start();

        }

        private void runApplication(object sender, RoutedEventArgs e)
        {
            if (!running)
            {
                running = true;
                client = new Thread(udp.clientTest);
                client.Start();
                startButton.Content = "Stop";
            }
            else
            {
                running = false;
                startButton.Content = "Start";
            }

        }

        //Event called when closing window
        private void cleanUp(object sender, System.ComponentModel.CancelEventArgs e)
        {
            running = false;
            server_running = false;
            udp.closeSockets();
        }

        private void changeIP_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
