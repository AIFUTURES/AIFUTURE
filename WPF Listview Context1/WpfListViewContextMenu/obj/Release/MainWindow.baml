   M S B A M L   `   `   `  ����   WpfListViewContextMenu'    !WpfListViewContextMenu.MainWindow   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     @d2http://schemas.microsoft.com/expression/blend/2008     Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006       � Title$  艾期科技��$	��403��$
��1200��.��� .+��� -    ��Name$ 	groupBox1��$�� ��$	��250��$��
35,69,20,0q�$	��Top=�.���  � ColumnDefinitions �� $	��33* ��� $	��90* ��� $
��551* �.+��� -    ��Name$ nutritionlistView�����	#FFDBF3F3���	#FF40C01D$��3��$��0,0,0,2.163q�����  � ContextMenu �� -    ��Name$ ZoneIformationList�� �� 	StaysOpen . 
������� J� BitmapEffect �� .@�m� -    m�Name$  RemoveNutritionContextMenu��$��Remove��	 �� View	 �� -   gridview��.P��� $��账户��
 �� Width$	
 100�� �� DisplayMemberBinding �� *	AccountID+�� $��浮动盈亏��$	
 100�� �� *	TemProfit+�� $��平仓盈亏��$	
 100�� �� *CloseProfit+�� $��成功交易手数��$	
 100�� �� *	DoneNum+�� $��平仓手数��$	
 100�� �� *
DoneDeal+�� $��撤单手数��$	
 100�� �� *	DrawNum+�� $��平均平仓盈利��$	
 100�� �� *	AveProfit+�� $��	总资金��$	
 100�� �� *	Balance+�� $��	手续费��$	
 100�� �� *
Commission+�� $��行情连接��$	
 100�� �� *	Connect+�� $��交易连接��$	
 100�� �� *Trade+�� -    ��Name$ label1��$��AIFUTURE��$	��Top=�$
��Bold"�$��15(�$��Center=�$��0,20,0,0q��� -    ��Name$ closeButton��$��Close��$��23��$��Right=�$��945,324,20,0q�$	��Top=�$��75���� -   $ ClearButton��$��清仓��$��Right=�$��0,324,120,0q�$��75��$��23��$	��Top=��� -	   $ 	AddButton��$��添加��$��Right=�$��0,324,216,0q�$��75��$��23��$	��Top=��� -
   $ StartButton��$��开仓测试��$��Right=�$��0,324,309,0q�$��75��$��23��$	��Top=