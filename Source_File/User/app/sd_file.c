
#include "sd_file.h"
#include <string.h>
#include <stdlib.h>

extern uint8_t ReceiveBuff[RECEIVEBUFF_SIZE];
extern uint8_t SendBuff[SENDBUFF_SIZE];

uint8_t write_flag=0;
char File_Receive_Name[200];  //文件名
void test(void);

//文件系统相关变量
FRESULT res;
DIR dir;
FILINFO  fno;
FIL fil;
UINT fnumber;

uint16_t  file_num; 

CUR_FILE  Cur_File;

uint16_t   NestDepth=0;  //文件夹嵌套深度

uint8_t   cur_env=0;    //当前所处环境是文件夹还是文件内部

Touch_Button  button[BUTTON_NUM];   //按键数组
char File_Name[BUTTON_NUM][FILE_NAME_LENGTH];  //文件名数组
uint8_t  IS_DIR[BUTTON_NUM];  //是否是文件夹数组
char CurPath[256];   //当前路径
  
BYTE file_read_buffer[5000];  //读取文件内容数组

char cur_page_read_buffer[FILE_PAGE_MAX_BTYE];
uint16_t  File_Pages[MAX_NUM_PAGE];
uint8_t  Total_Txt_Pages;

int8_t num_read_page = 0; // 文件内容的页数
int8_t file_num_read_page = 0; // 文件的页数
/****************************************************************/
static void Draw_File_Button(void *btn);     
static void Draw_Function_Button(void *btn); 
static void Draw_NextPage_Button(void *btn); // 绘制下一页按键 
static void Draw_PrePage_Button(void *btn); // 绘制下一页按键 

static void Command_Select_File(void *btn);
static void Command_Select_Function(void *btn);
static void Command_Select_NextPage(void *btn); //下一页功能选择
static void Command_Select_PrePage(void *btn); //上一页功能选择

static void File_Button_Disable(void);
//static void File_Button_Enable(void);
//static void Function_Button_Disable(void);
static void Fuction_Button_Enable(void);

static void File_Disable(void);
// 使能/失能下一页功能在 Function_Button_Dis/Enable中实现
// 注：下一页始终使能
//static void NextPage_Button_Enable(void); // 使能下一页功能选择
//static void NextPage_Button_Disable(void); // 失能下一页功能选择
static void File_Page_Init(void); //初始化文件内容每一页页数


/******************************************************************/

static void Open_File(char *path);

static uint16_t  Last_Slash(char *path);   //最后一个/的位置

