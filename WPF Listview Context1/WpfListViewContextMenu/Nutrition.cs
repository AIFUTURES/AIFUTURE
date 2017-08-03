using System.ComponentModel;

namespace WpfListViewContextMenu
{

    public class Nutrition : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private int _Delete;
        public int Delete
        {
            get { return _Delete; }
            set
            {
                _Delete = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Delete"));
                }
            }
        }

        private string _AccountID;
        public string AccountID
        {
            get { return _AccountID; }
            set
            {
                _AccountID = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("AccountID"));
                }
            }
        }
        private string _TemProfit;
        public string TemProfit
        {
            get { return _TemProfit; }
            set
            {
                _TemProfit = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("TemProfit"));
                }
            }
        }
        private string _CloseProfit;
        public string CloseProfit
        {
            get { return _CloseProfit; }
            set
            {
                _CloseProfit = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("CloseProfit"));
                }
            }
        }
        private string _DrawNum;
        public string DrawNum
        {
            get { return _DrawNum; }
            set
            {
                _DrawNum = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("DrawNum"));
                }
            }
        }
        private string _DoneNum;
        public string DoneNum
        {
            get { return _DoneNum; }
            set
            {
                _DoneNum = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("DoneNum"));
                }
            }
        }
        private string _AveProfit;
        public string AveProfit
        {
            get { return _AveProfit; }
            set
            {
                _AveProfit = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("AveProfit"));
                }
            }
        }
        private string _Balance;
        public string Balance
        {
            get { return _Balance; }
            set
            {
                _Balance = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Balance"));
                }
            }
        }

        private string _CurrMargin;
        public string CurrMargin
        {
            get { return _CurrMargin; }
            set
            {
                _CurrMargin = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("CurrMargin"));
                }
            }
        }

        private int _IDNum;
        public int IDNum
        {
            get { return _IDNum; }
            set
            {
                _IDNum = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("IDNum"));
                }
            }
        }

        private string _Position;
        public string Position
        {
            get { return _Position; }
            set
            {
                _Position = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Position"));
                }
            }
        }

        private string _DoneDeal;
        public string DoneDeal
        {
            get { return _DoneDeal; }
            set
            {
                _DoneDeal = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("DoneDeal"));
                }
            }
        }

        private string _AllOpen;
        public string AllOpen
        {
            get { return _AllOpen; }
            set
            {
                _AllOpen = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("AllOpen"));
                }
            }
        }

        private string _Commission;
        public string Commission
        {
            get { return _Commission; }
            set
            {
                _Commission = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Commission"));
                }
            }
        }

        private string _Connect;
        public string Connect
        {
            get { return _Connect; }
            set
            {
                _Connect = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Connect"));
                }
            }
        }
        private string _Trade;
        public string Trade
        {
            get { return _Trade; }
            set
            {
                _Trade = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Trade"));
                }
            }
        }

    }
}


