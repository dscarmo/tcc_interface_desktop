using System;
using System.Threading;
using System.Windows;

namespace InterfaceDrone
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static string up = "up";
        static string down = "down";

        static string decolar = "decolar";
        static string aterrissar = "aterrissar";

        static string rollEsquerda = "rollEsquerda";
        static string rollDireita = "rollDireita";

        static string pitchUp = "pitchUp";
        static string pitchDown = "pitchDown";

        static string yawEsquerda = "yawEsquerda";
        static string yawDireita = "yawDireita";

        internal static MainWindow frontEnd;
        public static volatile bool running = false;
        public static volatile bool server_running = false;
        public static UDP udp; 
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
            udp = new UDP();
            androidIP.Text = udp.default_ip;
            androidPort.Text = UDP.SERVER_PORT.ToString();
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
      
            AutoClosingMessageBox.Show("Loading...", "Performing clean-up tasks...", 1000);
        }

        private void tryChangeIP(string ip, int port)
        {
            try
            {
                if (!running)
                    udp.changeTargetIP(ip, port);
            }
            catch (Exception exception)
            {
                MessageBox.Show(exception.Message);
            }
        }

        private void changeIP_Click(object sender, RoutedEventArgs e)
        {
            tryChangeIP(androidIP.Text, Int32.Parse(androidPort.Text));         
        }

        private void useLocal_button_Click(object sender, RoutedEventArgs e)
        {
            tryChangeIP("127.0.0.1", UDP.SERVER_PORT);
            androidIP.Text = "127.0.0.1";
            androidPort.Text = UDP.SERVER_PORT.ToString(); 
        }

        private void useDefault_button_Copy_Click(object sender, RoutedEventArgs e)
        {
            tryChangeIP(udp.default_ip, UDP.SERVER_PORT);
            androidIP.Text = udp.default_ip;
            androidPort.Text = UDP.SERVER_PORT.ToString();
        }

        #region buttons
        private void upButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(up);
        }

        private void downButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(down);
        }

        private void rollLeftButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(rollEsquerda);
        }

        private void rollRightButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(rollDireita);
        }

        private void pitchUpButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(pitchUp);
        }

        private void pitchDownButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(pitchDown);
        }

        private void yawLeftButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(yawEsquerda);
        }

        private void yawRightButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(yawDireita);
        }

        private void takeOffButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(decolar);
        }

        private void landButton_Click(object sender, RoutedEventArgs e)
        {
            udp.sendCommand(aterrissar);
        }

        #endregion

        private void autoScroll(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            android.ScrollToEnd();
        }
    }
}
