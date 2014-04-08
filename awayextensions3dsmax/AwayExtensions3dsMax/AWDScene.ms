plugin Helper AWDScene
name:"AWDScene"
category:"AWDObjects"
classID:#(0x557f6af6, 0x7afe070e)
extends:dummy
(
	fn getAWDID = (
		newTime=getUniversalTime()
		newString="awd_"+newTime[1] as string+newTime[2] as string+newTime[3] as string+newTime[4] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+random 0.0 10000.0 as string
		return newString
	)	
		
	parameters main rollout:params
	(
		thisAWDID type:#string default:"noIDSet"
	)
	rollout params "AWD Scene Settings"
	(
		on params open do (
		)
	)
	on create do if thisAWDID=="noIDSet" then thisAWDID=getAWDID()
	on clone orig do 	thisAWDID=getAWDID()
		
)