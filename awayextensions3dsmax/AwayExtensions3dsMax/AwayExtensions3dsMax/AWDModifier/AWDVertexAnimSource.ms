plugin modifier AWDVertexAnimSource
name:"AWDVertexAnimSource"
category:"AWDModifier"
classID:#(0x10833805, 0xc1c12f3)
extends:EmptyModifier replaceUI:false version:1
(
	local AnimSetRollout		
	local AnimClonesRollout		

	
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
		mPointCon = obj.baseObject[#Master_Point_Controller]
		if mPointCon != undefined do deleteKeysByRange mPointCon.controller thisInterval

	)	
	
	fn bakeToSelectedClone thisVertexMod = (
		animName=0
		animID=0
		animStart=0
		animEnd=0
		animSkip=0
		animLoops=0
		animTransform=0
		if thisVertexMod.saveSelectedAnimClip>0 then (
			if thisVertexMod.saveSelectedAnimClip<=thisVertexMod.saveAnimClipNames.count then (
				animName=thisVertexMod.saveAnimClipNames[thisVertexMod.saveSelectedAnimClip]
				animID=thisVertexMod.saveAnimSourceIDs[thisVertexMod.saveSelectedAnimClip]
				animStart=thisVertexMod.saveStartFrames[thisVertexMod.saveSelectedAnimClip]
				animEnd=thisVertexMod.saveEndFrames[thisVertexMod.saveSelectedAnimClip]
				animSkip=thisVertexMod.saveSkipFrames[thisVertexMod.saveSelectedAnimClip]
				animStitchFinal=thisVertexMod.saveStitchFinals[thisVertexMod.saveSelectedAnimClip]
				animLoops=thisVertexMod.saveLoops[thisVertexMod.saveSelectedAnimClip]
				animTransform=thisVertexMod.saveTransform[thisVertexMod.saveSelectedAnimClip]
			)
		)
		if animStart==animEnd then (
			messageBox("No valid animationclip is selected")
			return()
		)
		rollout rlProgBar "Bar" width:114
		(
			progressBar pbProgress "" pos:[4,22] width:106 height:15 value:0 color:[0,0,0]		 
		)
		createDialog rlProgBar
		rootObj=selection[1]
		targetRootObj=undefined
		targetModifier=undefined
		create=true
		if thisVertexMod.selected_clone>0 then (
			if thisVertexMod.selected_clone<=thisVertexMod.savedClones.Count then (
				create=false
				targetModifier=thisVertexMod.savedClones[thisVertexMod.selected_clone]
				-- we have the correct modifierm but need to find the object that its applied to:
				--easy way: select the modifier, and check wich object is selected (if multiple are selected, take the first in the list)
				modPanel.SetCurrentObject targetModifier
				targetRootObj=selection[1]
				fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDAnimHelpers.ms")
				targetStartFrame=getNextStartFrame targetModifier
				createClip targetModifier animName animID targetStartFrame (targetStartFrame+(animEnd-animStart)) animSkip animStitchFinal animLoops animTransform
				thisInterval = (interval targetStartFrame (targetStartFrame+(animEnd-animStart)))
				deleteKeysByRangeObject targetRootObj thisInterval
			)
		)
		if create then (
			targetRootObj = copy rootObj	
			deleteKeys targetRootObj #allkeys
			addModifier targetRootObj (Turn_To_Mesh()) --now add Turn To Mesh modifier
			collapseStack targetRootObj --collapse the stack	
			global AWDVertexAnimSourceClone
			targetModifier = AWDVertexAnimSourceClone()
			addModifier targetRootObj targetModifier
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDAnimHelpers.ms")
			targetStartFrame=getNextStartFrame targetModifier
			createClip targetModifier animName animID animStart animEnd animSkip animStitchFinal animLoops animTransform
			deleteKeysByRangeObject targetRootObj thisInterval
			append thisVertexMod.savedClones targetModifier
			targetModifier.target_ID=thisVertexMod.thisAWDID
			targetModifier.targetRollout.txt_TargetSkel.text=thisVertexMod.name
		)
		i=animStart
		timeDifference=targetStartFrame-animStart
		didTheEnd=false	
		
		animate on(
			while i<=animEnd do(
				prog=((i-animStart) as float /(animEnd-animStart) as float)*100
				rlProgBar.pbProgress.value = prog
				rlProgBar.pbProgress.color.r = (255-((255/100)*prog))
				rlProgBar.pbProgress.color.g = ((255/100)*prog)
				rlProgBar.pbProgress.color.b = 0
				
				targetTime=(i+timeDifference) as integer
				verts=#()
				vertCnt=0			
				if classof rootObj == Editable_mesh then(	
					for vert in (rootObj.mesh.verts as bitarray) do(
						vertCnt+=1
						verts[vertCnt]=at time (i as integer) (meshop.getVert rootObj vert)
					)		
				)
				else if classof rootObj == Editable_poly or classof rootObj == PolyMeshObject then(		
					for vert in (rootObj.mesh.verts as bitarray) do(
						vertCnt+=1
						verts[vertCnt]=at time (i as integer) (polyop.getVert rootObj vert)
					)		
				)
				else(
					try(
						for vert in (rootObj.mesh.verts as bitarray) do(
							vertCnt+=1
							verts[vertCnt]=at time (i as integer) (meshop.getVert rootObj vert)
						)		
					)
					catch()
				)
				vertCnt=0
				for vert in (rootObj.mesh.verts as bitarray) do(
					vertCnt+=1
					vertVector=verts[vertCnt]
					at time (targetTime as integer) meshop.setVert targetRootObj vert [(vertVector.x),(vertVector.y),(vertVector.z)]
				)
				mPointCon = targetRootObj.baseObject[#Master_Point_Controller]
				if mPointCon != undefined do addNewKey targetRootObj.baseObject[#Master_Point_Controller] targetTime
				
				forcedEnd=false
				if i==animEnd then didTheEnd=true
					
				i+=1+animSkip
				
				if i>animEnd and not didTheEnd then i=animEnd--do the end
					
				forceCompleteRedraw 
			)
		)
		if thisVertexMod.saveSelectedAnimClip	>0 and thisVertexMod.saveSelectedAnimClip<=thisVertexMod.saveAnimClipNames.Count then(
			deleteItem  thisVertexMod.saveAnimClipNames thisVertexMod.saveSelectedAnimClip			
			deleteItem  thisVertexMod.saveAnimSourceIDs thisVertexMod.saveSelectedAnimClip
			deleteItem  thisVertexMod.saveStartFrames thisVertexMod.saveSelectedAnimClip
			deleteItem  thisVertexMod.saveEndFrames thisVertexMod.saveSelectedAnimClip
			deleteItem  thisVertexMod.saveSkipFrames thisVertexMod.saveSelectedAnimClip
			deleteItem  thisVertexMod.saveStitchFinals thisVertexMod.saveSelectedAnimClip	
			deleteItem  thisVertexMod.saveLoops thisVertexMod.saveSelectedAnimClip	
			deleteItem  thisVertexMod.saveTransform thisVertexMod.saveSelectedAnimClip	
		)
		thisVertexMod.saveSelectedAnimClip-=1
		thisInterval = (interval animStart animEnd)
		--deleteKeysByRangeObject rootObj thisInterval
		modPanel.SetCurrentObject targetModifier
		try destroyDialog rlProgBar catch()
	)	
	-- choose a new AWDAnimator as target Animator for a AWDObjectSettings - called from AWDObjectSettings
	fn chooseAnimationSet thisObj thisModifier = (		
			
		global animDisplayNames=#()
		global animSets=#()	
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		foundAny=false
		for i in objects do(
			for m in i.modifiers do(
				if (classof m as string)=="AWDAnimationSet" then(
					if m.animType==1 then(
						foundAny=true	
						allReadyAssigned=false
						if m.sourceSkeletonID!="noIDSet" then (
							if oneSkelSourceID==thisModifier.thisAWDID then(
								allReadyAssigned=true
								exit
							)
						)
						if not allReadyAssigned then (
							appendThis= (appendIfUnique animSets m)
							if appendThis then append animDisplayNames ("'"+m.name+"'  object: "+i.name) 							
						)
					)
				)
			)
		)
		
		if animSets.count>0 then(
			rollout select_animSet_rollout "Select a AWDAnimationSet-Modifier"	(
				listbox base_name "AWDAnimationSet-Modifier available in scene"  height:16 items:animDisplayNames selection:originalIdx
				on base_name selected selected do (
					append thisMod.saveAnimSets animSets[base_name.selection]		
					animSets[base_name.selection].sourceSkeletonID=thisMod.thisAWDID
					modPanel.SetCurrentObject animSets[base_name.selection]
					thisMod.animSetsSelected=animSets.Count
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
	 
	fn chooseClone thisObj thisModifier = (		
			
		global animDisplayNames=#()
		global animSets=#()	
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		foundAny=false
		for i in objects do(
			for m in i.modifiers do(
				if (classof m as string)=="AWDVertexAnimSourceClone" then(
					foundAny=true	
					allReadyAssigned=false
					if m.target_ID!="noIDSet" then (
						if m.target_ID==thisModifier.thisAWDID then(
							allReadyAssigned=true
						)
					)
					if not allReadyAssigned then (
						appendThis= (appendIfUnique animSets m)
						if appendThis then append animDisplayNames ("'"+m.name+"'  object: "+i.name) 							
					)
				)
			)
		)
		
		if animSets.count>0 then(
			rollout select_animSet_rollout "Select a AWDVertexAnimSourceClone-Modifier"	(
				listbox base_name "AWDVertexAnimSourceClone-Modifier available in scene"  height:16 items:animDisplayNames selection:originalIdx
				on base_name selected selected do (
					append thisMod.savedClones animSets[base_name.selection]		
					animSets[base_name.selection].target_ID=thisMod.thisAWDID
					modPanel.SetCurrentObject animSets[base_name.selection]
					thisMod.selected_clone=animSets.Count
					modPanel.SetCurrentObject thisMod
					thisMod.AnimClonesRollout.btn_selectClone.enabled=true
					thisMod.AnimClonesRollout.btn_clearClone.enabled=true
					DestroyDialog select_animSet_rollout
				)
			)
			--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
			createDialog select_animSet_rollout 300 240  modal:true
		)
		else	(
			if foundAny then	messagebox("All AWDVertexAnimSourceClone-modifier found in scene are allready assigned to this AWDVertexAnimSource.")
			else messagebox("No AWDVertexAnimSourceClone-modifier could be found in the scene")
		)
	)

		
	parameters vertexAnimParams rollout:vertexSourceRollout
	(
		thisAWDID type:#string default:"noIDSet"
		--exportSceneObjs type:#boolean ui:CBX_exportSceneObjs
		pointCnt type:#string default:"Points: 0" animatable:false
		neutralPose type:#integer ui:SPN_neutralPose default:0 animatable:false
		simpleMode type:#boolean ui:CBX_SimpleMode default:true animatable:false 
		simpleMode2 type:#boolean ui:CBX_SimpleMode2 default:true animatable:false
		
	)
	rollout vertexSourceRollout "AWD Vertex Anim Source" category:1
	(			
		label SPACER_1 "" height:5
		label lbl_jointCnt "" align:#left
		button btn_recheck "Recheck Point-Count" width:140
		label SPACER_2 "" height:5
		spinner SPN_neutralPose "Neutral Pose" range:[-9999999,9999999,1] type:#integer align:#right 
		label SPACER_3 "" height:5
		checkBox CBX_SimpleMode "Auto create anim-setup" width:140 enabled:true
		checkBox CBX_SimpleMode2 "Shared Animator" width:140 enabled:true
			
		on btn_recheck open do (
			try(
				pointCntInt=selection[1].numverts
				pointCnt= "Points: "+pointCntInt as string
				lbl_jointCnt.text=pointCnt
			)
			catch(
				messageBox("Warning! Could not get the point-count for this object. Object is not Mesh ?")
			)
		)
		on CBX_SimpleMode changed newValue do (	
			removeRollout AnimClonesRollout
			CBX_SimpleMode2.enabled=true
			if not newValue then (
				CBX_SimpleMode2.enabled=false
				addRollout AnimSetRollout
				addRollout AnimClonesRollout	
			)				
		)
		on vertexSourceRollout open do (
			if pointCnt == "Points: 0" then (
				pointCntInt=selection[1].numverts
				pointCnt= "Points: "+pointCntInt as string
			)
			lbl_jointCnt.text=pointCnt
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
			saveSelectedAnimClip=targetAnimClipsList.selection
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
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
			result = theobj.InstanceName
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
			saveSelectedAnimClip=targetAnimClipsList.selection
			curMod=modPanel.GetCurrentObject()
			updateUI curMod (saveSelectedAnimClip-1)
		)
	)
	parameters AnimSetParams rollout:AnimSetRollout
	(
		
		animSetsSelected type:#integer default:0
		saveAnimSets type:#maxObjectTab tabSize:0 tabSizeVariable:true
	) 
	rollout AnimSetRollout "Assigned to Animation Sets" category:3		
	(		
		listBox targetAnimSetsList "" items:#()
		button btn_newAnimSet "new"   width:30  pos:[13,150]
		button btn_addAnimSet "add"   width:30  pos:[48,150]
		button btn_selectAnimSet "sel"  width:30 pos:[83,150]
		button btn_clearAnimSet "del"   width:30  pos:[118,150]
		on targetAnimSetsList selected curIndex do(
			animSetsSelected=curIndex
		)
		on btn_newAnimSet pressed do (
			thisMesh=selection[1]
			thisMod= modPanel.getCurrentObject()
			MyNewModifier = AWDAnimationSet()
			MyNewModifier.name = "AWD Anim Set"
			addmodifier thisMesh MyNewModifier
			MyNewModifier.sourceSkeletonID=thisMod.thisAWDID
			append saveAnimSets MyNewModifier
			animSetsSelected=saveAnimSets.Count
			modPanel.setCurrentObject thisMod
			
		)
		on btn_addAnimSet pressed do(
			chooseAnimationSet selection[1] (modPanel.getCurrentObject())
		)
		--Remove currently selected item from the list:
		on btn_selectAnimSet pressed do
		(
			if targetAnimSetsList.items.count > 0 and targetAnimSetsList.selection > 0 do (				
				modPanel.setCurrentObject saveAnimSets[targetAnimSetsList.selection]
			)
		)
		--Remove currently selected item from the list:
		on btn_clearAnimSet pressed do
		(
			if targetAnimSetsList.items.count > 0 and targetAnimSetsList.selection > 0 do(
				--todo: delete this AWDSkeleton from the AWDAnimSet
				saveAnimSets = deleteItem saveAnimSets targetAnimSetsList.selection
				targetAnimSetsList.items = deleteItem targetAnimSetsList.items targetAnimSetsList.selection
				animSetsSelected=animSetsSelected-1
			)
 			if targetAnimSetsList.items.count > 0 then (
				if animSetsSelected>targetAnimSetsList.items.count then animSetsSelected=targetAnimSetsList.items.count
 				targetAnimSetsList.selection=animSetsSelected
			)
			
		)
		on AnimSetRollout open do (		
			if simpleMode then removeRollout AnimSetRollout
			else addRollout AnimSetRollout
			addRollout AnimClipsRollout
			--targetAnimClipsList.items=saveAnimClipNames
			--check the targetAnimSet-list
			targetAnimSetsList.items=#()
			newAnimSets=#()
			for oneAnimSet in saveAnimSets do(
				if oneAnimSet.sourceSkeletonID!=undefined then(
 					if oneAnimSet.sourceSkeletonID==thisAWDID then(
 						for i in objects do (
 							hasfound=false
 							for m in i.modifiers do(
 								if m==oneAnimSet then (
 									hasfound=true
									appendThis = (appendIfUnique newAnimSets m)
									if appendThis then targetAnimSetsList.items = append targetAnimSetsList.items (oneAnimSet.name as string)
 									exit
 								)
 							)
 						if hasfound then exit
 						)
					)
				)
			)		
			saveAnimSets=newAnimSets			
 			if targetAnimSetsList.items.count >=animSetsSelected then 
 			(
 				targetAnimSetsList.selection=animSetsSelected
 			)
			if targetAnimSetsList.items.count < animSetsSelected then animSetsSelected=targetAnimSetsList.items.count
			if targetAnimSetsList.items.count > 0 then targetAnimSetsList.selection=animSetsSelected
							
			btn_selectAnimSet.enabled=true
			btn_clearAnimSet.enabled=true
			if AnimationSetID=="noIDSet" then(
				animSetName.text=""
				btn_selectAnimSet.enabled=false
				btn_clearAnimSet.enabled=false
			)
		)
		
	)
	parameters AnimClonesParams rollout:AnimClonesRollout
	(
		
		selected_clone type:#integer default:0
		savedClones type:#maxObjectTab tabSize:0 tabSizeVariable:true
	) 
	rollout AnimClonesRollout "Vertex Source Clones" category:4	
	(		
		listBox targetClonesList "" items:#()
		--button btn_newClone "new"   width:30  pos:[13,150]
		button btn_addClone "add"   width:40  pos:[13,150]
		button btn_selectClone "sel"  width:40 pos:[61,150]
		button btn_clearClone "del"   width:40  pos:[108,150]
		on targetClonesList selected curIndex do(
			selected_clone=curIndex
		)
		on btn_newClone pressed do (
			thisMesh=selection[1]
			thisMod= modPanel.getCurrentObject()
			global AWDVertexAnimSourceClone
			MyNewModifier = AWDVertexAnimSourceClone()
			MyNewModifier.name = "AWD Vertex Anim Clone"
			addmodifier thisMesh MyNewModifier
			MyNewModifier.target_ID=thisMod.thisAWDID
			append savedClones MyNewModifier
			selected_clone=savedClones.Count
			modPanel.setCurrentObject thisMod
			
		)
		on btn_addClone pressed do(
			chooseClone selection[1] (modPanel.getCurrentObject())
		)
		--Remove currently selected item from the list:
		on btn_selectClone pressed do
		(
			if targetClonesList.items.count > 0 and targetClonesList.selection > 0 do (				
				modPanel.setCurrentObject savedClones[targetClonesList.selection]
			)
		)
		--Remove currently selected item from the list:
		on btn_clearClone pressed do
		(
			if targetClonesList.items.count > 0 and targetClonesList.selection > 0 do(
				savedClones = deleteItem savedClones targetClonesList.selection
				targetClonesList.items = deleteItem targetClonesList.items targetClonesList.selection
				selected_clone=selected_clone-1
			)
 			if targetClonesList.items.count > 0 then (
				if selected_clone>targetClonesList.items.count then selected_clone=targetClonesList.items.count
 				targetClonesList.selection=selected_clone
			)
			
		)
		on AnimClonesRollout open do (		
			--targetAnimClipsList.items=saveAnimClipNames
			--check the targetAnimSet-list
			targetClonesList.items=#()
			newClones=#()
			for oneClone in savedClones do(
				if oneClone.target_ID==thisAWDID then(
 					for i in objects do (
 						hasfound=false
 						for m in i.modifiers do(
 							if m==oneClone then (
 								hasfound=true
								appendThis = (appendIfUnique newClones m)
								if appendThis then targetClonesList.items = append targetClonesList.items (oneClone.name as string)
 								exit
 							)
 						)
						if hasfound then exit
					)
				)
			)		
			savedClones=newClones			
 			if targetClonesList.items.count >=selected_clone then 
 			(
 				targetClonesList.selection=selected_clone
 			)
			if targetClonesList.items.count < selected_clone then selected_clone=targetClonesList.items.count
			if targetClonesList.items.count > 0 then targetClonesList.selection=selected_clone
							
			btn_selectClone.enabled=true
			btn_clearClone.enabled=true
			if savedClones.count==0 then(
				btn_selectClone.enabled=false
				btn_clearClone.enabled=false
			)
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