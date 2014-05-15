
fn validateClips thisSourceObj= (
	valid=true
	if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveAnimSourceIDs.count then valid=false
	if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveStartFrames.count then valid=false
	if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveEndFrames.count then valid=false
	if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveSkipFrames.count then valid=false
	if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveStitchFinals.count then valid=false	
	if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveLoops.count then valid=false	
	if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveTransform.count then valid=false		
	if not valid then(	
		minCount=99999999
		if thisSourceObj.saveAnimClipNames.count<minCount then minCount=thisSourceObj.saveAnimClipNames.count
		if thisSourceObj.saveAnimSourceIDs.count<minCount then minCount=thisSourceObj.saveAnimSourceIDs.count
		if thisSourceObj.saveStartFrames.count<minCount then minCount=thisSourceObj.saveStartFrames.count
		if thisSourceObj.saveEndFrames.count<minCount then minCount=thisSourceObj.saveEndFrames.count		
		saveAnimClipNames=#()
		saveAnimSourceIDs=#()
		saveStartFrames=#()
		saveEndFrames=#()
		saveSkipFrames=#()
		saveStitchFinals=#()
		saveLoops=#()
		saveTransform=#()
		for i=1 to minCount do(	
			append saveAnimClipNames thisSourceObj.saveAnimClipNames[i]
			append saveAnimSourceIDs thisSourceObj.saveAnimSourceIDs[i]
			append saveStartFrames thisSourceObj.saveStartFrames[i]
			append saveEndFrames thisSourceObj.saveEndFrames[i]
			try(append saveSkipFrames thisSourceObj.saveSkipFrames[i])catch(append saveSkipFrames 0)
			try(append saveStitchFinals thisSourceObj.saveStitchFinals[i])catch(append saveStitchFinals false)
			try(append saveLoops thisSourceObj.saveLoops[i])catch(append saveLoops false)
			try(append saveTransform thisSourceObj.saveTransform[i])catch(append saveTransform false)
		)
		thisSourceObj.saveAnimClipNames=saveAnimClipNames
		thisSourceObj.saveAnimSourceIDs=saveAnimSourceIDs
		thisSourceObj.saveStartFrames=saveStartFrames
		thisSourceObj.saveEndFrames=saveEndFrames
		thisSourceObj.saveSkipFrames=saveSkipFrames
		thisSourceObj.saveStitchFinals=saveStitchFinals	
		thisSourceObj.saveLoops=saveLoops	
		thisSourceObj.saveTransform=saveTransform			
	)
)
	
 fn updateUI thisobj row= (
	thisobj.AnimClipsRollout.targetAnimClipsList.items=#()
	validateClips thisobj
	minCount=thisobj.saveAnimClipNames.Count
 	for i=1 to minCount do(
		names =thisobj.saveAnimClipNames[i]
		newstring=names
		if names.Count>20 then(
			newstring=substring names 1 19
			append newstring "..."
		)
		if names.count<=20 then (
			for i=newstring.count to 18 do append newstring "  "  )
		newstart=(thisobj.saveStartFrames[i]) as string
		newCnt=newstart.count
		if newCnt<20 then (
			for i=newstart.count to 18 do append newstart "  "  )
		else if newCnt>20 then (
			newstart=substring newstart 1 19
			append newstart ".."
		)	
		newend=(thisobj.saveEndFrames[i]) as string
		newCnt=newend.count
		if newCnt<20 then (
			for i=newend.count to 18 do append newend "  "  )
		else if newCnt>20 then (
			newend=substring newend 1 19
			append newend ".."
		)
		for i=newend.count to 18 do append newend "  "
			
 		newname=newstring+"  "+newstart+"  "+newend
 		thisobj.AnimClipsRollout.targetAnimClipsList.items = append thisobj.AnimClipsRollout.targetAnimClipsList.items newname
		thisobj.AnimClipsRollout.targetAnimClipsList.selection=saveSelectedAnimClip=row+1
 	)
)
	
fn getNextStartFrame thisObj = (	
	endFrame=0
	for onEndFrame in thisObj.saveEndFrames do if onEndFrame>endFrame then endFrame=onEndFrame
	return endFrame+1
)

fn createClip thisObj animName animID animStart animEnd animSkip animStitchFinal animLoops animTransform = (	
	append thisObj.saveAnimClipNames animName
	append thisObj.saveAnimSourceIDs animID
	append thisObj.saveStartFrames  animStart
	append thisObj.saveEndFrames animEnd
	append thisObj.saveSkipFrames animSkip
	append thisObj.saveStitchFinals animStitchFinal
	append thisObj.saveLoops animLoops
	append thisObj.saveTransform animTransform
	updateUI thisObj 0	
)