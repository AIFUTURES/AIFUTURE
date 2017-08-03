using System.Collections.ObjectModel;
using System.Threading;
using System.Windows;
using System.Drawing;
using System.Windows.Controls;

namespace WpfListViewContextMenu
{
    /// <summary>
    /// Window1.xaml 的交互逻辑
    /// </summary>
    public partial class Window1 : Window
    {
        static public ObservableCollection<Nutrition> variables;
        static public ObservableCollection<Order> orders;
        Thread t = new Thread(new ThreadStart(UpdateVari));
        
        static public int VariNum;
        static public int OrderNum;
        static public int pos;
        public Window1(int p)
        {
            InitializeComponent();
            VariNum =  MainWindow.clr.VariNum(p);
            OrderNum = MainWindow.clr.GetOrderLength(p);
            pos = p;
            //MessageBox.Show(pos.ToString() + "   " + VariNum, "Nutrition", MessageBoxButton.OK, MessageBoxImage.Information);
            loadvariable();
            loadRecord();
            t.Start();
            t.IsBackground = true;
            if(p == -1) MoneyLine.Visibility = (System.Windows.Visibility)1;

        }
        //初始化品种信息
        public void loadvariable()
        {
            variables = new ObservableCollection<Nutrition>();
            for (int i = 0; i < VariNum; i++)
            {
                Nutrition temnutrition = new Nutrition();
                temnutrition.IDNum = i;
                temnutrition.AccountID = "";
                temnutrition.TemProfit = "";
                temnutrition.CloseProfit = "";
                temnutrition.Position = "";
                variables.Add(temnutrition);
            }
            this.listvari.ItemsSource = variables;
        }

        public void loadRecord()
        {
            orders = new ObservableCollection<Order>();
            for (int i = 0; i < OrderNum; i++)
            {
                Order temorder = new Order();
                temorder.Variable = "";
                temorder.Time = "";
                temorder.Price = "";
                temorder.TradeType = "";
                temorder.TradeVol = "";
                temorder.Status = "";
                orders.Add(temorder);
            }
            this.listrecord.ItemsSource = orders;
        }

        static public void UpdateRecord()
        {
            int i = 0;
            for (int j = 0; j < OrderNum; j++)
            {
                Order temorder = orders[j];
                i = OrderNum - j - 1;               //实现倒序输出
                temorder.Variable = MainWindow.clr.GetOrderVari(pos, i); ;
                temorder.Time = MainWindow.clr.GetOrderTime(pos, i);
                temorder.Price = MainWindow.clr.GetOrderPrice(pos, i).ToString();
                temorder.TradeType = MainWindow.clr.GetOrderType(pos, i);
                temorder.TradeVol = MainWindow.clr.GetOrderVol(pos, i).ToString();
                temorder.Status = MainWindow.clr.GetOrderStatu(pos, i);
            }
        }
        static public void UpdateItem()
        {
            
            for (int i = 0; i < VariNum; i++)
            {
                Nutrition temnutrition = variables[i];
                int j = temnutrition.IDNum;
                temnutrition.AccountID = MainWindow.clr.GetVariID(pos, j);
                temnutrition.TemProfit = MainWindow.clr.GetVariPositionProfit(pos,j).ToString("0.00");
                temnutrition.CloseProfit = MainWindow.clr.GetVariCloseProfit(pos,j).ToString("0.00");
                temnutrition.Position = MainWindow.clr.GetVariPosition(pos, j).ToString();
                temnutrition.AllOpen = MainWindow.clr.GetVariAllOpen(pos, j).ToString();
                temnutrition.Commission = MainWindow.clr.GetVariCommission(pos, j).ToString("0.00");

            }
        }
        static void UpdateVari()
        {
            while (true)
            {
                UpdateItem();
                UpdateRecord();
                System.Threading.Thread.Sleep(2000);
            }
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        public void Refresh()
        {
            int TemVariNum = MainWindow.clr.VariNum(pos);
            if (TemVariNum > VariNum)
            {
                for (int i = VariNum; i < TemVariNum; i++)
                {
                    Nutrition temnutrition = new Nutrition();
                    temnutrition.IDNum = i;
                    temnutrition.AccountID = "";
                    temnutrition.TemProfit = "";
                    temnutrition.CloseProfit = "";
                    temnutrition.Position = "";
                    variables.Add(temnutrition);
                }
                VariNum = TemVariNum;
            }

            int TemOrderNum = MainWindow.clr.GetOrderLength(pos);
            if (TemOrderNum > OrderNum)
            {
                for (int i = 0; i < TemOrderNum - OrderNum; i++)
                {
                    Order temorder = new Order();
                    temorder.Variable = "";
                    temorder.Time = "";
                    temorder.Price = "";
                    temorder.TradeType = "";
                    temorder.TradeVol = "";
                    orders.Add(temorder);
                }
                OrderNum = TemOrderNum;
            }
        }
        private void ClearMenu_OnClick(object sender, RoutedEventArgs e)
        {
            Nutrition temn = (Nutrition)listvari.SelectedItem;
            int IDNum = temn.IDNum;
            string IDAccount = temn.AccountID;
            MessageBoxButton messButton = MessageBoxButton.OKCancel;
            MessageBoxResult dr = MessageBox.Show("是否清仓品种:"+ IDAccount, "清仓", messButton);

            if (dr == MessageBoxResult.OK)
            {
                MessageBoxButton messButton2 = MessageBoxButton.OKCancel;
                MessageBoxResult dr2 = MessageBox.Show("真的确定吗", "确认", messButton2);
                if(dr2 == MessageBoxResult.OK)
                {
                    //清仓特定品种
                    MainWindow.clr.ClearVariable(pos, IDNum);
                }
            }
        }

        private void button_Fresh(object sender, RoutedEventArgs e)
        {
            Refresh();
        }

        private void MoneyLineClick(object sender, RoutedEventArgs e)
        {
            //MainWindow.clr.PrepareMoney(pos);
            MoneyLine m = new MoneyLine(pos);
            m.Show();
        }
    }
}