/**
* @brief  Touch_Button_Init 初始化按钮参数
* @param  无
* @retval 无
*/
void Touch_Button_Init(void){
	uint16_t  i;
	uint16_t file_current_x=FILE_BUTTON_START_X ;
	uint16_t file_current_y=FILE_BUTTON_START_Y;
	
    uint8_t start_position = FUNCTION_BUTTON_NUM + file_num_read_page * NUM_FILE_ONE_PAGE;
    uint8_t end_position = FUNCTION_BUTTON_NUM + (file_num_read_page + 1) * NUM_FILE_ONE_PAGE;
    
	//传输功能与打开文件或目录功能用按键来实现
	//第一个按钮为固定按钮，不随文件的改动而变化
	//功能为退出文件显示，或返回上级目录
    button[0].start_x = FUNCTION_BUTTON_START_X;
    button[0].start_y = FUNCTION_BUTTON_START_Y;
    button[0].end_x = FUNCTION_BUTTON_START_X + FUNCTION_BLOCK_WIDTH ;
    button[0].end_y = FUNCTION_BUTTON_END_Y;
    button[0].para = CL_GREY;
	if(NestDepth > 0){
	     button[0].en=1;   // 退出按键使能
	}
	else{
		 button[0].en=0;
	}
    button[0].touch_flag = 0;  
    button[0].draw_btn = Draw_Function_Button;
    button[0].btn_command = Command_Select_Function ; 

    // 1号按键 ： 下一页
    button[1].start_x = FUNCTION_BUTTON_START_X + 2 * FUNCTION_BLOCK_WIDTH;
    button[1].start_y = FUNCTION_BUTTON_START_Y;
    button[1].end_x = FUNCTION_BUTTON_START_X + 3 * FUNCTION_BLOCK_WIDTH ;
    button[1].end_y = FUNCTION_BUTTON_END_Y;
    button[1].para = CL_GREY;
    button[1].en = 1;
    button[1].touch_flag = 0;  
    button[1].draw_btn = Draw_NextPage_Button;
    button[1].btn_command = Command_Select_NextPage ;  
    // 结束按键1定义

    // 2号按键 ： 上一页
    button[2].start_x = FUNCTION_BUTTON_START_X + FUNCTION_BLOCK_WIDTH;
    button[2].start_y = FUNCTION_BUTTON_START_Y;
    button[2].end_x = FUNCTION_BUTTON_START_X + 2 * FUNCTION_BLOCK_WIDTH ;
    button[2].end_y = FUNCTION_BUTTON_END_Y;
    button[2].para = CL_GREY;
    button[2].en = 1;
    button[2].touch_flag = 0;  
    button[2].draw_btn = Draw_PrePage_Button;
    button[2].btn_command = Command_Select_PrePage;  
    // 结束按键2定义
    
	//文件显示模块
    File_Disable(); // disable 无关文件
	for(i = start_position; i < end_position; i++){
		button[i].start_x = file_current_x;
        button[i].start_y = file_current_y;
        button[i].end_x = file_current_x+ FILE_BLOCK_WIDTH;
        button[i].end_y = file_current_y+FILE_BLOCK_HEIGHT;
		if (IS_DIR[i-FUNCTION_BUTTON_NUM]) { //判断是否是目录
			button[i].para = CL_ORANGE;
			button[i].is_dir=1;
		} else {
            button[i].para = CL_GREEN;
			button[i].is_dir=0;
		}
		button[i].en=1;
        button[i].touch_flag = 0;  
		strcpy(button[i].fname,File_Name[i-FUNCTION_BUTTON_NUM]);
        button[i].draw_btn = Draw_File_Button;
        button[i].btn_command = Command_Select_File;  
		
		//防止溢出
		file_current_x += FILE_BLOCK_WIDTH;
		if (file_current_x >= FILE_BUTTON_END_X) {
			file_current_x =FILE_BUTTON_START_X;
			file_current_y +=FILE_BLOCK_HEIGHT;
		}
	}
	
	
}

