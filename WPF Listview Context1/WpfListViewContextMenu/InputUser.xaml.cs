using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Xml;

namespace WpfListViewContextMenu
{
    /// <summary>
    /// InputUser.xaml 的交互逻辑
    /// </summary>
    public partial class InputUser : Window
    {
        public static DataTable dt;
        public InputUser()
        {
            InitializeComponent();
            dt = new DataTable();
            GetData();              //读取之前存的数据
            InitializeCombo();
        }
        //初始化用户名输入框
        private void InitializeCombo()
        {
            for (int i = 0; i < dt.Rows.Count; i++)
            {
                user.Items.Add(dt.Rows[i][0]);
            }
            user.SelectedIndex = 0;
            if(user.Items.Count > 1)
            {
                broker.Text = dt.Rows[0][1].ToString();
                ctp.Text = dt.Rows[0][2].ToString();
                ProductInfo.Text = dt.Rows[0][3].ToString();
                AuthCode.Text = dt.Rows[0][4].ToString();
            }
        }

        private void Add(object sender, RoutedEventArgs e)
        {
            Update();
            string Suser = user.Text;
            string Spass = password.Password;
            string sbroker = broker.Text;
            string sctp = ctp.Text;
            string sProductInfo = ProductInfo.Text;
            string sAutheCode = AuthCode.Text;
            MainWindow.clr.AddInvesterID(Suser);
            MainWindow.clr.AddPassWord(Spass);
            MainWindow.clr.AddBrokerID(sbroker);
            MainWindow.clr.AddTradeAddr(sctp);
            MainWindow.clr.AddProductInfo(sProductInfo);
            MainWindow.clr.AddAuthCode(sAutheCode);
            MainWindow.clr.Connect(MainWindow.AccountIDNum);
            MainWindow.AccountIDNum += 1;
            MainWindow.LoadListView();
            this.Close();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void ClearButton(object sender, RoutedEventArgs e)
        {
            user.Text = "";
            password.Password = "";
            broker.Text = "";
            ctp.Text = "";
        }

        public static bool WriteToXml(string address)
        {
            try
            {
                //如果文件DataTable.xml存在则直接删除
                if (File.Exists(address))
                {
                    File.Delete(address);
                }
                XmlTextWriter writer =
                 new XmlTextWriter(address, Encoding.GetEncoding("GBK"));
                writer.Formatting = Formatting.Indented;
                //XML文档创建开始
                writer.WriteStartDocument();
                writer.WriteComment("DataTable: " + dt.TableName);
                writer.WriteStartElement("DataTable"); //DataTable开始
                writer.WriteAttributeString("TableName", dt.TableName);
                writer.WriteAttributeString("CountOfRows", dt.Rows.Count.ToString());
                writer.WriteAttributeString("CountOfColumns", dt.Columns.Count.ToString());
                writer.WriteStartElement("ClomunName", ""); //ColumnName开始
                for (int i = 0; i < dt.Columns.Count; i++)
                {
                    writer.WriteAttributeString(
                     "Column" + i.ToString(), dt.Columns[i].ColumnName);
                }
                writer.WriteEndElement(); //ColumnName结束
                                          //按行各行
                for (int j = 0; j < dt.Rows.Count; j++)
                {
                    writer.WriteStartElement("Row" + j.ToString(), "");
                    //打印各列
                    for (int k = 0; k < dt.Columns.Count; k++)
                    {
                        writer.WriteAttributeString(
                         "Column" + k.ToString(), dt.Rows[j][k].ToString());
                    }
                    writer.WriteEndElement();
                }
                writer.WriteEndElement(); //DataTable结束
                writer.WriteEndDocument();
                writer.Close();
                //XML文档创建结束
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                return false;
            }
            return true;
        }

        public static DataTable ReadFromXml(string address)
        {
            DataTable dtRead = new DataTable();
            if (!File.Exists(address))
            {
                return dtRead;
            }
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(address);
            XmlNode root = xmlDoc.SelectSingleNode("DataTable");
            //读取表名
            dtRead.TableName = ((XmlElement)root).GetAttribute("TableName");
            //Console.WriteLine("读取表名： {0}", dt.TableName);
            //读取行数
            int CountOfRows = 0;
            if (!int.TryParse(((XmlElement)root).
            GetAttribute("CountOfRows").ToString(), out CountOfRows))
            {
                throw new Exception("行数转换失败");
            }
            //读取列数
            int CountOfColumns = 0;
            if (!int.TryParse(((XmlElement)root).
            GetAttribute("CountOfColumns").ToString(), out CountOfColumns))
            {
                throw new Exception("列数转换失败");
            }
            //从第一行中读取记录的列名
            foreach (XmlAttribute xa in root.ChildNodes[0].Attributes)
            {
                dtRead.Columns.Add(xa.Value);
                    //Console.WriteLine("建立列： {0}", xa.Value);
            }
                //从后面的行中读取行信息
            for (int i = 1; i < root.ChildNodes.Count; i++)
            {
                string[] array = new string[root.ChildNodes[0].Attributes.Count];
                for (int j = 0; j < array.Length; j++)
                {
                   array[j] = root.ChildNodes[i].Attributes[j].Value.ToString();
                }
                dtRead.Rows.Add(array);
                //Console.WriteLine("行插入成功");
            }
            return dtRead;
        }


        public static void GetData()
        {
            DataTable DtRead = ReadFromXml("UserRecord.xml");
            for (int i = 0; i < DtRead.Rows.Count; i++)
            {
                for (int j = 0; j < DtRead.Columns.Count; j++)
                {
                    Console.Write(DtRead.Rows[i][j].ToString() + "\t");
                }
                Console.WriteLine();
            }
            if(DtRead.Columns.Count == 0 && dt.Columns.Count == 0)
            {
                dt.Columns.Add("User");
                dt.Columns.Add("Broker");
                dt.Columns.Add("CTP");
                dt.Columns.Add("ProductInfo");
                dt.Columns.Add("AuthCodde");
            }
            else if(DtRead.Columns.Count != 0)
                dt = DtRead;
        }

        public static void SetData()
        {
            WriteToXml("UserRecord.xml");
        }

        private void Update()
        {
            string Suser = user.Text;
            string sbroker = broker.Text;
            string sctp = ctp.Text;
            string sproduct = ProductInfo.Text;
            string sAuthCode = AuthCode.Text;
            for (int i = 0; i < dt.Rows.Count; i++)
            {
                if(dt.Rows[i][0].Equals(Suser) && !Suser.Equals(""))
                {
                    dt.Rows[i][1] = sbroker;
                    dt.Rows[i][2] = sctp;
                    dt.Rows[i][3] = sproduct;
                    dt.Rows[i][4] = sAuthCode;
                    SetData();
                    return;
                }
            }
            dt.Rows.Add(Suser, sbroker, sctp, sproduct, sAuthCode);
            SetData();
        }
        private void DeleteUser(object sender, RoutedEventArgs e)
        {
            MessageBoxButton messButton = MessageBoxButton.OKCancel;
            MessageBoxResult dr = MessageBox.Show("是否删除当前账号的登陆信息?\n下次进入此界面生效", "删除账号", messButton);
            if (dr != MessageBoxResult.OK)
            {
                return;
            }
            string Suser = user.Text;
            for (int i = 0; i < dt.Rows.Count; i++)
            {
                if (dt.Rows[i][0].Equals(Suser) && !Suser.Equals(""))
                {
                    dt.Rows.RemoveAt(i);
                    SetData();
                }
            }
        }

        private void ShowData(object sender, RoutedEventArgs e)
        {
            GetData();
        }

        private void user_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (user.SelectedItem == null) return;
            string Suser = user.SelectedItem.ToString();
            for (int i = 0; i < dt.Rows.Count; i++)
            {
                if (dt.Rows[i][0].Equals(Suser) && !Suser.Equals(""))
                {
                    broker.Text = dt.Rows[i][1].ToString();
                    ctp.Text = dt.Rows[i][2].ToString();
                    ProductInfo.Text = dt.Rows[i][3].ToString();
                    AuthCode.Text = dt.Rows[i][4].ToString();
                }
            }
        }

        private void Grid_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                this.Add(sender, e);
            }
        }
    }
}
