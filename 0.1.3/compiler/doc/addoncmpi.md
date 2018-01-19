外挂编译说明
==========
版本 ver20170209
---------------
______________________________
变更履历
------

| 序号 |     版本    |         描述                                     | 备注 |
| --- | ----------- | ----------------------------------------------- |: --- :|
| 1   | ver20170103 | 文档做成                                         | （无） |
| 2   | ver20170110 | 1.2.2 添加ETA type的含义，解释CardMode和TollMode。 | （无） |
| 3   | ver20170111 | 1.2.1 添加CondType取值含义的说明。                 | （无） |
| 4   | ver20170117 | 1.2.1 更正CondType取值含义的说明。                 | （无） |
| 5   | ver20170209 | 追加2中的part 1 .. part 3。                      |  (无) |
###前言
> 外挂编译程序先以mid文件做输入，生成db文件。再由DB文件生成bin文件。
> bin文件是导航应用或相关程序的目标文件。


###一 格式说明
以下格式说明所有数值的字节序均为little endian。
####1. CR和Toll信息的的bin文件
此bin文件由一个头部(header)和它后面记录序列构成，形式如下。

    头部
    记录 1
    记录 2
    ...  
    记录 n
##### 1.1 bin文件头(header)
文件头(header)从文件的字节偏移量“0”开始，是固定长度的16字节，包含如下两个字段。

1. byte 0..3，共4字节：存储记录序列中包含记录(record)的个数。
* byte 4..7，共4字节：存储记录序列的总大小，单位16字节。
* byte 8..15，共8字节：未使用，用数值“0”填充。

###### 1.2 bin文件记录(record)
记录序列从bin文件的字节偏移量“16”开始，每一个记录的大小是16的整数倍。


* 每个记录记述了一对in/out link的四维link ID的CR或Toll信息，也就是说一个记录至少包含一个CR信息或Toll(收费站)信息。

###### 1.2.1 CR和Toll（收费站）记录的头部。
* CR和Toll记录(record)的联合头部(header)，固定的16字节。主要记录了通行条件的进入/脱出link等信息信息，主要是从C表中取得的。
	1. byte 0..4，共5字节。禁行的进入link ID，此ID为四维link ID。
	+ byte 5..9，共5字节。禁行的脱出link ID，此ID为四维link ID。
	+ byte 10，共1字节。
		- bit 0..3, 4 bits. CondType. 其取值含义如下， 
			* value 0 禁止驶入
			* value 1 顺行
			* value 2 收费站
			* value 3 大门
			* value 4 分叉口提示
			* value 5 交叉信号灯
			* value 6 坡度
			* value 7 限速标牌
		- bit 4..7, 4 bits. CR个数。
	+ byte 11，共1字节。
		- bit 0，共1 bit。该记录中Toll ETA信息有无标志位。
			* 0，表示该记录没有Toll ETA信息。
			* 1，表示该记录存在Toll ETA信息。
		- bit 1，共1 bit。该记录中Toll Pattern信息有无标志位。
			* 0，表示该记录没有Toll Pattern信息。
			* 1，表示该记录存在Toll Pattern信息。
		- bit 2..7，共6 bits。未使用，用数值“0”填充。
	+ byte 12..15，共4字节。未使用，用数值“0”填充。
	
######1.2.2 收费站（Toll）信息
收费站信息包含Toll ETA和Toll Pattern信息。

1. Toll ETA 信息

	若record header中Toll ETA的标志位为“1”，则该字段存在。从record header结束位置后偏移量为“0”字节的位置开始，为固定长度的16字节，其中各字段含义如下。
	+ byte 0，共1字节，包含如下两个字段。
		- bit 0..3，共4个bits，取值范围0..15。为ETA的type（数值含义同四维数据），这里列出数值的含义如下。
			* value 0 未调查
			* value 1 领卡
			* value 2 交卡付费
			* value 3 固定收费（次费）
			* value 4 交卡付费后再领卡
			* value 5 交卡付费并代收固定费用
			* value 6 验票
			* value 7 领卡并代收固定费用
			* value 8 持卡打标识不收费
			* value 9 验票领卡
			* value 10 交卡不收费
			* value 11..15 （未使用）
		- bit 4..7，共4个bits。为车线信息的数目，取值范围0..15。
	+ byte 1..15，共15字节。表示车线信息1..15，每个车线信息占1字节。这里所存储的所有车线信息，要么都是Toll Mode，要么都是CardMode。
		- 当ETA的type取值为1时，每个车线信息存储的是CardMode。
		
			| 值 | 描述   |
			| -- | ----- | 
			| 0  | 未调查 |
			| 1  | ETC   |
			| 2  | 人工   | 
			| 3  | 自助   |
			
		- 当ETA的type取值为2、3、4、6时，每个车线信息存储的是TollMode。
		
			|     值   |      描述       |
			| -------- | -------------- |
			| 0000000 | 未调查           |
			| 0000001 | ETC             |
			| 0000010 | 现金             |
			| 0000100 | 银行卡（借记卡）   |
			| 0001000 | 信用卡           |
			| 0010000 | IC卡             |
			| 0100000 | 预付卡（年票、月票）|
			| 1000000 | 预留             |
			
		__注__ ：除以上列出的取值外，ETA type的其它取值在北京的实际数据中暂未遇到。
		
