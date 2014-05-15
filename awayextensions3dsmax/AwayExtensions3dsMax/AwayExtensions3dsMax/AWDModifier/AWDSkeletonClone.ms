plugin modifier AWDSkeletonClone
name:"AWDSkeletonClone"
category:"AWDModifier"
classID:#(0x15236fca, 0x19623ce0)
extends:EmptyModifier replaceUI:false version:1
(	
	
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
		
	fn bakeToTargetSkeleton thisSkeleton = (
		animName=0
		animID=0
		animStart=0
		animEnd=0
		animSkip=0
		if thisSkeleton.saveSelectedAnimClip>0 then (
			if thisSkeleton.saveSelectedAnimClip<=thisSkeleton.saveAnimClipNames.count then (
				animName=thisSkeleton.saveAnimClipNames[thisSkeleton.saveSelectedAnimClip]
				animID=thisSkeleton.saveAnimSourceIDs[thisSkeleton.saveSelectedAnimClip]
				animStart=thisSkeleton.saveStartFrames[thisSkeleton.saveSelectedAnimClip]
				animEnd=thisSkeleton.saveEndFrames[thisSkeleton.saveSelectedAnimClip]
				animSkip=thisSkeleton.saveSkipFrames[thisSkeleton.saveSelectedAnimClip]
				animStitchFinal=thisSkeleton.saveStitchFinals[thisSkeleton.saveSelectedAnimClip]
				animLoop=thisSkeleton.saveLoops[thisSkeleton.saveSelectedAnimClip]
				animTransform=thisSkeleton.saveTransform[thisSkeleton.saveSelectedAnimClip]
			)
		)
		if animStart==animEnd then (
			messageBox("No valid animationclip is selected")
			return()
		)
			
		rootObj=selection[1]
		targetRootObj=undefined
		targetModifier=undefined
		if thisSkeleton.target_ID!="noIDSet" then (
			for obj in objects do(
				for m in obj.modifiers do(
					if (classof m as string)=="AWDSkeleton" then (
						if m.thisAWDID==thisSkeleton.target_ID then (
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
		targetStartFrame=targetModifier.createClip targetModifier animName animID animStart animEnd animSkip animStitchFinal animLoop animTransform
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
		deleteItem  thisSkeleton.saveAnimClipNames thisSkeleton.saveSelectedAnimClip			
		deleteItem  thisSkeleton.saveAnimSourceIDs thisSkeleton.saveSelectedAnimClip
		deleteItem  thisSkeleton.saveStartFrames thisSkeleton.saveSelectedAnimClip
		deleteItem  thisSkeleton.saveEndFrames thisSkeleton.saveSelectedAnimClip
		deleteItem  thisSkeleton.saveSkipFrames thisSkeleton.saveSelectedAnimClip
		deleteItem  thisSkeleton.saveStitchFinals thisSkeleton.saveSelectedAnimClip	
		deleteItem  thisSkeleton.saveLoops thisSkeleton.saveSelectedAnimClip
		deleteItem  thisSkeleton.saveTransform thisSkeleton.saveSelectedAnimClip		
		thisSkeleton.saveSelectedAnimClip-=1
		thisInterval = (interval animStart animEnd)
		deleteKeysByRangeObject rootObj thisInterval
		modPanel.SetCurrentObject targetModifier
		
		
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
		saveStitchFinals type:#boolTab tabSize:0 tabSizeVariable:true
		saveLoops type:#boolTab tabSize:0 tabSizeVariable:true
		saveTransform type:#boolTab tabSize:0 tabSizeVariable:true		
		
	) 
	rollout AnimClipsRollout "Animation Clips" category:2	
	(				
		label lbl_listHeader " Name    Start       End"    width:140 pos:[8,10]
		listbox targetAnimClipsList "" items:#()   width:150 pos:[6,28]
		button btn_newFromRange "New Clip from Range" width:135 pos:[13,170]
		button btn_SetRange "Set Range from Clip" width:135 pos:[13,200]
		button btn_SetRangetoClip "Set Range to Clip" width:135 pos:[13,230]
		button btn_AWDClipManager "ClipManager" width:135 pos:[13,260]
		button btn_bakeToClone "Bake to clone" width:135 pos:[13,290]
		on btn_bakeToClone pressed do(
			curMod=modPanel.GetCurrentObject()
			bakeToSelectedClone curMod
		)
		on targetAnimClipsList selected curState do(
			print targetAnimClipsList.selection
		)
		
		on btn_AWDClipManager pressed do(
			curMod=modPanel.GetCurrentObject()
			global openAWDClipManager
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimationClipManager.ms")
			saveSelectedAnimClip=targetAnimClipsList.selection
			openAWDClipManager curMod "Vertex" targetAnimClipsList.selection
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
			irowCnt=saveAnimClipNames.count+1
			theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDAnimationClip #"+irowCnt as string)
			theobj.text ="Add name for new AWDAnimationClip"
			DialogResult = theObj.Showmodal()
			dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
			result = theobj.InstanceName
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
			try(
				append saveAnimClipNames result
				append saveAnimSourceIDs (getAWDID() as string)
				append saveStartFrames animationRange.start
				append saveEndFrames animationRange.end
				append saveSkipFrames 0
				append saveStitchFinals false
				append saveLoops false
				append saveTransform false
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
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
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
	on clone orig do (		
		fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
		thisAWDID=getAWDID()
	)

)