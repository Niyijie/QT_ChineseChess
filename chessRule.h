#ifndef COMMON_H
#define COMMON_H
//棋盘数据定义
#define IMG_OFFSET 20        //图片相对中心的偏移量
#define BOARD_POS(x) (x+2)   //真正棋盘的偏移量
#define RBX 182              //真实x起点
#define RBY 54               //真实y起点
#define BX (182-20)          //棋盘起点x(已经计算了偏移量)
#define BY (54-20)           //棋盘起点y(已经计算了偏移量)
#define STEP_LEN 42          //步长

//棋盘规矩定义
#define INVALID -99  //无效区域
#define CANSET true     //可下位置
#define EMPTY 0      //空区域
#define CHANGETURN(x) (-x)    //切换回合

#define REDTURN 10     //我方回合
#define BLACKTURN -10  //敌方回合
#define RED 10       //红色
#define BLACK -10    //黑色
//黑棋
#define BKING -1
#define BROOK -2
#define BKNIGNT -3
#define BELEPHANT -4
#define BMANDARIN -5
#define BCANNON -6
#define BPAWN -7
//红棋
#define RKING 1
#define RROOK 2
#define RKNIGNT 3
#define RELEPHANT 4
#define RMANDARIN 5
#define RCANNON 6
#define RPAWN 7
//棋子种类
#define KING 1
#define ROOK 2
#define KNIGNT 3
#define ELEPHANT 4
#define MANDARIN 5
#define CANNON 6
#define PAWN 7

#define SINGLEBATTLE 100      //单机对战
#define MACHINEBATTLE 101     //人机对战
#define NETBATTLE 102         //网络对战

#define ASK 11 //询问
#define YES 12 //同意
#define NO 13  //不同意


#endif // COMMON_H
