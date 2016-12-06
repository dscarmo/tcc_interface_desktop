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
        public MainWindow()
        {
            InitializeComponent();
        }

        private void windowLoaded(object sender, RoutedEventArgs e)
        {
            //MessageBox.Show("Bem-vindo ao sistema de controle de drone do iVision.");
            UDP udp = new UDP();

            Thread server = new Thread(udp.server);
            server.Start();

            Thread client = new Thread(udp.client);
            client.Start();
        }
    }
}
