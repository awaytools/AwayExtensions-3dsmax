plugin modifier AWDVertexAnimSourceClone
name:"AWDVertexAnimSourceClone"
category:"AWDModifier"
classID:#(0x7e8f1ceb, 0x24bb35d2)
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
	fn updateUI thisSkelet row= (
		thisSkelet.AnimClipsRollout.targetAnimClipsList.items=#()
		valid=true
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveAnimSourceIDs.count then valid=false
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveStartFrames.count then valid=false
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveEndFrames.count then valid=false
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveSkipFrames.count then valid=false
		if thisSkelet.saveAnimClipNames.count!=thisSkelet.saveStitchFinals.count then valid=false
		minCount=thisSkelet.saveAnimClipNames.count
		if not valid then(	
			if thisSkelet.saveAnimClipNames.count<minCount then minCount=thisSkelet.saveAnimClipNames.count
			if thisSkelet.saveAnimSourceIDs.count<minCount then minCount=thisSkelet.saveAnimSourceIDs.count
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
	
	fn chooseVertexAnimSource thisObj thisModifier = (		
			
		global animDisplayNames=#()
		global animSets=#()	
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		foundAny=false
		cnt=1
		for i in objects do(
			for m in i.modifiers do(
				if (classof m as string)=="AWDVertexAnimSource" then(
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
			rollout select_animSet_rollout "Select a AWDVertexAnimSource-Modifier"	(
				listbox base_name "AWDVertexAnimSource-Modifier available in scene"  height:16 items:animDisplayNames selection:originalIdx
				on base_name selected selected do (
					thisMod.target_ID = animSets[base_name.selection].thisAWDID
					thisMod.target_Name = animSets[base_name.selection].name	

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
			if foundAny then	messagebox("The AWDVertexAnimSource allready is assigned to all available AWDAnimationSets (Skeleton) in the scene.")
			else messagebox("No AWDVertexAnimSource-modifier with of type='Skeleton' could be found in the scene")
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
		
	parameters targetParams rollout:targetRollout
	(
		thisAWDID type:#string default:"noIDSet"
		jointCnt type:#string default:"valid: no target set" animatable:false
		target_ID type:#string animatable:false
		target_Name type:#string animatable:false ui:txt_TargetSkel
		
	)
	rollout targetRollout "AWD Vertex Anim Source Clone" category:1
	(			
		group "Target Vertex Anim"(
			edittext txt_TargetSkel "" readOnly:true animatable:false
			button btn_choose "choose" width:45 across:3 animatable:false
			button btn_select "select"  width:45  animatable:false
			button btn_clear "clear"  width:45  animatable:false
		)
		label SPACER_1 "" height:5
		label lbl_jointCnt "" align:#left
		label SPACER_2 "" height:5
		on btn_choose pressed do(
			curMod=modPanel.GetCurrentObject()
			chooseVertexAnimSource selection[1] curMod
		)
		
		on btn_select pressed do(
			curMod=modPanel.GetCurrentObject()
			exitThis=false
			if target_ID != "noIDSet" then (
				for i in objects do(
					for m in i.modifiers do(
						if (classof m as string)=="AWDVertexAnimSource" then(
							if curMod.target_ID==m.thisAWDID then(
								vertexCloneCnt=0
								for vertexClone in m.savedClones do(							
									vertexCloneCnt+=1
									if vertexClone==curMod then (
										m.selected_clone=vertexCloneCnt
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
			if not exitThis then (
				target_ID="noIDSet"
				target_Name=""
				txt_TargetSkel.text=""
			)
		)
		on btn_clear pressed do(
			target_ID="noIDSet"
			target_Name=""
			txt_TargetSkel.text=""
		)
		on targetRollout open do (
-- 			global addAWDModifier
-- 			registerTimeCallback tcb
-- 			try callbacks.removeScripts id:#AWDModDeleteListener catch()
-- 			callbacks.addScript #modPanelSelChanged "addAWDModifier()" id:#AWDModDeleteListener
			lbl_jointCnt.text=jointCnt
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
		on targetAnimClipsList selected curState do(
			print targetAnimClipsList.selection
		)
		
		on btn_AWDClipManager pressed do(
			curMod=modPanel.GetCurrentObject()
			global openAWDClipManager
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimationClipManager.ms")
			saveSelectedAnimClip=targetAnimClipsList.selection
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
			saveSelectedAnimClip=targetAnimClipsList.selection
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