/**
* @brief  Buttuon_Fresh  刷新按钮，与文件系统保持一致
* @param  无
* @retval 无
*/
void Button_Fresh(char *path){
	 uint8_t i;
    
	 find_sd_file(path);
	 LCD_SetBackColor(CL_WHITE);
	 ILI9341_Clear (0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
	
	 Touch_Button_Init();
	 
     // 刷新显示
	 for (i = 0; i < FUNCTION_BUTTON_NUM + file_num; i++){
        if(button[i].en) {
            button[i].draw_btn(&button[i]);
        }   
     }
     
     
    
    
          
     Cur_File.CurrentFileName=NULL;
     if (strcmp(CurPath,path) != 0) {
         memset(CurPath, '\0', sizeof(CurPath));
         strcpy(CurPath,path);
     }
     cur_env=0;
}

/**
* @brief  Draw_File_Button  描绘显示文件的按钮 
* @param  无
* @retval 无
*/
static void Draw_File_Button(void *btn){
	Touch_Button *ptr = (Touch_Button *)btn;
	uint32_t  temp_color;
	 if(ptr->touch_flag==0){      //按键未被选中
		 LCD_SetColors(ptr->para,CL_WHITE);
		 temp_color=ptr->para;
     ILI9341_DrawRectangle(	ptr->start_x,
							ptr->start_y,
							ptr->end_x - ptr->start_x,
							ptr->end_y - ptr->start_y,1);
	 }
	 else{    //按键被选中
		 LCD_SetColors(CL_WHITE,CL_WHITE);
		 temp_color=CL_WHITE;
		 ILI9341_DrawRectangle(	ptr->start_x,
								ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,1);
	 }
	 
	 //显示文件名
	 LCD_SetColors(CL_BLACK,temp_color);
	 LCD_SetFont(&Font8x16);
	 ILI9341_DispString_EN_CH_Custom(ptr->start_x,ptr->start_y,ptr->end_x,ptr->end_y,ptr->fname);
	 
	 //描绘边框
     ILI9341_DrawRectangle(	ptr->start_x,
															ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,0);
	 
}

/**
* @brief  Draw_Function_Button  描绘返回按钮 
* @param  无
* @retval 无
*/
static void Draw_Function_Button(void *btn){
	uint32_t temp_color;
	Touch_Button *ptr = (Touch_Button *)btn;
	
	 if(ptr->touch_flag==0){      //按键未被选中
		 LCD_SetColors(ptr->para,CL_WHITE);
		 temp_color=ptr->para;
        ILI9341_DrawRectangle(	ptr->start_x,
															ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,1);
	 }
	 else{    //按键被选中
		 LCD_SetColors(CL_GREY3	,CL_WHITE);
		 temp_color=CL_GREY3;
		 ILI9341_DrawRectangle(	ptr->start_x,
															ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,1);
	 }
	 
	 //显示功能名
	 LCD_SetColors(CL_RED,temp_color);
	 LCD_SetFont(&Font8x16);
	 ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16*2 )/2,                 
							   ptr->start_y + ((ptr->end_y - ptr->start_y-16)/2),	
							   "返回");
	 
	 //描绘边框
     ILI9341_DrawRectangle(	ptr->start_x,
							ptr->start_y,
							ptr->end_x - ptr->start_x,
							ptr->end_y - ptr->start_y,0);
}

/**
* @brief  Draw_NextPage_Button  描绘选择下一页的按钮 
* @param  无
* @retval 无
*/
static void Draw_NextPage_Button(void *btn) { 
    uint32_t temp_color;
	Touch_Button *ptr = (Touch_Button *)btn;
   
    if (ptr->touch_flag == 0) {      //按键未被选中   
		LCD_SetColors(ptr->para,CL_WHITE);
		temp_color=ptr->para;
        ILI9341_DrawRectangle(ptr->start_x, ptr->start_y,
                              ptr->end_x - ptr->start_x, 
                              ptr->end_y - ptr->start_y, 1);

	} else {  // 下一页按键被选中   
        LCD_SetColors(CL_GREY3, CL_WHITE);
		temp_color = CL_GREY3;
		ILI9341_DrawRectangle(ptr->start_x, 
							  ptr->start_y, 
							  ptr->end_x - ptr->start_x,
							  ptr->end_y - ptr->start_y, 1);
    }
     //显示功能名
	 LCD_SetColors(CL_RED,temp_color);
	 LCD_SetFont(&Font8x16);
	 ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16*2 ) / 2,                 
							  ptr->start_y + ((ptr->end_y - ptr->start_y- 16) / 2),	
							   "下一页");
	 
	 //描绘边框
     ILI9341_DrawRectangle(	ptr->start_x,
							ptr->start_y,
							ptr->end_x - ptr->start_x,
							ptr->end_y - ptr->start_y, 0);
    
}

