plugin modifier AWDAnimationSet
name:"AWDAnimationSet"
category:"AWDModifier"
classID:#(0x644136ba, 0x42ce748b)
extends:EmptyModifier replaceUI:false version:1
(		
	
	fn getAWDID = (
		newTime=getUniversalTime()
		newString="awd"+newTime[1] as string+newTime[2] as string+newTime[3] as string+newTime[4] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		--newString=newString+random 0.0 10000.0 as string
		return newString
	)
	
	fn chooseVertex thisObj thisModifier = (	
		global availableModsDisplayNames=#()
		global availableModsNamesList=#()
		global availableModsList=#()
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		foundThis=false
		for i in objects do (
			for m in i.modifiers do(
				if (classof m as string)=="AWDVertexAnimSource" then(
					if sourceVertexID != "noIDSet" then if m.thisAWDID==sourceVertexID then (
						foundThis=true
						originalIdx=(availableModsList.count+1)
					)
					appendThis= (appendIfUnique availableModsList m)
					if appendThis then(
						append availableModsDisplayNames ("'"+m.name+"'  object: "+i.name) 
					)
				)
			)
		)
		if availableModsList.count==1 and foundThis then messagebox("The only existing AWDVertexAnimSource-modifier is allready assigned")
		else if availableModsList.count>0 then (
			rollout select_mesh_rollout "Select a AWDVertexAnimSource-Modifier"	(
				listbox base_name "AWDVertexAnimSource-Modifier available in scene"  height:16 items:availableModsDisplayNames selection:originalIdx
				on base_name selected selected do (
					thisMod.sourceVertexID=availableModsList[base_name.selection].thisAWDID
					thisMod.params.txt_source.text = availableModsList[base_name.selection].name
					append availableModsList[base_name.selection].saveAnimSets thisMod
					thisMod.params.btn_selectSource.enabled=true
					thisMod.params.btn_clearSource.enabled=true
					DestroyDialog select_mesh_rollout
				)
			)
			createDialog select_mesh_rollout 300 240  modal:true
		)
		else	messagebox("No AWDVertexAnimSource-modifier found in the scene")	
	)
	
	fn chooseSkeleton thisObj thisModifier = (	
		global availableModsDisplayNames=#()
		global availableModsNamesList=#()
		global availableModsList=#()
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		foundThis=false
		for i in objects do (
			for m in i.modifiers do(
				if (classof m as string)=="AWDSkeleton" then(
					if thisMod.sourceSkeletonID != "noIDSet" then if m.thisAWDID==thisMod.sourceSkeletonID then (
						foundThis=true
						originalIdx=(availableModsList.count+1)
					)
					appendThis= (appendIfUnique availableModsList m)
					if appendThis then(
						append availableModsDisplayNames ("'"+m.name+"'  object: "+i.name) 
					)
				)
			)
		)
		if availableModsList.count==1 then if foundThis then messagebox("The only existing AWDSkeleton-modifier is allready assigned")
		else if availableModsList.count>0 then (
			rollout select_mesh_rollout "Select a AWDSkeleton-Modifier"	(
				listbox base_name "AWDSkeleton-Modifier available in scene"  height:16 items:availableModsDisplayNames selection:originalIdx
				on base_name selected selected do (
					thisMod.sourceSkeletonID=availableModsList[base_name.selection].thisAWDID
					
					print thisMod.sourceSkeletonID
					thisMod.params.txt_source.text = availableModsList[base_name.selection].name
					append availableModsList[base_name.selection].saveAnimSets thisMod
					thisMod.params.btn_selectSource.enabled=true
					thisMod.params.btn_clearSource.enabled=true
					DestroyDialog select_mesh_rollout
				)
			)
			--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
			createDialog select_mesh_rollout 300 240  modal:true
		)
		else	messagebox("No AWDSkeleton-modifier found in the scene")		
		
	)
	-- choose a new AWDAnimator as target Animator for a AWDObjectSettings - called from AWDObjectSettings
	fn chooseAnimator thisObj thisModifier = (	
		global availableModsDisplayNames=#()
		global availableModsNamesList=#()
		global availableModsList=#()
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		--global to allow to access vars from dialog-rollout.			
		foundAny=false
		for i in objects do(
			for m in i.modifiers do(
				if (classof m as string)=="AWDAnimator" then(
					foundAny=true
					--check if this AWDAnimator is allready assigned to the AnimSet						
					addToList=true						
					for allreadyInList in thisMod.saveAnimators do(
						if allreadyInList == m then(			
							addToList=false
							exit
						)	
					)
					if addToList then (
						appendThis= (appendIfUnique availableModsList m)
						if appendThis then(
							append availableModsDisplayNames ("'"+m.name+"'  object: "+i.name) 
						)
					)
				)
			)
		)		
		if availableModsList.count>0 then(
			rollout select_animator_rollout "Select a AWDAnimator Modifier"	(
				listbox base_name "AWDAnimator-Modifier available in scene"  height:16 items:availableModsDisplayNames selection:originalIdx
				on base_name selected selected do (
					append thisMod.saveAnimators availableModsList[base_name.selection]
					availableModsList[base_name.selection].AnimationSetID=thisMod.thisAWDID
					print thisMod.thisAWDID
					modPanel.SetCurrentObject availableModsList[base_name.selection]
					modPanel.SetCurrentObject thisMod
					DestroyDialog select_animator_rollout
				)
			)
			--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
			createDialog select_animator_rollout 300 240  modal:true
		)
		else	(
			if foundAny then	messagebox("This AWDAnimationSet is allready assigned to all the AWDAnimators in the scene.")
			else messagebox("No AWDAnimator-modifier found in the scene")
		)
	)

	parameters main rollout:params
	(
		thisAWDID type:#string default:"noIDSet"
		
		animType type:#integer default:1 ui:rdoGrp_animType
		
		sourceSkeletonID type:#string default:"noIDSet"
		sourceVertexID type:#string default:"noIDSet"
		
		simpleMode type:#boolean default:true ui:cbx_simpleMode
		
		saveSelectedAnimClip type:#integer default:1
		saveAnimClipNames type:#stringTab tabSize:0 tabSizeVariable:true
		saveAnimSourceIDs type:#stringTab tabSize:0 tabSizeVariable:true
		saveStartFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveEndFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveSkipFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveActiveStates type:#boolTab tabSize:0 tabSizeVariable:true
		saveStitchFinals type:#boolTab tabSize:0 tabSizeVariable:true
		--nodeTab
	)
	rollout params "AWD Animation Set"
	(		
		label lbl_AnimType "Anim-Type:" width:65 pos:[10,13]
		dropDownList rdoGrp_animType "" items:#("Skeleton", "Vertex") width:80 pos:[70,10]
		label lbl_1 "" height:2 
		label lbl_animSource "Skeleton Animation Source"
		editText  txt_source "" readOnly:true width:130 
		button btn_newSource "New"   width:62 across:2
		button btn_addSource "Choose"   width:62 
		button btn_selectSource "Select"  width:62 across:2
		button btn_clearSource"Clear"   width:62
		label lbl_2 "" height:2 
		checkBox cbx_simpleMode "Use all connected clips" enabled:false
		button btn_OpenAnimClipManager "Open Clip Manager" width:130 
		
		on cbx_simpleMode changed thisValue do (
			btn_OpenAnimClipManager.enabled=true
			if simpleMode then btn_OpenAnimClipManager.enabled=false
		)
		
		on btn_newSource pressed do(
			curAnimSetMod=modPanel.GetCurrentObject()
			if animType==1 then (
				global AWDSkeleton
				MyNewModifier = AWDSkeleton()
				MyNewModifier.name = "AWD Skeleton"
				append MyNewModifier.saveAnimSets curAnimSetMod
				sourceSkeletonID=MyNewModifier.thisAWDID
				addmodifier  selection[1] MyNewModifier
				modPanel.SetCurrentObject curAnimSetMod
				btn_selectSource.enabled=true
				btn_clearSource.enabled=true
				txt_source.text = MyNewModifier.name 
			)
			else if animType==2 then (
				global AWDVertexAnimSource
				MyNewModifier = AWDVertexAnimSource()
				MyNewModifier.name = "AWD AWDVertexAnimSource"
				append MyNewModifier.saveAnimSets curAnimSetMod
				sourceVertexID=MyNewModifier.thisAWDID
				addmodifier  selection[1] MyNewModifier
				modPanel.SetCurrentObject curAnimSetMod
				btn_selectSource.enabled=true
				btn_clearSource.enabled=true
				txt_source.text = MyNewModifier.name 
			)
			btn_newSource.enabled=false
		)
		on btn_addSource pressed do (
			curMod=modPanel.getCurrentObject()
			if animType==1 then chooseSkeleton selection[1] curMod
			else chooseVertex selection[1] curMod
			btn_selectSource.enabled=true
			btn_clearSource.enabled=true
		)
		on btn_selectSource pressed do (
			curMod=modPanel.getCurrentObject()
			if animType==1 then (
				if sourceSkeletonID!="noIDSet" then (
					for i in objects do(
						modifierCnt=1
						for m in i.modifiers do(
							if (classof m as string)=="AWDSkeleton" then(
								if m.thisAWDID==sourceSkeletonID then (
									newIdx=1
									animFound=false
									for oneAnimSet in m.saveAnimSets do(
										if oneAnimSet==curMod then(
											animFound=true
											m.animSetsSelected=newIdx
											exit
										)
										newIdx+=1
									)
									if not animFound then (
										append m.saveAnimSets curMod
									)
									modPanel.setCurrentObject m
									exit
								)
								modifierCnt+=1
							)
						)
					)
				)
			)
			else if animType==2 then (
				if sourceVertexID!="noIDSet" then (
					for i in objects do(
						modifierCnt=1
						for m in i.modifiers do(
							if (classof m as string)=="AWDVertexAnimSource" then(
								if m.thisAWDID==sourceVertexID then (
									newIdx=1
									animFound=false
									for oneAnimSet in m.saveAnimSets do(
										if oneAnimSet==curMod then(
											animFound=true
											m.animSetsSelected=newIdx
											exit
										)
										newIdx+=1
									)
									if not animFound then (
										append m.saveAnimSets curMod
									)
									modPanel.setCurrentObject m
									exit
								)
								modifierCnt+=1
							)
						)
					)
				)
			)
		)
		on btn_clearSource pressed do (	
 			if animType==1 then sourceSkeletonID = "noIDSet"
 			else if animType==2 then sourceVertexID = "noIDSet"
			txt_source.text = ""
			btn_selectSource.enabled=false
			btn_clearSource.enabled=false
		)
		on rdoGrp_animType selected thisState do (
 			btn_selectSource.enabled=true
 			btn_clearSource.enabled=true
 			btn_newSource.enabled=false
 			foundSkel=0
 			foundVertex=0
			txt_source.text = ""
 			for m in selection[1].modifiers do(
 				if classof m as string=="AWDSkeleton" then foundSkel+=1
 				if classof m as string=="AWDVertexAnimSource" then foundVertex+=1
 			)
 			if thisState==1 then(
				lbl_animSource.text="Skeleton Animation Source"
 				if foundSkel==0 then btn_newSource.enabled=true
				if sourceSkeletonID != "noIDSet" then(
					isFound=false
					for i in objects do(
						for m in i.modifiers do(
							if m.thisAWDID==sourceSkeletonID then (
								isFound=true				
								sourceSkeletonID=m.thisAWDID
								txt_source.text = m.name
								exit								
							)
						)
						if isFound then (
							exit
						)
					)
				)	
			)
			else if thisState==2 then(
				lbl_animSource.text="Vertex Animation Source"
				if foundVertex==0 then btn_newSource.enabled=true
 				if foundVertex==0 then btn_newSource.enabled=true
				if sourceVertexID != "noIDSet" then(
					isFound=false
					for i in objects do(
						for m in i.modifiers do(
							if m.thisAWDID==sourceVertexID then (
								isFound=true	
								sourceVertexID=m.thisAWDID
								txt_source.text = m.name			
								exit								
							)
						)
						if isFound then (
							exit
						)
					)
				)	
			)
			if txt_source.text=="" then (
				btn_selectSource.enabled=false
				btn_clearSource.enabled=false
			)
		)
		
		on btn_OpenAnimClipManager pressed do(
			curMod=modPanel.GetCurrentObject()
			global openAWDSetManager
			--fileIn (getDir #maxRoot + "\\plugins\\Away3dExtensions3dsMax\\AWDAnimationClipManager.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
					
			openAWDSetManager curMod
		)
		
		on params open do (
			btn_OpenAnimClipManager.enabled=true
			if simpleMode then btn_OpenAnimClipManager.enabled=false
				
			btn_newSource.enabled=false
			btn_selectSource.enabled=true
			btn_clearSource.enabled=true
			foundSkel=0
			foundVertex=0
			for m in selection[1].modifiers do(
				if classof m as string=="AWDSkeleton" then foundSkel+=1
				if classof m as string=="AWDVertexAnimSource" then foundVertex+=1
			)
			txt_source.text = ""
  			if animType==1 then (
				lbl_animSource.text="Skeleton Animation Source"
 				if foundSkel==0 then btn_newSource.enabled=true
				if sourceSkeletonID != "noIDSet" then(
					isFound=false
					for i in objects do(
						for m in i.modifiers do(
							if (classof m as string) == "AWDSkeleton" then (
								if m.thisAWDID==sourceSkeletonID then (
									isFound=true	
									sourceSkeletonID=m.thisAWDID
									txt_source.text = m.name
									exit								
								)
							)
						)
						if isFound then (
							exit
						)
					)
				)				
 			)
 			else if  animType==2 then (
				lbl_animSource.text="Vertex Animation Source"
				newSourceVertexIDs=#()
 				if foundVertex==0 then btn_newSource.enabled=true
				if sourceVertexID != "noIDSet" then(
					isFound=false
					for i in objects do(
						for m in i.modifiers do(
							if m.thisAWDID==sourceVertexID then (
								isFound=true				
								sourceVertexID=m.thisAWDID
								txt_source.text = m.name	
								exit								
							)
						)
						if isFound then (
							exit
						)
					)
				)	
 			)
				
			if txt_source.text == "" then (
				btn_selectSource.enabled=false
				btn_clearSource.enabled=false
			)
			--btn_SelectAnimSet.enabled=true
			--btn_ClearAnimSet.enabled=true
			--if AnimationSetID=="noIDSet" then(
			--	animSetName.text=""
			--	btn_SelectAnimSet.enabled=false
			--	btn_ClearAnimSet.enabled=false
			--)
			--thisMod=modPanel.getCurrentObject()
			--thisObj=selection[1]
			--if thisObj != undefined and thisMod != undefined  then(
			--	exists=0
			--	for m in thisObj.modifiers do	if (classof(m) as string)=="AWDAnimator" then exists=exists+1
			--	if exists>1 then (
			--		deleteModifier thisObj thisMod
			--		messagebox("A object can only have on AWDAnimator-Modifier applied.\n\This object allready contains a AWDModifier!")
			--	)
			--)
		)
	)
	
	parameters AWDAnimatorParams rollout:AWDAnimatorRollout
	(
		saveSelectedAnimator type:#integer default:1
		saveAnimators type:#maxObjectTab tabSize:0 tabSizeVariable:true
		
	)
	rollout AWDAnimatorRollout "Used by AWDAnimators"
	(		
		listbox targetAnimatorsList "" items:#()
		button btn_newAnimator "new"   width:30 pos:[14,150]
		button btn_addAnimator "add"   width:30 pos:[48,150]
		button btn_selectAnimator "sel"  width:30 pos:[83,150]
		button btn_clearAnimator "del"   width:30 pos:[119,150]	
		
 
		on btn_newAnimator pressed do(
			curAnimSetMod=modPanel.GetCurrentObject()
			global AWDAnimator
			MyNewModifier = AWDAnimator()
			MyNewModifier.name = "AWD Animator"
			MyNewModifier.AnimationSetID=curAnimSetMod.thisAWDID
			appendIfUnique saveAnimators MyNewModifier
			saveSelectedAnimator=saveAnimators.count
			addmodifier  selection[1] MyNewModifier
			modPanel.SetCurrentObject curAnimSetMod
			btn_selectAnimator.enabled=true
			btn_clearAnimator.enabled=true
		)
		on btn_addAnimator pressed do(
			chooseAnimator selection[1] (modPanel.getCurrentObject())
		)
		on btn_selectAnimator pressed do(
			if targetAnimatorsList.items.count > 0 and targetAnimatorsList.selection > 0 do (
				modPanel.setCurrentObject  saveAnimators[targetAnimatorsList.selection]
			)
		)
		on targetAnimatorsList selected curIndex do(
			saveSelectedAnimator=curIndex
		)
		--Remove currently selected item from the list:
		on btn_clearAnimator pressed do
		(
			if targetAnimatorsList.items.count > 0 and targetAnimatorsList.selection > 0 then (			
				saveAnimators = deleteItem saveAnimators targetAnimatorsList.selection
				targetAnimatorsList.items = deleteItem targetAnimatorsList.items targetAnimatorsList.selection
			)
			if targetAnimatorsList.items.count == 0 then (
				btn_selectAnimator.enabled=false
				btn_clearAnimator.enabled=false
			)
		)
		on AWDAnimatorRollout open do (
				--lbx_sourceList.selection=saveSelectedAnimator
			for oneAnimator in saveAnimators do targetAnimatorsList.items = append targetAnimatorsList.items oneAnimator.name

			tmpList=#()
			tmpListObjs=#()
			for oneObj in saveAnimators do(
				if oneObj.AnimationSetID==thisAWDID then (
					append tmpList oneObj.name
					append tmpListObjs oneObj
				)
			)
			saveAnimators=tmpListObjs
			targetAnimatorsList.items = tmpList
			if targetAnimatorsList.items.count < saveSelectedAnimator then saveSelectedAnimator=targetAnimatorsList.items.count
			if targetAnimatorsList.items.count > 0 then targetAnimatorsList.selection=saveSelectedAnimator
			
			btn_selectAnimator.enabled=true
			btn_clearAnimator.enabled=true
			if saveAnimators.count==0 then(
				btn_selectAnimator.enabled=false
				btn_clearAnimator.enabled=false
			)
		)
	)
	
	on create do (
		--curAnimSetMod=modPanel.GetCurrentObject()
		--delegate.name = uniquename "AWD Anim Set"
		if thisAWDID=="noIDSet" then(
			thisAWDID=getAWDID()
		)
	)
	on clone orig do (				
		thisAWDID=getAWDID()		
	)

)