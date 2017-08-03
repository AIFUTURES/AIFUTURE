using System.ComponentModel;

namespace WpfListViewContextMenu
{

    public class Order : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private string _Variable;
        public string Variable
        {
            get { return _Variable; }
            set
            {
                _Variable = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Variable"));
                }
            }
        }

        private string _Time;
        public string Time
        {
            get { return _Time; }
            set
            {
                _Time = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Time"));
                }
            }
        }

        private string _TradeVol;
        public string TradeVol
        {
            get { return _TradeVol; }
            set
            {
                _TradeVol = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("TradeVol"));
                }
            }
        }

        private string _TradeType;
        public string TradeType
        {
            get { return _TradeType; }
            set
            {
                _TradeType = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("TradeType"));
                }
            }
        }

        private string _Price;
        public string Price
        {
            get { return _Price; }
            set
            {
                _Price = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Price"));
                }
            }
        }

        private string _Status;
        public string Status
        {
            get { return _Status; }
            set
            {
                _Status = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Status"));
                }
            }
        }

    }
}


//public class Nutrition
//{
//    public string AccountID { get; set; }
//    public string TemProfit { get; set; }
//    public string CloseProfit { get; set; }
//    public string DrawNum { get; set; }
//    public string DoneNum { get; set; }
//    public string AveProfit { get; set; }
//    public string Balance { get; set; }
//    public string CurrMargin { get; set; }
//    public int IDNum { get; set; }
//}