/**
* @brief  Draw_PrePage_Button  描绘选择上一页的按钮 
* @param  无
* @retval 无
*/
static void Draw_PrePage_Button(void *btn) { 
    uint32_t temp_color;
	Touch_Button *ptr = (Touch_Button *)btn;
   
    if (ptr->touch_flag == 0) {      //按键未被选中   
		LCD_SetColors(ptr->para,CL_WHITE);
		temp_color=ptr->para;
        ILI9341_DrawRectangle(ptr->start_x, ptr->start_y,
                              ptr->end_x - ptr->start_x, 
                              ptr->end_y - ptr->start_y, 1);

	} else {  // 下一页按键被选中   
        LCD_SetColors(CL_GREY3, CL_WHITE);
		temp_color = CL_GREY3;
		ILI9341_DrawRectangle(ptr->start_x, 
							  ptr->start_y, 
							  ptr->end_x - ptr->start_x,
							  ptr->end_y - ptr->start_y, 1);
    }
     //显示功能名
	 LCD_SetColors(CL_RED,temp_color);
	 LCD_SetFont(&Font8x16);
	 ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16*2 ) / 2,                 
							  ptr->start_y + ((ptr->end_y - ptr->start_y- 16) / 2),	
							   "上一页");
	 
	 //描绘边框
     ILI9341_DrawRectangle(	ptr->start_x,
							ptr->start_y,
							ptr->end_x - ptr->start_x,
							ptr->end_y - ptr->start_y, 0);
    
}

/**
* @brief  Command_Select_File   切换当前选中的文件
* @param   无
* @retval 无
*/
static void Command_Select_File(void *btn){
	Touch_Button *ptr = (Touch_Button *)btn;
	Cur_File.CurrentFileName=ptr->fname;
	Cur_File.is_dir=ptr->is_dir;
}

/**
* @brief  Command_Select_Function   选中返回键
* @param   无
* @retval 无
*/
static void Command_Select_Function(void *btn){
	uint16_t index;
	char temp_path[256];
	memset(temp_path,'\0',sizeof(temp_path));
	
	index=Last_Slash(CurPath);
	strncpy(temp_path,CurPath,index);
	NestDepth--;
	Button_Fresh(temp_path);
	cur_env=0;
    num_read_page = 0;
}

/**
* @brief  Command_Select_NextPage   选中下一页功能键
* @param   无
* @retval 无
*/

static void Command_Select_NextPage(void *btn) {
	  uint16_t num=0,i;
    if (cur_env == 1) { //文件内部
        LCD_SetBackColor(CL_WHITE);
        LCD_SetTextColor(BLACK);
        ILI9341_Clear (0,0, LCD_X_LENGTH, LCD_Y_LENGTH);
        
        // 清屏
        
       num_read_page = (num_read_page + 1) % (Total_Txt_Pages); 
        //页数加一，如果已经到了最后一页，那么下一页就是第一页
        for(i =0;i<num_read_page;i++){
					num+=File_Pages[i];
				}
				memset(cur_page_read_buffer,'\0',sizeof(cur_page_read_buffer));
        strncpy((char*)cur_page_read_buffer, 
                (char*)file_read_buffer + num, 
                File_Pages[num_read_page]);
                
		ILI9341_DispString_EN_CH_Custom(0, 0, LCD_X_LENGTH,
                                        LCD_Y_LENGTH - FUNCTION_BLOCK_HEIGHT,
                                        (char *)cur_page_read_buffer);
		// 输出内容到显示屏	
		
		button[0].draw_btn(&button[0]);
        button[1].draw_btn(&button[1]); //绘制下一页按键 
        button[2].draw_btn(&button[2]); //绘制上一页按键
    } else { //文件外部
        if (++file_num_read_page >= FILE_MAX_NUM_PAGE) {
            file_num_read_page--;
            return;
        } 
        Fuction_Button_Enable();
		Button_Fresh(CurPath);
    }
}

/**
* @brief  Command_Select_PrePage   选中上一页功能键
* @param   无
* @retval 无
*/

