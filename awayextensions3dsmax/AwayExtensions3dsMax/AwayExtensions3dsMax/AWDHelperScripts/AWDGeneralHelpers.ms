--generates a unique AWD-ID for any asset
fn getAWDID = (
	newTime=getUniversalTime()
	newString="awd_"+newTime[1] as string+newTime[2] as string+newTime[3] as string+newTime[4] as string
	newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
	newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
	newString=newString+random 0.0 10000.0 as string
	return newString
)