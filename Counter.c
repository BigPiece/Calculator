//
//  Counter.c
//  Counter
//
//  Created by qws on 2018/12/24.
//  Copyright © 2018 qws. All rights reserved.
//

#include "Counter.h"

static double sum,inputNum,result,operandLeft,operandRight;
static Function curtF;
static int dotIdx;
static int error;
static double *inputs;
static int count;
static int hasResult;
static int c_enable;
static int sumEnable;
static int backDecimalCount;
static int decimalCount;
static char *c_retStr;
static char *backStr;

static C_CallBackFun errorBlock;
static C_CallBackFun sumEnableBlock;
static C_CallBackFun enableBlock;

//销毁
void c_destory(void)
{
    free(inputs);
    inputs = 0;
    free(c_retStr);
    c_retStr = 0;

    errorBlock = 0;
    sumEnableBlock = 0;
    enableBlock = 0;
}

//初始化
void c_init(void)
{
    c_clearOn();
}

#pragma operates
//清空开关
 char* c_clearOn(void)
{
    setEnable(!c_enable);
    setError(0);
    setSumEnable(0);
    backDecimalCount = 0;
    hasResult = 0;
    operandLeft = 0;
    operandRight = 0;
    count = 0;
    inputNum = 0;
    result = 0;
    sum = 0;
    curtF = FunNum;
    inputs = malloc(sizeof(char) * 13);
    c_retStr = malloc(sizeof(char) * 16);
    backStr = malloc(sizeof(char) * 16);
    dotIdx = 0;
    sum = 0;
    c_retStr[0] = '0';
    c_retStr[1] = '.';
    c_retStr[2] = '\0';
    
    if (!c_enable) {
        c_retStr[0] = '\0';
    }
    
    
    return c_retStr;
}

//接收输入一个数字 0~9
 char* c_inputNum(double c_num)
{
    printf("inputNum: %f \n",c_num);
    if (!c_enable ) {
        c_retStr[0] = '\0';
    }
    
    if (error) {
        c_retStr = errorReturn();
    }
    
    if (count == 10) {
        setError(1);
    } else {
        inputs[count] = c_num;
        count++;
        inputNum = c_getNumFromInpus();
    }
    printf("count = %d \n",count);
    getCharFromNum(inputNum, 0, -1);
    
    printf("inpu before cc = %s",c_retStr);
    return c_retStr;
    
}

//退格 取消当前现实数字的最后一位
char* c_back(void)
{
    if (error) {
        setError(0);
    } else {
        int strCount = checkArrayCount();

        if (strCount == 2) {
            if (c_retStr[0] != '0') {
                c_retStr[0] = '0';
            }
        } else {
            c_retStr[strCount - 1] = '\0';
        }
    }
    return c_retStr;
    
//    double newNum = count == 0 ? result : c_getNumFromInpus();
//    if (error) {
//        setError(0);
//    } else {
//        if (count == 0) { //没有输入，修改result
//            double fract = fmod(result, 1.);//小数
//            double zhengShu = result - fract;
//            if (backDecimalCount == 0) { //校验
//                backDecimalCount = checkDecimalCount(fract);
//            }
//            if (backDecimalCount == 0) { //没有小数
//                newNum = setResult((int)(result / 10)) ;
//            } else {
//                int a = (int)(fract * pow(10, backDecimalCount));
//                a /= 10;
//                backDecimalCount--;
//                double newFract = ((float)a) / pow(10, backDecimalCount);
//                newNum = setResult(zhengShu + newFract);
//            }
//
//        } else { //有输入，修改inputs
//            if (count > 0) {
//                count--;
//                if (count == dotIdx) {
//                    dotIdx = 0;
//                }
//            }
//            newNum = c_getNumFromInpus();
//        }
//    }
//
//    return newNum;
}

//ce 将当前输入重置为0，如果有错误则先取消错误
char* c_clearError(void)
{
    if (error) {
        setError(0);
        c_retStr = errorReturn();
    } else {
        if (count == 0 ) {
            c_retStr = getCharFromNum(result, 1, -1);
        } else {
            count = 0;
            inputNum = 0;
            dotIdx = 0;
            c_retStr = getCharFromNum(0, 1, -1);
        }
    }
    return c_retStr;
}

//. 小数点
void c_dot(void)
{
    if (error) {
        return;
    }
    if (count == 0) {
        inputs[0] = 0;
        count = 1;
    }
    dotIdx = count;
}

//gt 按钮 记录总和
char* c_gt(void)
{
    if (error) {
        c_retStr = errorReturn();
    } else {
        double backNum = 0;
        if (sumEnable) {
            backNum = sum;
            sum = 0;
        } else {
            if (count == 0) {
                backNum = result;
            } else {
                backNum = c_getNumFromInpus();
            }
        }
        setSumEnable(!sumEnable);
        c_retStr = getCharFromNum(backNum, 0, -1);
    }
    return c_retStr;
}