static void Command_Select_PrePage(void *btn) { 
     uint16_t num=0,i;
     if (cur_env == 1) { //文件内部
         
         LCD_SetBackColor(CL_WHITE);
         LCD_SetTextColor(BLACK);
         ILI9341_Clear (0,0, LCD_X_LENGTH, LCD_Y_LENGTH); 
         // 清屏
         
          if (num_read_page > 0 ) {
            num_read_page = num_read_page - 1;
         }
          for(i =0;i<num_read_page;i++){
					num+=File_Pages[i];
				}
         
         //页数减一
        memset(cur_page_read_buffer,'\0',sizeof(cur_page_read_buffer));
         strncpy((char*)cur_page_read_buffer, 
                 (char*)file_read_buffer +num, 
                 File_Pages[num_read_page]);
		 // 截取需要显示页的数据
                
		 ILI9341_DispString_EN_CH_Custom(0, 0, LCD_X_LENGTH,
                                         LCD_Y_LENGTH - FUNCTION_BLOCK_HEIGHT,
                                         (char *)cur_page_read_buffer);
		 // 输出内容到显示屏	
		
		 button[0].draw_btn(&button[0]);
         button[1].draw_btn(&button[1]); //绘制下一页按键    
         button[2].draw_btn(&button[2]); //绘制上一页按键 
     } else { //文件外部
        if (--file_num_read_page < 0) {
            file_num_read_page++;
            return;
        }
        Fuction_Button_Enable();
		Button_Fresh(CurPath);
     }
}

/**
* @brief  Touch_Button_Down 按键被按下时调用的函数，由触摸屏调用
* @param  x 触摸位置的x坐标
* @param  y 触摸位置的y坐标
* @retval 无
*/
void Touch_Button_Down(uint16_t x,uint16_t y)
{
  uint8_t i;
  for(i=0;i<file_num+FUNCTION_BUTTON_NUM;i++)
  {  
	if(button[i].en==0)  //按键是否使能
			continue;
    /* 触摸到了按钮 */
    if(x<=button[i].end_x && y<=button[i].end_y && y>=button[i].start_y && x>=button[i].start_x)
    {
      if(button[i].touch_flag == 0)     /*原本的状态为没有按下，则更新状态*/
      {
      button[i].touch_flag = 1;         /* 记录按下标志 */
      
      button[i].draw_btn(&button[i]);  /*重绘按钮*/
      }        
      
    }
    else if(button[i].touch_flag == 1) /* 触摸移出了按键的范围且之前有按下按钮 */
    {
      button[i].touch_flag = 0;         /* 清除按下标志，判断为误操作*/
      
      button[i].draw_btn(&button[i]);   /*重绘按钮*/
			
	  Cur_File.CurrentFileName=NULL;  //当前未选中文件
			
    }

  }

}

/**
* @brief  Touch_Button_Up 按键被释放时调用的函数，由触摸屏调用
* @param  x 触摸最后释放时的x坐标
* @param  y 触摸最后释放时的y坐标
* @retval 无
*/
void Touch_Button_Up(uint16_t x,uint16_t y)
{
   uint8_t i; 
   for(i=0;i<file_num+FUNCTION_BUTTON_NUM;i++)
   {
	  if(button[i].en==0)   //按键是否使能
			 continue;
     /* 触笔在按钮区域释放 */
      if((x<button[i].end_x && x>button[i].start_x && y<button[i].end_y && y>button[i].start_y))
      {        
			if(i==0 || i == 1 || i == 2) {
              button[i].touch_flag = 0;       /*释放触摸标志*/
        
              button[i].draw_btn(&button[i]); /*重绘按钮*/        
			}
      
        button[i].btn_command(&button[i]);  /*执行按键的功能命令*/
        
        break;
      }
    }  

}


