   M S B A M L   `   `   `  ���� C  ?AIFUTURE, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null?$clr-namespace:WpfListViewContextMenuWpfListViewContextMenu  $    WpfListViewContextMenu.Window1   R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     @d2http://schemas.microsoft.com/expression/blend/2008     Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     6local$clr-namespace:WpfListViewContextMenu       � Title$  Window1��$	��600��$	��750��.��� .+��� -    ��Name$ listvari��$��46,55,0,314q�$��Center=�$
��Left=�$	��650�� � ContextMenu �� -    ��Name$ ZoneIformationList�� �� 	StaysOpen . 
������� J� BitmapEffect �� .@�m� -    m�Name$ 	ClearMenu��$��清仓当前品种�� �� View �� .P��� $��品种�� �� Width$	 100��	 �� DisplayMemberBinding	 �� *	AccountID+�� $��浮动盈亏��$	 100��	 �� *	TemProfit+�� $��平仓盈亏��$	 100��	 �� *CloseProfit+�� $��持仓��$	 100��	 �� *
Position+�� $��今日开仓总量��$	 100��	 �� *	AllOpen+�� $��	手续费��$	 100��	 �� *
Commission+�� -   
 ��Name$
 button��$
��Exit��$��Right=�$��	0,0,46,10q�$��Bottom=�$��75���� -   $
 buttonFresh��$��刷新��$��Right=�$��
0,0,46,280q�$��Bottom=�$��75���� -   $ 
listrecord��$��46,300,0,69q�$
��Left=�$	��650��$��Center=� �� .P��� $��品种��$	 100��	 �� *
Variable+�� $��成交时间��$	 100��	 �� *Time+�� $��	成交量��$	 100��	 �� *
TradeVol+�� $��价格��$	 100��	 �� *Price+�� $��开/平��$	 100��	 �� *	TradeType+�� $��状态��$	 100��	 �� *Status+�� $��持仓情况��$��
346,25,0,0q�$	��Top=�$
��Left=��� $��报单成交��$��346,270,0,0q�$	��Top=�$
��Left=��� -   $
 	MoneyLine��$��	资金线��$��Right=�$��0,0,139,280q�$��Bottom=�$��75��