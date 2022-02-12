#!/bin/bash
#./mcc "i = 0; 
#while( i == 3 ) i = i+ 1;"
./mcc "j = 0; k = 0;i = 0;while(i < 3){
    i = i + 1;
    j = j + 1;
    k = k = 1;
}
    return j + k;
    "