/**
* @brief  find_sd_file 为查找sd卡所有文件
* @param  无
* @retval 无
*/
void find_sd_file(char *path){
	char lf[FILE_NAME_LENGTH];  //长文件名数组
	memset(IS_DIR,0,sizeof(IS_DIR));//初始化是否是目录表
    File_Name_Init();  //初始化文件名表
	res=f_opendir(&dir,path);
	file_num=0;	  
	if(res==FR_NO_PATH){
		printf("no path");
		
	}
	else if(res!=FR_OK){
		printf("open failed");
	}
	else{
		while(1){
            memset(lf,'\0',sizeof(lf));
			fno.lfname=lf;
			fno.lfsize=sizeof(lf);
			res=f_readdir(&dir,&fno);
			if(res!=FR_OK || !fno.fname[0]){  //目录读完了
				break;
			}
			else if(fno.fattrib & AM_HID){  //隐藏文件
				
			}
			else if(fno.fattrib & AM_DIR){  //文件夹
				 //printf("dir name is %s\n",fno.fname);
				 strcpy(File_Name[file_num],fno.fname);
                 IS_DIR[file_num]=1;
				 file_num++;
			}
			else if (fno.fattrib & AM_SYS){  //系统文件
				
			}
			else{  //其他文件，包括只读，存储
				
				if(!fno.lfname[0]){   //短文件名
					if(fno.fname[0]){
						//printf("file name is %s\n",fno.fname);
						strcpy(File_Name[file_num],fno.fname);
				    file_num++;
					}
				}
				else{  //长文件名
					  //printf("file name is %s\n",fno.lfname);
						strcpy(File_Name[file_num],fno.lfname);
				    file_num++;
				}
			}
 
		}
	}
	f_closedir(&dir);
	 
}


/**
* @brief   初始化数组
* @param  无
* @retval 无
*/
void File_Name_Init(void){
	uint16_t i;
	for(i=0;i<BUTTON_NUM;i++){
		memset((char *)File_Name[i], '\0', sizeof(File_Name[i]));
	}
}


/**
* @brief  检测按键状态，并执行相应功能
* @param  路径
* @retval 无
*/
void  KeyTrace(void){
	//key1按下传输文件开始
	if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )  
		{
			LED1_ON;  
			USARTx_SendFile();
			LED1_OFF;
		} 
		
   //key2按下打开文件或目录
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			LED2_ON;
			Open_File(CurPath);
			LED2_OFF;
		}		
}

/**
* @brief  在显示屏上显示文件内容
* @param  无
* @retval 无
*/
static void Open_File(char *path){
	
	char file_path[256];
	if(Cur_File.CurrentFileName==NULL){   //没有选择文件
		return;
	}
	LCD_SetBackColor(CL_WHITE);
	ILI9341_Clear (0,0, LCD_X_LENGTH, LCD_Y_LENGTH);
	
	//更新路径为xx/xx/../当前文件名
	memset(file_path,'\0',sizeof(file_path));
	strcat(file_path,path);
	strcat(file_path,"/");
	strcat(file_path,Cur_File.CurrentFileName);
	
	memset(CurPath,'\0',sizeof(CurPath));
	strcpy(CurPath,file_path);
	NestDepth++;
	if(Cur_File.is_dir==0) {  //选中的是文件
		memset(file_read_buffer,'\0',sizeof(file_read_buffer));
		File_Button_Disable();
	    Fuction_Button_Enable();
		res=f_open(&fil,file_path,FA_OPEN_EXISTING | FA_READ);
		if(res==FR_OK){
			res=f_read(&fil,file_read_buffer,sizeof(file_read_buffer),&fnumber);
			
      File_Page_Init();
            //将读取到的文件内容的前FILE_PAGE_MAX_BTYE个字节复制给当前页面缓存字符串
			memset(cur_page_read_buffer,'\0',sizeof(cur_page_read_buffer));
      strncpy((char*)cur_page_read_buffer, 
                    (char*)file_read_buffer , File_Pages[0]
                     );
										 
			if(res==FR_OK){ 
				ILI9341_DispString_EN_CH_Custom(0, 0, LCD_X_LENGTH,
                                                LCD_Y_LENGTH - FUNCTION_BLOCK_HEIGHT,
                                                (char *)cur_page_read_buffer);
			}
		}
		f_close(&fil);
		button[0].draw_btn(&button[0]);
        button[1].draw_btn(&button[1]); //绘制下一页按键
        button[2].draw_btn(&button[2]); //绘制上一页按键
		cur_env=1;
	}
	else{   //选中的是文件夹
		 Fuction_Button_Enable();
		 Button_Fresh(file_path);
	}

}

