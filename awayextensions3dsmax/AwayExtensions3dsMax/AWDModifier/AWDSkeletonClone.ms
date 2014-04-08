plugin modifier AWDSkeletonClone
name:"AWDSkeletonClone"
category:"AWDModifier"
classID:#(0x15236fca, 0x19623ce0)
extends:EmptyModifier replaceUI:false version:1
(	
	fn getAWDID = (
		newTime=getUniversalTime()
		newString="awd_"+newTime[1] as string+newTime[2] as string+newTime[3] as string+newTime[4] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+random 0.0 10000.0 as string
		return newString
	)	
	
	fn deleteKeysByRange con deleteRange =
	(
		for x in con.keys.count to 1 by -1 do	(
			if (con.keys[x].time >= deleteRange.start) and (con.keys[x].time <= deleteRange.end) do deleteKey con x
		)
	)

	fn deleteKeysByRangeObject obj thisInterval= (
		--mPointCon = obj.baseObject[#Master_Point_Controller]
		--if mPointCon != undefined do deleteKeysByRange mPointCon.controller (interval 0f 50f)
		deleteKeysByRange obj.position.controller thisInterval
		deleteKeysByRange obj.rotation.controller thisInterval
		deleteKeysByRange obj.scale.controller thisInterval
		for child in obj.children do deleteKeysByRangeObject child thisInterval		
	)
	fn CollectAllTargetObjs obj thisList = (
		append thisList obj
		for child in obj.children do CollectAllTargetObjs child thisList
	)
		
	fn bakeToTargetSkeleton thisSkelet = (
		animName=0
		animID=0
		animStart=0
		animEnd=0
		animSkip=0
		if thisSkelet.saveSelectedAnimClip>0 then (
			if thisSkelet.saveSelectedAnimClip<=thisSkelet.saveAnimClipNames.count then (
				animName=thisSkelet.saveAnimClipNames[thisSkelet.saveSelectedAnimClip]
				animID=thisSkelet.saveAnimSourceIDs[thisSkelet.saveSelectedAnimClip]
				animStart=thisSkelet.saveStartFrames[thisSkelet.saveSelectedAnimClip]
				animEnd=thisSkelet.saveEndFrames[thisSkelet.saveSelectedAnimClip]
				animSkip=thisSkelet.saveSkipFrames[thisSkelet.saveSelectedAnimClip]
				animStitchFinal=thisSkelet.saveStitchFinals[thisSkelet.saveSelectedAnimClip]
			)
		)
		if animStart==animEnd then (
			messageBox("No valid animationclip is selected")
			return()
		)
			
		rootObj=selection[1]
		targetRootObj=undefined
		targetModifier=undefined
		if thisSkelet.target_ID!="noIDSet" then (
			for obj in objects do(
				for m in obj.modifiers do(
					if (classof m as string)=="AWDSkeleton" then (
						if m.thisAWDID==thisSkelet.target_ID then (
							targetRootObj=obj
							targetModifier=m
							exit
						)
					)
				)
				if targetModifier!=undefined then exit
			)
		)
		if targetModifier==undefined then(
			messageBox("No valid target AWDSkeleton set!")
			return()
		)
		targetStartFrame=targetModifier.createClip targetModifier animName animID animStart animEnd animSkip animStitchFinal
		thisInterval = (interval targetStartFrame (targetStartFrame+(animEnd-animStart)))
		deleteKeysByRangeObject targetRootObj thisInterval

		print ("targetStartFrame = "+(targetStartFrame as string))
		-- find the animation in the targetModifier
		i=animStart
		timeDifference=targetStartFrame-animStart
		didTheEnd=false
		allTargetObjects=#()
		CollectAllTargetObjs targetRootObj allTargetObjects
		allSourceObjTransforms=#()
		CollectAllTargetObjs rootObj allSourceObjTransforms
		animate on(
			while i<=animEnd do(
				objCnter=1
				targetTime=(i+timeDifference) as integer
				for obj in allTargetObjects do(	
					thisSource=allSourceObjTransforms[objCnter]
					at time (targetTime as integer) in coordsys world  obj.transform =  at time (i as integer) in coordsys world thisSource.transform
					addNewKey obj[3] (targetTime as integer)
					objCnter+=1				
				)					
				forcedEnd=false
				if i==animEnd then didTheEnd=true
					
				i+=1+animSkip
				
				if i>animEnd and not didTheEnd then i=animEnd--do the end
				
			)
		)
		deleteItem  thisSkelet.saveAnimClipNames thisSkelet.saveSelectedAnimClip			
		deleteItem  thisSkelet.saveAnimSourceIDs thisSkelet.saveSelectedAnimClip
		deleteItem  thisSkelet.saveStartFrames thisSkelet.saveSelectedAnimClip
		deleteItem  thisSkelet.saveEndFrames thisSkelet.saveSelectedAnimClip
		deleteItem  thisSkelet.saveSkipFrames thisSkelet.saveSelectedAnimClip
		deleteItem  thisSkelet.saveStitchFinals thisSkelet.saveSelectedAnimClip	
		thisSkelet.saveSelectedAnimClip-=1
		thisInterval = (interval animStart animEnd)
		deleteKeysByRangeObject rootObj thisInterval
		modPanel.SetCurrentObject targetModifier
		
		
	)	
	fn updateUI thisSkelet row= (
		thisSkelet.AnimClipsRollout.targetAnimClipsList.items=#()
		valid=true
		--if saveAnimClipNames.count!=saveAnimSourceIDs.count then valid=false
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveStartFrames.count then valid=false
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveEndFrames.count then valid=false
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveSkipFrames.count then valid=false
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveStitchFinals.count then valid=false
		minCount=thisSkelet.saveAnimClipNames.count
		if not valid then(	
			if thisSkelet.saveAnimClipNames.count<minCount then minCount=thisSkelet.saveAnimClipNames.count
			--if saveAnimSourceIDs.count<minCount then minCount=saveAnimSourceIDs.count
			if thisSkelet.saveStartFrames.count<minCount then minCount=thisSkelet.saveStartFrames.count
			if thisSkelet.saveEndFrames.count<minCount then minCount=thisSkelet.saveEndFrames.count
			if thisSkelet.saveSkipFrames.count<minCount then minCount=thisSkelet.saveSkipFrames.count
			if thisSkelet.saveStitchFinals.count<minCount then minCount=thisSkelet.saveStitchFinals.count
		)
 		for i=1 to minCount do(
			names =thisSkelet.saveAnimClipNames[i]
			newstring=names
			if names.Count>6 then(
				newstring=substring names 1 5
				append newstring "..."
			)
			if names.count<=6 then (
				for i=newstring.count to 6 do append newstring "  "  )
			newstart=(thisSkelet.saveStartFrames[i]) as string
			newCnt=newstart.count
			if newCnt<6 then (
				for i=newstart.count to 4 do append newstart "  "  )
			else if newCnt>6 then (
				newstart=substring newstart 1 5
				append newstart ".."
			)	
			newend=(thisSkelet.saveEndFrames[i]) as string
			newCnt=newend.count
			if newCnt<6 then (
				for i=newend.count to 4 do append newend "  "  )
			else if newCnt>6 then (
				newend=substring newend 1 5
				append newend ".."
			)
			for i=newend.count to 4 do append newend "  "
				
 			newname=newstring+"  "+newstart+"  "+newend
 			thisSkelet.AnimClipsRollout.targetAnimClipsList.items = append thisSkelet.AnimClipsRollout.targetAnimClipsList.items newname
			thisSkelet.AnimClipsRollout.targetAnimClipsList.selection=saveSelectedAnimClip=row+1
 		)
	)	
	
	fn createClip thisModifier animName animID animStart animEnd animSkip animStitchFinal = (	
		lastEndFrame=-1
		for endFrame in thisModifier.saveEndFrames do(
			if endFrame>lastEndFrame then lastEndFrame=endFrame
		)
		lastEndFrame+=1
		append thisModifier.saveAnimClipNames animName
		append thisModifier.saveAnimSourceIDs animID
		append thisModifier.saveStartFrames  lastEndFrame
		append thisModifier.saveEndFrames (lastEndFrame+(animEnd-animStart))
		append thisModifier.saveSkipFrames animSkip
		append thisModifier.saveStitchFinals animStitchFinal
		--newModifier.saveStitchFinals.count
		updateUI thisModifier 0		
		return lastEndFrame
		
	)
	-- choose a new AWDAnimator as target Animator for a AWDObjectSettings - called from AWDObjectSettings
	fn chooseSkeleton thisObj thisModifier = (		
			
		global animDisplayNames=#()
		global animSets=#()	
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		foundAny=false
		cnt=1
		for i in objects do(
			for m in i.modifiers do(
				if (classof m as string)=="AWDSkeleton" then(
					foundAny=true	
					if thisMod.target_ID==m.thisAWDID then(
						originalIdx=cnt
					)
					cnt+=1
					appendThis= (appendIfUnique animSets m)
					if appendThis then append animDisplayNames ("'"+m.name+"'  object: "+i.name) 						
						
				)
			)
		)
		
		if animSets.count>0 then(
			rollout select_animSet_rollout "Select a AWDSkeleton-Modifier"	(
				listbox base_name "AWDSkeleton-Modifier available in scene"  height:16 items:animDisplayNames selection:originalIdx
				on base_name selected selected do (
					thisMod.target_ID = animSets[base_name.selection].thisAWDID

					append animSets[base_name.selection].savedClones thisMod
					modPanel.SetCurrentObject animSets[base_name.selection]
					--thisMod.animSetsSelected=animSets.Count
					modPanel.SetCurrentObject thisMod
					DestroyDialog select_animSet_rollout
				)
			)
			--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
			createDialog select_animSet_rollout 300 240  modal:true
		)
		else	(
			if foundAny then	messagebox("The AWDSkeleton allready is assigned to all available AWDAnimationSets (Skeleton) in the scene.")
			else messagebox("No AWDAnimationSets-modifier with of type='Skeleton' could be found in the scene")
		)
	)	
	
	
	fn checkParent thisMesh = (
		if thisMesh != undefined then(
			for m in thisMesh.modifiers do(
				if classof m as string == "AWDSkeleton" then messagebox("Found a AWDSkeleton-modifier one of the parents.")
			)
			checkParent thisMesh.parent			
		)
	)
	 
	fn constructHirarchyObjList thisMesh = (
		thisList=#(thisMesh)
		for child in thisMesh.children do(
			returnList=constructHirarchyObjList child 
			for this in returnList do(
				append thisList this
			)
		)
		return thisList
	)
-- 	fn time_cb = (
-- 		global retargetingSources
-- 		global retargetingTargets
-- 		if retargetingSources !=undefined and retargetingTargets!=undefined  then(
-- 			if retargetingSources.count == retargetingTargets.count then (
-- 				for i=1 to retargetingSources.count do(
-- 					if retargetingTargets[i].count==retargetingSources[i].count then (
-- 						for objIdx=1 to retargetingSources[i].count do(
-- 							retargetingTargets[i][objIdx].transform=retargetingSources[i][objIdx].transform
-- 						)						
-- 					)
-- 				)
-- 			)
-- 		)
-- 		print currentTime
-- 	)
-- 	fn tcb = time_cb()
	fn constructHirarchyList thisMesh = (
		thisList=#(thisMesh.children.count)
		for child in thisMesh.children do(
			for m in child.modifiers do(
				if classof m as string == "AWDSkeleton" then messagebox("Found a AWDSkeleton-modifier furter down the hirarchy.")
			)
			returnList=constructHirarchyList child 
			for this in returnList do(
				append thisList this
			)
		)
		return thisList
	)
		
	parameters skeletonParams rollout:skeletonRollout
	(
		thisAWDID type:#string default:"noIDSet"
		jointCnt type:#string default:"valid: no target set" animatable:false
		hirarchyString type:#string animatable:false
		target_ID type:#string animatable:false		
	)
	rollout skeletonRollout "AWD Skeleton Clone" category:1
	(			
		group "Target Skeleton"(
			edittext txt_TargetSkel "" readOnly:true
			button btn_choose "choose" width:45 across:3
			button btn_select "select"  width:45 
			button btn_clear "clear"  width:45  animatable:false
		)
		label SPACER_1 "" height:5
		label lbl_jointCnt "" align:#left
		label SPACER_2 "" height:5
		on btn_choose pressed do(
			curMod=modPanel.GetCurrentObject()
			chooseSkeleton selection[1] curMod
		)
		
		on btn_select pressed do(
			curMod=modPanel.GetCurrentObject()
			exitThis=false
			if target_ID != "noIDSet" then (
				for i in objects do(
					for m in i.modifiers do(
						if (classof m as string)=="AWDSkeleton" then(
							if curMod.target_ID==m.thisAWDID then(
								skelCloneCnt=0
								for skelClone in m.savedClones do(							
									skelCloneCnt+=1
									if skelClone==curMod then (
										m.selected_clone=skelCloneCnt
										exit	
									)
								)
								modPanel.setCurrentObject m
								exitThis=true
								exit
													
							)
						)
					)
					if exitThis then exit
				)
			)
			btn_select.enabled=true
			if not exitThis then (
				target_ID="noIDSet"
				target_Name=""
				txt_TargetSkel.text=""
				btn_select.enabled=false
			)
		)
		on btn_clear pressed do(
			target_ID="noIDSet"
			target_Name=""
			txt_TargetSkel.text=""
		)
		on skeletonRollout open do (
			curMod=modPanel.GetCurrentObject()
			lbl_jointCnt.text=jointCnt
			exitThis=false
			if target_ID != "noIDSet" then (
				for i in objects do(
					for m in i.modifiers do(
						if (classof m as string)=="AWDSkeleton" then(
							if curMod.target_ID==m.thisAWDID then(
								txt_TargetSkel.text=m.name
								exitThis=true
								exit
													
							)
						)
					)
					if exitThis then exit
				)
			)
			btn_select.enabled=true
			if not exitThis then (
				target_ID="noIDSet"
				target_Name=""
				txt_TargetSkel.text=""
				btn_select.enabled=false
			)		
		)
	)	
	parameters AnimClipsparams rollout:AnimClipsRollout
	(
		
		saveSelectedAnimClip type:#integer default:1
		saveAnimClipNames type:#stringTab tabSize:0 tabSizeVariable:true
		saveAnimSourceIDs type:#stringTab tabSize:0 tabSizeVariable:true
		saveStartFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveEndFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveSkipFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveActiveStates type:#boolTab tabSize:0 tabSizeVariable:true
		saveStitchFinals type:#boolTab tabSize:0 tabSizeVariable:true
		
	) 
	rollout AnimClipsRollout "Animation Clips" category:2		
	(				
		label lbl_listHeader " Name    Start       End"    width:140 pos:[8,10]
		listbox targetAnimClipsList "" items:#()   width:150 pos:[6,28]
		button btn_newFromRange "New Clip from Range" width:135 pos:[13,170]
		button btn_SetRange "Set Range from Clip" width:135 pos:[13,200]
		button btn_SetRangetoClip "Set Range to Clip" width:135 pos:[13,230]
		button btn_AWDClipManager "ClipManager" width:135 pos:[13,260]
		button btn_bakeToTarget "Bake to target skeleton" width:135 pos:[13,290]
		
		on btn_bakeToTarget pressed do (
			curMod=modPanel.GetCurrentObject()
			bakeToTargetSkeleton curMod
		)
		on targetAnimClipsList selected curState do(
			print targetAnimClipsList.selection
		)
		
		on btn_AWDClipManager pressed do(
			curMod=modPanel.GetCurrentObject()			
			global openAWDClipManager
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimationClipManager.ms")					
			openAWDClipManager curMod "Skeleton" targetAnimClipsList.selection
		)
		on cbx_anim_only changed curState do(
			if curState then (
				SPN_jointPerVert.enabled=false
			)
			else (
				SPN_jointPerVert.enabled=true
			)
		)			
		on btn_newFromRange pressed do (
			curMod=modPanel.GetCurrentObject()
			firstframe=0
			lastframe=0
			valid=false
			try(
				append saveStartFrames animationRange.start
				append saveEndFrames animationRange.end
				append saveAnimClipNames "new clip"
				append saveAnimSourceIDs ""
				append saveSkipFrames 0
				append saveActiveStates true
				append saveStitchFinals true
				valid=true
			)
			catch()
			if not valid then messageBox("Error - clip was not added")
			curMod=modPanel.GetCurrentObject()
			updateUI curMod targetAnimClipsList.selection
			targetAnimClipsList.selection=(targetAnimClipsList.items.count)
		)
		on btn_SetRange pressed do (
			curMod=modPanel.GetCurrentObject()
			firstframe=0
			lastframe=0
			valid=false
			try(
				firstframe=saveStartFrames[targetAnimClipsList.selection]
				lastframe=saveEndFrames[targetAnimClipsList.selection]
				valid=true
			)
			catch()
			if firstframe<lastframe and valid then animationRange = interval firstframe lastframe --Set range to frames 10 to 50
			else messageBox("Invalid time-range set for the selected clip")
			curMod=modPanel.GetCurrentObject()
			updateUI curMod (targetAnimClipsList.selection-1)
		)
		on btn_SetRangetoClip pressed do (
			valid=false
			try(
				saveStartFrames[targetAnimClipsList.selection]=animationRange.start
				saveEndFrames[targetAnimClipsList.selection]=animationRange.end
				valid=true
			)
			catch()
			if not valid then messageBox("Error - time range was not set for the clip")
			curMod=modPanel.GetCurrentObject()
			updateUI curMod (targetAnimClipsList.selection-1)
		)
		on AnimClipsRollout open do (	
			curMod=modPanel.GetCurrentObject()
			print (saveStartFrames.count as string)
			print "yes2"
			updateUI curMod (saveSelectedAnimClip-1)
		)
	)
	on create do (
		if thisAWDID=="noIDSet" then(
			thisAWDID=getAWDID()
		)
		--name = uniquename "AWDSkeleton"
		--checkParent selection[1].parent
		--hirarchyList=constructHirarchyList selection[1]
		--firstList=constructHirarchyObjList selection[1]
		--secondList=constructHirarchyObjList objects[1]
		
		--global retargetingSources=#(firstList)
		--global retargetingTargets=#(secondList)
		--if hirarchyList.count<2 then 	print "toDo the skeleton contains no joints. its not valid. delete!"
		--jointCnt = "Joints: "+hirarchyList.count as string
		--if thisAWDID=="noIDSet" then thisAWDID=getAWDID()
		--append savedInfos ("no information available"+thisAWDID as string)
	)
	on clone orig do thisAWDID=getAWDID()

)