//= 等号
char* c_compute(void)
{
    if (error) {
        c_retStr = errorReturn();
    } else {
        c_endInput();
        c_caculate();
        
        if (hasResult) {//第一次输入时还没有计算过结果
            setSumEnable(1);
            sum += result;
        }
        c_retStr = getCharFromNum(result, 0, -1);
    }
    
    return c_retStr;
}

//+
char* c_add(void)
{
    return c_operate(FunAdd);
}

//-
char* c_sub(void)
{
    return c_operate(FunSub);
}

//*
char* c_mul(void)
{
    return c_operate(FunMul);
}

//除法
char* c_divi(void)
{
    return c_operate(Fundivi);
}

//点击运算符的操作
char* c_operate(Function f)
{
    if (error) {
        c_retStr = errorReturn();
        return c_retStr;
    }
    curtF = f;
    if (count == 0) { //没有输入显示结果
        c_retStr = getCharFromNum(result, 0, -1);
    } else { //有输入,计算后显示结果
        c_endInput();
        c_caculate();
        dotIdx = 0;
        c_retStr = getCharFromNum(result, 0, -1);
    }
    return c_retStr;
}

#pragma Functions
int checkArrayCount(void)
{
    int aC = 0;
    while (c_retStr[aC] != '\0') {
        aC++;
    }
    return aC;
}

//把c_retStr内容全部改为'\0'
void clearc_retStr(void)
{
    int cidx = 0;
    while (1) {
        char c = c_retStr[cidx];
        if (c != '\0') {
            c_retStr[cidx] = 0;
            cidx++;
        } else {
            return;
        }
    }
    return;
}

//把一个double 转化为字符串（带有符号和小数点），maxCount为转化后的字符串长度，bu0 == -1不补0 默认补0
char* getCharFromNum(double num,int maxCount, int bu0)
{
    clearc_retStr();
    
    int defaultCount = 10;
    if (maxCount != 0 && maxCount < defaultCount) { //maxCount=0 或 大于defaultCount默认10位数
        defaultCount = maxCount;
    }
    
    int dIdx,sign;
    c_retStr = fcvt(num, defaultCount -1, &dIdx, &sign); //小数点后n位不够补0，小数点位置 和标志
    
    char newStr[32];
    int insertIdx = 0;
    if (sign == 1) { //插入负号
        newStr[0] = '-';
        insertIdx++;
    }

    if (dIdx <= 0) { //0.000
        newStr[insertIdx] = '0';
        newStr[insertIdx+1] = '.';
        insertIdx += 2;
        for (int i = 0 ; i < abs(dIdx); i++) { //插入0
            newStr[insertIdx] = '0';
            insertIdx++;
        }
        for (int i = 0; i < defaultCount - 1 - abs(dIdx); i++) { //插入数字
            newStr[insertIdx] = c_retStr[i];
            insertIdx++;
        }
    } else {
        int iCount = 0;
        for (int i = 0; i < dIdx; i++) { //整数部分
            newStr[insertIdx] = c_retStr[i];
            insertIdx++;
            iCount++;
            if (iCount >= defaultCount) {
                break;
            }
        }
        newStr[insertIdx] = '.'; //小数点
        insertIdx++;
        for (int i = 0; i < defaultCount - iCount; i++) { //小数部分
            newStr[insertIdx] = c_retStr[iCount + i];
            insertIdx++;
        }
    }
    newStr[insertIdx] = '\0'; //最后插入结束位
    c_retStr = newStr;
    
    //默认补0
    if (bu0 == -1) { //不需要补0，长度改变，去除小数最后的0
        while (c_retStr[defaultCount-1] != '.' && c_retStr[defaultCount - 1] == '0') {
            //如果最后一位是小数点后面的0则删除
            c_retStr[defaultCount-1] = '\0';
            defaultCount--;
        }
    }
    
    getDoubleFromChars(c_retStr);
    
    printf("\ngetChar c_retStr = %s idx = %d sign = %d\n ",c_retStr,dIdx,sign);
    printf("inpu before cc = %s",c_retStr);

    return c_retStr;
}

double getDoubleFromChars(char *str)
{
    double n = 0.0;
    double tmp = 10.0;
    int flag = 0;
    
    if(*str == '-')
    {
        flag = 1;
        str++;
    }
    while(*str >= '0' && *str <= '9')
    {
        n = n*10 + (*str - '0');
        str++;
    }
    if(*str == '.')
    {
        str++;
        while(*str >= '0' && *str <= '9')
        {
            n = n + (*str -'0')/tmp;;
            tmp = tmp * 10;
            str++;
        }
    }
    if(flag == 1)
    {
        n = -n;
    }
    printf("\n getDouble = %.9f\n\n",n);
    return n;
}