2. Toll Pattern信息

	若record header中Toll Pattern的标志位为“1”，则该字段存在。从Toll ETA信息结束位置后偏移量为“0”字节的位置开始，为固定长度的16字节，其中各字段含义如下。
	+ byte 0..3，共4字节。记录Toll Pattern图片的编号。
	+ byte 4..7，共4字节。记录Toll Arrow图片的编号。
	+ byte 8..15，共8字节。未使用，用数值“0”填充。
	
######1.2.3 CR信息
1. CR数据记录的序列。CR数据记录序列从收费站信息结束后的偏移量为“0”字节的位置开始，包含的记录个数为record header中的CR个数。序列中每个记录的长度为固定长度的16字节，其中各字段含义如下。
	+ byte 0, 1 byte.
		- bit 0..1, 2 bits : VPDir
		- bit 2..3, 2 bits : VPAproxy
		- bit 4..7, 4 bits : VPeriod type
	+ byte 1..5, 5 bytes. 保留字段。
	+ byte 6..11, 6 bytes. VPeriod，用于记录禁行时间。
	+ byte 12..15, 4 bytes. Vehicle type，禁行车辆的位(bit)集，32 bits。

* CR数据记录的序列。


####2. Highway Junction的bin文件
Highway Junction的bin文件没有设计一个header部分，单纯的由若干个定长为24字节的记录构成。每个记录分3部分，每个部分的含义如下。

* part 1 : 8 bytes.
	+ bit 00..39, 共40 bits : HW Junction的四维ID。
	+ bit 40..47, 共8 bits : not defined.
	+ bit 48..51, 共4 bits : access type.详情待定。
	+ bit 52..55, 共4 bits : attribute.详情待定。
	+ bit 56..63, 共8 bits : 记录HW junction的固定设施.
		1. bit 56 : 餐馆标志位
			- value 1, restaurant. 
			- value 0, no restaurant available.
		2. bit 57 : 商店标志位
			- value 1, shop. 
			- value 0, no shop available.
		3. bit 58 : 旅店标志位
			- value 1, inn. 
			- value 0, no inn available.
		4. bit 59 : 公厕标志位
			- value 1, public toilet. 
			- value 0, no public toilet available.
		5. bit 60..63 : Natural number, the gasoline station. 值域[0, 15]。
			* value 0 : not defined.
			* value 1 : PetreChina.
			* value 2 : sinopec.
			* value 3 : shell.
			* value 4 : Mobil.
			* value 5 : Brithish Petroleum.
			* value 6..14 : not defined.
			* value 15 : other gasoline station.
* part 2 : 6 bytes. 记录HW junction的（四维）Node ID。
	+ bit 00..39, 共40 bits : NodeID 
	+ bit 40..63, 共16 bits : not defined.
* part 3 : 10 bytes. 记录进入/脱出link ID。
	1. byte 0..4, 共5个字节: 进入HW junction的四维link ID。
	2. byte 5..9, 共5个字节: 脱出HW junction的四维link ID。

###二 编译工具
####1. 运行环境
Ubuntu 16.04
####2. 使用说明
可执行程序文件名为

> addonc

#####2.1 mid文件转db文件

如果文件名为 \*.mid ，则生成 \*.db。
######2.1.1 用于转化的输入、输出文件名约定
1. 输入文件与输出文件的主文件名做如下约定。
	+ 以C、CR、Toll_ETA、Toll_Pattern开头；
	+ 以beijing、tianjing、liaoning等省、直辖市名的汉语拼音结尾；  
	例如：  
	__Cbeijing.mid__ 为合法的输入文件名，  
	__Cliaoning.db__ 为合法的输出文件名。
2. 输入文件与输出文件的扩展名约定。
	+ 输入文件的扩展名为mid，且大小写敏感。  
	例如：  
	__CRbeijing.mid__ 为合法文件名，  
	~~CRtianjin.Mid~~ 为非法文件名。
	+ 输出文件的扩展名为db，且大小写敏感。  
	例如：

#####2.2 db文件转bin文件

如果文件名为 \*.db ，则生成 \*.bin。

