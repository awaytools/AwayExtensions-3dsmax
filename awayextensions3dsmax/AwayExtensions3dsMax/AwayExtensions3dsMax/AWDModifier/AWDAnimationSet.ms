plugin modifier AWDAnimationSet
name:"AWDAnimationSet"
category:"AWDModifier"
classID:#(0x644136ba, 0x42ce748b)
extends:EmptyModifier replaceUI:false version:1
(		
	
	fn openAWDSetManager thisMod = (
		thisObj = selection[1]
		global thisSkelet=thisMod
		newStringList=""
		newStringListObjs=""
		global newClipsDisplay=#()
		global newClipsSourceIDs=#()
		global newClipsIDs=#()
		global newClipsNames=#()
		global selectedAnimClip=1
		global openAWDSetManager
		if thisMod.animType==1 then (
			if thisMod.sourceSkeletonID != "noIDSet" then(
				isFound=false
				for i in objects do(
					for m in i.modifiers do(
						if classof m as string=="AWDSkeleton" then (
							if m.thisAWDID==thisMod.sourceSkeletonID then (
								clipCnt=1
								for oneName in m.saveAnimClipNames do(
									isAppend=appendIfUnique newClipsIDs m.saveAnimSourceIDs[clipCnt]
									if isAppend then (
										append  newClipsDisplay (oneName+" - object: '"+i.name+"'")
										append newClipsSourceIDs m.thisAWDID
										append newClipsNames m.saveAnimClipNames[clipCnt]
									)
									clipCnt+=1
								)		
								for oneClone in m.savedClones do(										
									clipCnt=1
									for oneName in oneClone.saveAnimClipNames do(
										isAppend=appendIfUnique newClipsIDs oneClone.saveAnimSourceIDs[clipCnt]
										if isAppend then (
											append  newClipsDisplay (oneName+" - object: '"+oneClone.name+"'")
											append newClipsSourceIDs oneClone.thisAWDID
											append newClipsNames oneClone.saveAnimClipNames[clipCnt]
										)
										clipCnt+=1										
									)
								)
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
		if thisMod.animType==2 then (
			if thisMod.sourceVertexID != "noIDSet" then(
				isFound=false
				for i in objects do(
					for m in i.modifiers do(
						if classof m as string=="AWDVertexAnimSource" then (
							print m.thisAWDID
							print sourceVertexID
							if m.thisAWDID==thisMod.sourceVertexID then (
								clipCnt=1
								for oneName in m.saveAnimClipNames do(
									isAppend=appendIfUnique newClipsIDs m.saveAnimSourceIDs[clipCnt]
									if isAppend then (
										append  newClipsDisplay (oneName+" - object: '"+i.name+"'")
										append newClipsSourceIDs m.thisAWDID										
										append newClipsNames m.saveAnimClipNames[clipCnt]
									)
									clipCnt+=1										
								)
								for oneClone in m.savedClones do(										
									clipCnt=1
									for oneName in oneClone.saveAnimClipNames do(
										isAppend=appendIfUnique newClipsIDs oneClone.saveAnimSourceIDs[clipCnt]
										if isAppend then (
											append  newClipsDisplay (oneName+" - object: '"+oneClone.name+"'")
											append newClipsSourceIDs oneClone.thisAWDID
											append newClipsNames oneClone.saveAnimClipNames[clipCnt]
										)
										clipCnt+=1										
									)
								)
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
		if thisMod.animType==3 then (
			--todo collect all UVAnimClips
		)
			
		dialogPosition=[0,0];
		try (
			dialogPosition=(GetDialogPos skelInfo_rollout)
			destroyDialog skelInfo_rollout
			) 
		catch (
			hasDialog=false
		)
					global thisSelected
		try destroyDialog rlProgBar catch()
		try destroyDialog skelInfo_rollout catch()
		rollout animSet_rollout "AnimAtion Set Manager" width:114
		(
				listbox targetAnimClipsList "Assigned AnimationClips:" items:#() height:16  width:200 pos:[10,40]
				listbox sourceAnimClipsList "Available AnimationClips:" items:#() height:16  width:400 pos:[220,40]
				button btn_assignClip "Add Selected Clip (insert)" width:195 pos:[220,280]
				--button btn_exchangeClip "Add Selected Clip (overwrite)" width:195 pos:[425,280]
				button btn_removeClip "Remove Selected Clip" width:200 pos:[10,280]
				button btn_openManager "Open AWDClipManager" width:400 pos:[220,10]
				button btn_selectAnimSet "Select AnimSet in mod-panel" width:195 pos:[10,10]
				on btn_openManager pressed do(
					if sourceAnimClipsList.items.count > 0 and sourceAnimClipsList.selection > 0 then (	
						if newClipsSourceIDs[sourceAnimClipsList.selection] != "noIDSet" then(						
							if thisSkelet.animType==1 then (
								isFound2=false
								for i in objects do(
									for m in i.modifiers do(
										if classof m as string=="AWDSkeleton" or classof m as string=="AWDSkeletonClone"  then (
											if m.thisAWDID==newClipsSourceIDs[sourceAnimClipsList.selection] then (
												isFound2=true
												clipCnt=1
												selectedIdx=1
												for oneClip in m.saveAnimSourceIDs do(
													if oneClip==newClipsIDs[sourceAnimClipsList.selection] then	selectedIdx=clipCnt
													clipCnt+=1
												)
												fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimationClipManager.ms")
												openAWDClipManager m "Skeleton" selectedIdx
												exit						
											)
										)
									)
									if isFound2 then (
										exit
									)
								)
							)			
							if thisSkelet.animType==2 then (
								isFound2=false
								for i in objects do(
									for m in i.modifiers do(
										if classof m as string=="AWDVertexAnimSource" or  classof m as string=="AWDVertexAnimSourceClone"  then (
											if m.thisAWDID==newClipsSourceIDs[sourceAnimClipsList.selection] then (
												isFound2=true
												clipCnt=1
												selectedIdx=1
												for oneClip in m.saveAnimSourceIDs do(
													if oneClip==newClipsIDs[sourceAnimClipsList.selection] then selectedIdx=clipCnt
													clipCnt+=1
												)
												fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimationClipManager.ms")
												print (selectedIdx as string)
												print (newClipsSourceIDs[sourceAnimClipsList.selection] as string)
												openAWDClipManager m "Vertex" selectedIdx
												exit						
											)
										)
									)
									if isFound2 then (
										exit
									)
								)
							)	
						)
					)
				)
				on btn_selectAnimSet pressed do(
					max modify mode 
					modPanel.SetCurrentObject thisSkelet
				)
				on btn_removeClip pressed do(
					thisSelected=targetAnimClipsList.selection
					if targetAnimClipsList.items.count > 0 and targetAnimClipsList.selection > 0 then (			
						thisSkelet.saveAnimSourceIDs  = deleteItem thisSkelet.saveAnimSourceIDs  targetAnimClipsList.selection			
						thisSkelet.saveAnimIDs  = deleteItem thisSkelet.saveAnimIDs  targetAnimClipsList.selection			
						thisSkelet.saveAnimClipNames  = deleteItem thisSkelet.saveAnimClipNames  targetAnimClipsList.selection
						targetAnimClipsList.items = deleteItem targetAnimClipsList.items targetAnimClipsList.selection
					)
					global animSetClipsDisplay=#()
					thisCnt=1
					for animSourceID in thisSkelet.saveAnimSourceIDs do(
						if(thisSkelet.saveAnimClipNames[thisCnt]!=undefined) then (
							thisString=thisCnt as string +" "+ thisSkelet.saveAnimClipNames[thisCnt]
							thisCnt+=1
							append animSetClipsDisplay thisString
						)
					)
					if thisSelected<1 or thisSelected>animSetClipsDisplay.Count then thisSelected=1
					animSet_rollout.targetAnimClipsList.items=animSetClipsDisplay		
					if thisSelected>0 and thisSelected<=animSetClipsDisplay.Count then animSet_rollout.targetAnimClipsList.selection=thisSelected
					btn_removeClip.enabled=true
					if targetAnimClipsList.items.count == 0 then (
						btn_removeClip.enabled=false
					)
				)
				on btn_assignClip pressed do(
					global thisSelected=1
					updateThis=false
					if targetAnimClipsList.selection>=targetAnimClipsList.items.Count or targetAnimClipsList.selection<1 then (
						test=appendIfUnique thisSkelet.saveAnimClipNames newClipsNames[sourceAnimClipsList.selection]
						if test then (
							updateThis=true
							append thisSkelet.saveAnimSourceIDs newClipsSourceIDs[sourceAnimClipsList.selection]
							append thisSkelet.saveAnimIDs newClipsIDs[sourceAnimClipsList.selection]
							thisSelected=thisSkelet.saveAnimIDs.Count
						)
						else (
							messageBox("Cannot assign this clip, because the clip-name is allready used.")
						)
					)
					else if targetAnimClipsList.selection>0 then (
						test=appendIfUnique thisSkelet.saveAnimClipNames newClipsNames[sourceAnimClipsList.selection]
						if test then (
							updateThis=true
							thisSkelet.saveAnimClipNames  = deleteItem thisSkelet.saveAnimClipNames  thisSkelet.saveAnimClipNames.Count
							insertItem newClipsSourceIDs[sourceAnimClipsList.selection] thisSkelet.saveAnimSourceIDs  targetAnimClipsList.selection
							insertItem newClipsIDs[sourceAnimClipsList.selection] thisSkelet.saveAnimIDs  targetAnimClipsList.selection
							insertItem newClipsNames[sourceAnimClipsList.selection] thisSkelet.saveAnimClipNames targetAnimClipsList.selection
							thisSelected=targetAnimClipsList.selection
						)
						else (
							messageBox("Cannot assign this clip, because the clip-name is allready used.")
						)
					)
					if(updateThis) then (
						global animSetClipsDisplay=#()
						thisCnt=1
						for animSourceID in thisSkelet.saveAnimSourceIDs do(
							if(thisSkelet.saveAnimClipNames[thisCnt]!=undefined) then (
								thisString=thisCnt as string +" "+ thisSkelet.saveAnimClipNames[thisCnt]
								thisCnt+=1
								append animSetClipsDisplay thisString
							)
						)
						animSet_rollout.targetAnimClipsList.items=animSetClipsDisplay		
						if thisSelected>0 and thisSelected<=animSetClipsDisplay.Count then animSet_rollout.targetAnimClipsList.selection=thisSelected
						btn_removeClip.enabled=true
						if targetAnimClipsList.items.count == 0 then (
							btn_removeClip.enabled=false
						)
					)
				)
				
				on targetAnimClipsList selected thisSelectIdx do(
-- 					lbl_maxJPV_isIdentityMatrix.visible=false
-- 					lbl_maxJPV_isIdentityMatrix3.visible=false
-- 					lbl_maxJPV_isIdentityMatrix4.visible=false
-- 					btn_fixBindPose.visible=false
-- 					selectedIdx=1
-- 					if targetAnimClipsList.selection>0 then selectedIdx=targetAnimClipsList.selection
-- 					if selectedIdx<=newSkinnedObjs.Count then(
-- 						if newSkinnedObjsInfos[selectedIdx].Count==3 then (
-- 							if not newSkinnedObjsInfos[selectedIdx][2] then (
-- 								lbl_maxJPV_isIdentityMatrix.visible=true
-- 								lbl_maxJPV_isIdentityMatrix3.visible=true
-- 								lbl_maxJPV_isIdentityMatrix4.visible=true
-- 								btn_fixBindPose.visible=true
-- 							)
-- 							lbl_used_skinned.text= "joints used: "+newSkinnedObjsInfos[selectedIdx][1] as string
-- 							lbl_maxJPV_skinned.text= "max joints per vert: "+newSkinnedObjsInfos[selectedIdx][3] as string
-- 						)
-- 					)
				)
				on animSet_rollout open do(
					
					btn_removeClip.enabled=true
					if targetAnimClipsList.items.count == 0 then (
						btn_removeClip.enabled=false
					)
-- 					lbl_maxJPV_isIdentityMatrix.visible=false
-- 					lbl_maxJPV_isIdentityMatrix3.visible=false
-- 					lbl_maxJPV_isIdentityMatrix4.visible=false
-- 					btn_fixBindPose.visible=false
-- 					selectedIdx=1
-- 					if targetAnimClipsList.selection>0 then selectedIdx=targetAnimClipsList.selection
-- 					if selectedIdx<=newSkinnedObjs.Count then(
-- 						if newSkinnedObjsInfos[selectedIdx].Count==3 then (
-- 							if not newSkinnedObjsInfos[selectedIdx][2] then (
-- 								lbl_maxJPV_isIdentityMatrix.visible=true
-- 								lbl_maxJPV_isIdentityMatrix3.visible=true
-- 								lbl_maxJPV_isIdentityMatrix4.visible=true
-- 								btn_fixBindPose.visible=true
-- 							)
-- 							lbl_used_skinned.text= "joints used: "+newSkinnedObjsInfos[selectedIdx][1] as string
-- 							lbl_maxJPV_skinned.text= "max joints per vert: "+newSkinnedObjsInfos[selectedIdx][3] as string
-- 						)
-- 					)
				)
		)
		createDialog animSet_rollout  width:630 height:310 style:#(#style_titlebar, #style_border, #style_sysmenu, #style_minimizebox)
		if dialogPosition!=[0,0] then SetDialogPos animSet_rollout dialogPosition		
		animSet_rollout.sourceAnimClipsList.items=newClipsDisplay			
		global animSetClipsDisplay=#()
		cnt=1
		newNames=#()
		newIDs=#()
		newSourceIDs=#()
		for animSourceID in thisSkelet.saveAnimSourceIDs do(
			if thisSkelet.saveAnimClipNames[cnt]!=undefined and thisSkelet.saveAnimIDs[cnt]!=undefined and thisSkelet.saveAnimSourceIDs[cnt]!=undefined then (
				thisString=cnt as string +" "+ thisSkelet.saveAnimClipNames[cnt]
				append animSetClipsDisplay thisString
				append newNames thisSkelet.saveAnimClipNames[cnt]
				append newIDs thisSkelet.saveAnimIDs[cnt]
				append newSourceIDs thisSkelet.saveAnimSourceIDs[cnt]
				cnt+=1
			)
		)
		thisSkelet.saveAnimSourceIDs  = newSourceIDs		
		thisSkelet.saveAnimIDs  = newIDs		
		thisSkelet.saveAnimClipNames  = newNames 
		thisSelected=1
		global animSet_rollout
		animSet_rollout.targetAnimClipsList.items=animSetClipsDisplay		
		if thisSelected>0 and thisSelected<=animSetClipsDisplay.Count then animSet_rollout.targetAnimClipsList.selection=thisSelected
		if animSet_rollout.targetAnimClipsList.items.count > 0 then (
			animSet_rollout.btn_removeClip.enabled=true
		)
		--messageBox(newStringList)
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
		
		saveAnimClipNames type:#stringTab tabSize:0 tabSizeVariable:true
		saveAnimSourceIDs type:#stringTab tabSize:0 tabSizeVariable:true
		saveAnimIDs type:#stringTab tabSize:0 tabSizeVariable:true
	)
	rollout params "AWD Animation Set"
	(		
		label lbl_AnimType "Anim-Type:" width:65 pos:[10,13]
		dropDownList rdoGrp_animType "" items:#("Skeleton", "Vertex", "UV") width:80 pos:[70,10]
		label lbl_1 "" height:2 
		label lbl_animSource "Skeleton Animation Source"
		editText  txt_source "" readOnly:true width:130 
		button btn_newSource "New"   width:62 across:2
		button btn_addSource "Choose"   width:62 
		button btn_selectSource "Select"  width:62 across:2
		button btn_clearSource"Clear"   width:62
		label lbl_2 "" height:2 
		checkBox cbx_simpleMode "Use all available clips" enabled:true
		button btn_OpenAnimClipManager "Open Anim-Set Manager" width:130 
		
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
			txt_source.visible=true
			btn_newSource.visible=true
			btn_addSource.visible=true
			btn_selectSource.visible=true
			btn_clearSource.visible=true
			lbl_animSource.visible=true
 			if thisState==1 then(
				lbl_animSource.text="Skeleton Animation Source"
 				if foundSkel==0 then btn_newSource.enabled=true
				if sourceSkeletonID != "noIDSet" then(
					isFound=false
					for i in objects do(
						for m in i.modifiers do(
							if classof m as string=="AWDSkeleton" then (
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
			else if thisState==2 then(
				lbl_animSource.text="Vertex Animation Source"
				if foundVertex==0 then btn_newSource.enabled=true
 				if foundVertex==0 then btn_newSource.enabled=true
				if sourceVertexID != "noIDSet" then(
					isFound=false
					for i in objects do(
						for m in i.modifiers do(
							if classof m as string=="AWDVertexAnimSource" then (
								if m.thisAWDID==sourceVertexID then (
									isFound=true	
									sourceVertexID=m.thisAWDID
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
			else if thisState==3 then(
				txt_source.visible=false
				btn_newSource.visible=false
				btn_addSource.visible=false
				btn_selectSource.visible=false
				btn_clearSource.visible=false
				lbl_animSource.visible=false
			)
				
			if txt_source.text=="" then (
				btn_selectSource.enabled=false
				btn_clearSource.enabled=false
			)
		)
		
		on btn_OpenAnimClipManager pressed do(
			curMod=modPanel.GetCurrentObject()
			openAWDSetManager curMod
		)
		
		on params open do (
			btn_OpenAnimClipManager.enabled=true
			if simpleMode then btn_OpenAnimClipManager.enabled=false
				
			btn_newSource.enabled=false
			btn_selectSource.enabled=true
			btn_clearSource.enabled=true
			txt_source.visible=true
			btn_newSource.visible=true
			btn_addSource.visible=true
			btn_selectSource.visible=true
			btn_clearSource.visible=true
			lbl_animSource.visible=true
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
							if classof m as string=="AWDVertexAnimSource" then (
								if m.thisAWDID==sourceVertexID then (
									isFound=true				
									sourceVertexID=m.thisAWDID
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
			else if animType==3 then (
				txt_source.visible=true
				btn_newSource.visible=true
				btn_addSource.visible=true
				btn_selectSource.visible=true
				btn_clearSource.visible=true
				lbl_animSource.visible=true
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
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
			thisAWDID=getAWDID()
		)
	)
	on clone orig do (	
		fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
		thisAWDID=getAWDID()		
	)

)