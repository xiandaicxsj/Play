#!/usr/bin/python
import os
import sys
HexList=[0,1,2,3,4,5,6,7,8,9,'a','b','c','d','e','f']
def DtoHex(strData):
	data=int(strData)
	retList=[]
	left=data%16
	div=data/16
	retList.append(left)
	while div:
		left=div%16
		div=div/16
		retList.append(left)
	tmpList=retList[::-1]
	retStr=''
	for s in tmpList:
	 	retStr=retStr+str(HexList[s]) 
	print retStr+'h'
def delT(data):
	if data[len(data)-1]=='m' or data[len(data)-1]=='M':
		tmpData=int(data[0:len(data)-1])*1024*1024
		DtoHex(str(tmpData))
		return
	if data[len(data)-1]=='k' or data[len(data)-1]=='K':
		tmpData=int(data[0:len(data)-1])*1024
		DtoHex(str(tmpData))
		return
	DtoHex(data)
		
data=str(sys.argv[1])
delT(data)





       			
	
	
