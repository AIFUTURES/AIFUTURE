using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace WpfListViewContextMenu
{
    public partial class MoneyLine: Form
    {
        public int pos;
        public int length;
        List<string> xData;
        List<double> yData;
        public MoneyLine(int p)
        {
            pos = p;
            InitializeComponent();
            xData = new List<string>();
            yData = new List<double>();
        }

        private void Money_Click(object sender, EventArgs e)
        {
            MessageBoxButton messButton = MessageBoxButton.OKCancel;
            MessageBoxResult dr = System.Windows.MessageBox.Show("查询时间约为(查询天数 * 2)秒，且期间无法进行其他操作，是否查询", "查询资金曲线", messButton);
            if (dr == MessageBoxResult.OK)
            {
                string begin = BeginTime.Text;
                string end = EndTime.Text;
                int r = MainWindow.clr.SetMoneyDate(begin, end, pos);
                if(r == 1)
                {
                    MainWindow.clr.PrepareMoney(pos);
                    length = MainWindow.clr.GetMoneyLength(pos);
                    MoneyLineData();
                }
                else
                {
                    System.Windows.MessageBox.Show("日期格式错误，请仔细检查", "错误", MessageBoxButton.OK, MessageBoxImage.Information);
                }
            }
        }
        public void GetData() { }
        public void MoneyLineData()
        {
            chart1.Series[0].Points.Clear();
            xData.Clear();
            yData.Clear();
            double maxy = 0;
            double miny = 100000000;
            for (int i = 0; i <length; i ++)
            {
                xData.Add(MainWindow.clr.GetMoneyDate(pos, i));
                double yd = Convert.ToDouble(MainWindow.clr.GetMoneyValue(pos, i));
                yData.Add(yd);
                if (yd > maxy) maxy = yd;
                if (yd < miny) miny = yd;
            }
            maxy = maxy * 1.2;
            miny = miny * 0.8;
            //线条颜色
            chart1.Series[0].Color = Color.Green;
            //线条粗细
            chart1.Series[0].BorderWidth = 3;
            //标记点边框颜色      
            chart1.Series[0].MarkerBorderColor = Color.Black;
            //标记点边框大小
            chart1.Series[0].MarkerBorderWidth = 1;
            //标记点中心颜色
            chart1.Series[0].MarkerColor = Color.Red;
            //标记点大小
            chart1.Series[0].MarkerSize = 2;
            //标记点类型     
            chart1.Series[0].MarkerStyle = MarkerStyle.Circle;
            //将文字移到外侧
            chart1.Series[0]["PieLabelStyle"] = "Outside";
            //绘制黑色的连线
            chart1.Series[0]["PieLineColor"] = "Black";
            chart1.ChartAreas[0].AxisY.Maximum = maxy;
            chart1.ChartAreas[0].AxisY.Minimum = miny;
            chart1.Series[0].Points.DataBindXY(xData, yData);
        }

    }
}