//计算结果
double c_caculate(void)
{
    operandLeft = hasResult ? result : inputNum; //如果不是第一次计算则已经有了结果，左边计算数则为result,否则为inputNum
    operandRight = inputNum;
    if (!hasResult) {
        hasResult = 1;
        setResult(operandLeft);
        return inputNum;
    }
    switch (curtF) {
        case FunAdd:
            setResult(operandLeft + operandRight);
            break;
        case FunSub:
            setResult(operandLeft - operandRight);
            break;
        case FunMul:
            setResult(operandLeft * operandRight);
            break;
        case Fundivi:
            if (operandRight == 0) { //除0
                setError(1);
                return 0;
            }
            setResult(operandLeft / operandRight);
            break;
        default:
            break;
    }
    operandLeft = result;
    return result;
}

//如果有错误，返回的值，如果有输入则返回当前输入的值，没有则返回结果值
char* errorReturn(void)
{
    double num = count == 0 ? result : c_getNumFromInpus();
    return getCharFromNum(num, 1, -1);
}

//结束输入
void c_endInput(void)
{
    if (count != 0) { //有输入
        inputNum = c_getNumFromInpus();
        count = 0;
    }
}

//从输入缓存获取输入的数字
double c_getNumFromInpus(void)
{
    double willShowNum = 0;
    
    for (int i = 0 ; i < count; i++) {
        if (dotIdx == 0) {
            willShowNum += inputs[i] * pow(10, count - 1 - i);
        } else {
            if (i < dotIdx - 1) { //整数部分
                willShowNum += inputs[i] * pow(10, dotIdx - 1 - i);
            } else { //小数部分
                willShowNum += inputs[i] / pow(10, i - dotIdx + 1);
            }
        }
        printf("inputs idx : %d  num = %f \n",i,inputs[i]);
    }
    printf("willshow = %f",willShowNum);
    
    return willShowNum;
}


//获取一个数字的整数部分的位数
int checkIntegerCount(double num)
{
    num = fabs(num);
    if (num < 1.0) {
        return 1;
    }
    int iCount = 0;
    double zs = getIntegerFromDouble(num);
    while (zs >= 1) {
        iCount++;
        zs = (int)(zs / 10.);
    }
    printf("num = %f,iCount =%d",num,iCount);
    return iCount;
}

//获取一个数字小数部分的位数
int checkDecimalCount(double num)
{
    num = fabs(num);
    int dCount = 0;
    double decimal = getDecimalFromDouble(num);
    double precision = 1./ pow(10, 9);
    while (decimal/pow(10, dCount) >= precision) {
        dCount++;
        int zs = decimal * 10;
        decimal = decimal * 10 - zs;
    }
    printf("num = %f,dCount =%d",num,dCount);
    return dCount;
}

//获取一个数字的小数部分
double getDecimalFromDouble(double num)
{
    num = fabs(num);
    double fract = fmod(num, 1.);//小数
    printf("num = %f fract = %f",num,fract);
    return fract;
}

//获取一个数字的整数部分
double getIntegerFromDouble(double num)
{
    num = fabs(num);
    double fract = fmod(num, 1.);//小数
    double zhengShu = num - fract;
    printf("num = %f zhengshu = %f",num,zhengShu);
    return zhengShu;
}

#pragma Setter & Getter

//设置结果
double setResult(double num)
{
//    int sign = num >= 0 ? 1 : -1;
//    num = fabs(num);
    int iCount = checkIntegerCount(num);
    int dCount = checkDecimalCount(num);
    if (num > 9999999999) {
        setError(1);
        num = num / pow(10, iCount-1);
        result = num;
        printf("error num = %f",num);
    } else {
        if (iCount + dCount > 10) {
            double fract = getDecimalFromDouble(num);
            int decimalCount = 10 - iCount;
            result = getIntegerFromDouble(num) + floor(fract * pow(10, decimalCount)) / pow(10, decimalCount);
        } else {
            result = num;
        }
    }
    
    decimalCount = checkDecimalCount(num);
    getCharFromNum(result,0,0);
    return result;
}

//外部获取是否打开了求和按钮（GT)
int c_isSumEnable(void)
{
    return sumEnable;
}

//外部获取是否 启动
int c_isEnable(void)
{
    return c_enable;
}

//设置求和按钮打开
int setSumEnable(int _enable)
{
    sumEnable = _enable;
    if (*sumEnableBlock != 0) {
        (*sumEnableBlock)(_enable);
    }
    return sumEnable;
}

//设置是否启动
int setEnable(int _enable)
{
    c_enable = _enable;
    if (*enableBlock != 0) {
        (*enableBlock)(_enable);
    }
    return c_enable;
}

//设置错误
int setError(int _e)
{
    error = _e;
    if (*errorBlock != 0) {
        (*errorBlock)(_e);
    }
    return error;
}


//设置错误回调的函数
void c_SetErrorBlock(C_CallBackFun fun)
{
    errorBlock = fun;
    return;
}

//设置求和开关的回调的函数
void c_SetSumEnableBlock(C_CallBackFun fun)
{
    sumEnableBlock = fun;
    return;
}

void c_SetEnableBlock(C_CallBackFun fun)
{
    enableBlock = fun;
    return;
}

