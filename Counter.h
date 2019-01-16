//
//  Counter.h
//  Counter
//
//  Created by qws on 2018/12/24.
//  Copyright © 2018 qws. All rights reserved.
//

#ifndef Counter_h
#define Counter_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef enum function {
    FunAdd,
    FunSub,
    FunMul,
    Fundivi,
    FunNum
} Function;

void c_init(void);
void c_destory(void);
int c_isSumEnable(void);
int c_isEnable(void);
 char* c_clearOn(void);
 char* c_inputNum(double c_num);
char* c_back(void);
char* c_clearError(void);
void c_dot(void);
char* c_gt(void);
char* c_operate(Function f);
double c_caculate(void);
char* c_compute(void);
char* c_add(void);
char* c_sub(void);
char* c_mul(void);
char* c_divi(void);

typedef void (*C_CallBackFun)(int);
void c_SetErrorBlock(C_CallBackFun fun);
void c_SetSumEnableBlock(C_CallBackFun fun);
void c_SetEnableBlock(C_CallBackFun fun);

#pragma mark -
#pragma mark - private
double c_getNumFromInpus(void);
double getDecimalFromDouble(double num);
double getIntegerFromDouble(double num);
int checkIntegerCount(double num);
int checkDecimalCount(double num);
int setError(int _e);
double setResult(double num);
int setEnable(int _enable);
int setSumEnable(int _enable);
void c_endInput(void);
char* errorReturn(void);
double getDoubleFromChars(char *str);
char* getCharFromNum(double num,int maxCount, int bu0);
int checkArrayCount(void);

#endif /* Counter_h */