/**
  * @brief  USARTx 接受文件，识别文件并保存在sd卡
  * @param  无
  * @retval 无
  */

void  USARTx_ReceiveFile(void){
    uint16_t  total_num;
    uint16_t index;
	char temp_data[RECEIVEBUFF_SIZE];
	 
	memset((char *)temp_data,'\0',sizeof(temp_data));
	strncpy((char *)temp_data,(char *)ReceiveBuff,strlen("txt_data_sta"));
	if(strcmp("txt_data_sta",temp_data)==0){   //识别到是文件传输开始
		LED1_ON;
		//printf("detect file\n");
		write_flag=1;
		memset((char *)File_Receive_Name,'\0',sizeof(File_Receive_Name));
		
		if(cur_env==1){  //当前位于文件内部
			index=Last_Slash(CurPath);
			strncpy(File_Receive_Name,CurPath,index);
			NestDepth--;
		}
		else{  //当前在文件夹内部
			strcpy(File_Receive_Name,CurPath);
		}
		memset(CurPath,'\0',sizeof(CurPath));
		strcpy(CurPath,File_Receive_Name);
		strcat(File_Receive_Name,"/");
		strcat(File_Receive_Name,(char *)ReceiveBuff+strlen("txt_data_sta"));
//		memset((char *)temp_data,'\0',sizeof(temp_data));
		
		res=f_open(&fil,(TCHAR *)File_Receive_Name,FA_CREATE_ALWAYS|FA_WRITE);
		if(res==FR_OK){
			//printf("file create success,file name is %s\n",(char *)File_Receive_Name);
		}
    f_close(&fil);
	}
	else if(strcmp("txt_data_end",temp_data)==0&&write_flag==1){ //识别到是文件传输结束
		write_flag=0;
		Button_Fresh(CurPath);
		LED1_OFF;
		
	}
	else if(write_flag==1){  //识别到是文件内容
		 
		res=f_open(&fil,File_Receive_Name,FA_OPEN_EXISTING|FA_WRITE);
       
        
    if(strlen((char *)ReceiveBuff)< sizeof(ReceiveBuff)){
			total_num=strlen((char *)ReceiveBuff);
		}
		else{
			total_num=sizeof(ReceiveBuff);
		}
        
        
        
		 f_lseek(&fil,f_size(&fil));  //将文件指针移动到文件末尾
		res=f_write(&fil,ReceiveBuff,total_num,&fnumber);

		f_close(&fil);
	}
	else{
		//printf("no meaning data");
	}

  memset(ReceiveBuff,'\0',sizeof(ReceiveBuff));
}
 
/**
  * @brief  USARTx 接受文件，识别文件并保存在sd卡
  * @param  无
  * @retval 无
  */
void  USARTx_SendFile(void){
	char file_path[256];
	if(Cur_File.CurrentFileName==NULL)
		return;
	memset(SendBuff,'\0',sizeof(SendBuff));
	strcpy((char *)SendBuff,"txt_data_sta");
	strcat((char *)SendBuff,Cur_File.CurrentFileName);
	//printf("%s\n",SendBuff);
	Usart_DMA_TX_Wait();
	
	//第二次发送，发送文件内容
	memset(file_path,'\0',sizeof(file_path));
	strcpy(file_path,CurPath);
	if(cur_env==0){  //当前不在所选文件内部
		strcat(file_path,"/");
		strcat(file_path,Cur_File.CurrentFileName);
	}
	res=f_open(&fil,file_path, FA_OPEN_EXISTING|FA_READ);
  if(res==FR_OK){
		while(1){
		    memset((char *)SendBuff,'\0',sizeof(SendBuff));
			res=f_read(&fil,(BYTE *)SendBuff,sizeof(SendBuff),&fnumber);
			Usart_DMA_TX_Wait();
			if(fnumber < sizeof(SendBuff)){  //文件读取结束
				break;
			}
		}
	}
  f_close(&fil);
	
	//第三次发送，代表文件发送结束
	memset((char *)SendBuff,'\0',sizeof(SendBuff));
	strcpy((char *)SendBuff,"txt_data_end");
	Usart_DMA_TX_Wait(); 
}



