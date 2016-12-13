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

        internal static MainWindow main;
        internal string AndroidDisplay
        {
            get
            {
                return android.Text;
            }

            set
            {
                Dispatcher.Invoke(new Action(() => {
                                                        android.Text = value;
                                                   }
                                             )
                                 );
            }
        }
        public static UDP udp = new UDP();

        public MainWindow()
        {
            InitializeComponent();
            main = this;
        }

        private void windowLoaded(object sender, RoutedEventArgs e)
        {
            //MessageBox.Show("Bem-vindo ao sistema de controle de drone do iVision. Inicie a aplicação ponte android.");
            

            Thread server = new Thread(udp.server);
            server.Start();
 
        }

        private void runApplication(object sender, RoutedEventArgs e)
        {
            Thread client = new Thread(udp.client);
            client.Start();

            //Criar thread pra fazer update do android display igual como fazia nos angulos no kinect
            while (true)
            {
                Console.WriteLine(AndroidDisplay);
                Thread.Sleep(500);
            }
        }
    }
}
