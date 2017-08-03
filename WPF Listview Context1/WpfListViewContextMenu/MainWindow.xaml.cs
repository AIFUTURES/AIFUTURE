
using System.Collections.ObjectModel;
using System.Threading;
using System.Windows;
using Microsoft.VisualBasic;
using System.Data.SqlClient;
namespace WpfListViewContextMenu
{

    public partial class MainWindow : Window
    {
        static public ObservableCollection<Nutrition> nutritions;
        public Window1 w1;
        public InputUser InputWindow;       //输入新账户的窗口
        static public int accountnum;       //账户数量
        static public clrClass clr;         //c++接口类
        static public int AccountIDNum;     //账户的数量
        static public int DeleteNum;        //删除的账户的数量
        Thread t1 = new Thread(new ThreadStart(Update));
        static object locker = new object();
        public MainWindow()
        {
            InitializeComponent();
            InitialListView();
            clr = new clrClass();  //新建一个托管类对象
            clr.initialize();
            clr.prepare();
            t1.Start();
            t1.IsBackground = true;
            TestButton.Visibility = (System.Windows.Visibility)1;
        }

        static private void UpdateItem()
        {
            lock (locker)
            {
                for (int i = 0; i < accountnum + 1 - DeleteNum; i++)
                {
                    Nutrition temnutrition = nutritions[i];
                    int pos = temnutrition.IDNum;
                    temnutrition.AccountID = clr.AccountName(pos);
                    temnutrition.TemProfit = clr.GetPositionProfit(pos).ToString("0.00");
                    temnutrition.CloseProfit = clr.GetCloseProfit(pos).ToString("0.00");
                    temnutrition.DrawNum = clr.GetUnDoneNum(pos).ToString();
                    temnutrition.DoneNum = clr.GetDoneNum(pos).ToString()+" / "+ clr.GetUnDoneNum(pos).ToString();
                    temnutrition.AveProfit = clr.GetAveWin(pos).ToString("0.00");
                    temnutrition.Balance = clr.GetBalance(pos).ToString("0.00");
                    temnutrition.CurrMargin = clr.GetMargin(pos).ToString("0.00");
                    temnutrition.Commission = clr.GetCommission(pos).ToString("0.00");
                    temnutrition.DoneDeal = clr.GetDoneDeal(pos).ToString();
                    temnutrition.Connect = clr.GetConnectFlag() == 0 ? "未连接" : "已连接";
                    temnutrition.Trade = clr.GetTrade(pos) == 1 ? "已连接" : "";
                }
            }
        }

        public static void Update()
        {
            while (true)
            {
                clr.update();
                UpdateItem();
                clr.Check();
                System.Threading.Thread.Sleep(1000);
            }
        }
        private void InitialListView()
        {
            AccountIDNum = 0;
            DeleteNum = 0;
            nutritions = new ObservableCollection<Nutrition>();
            nutritions.Clear();
            Nutrition nutrition = new Nutrition();
            nutrition.IDNum = -1;
            nutrition.AccountID = "汇总账户";
            nutritions.Add(nutrition);
            nutritionlistView.ItemsSource = nutritions;
            accountnum = 0;
        }
        static public void LoadListView()
        {
            lock (locker)
            {
                int temaccountnum = clr.AccountNum();
                if (temaccountnum > accountnum)
                {
                    for (int i = accountnum; i < temaccountnum; i++)
                    {
                        Nutrition temnutrition = new Nutrition();
                        temnutrition.IDNum = i;
                        temnutrition.Delete = 0;
                        temnutrition.AccountID = clr.AccountName(i);
                        nutritions.Add(temnutrition);
                    }
                    accountnum = temaccountnum;
                }
            }
        }


        private void RemoveNutritionContextMenu_OnClick(object sender, RoutedEventArgs e)
        {
            lock (locker)
            {
                Nutrition temn = (Nutrition)nutritionlistView.SelectedItem;
                MessageBoxButton messButton = MessageBoxButton.OKCancel;
                MessageBoxResult dr = MessageBox.Show("是否移除账号：" + temn.AccountID, "移除账户", messButton);
                if (dr == MessageBoxResult.OK)
                {
                    clr.CloseAccount(temn.IDNum);
                    nutritions.Remove(temn);
                    DeleteNum++;
                }
            }
            
        }
        private void ClearMenu_OnClick(object sender, RoutedEventArgs e)
        {
            Nutrition temn = (Nutrition)nutritionlistView.SelectedItem;
            int IDNum = temn.IDNum;
            string IDAccount = temn.AccountID;
            MessageBoxButton messButton = MessageBoxButton.OKCancel;
            MessageBoxResult dr = MessageBox.Show("是否清仓，账号："+IDAccount, "清仓", messButton);
            if (dr == MessageBoxResult.OK)
            {
                MessageBoxButton messButton2 = MessageBoxButton.OKCancel;
                MessageBoxResult dr2 = MessageBox.Show("真的确定吗", "确认", messButton2);
                if (dr2 == MessageBoxResult.OK)
                {
                    clr.ClearAccount(IDNum);
                }
            }
        }

        private void closeButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void nutritionlistView_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            Nutrition temn = (Nutrition)nutritionlistView.SelectedItem;
            if (temn == null) return;
            int pos = temn.IDNum;
            w1 = new Window1(pos);
            w1.Title = "账户：" + temn.AccountID;
            w1.Show();

            //string s = clr.returnmoney().ToString();
            // MessageBox.Show("You are double click Name:" +s, "Nutrition", MessageBoxButton.OK, MessageBoxImage.Information);
        }

        private void ClearButton_click(object sender, RoutedEventArgs e)
        {
            string str = Interaction.InputBox("清除所有账户仓位，请输入清仓密码", "清仓操作", "", 50, 50);
            if (str.Equals("2727"))
            {
                MessageBox.Show("进行清仓操作，请留意持仓情况", "Nutrition", MessageBoxButton.OK, MessageBoxImage.Information);
                clr.EClear();
            }
        }

        private void AddButtonClick(object sender, RoutedEventArgs e)
        {
            InputWindow = new InputUser();
            InputWindow.Show();
        }

        private void StartButtonClick(object sender, RoutedEventArgs e)
        {
            MessageBoxButton messButton = MessageBoxButton.OKCancel;
            MessageBoxResult dr = MessageBox.Show("此功能会在所有账户开仓，若有实盘账户，请取消\n是否开仓？", "开仓测试", messButton);
            if(dr == MessageBoxResult.OK)
            {
                clr.OpenTest();
            }
            
        }
    }
}