/**
* @brief  文件按钮禁用
* @param  无
* @retval 无
*/
static void File_Button_Disable(void){
	uint16_t i;
	for(i=FUNCTION_BUTTON_NUM;i<file_num+FUNCTION_BUTTON_NUM;i++){
		button[i].en=0;
	}
}

/**
* @brief  文件按钮使能
* @param  无
* @retval 无
*/
//static void File_Button_Enable(void){
//	uint16_t i;
//	for(i=FUNCTION_BUTTON_NUM;i<file_num+FUNCTION_BUTTON_NUM;i++){
//		button[i].en=1;
//	}
//}

/**
* @brief  功能按键禁用
* @param  无
* @retval 无
*/
//static void Function_Button_Disable(void){
//	uint16_t i;
//	for(i=0;i<FUNCTION_BUTTON_NUM;i++){
//        if (i == 1) { // 1号按键是下一页，始终使能
//            continue;
//        }
//		button[i].en=0;
//	}
//}

/**
* @brief  功能按键使能
* @param  无
* @retval 无
*/
static void Fuction_Button_Enable(void){
	uint16_t i;
	for(i=0;i<FUNCTION_BUTTON_NUM;i++){
		button[i].en=1;
	}
}


/**
* @brief  最后一个/的位置
* @param  无
* @retval 无
*/
static uint16_t  Last_Slash(char *path){
	uint16_t i;
	uint16_t  index=0;
	
	for(i=0;i<strlen(path);i++){
		if(path[i]=='/'){
			index=i;
		}
	}
	return index;
}


static void File_Disable(void) {
     uint8_t i;
     uint8_t start_position = FUNCTION_BUTTON_NUM + file_num_read_page * NUM_FILE_ONE_PAGE;
     uint8_t end_position = FUNCTION_BUTTON_NUM + (file_num_read_page + 1) * NUM_FILE_ONE_PAGE;
     for (i = FUNCTION_BUTTON_NUM; i < BUTTON_NUM; i++) {
        if (i < start_position || i >= end_position) {
            button[i].en = 0;
        }
     }
}

/**
* @brief  初始化文件内容每一页的页数数量
* @param  无
* @retval 无
*/
static void File_Page_Init(void){
	uint16_t count=0,cur_page=0;
	char *pStr=(char *)file_read_buffer;
	Total_Txt_Pages=0;
	num_read_page=0;
	 while(*pStr!='\0'){
		 if(*pStr<=126){
			 count++;
			 pStr++;
			 if(count==FILE_PAGE_MAX_BTYE-1){
				 File_Pages[cur_page]=count;
				 count=0;
				 cur_page++;
				 Total_Txt_Pages++;
				  
			 }
			  
		 }
		 else{
			 count+=2;
			 pStr+=2;
			 if(count>=FILE_PAGE_MAX_BTYE-1){
				 File_Pages[cur_page]=count;
				 count=0;
				 cur_page++;
				 Total_Txt_Pages++;
				  
			 }
			  
		 }
		 if(cur_page>=MAX_NUM_PAGE){
			 Total_Txt_Pages--;
			 return;
		 }
	 }
	 if(count!=0){
		 File_Pages[cur_page]=count;
		 Total_Txt_Pages++;
	 }
//	 for(count=0;count<Total_Txt_Pages;count++){
//		 printf("%d\n",File_Pages[count]);
//	 }
}

