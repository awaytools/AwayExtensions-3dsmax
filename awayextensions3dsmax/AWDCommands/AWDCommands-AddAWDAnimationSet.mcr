macroScript AddAWDAnimationSet
	category:"AWDCommands"
	toolTip:""
(
fn getAWDID = (
	newTime=getUniversalTime()
	newString="awd_"+newTime[1] as string+newTime[2] as string+newTime[3] as string+newTime[4] as string
	newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
	newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
	newString=newString+random 0.0 10000.0 as string
	return newString
)

meshes = selection
if meshes.count>=1 then (	
		for i in meshes do(
			newAwdCnt=1
			for onemesh in objects do(
				for m in onemesh.modifiers do(
					if classof  m as string=="AWDAnimationSet" then(
						oldAWDCnt=0
						try oldAWDCnt=m.awdCnt+1
						catch oldAWDCnt=0
						if oldAWDCnt>=newAwdCnt then newAwdCnt=oldAWDCnt		
					)
				)
			)
			newName="AWD Anim Set "+newAwdCnt as string
			MyNewModifier = AWDAnimationSet()
			MyNewModifier.name = newName
			addmodifier i MyNewModifier			
		)
	)
else(
	selected=#()
	testCnter=0
	for i in objects do(
		for m in i.modifiers do(
			if classof  m as string=="AWDAnimationSet" then(
				appendIfUnique selected i
					
			)
		)
	)	
	answer=queryBox ("Nothing is selected.\n\n Found "+testCnter as string+" AnimationClips in the scene.\n\n Select the "+selected.count as string + " Objects that are holding the AnimationClips ?")
	if answer==true then(
		select selected
	)
	)
)