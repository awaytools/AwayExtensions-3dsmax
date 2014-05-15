plugin modifier AWDAnimator
name:"AWDAnimator"
category:"AWDModifier"
classID:#(0x2b403bbe, 0x18c06aba)
extends:EmptyModifier replaceUI:false version:1
(
	-- choose a new AWDAnimator as target Animator for a AWDObjectSettings - called from AWDObjectSettings
	fn chooseAnimationSet thisObj thisModifier animationSetID = (	
		--global to allow to access vars from dialog-rollout.		
				
		global animDisplayNames=#()
		global animMods=#()	
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		--search for all AWDAnimationSets in the scene
		meshes = $*
		if meshes!=undefined then (
			for i in meshes do(
				for m in i.modifiers do(
					if (classof m as string)=="AWDAnimationSet" then(	
						if m.thisAWDID==animationSetID then originalIdx=animMods.count+1							
						appendThis= (appendIfUnique animMods m)
						if appendThis then(
							append animDisplayNames ("'"+m.name+"'  object: "+i.name) 
							
						)	
					)
				)
			)
		)
		if animMods.count>0 then(
			rollout select_animationset_rollout "Select a AWDAnimationSet Modifier"	(
				listbox base_name "AWDAnimationSet-Modifier found in scene"  height:16 items:animDisplayNames selection:originalIdx
				on base_name selected selected do (
					thisMod.AnimationSetID=animMods[base_name.selection].thisAWDID					
					append animMods[base_name.selection].saveAnimators thisMod
					animMods[base_name.selection].saveSelectedAnimator=animMods[base_name.selection].saveAnimators.count
					thisMod.params.btn_SelectAnimSet.enabled=true
					thisMod.params.btn_ClearAnimSet.enabled=true	
					
					DestroyDialog select_animationset_rollout
				)
			)
			--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
			createDialog select_animationset_rollout 300 240  modal:true
			
		)
		else	messagebox("No AWDAnimationSet-modifier found in the scene")	
	)

	fn selectAnimationSet thisModifier targetAnimationSetID = (						
		if targetAnimationSetID=="noIDSet" then (
			messagebox("No AWDAnimationSet-modifier has been assigned.")
		)
		else (
			isFound=false
			for i in objects do(
				for m in i.modifiers do(
					if (classof  m as string)=="AWDAnimationSet" then(
						if m.thisAWDID==targetAnimationSetID then (
							m.saveSelectedAnimator=0
							cnt=1
							for oneAnimator in m.saveAnimators do(
								if oneAnimator == thisModifier then(
									m.saveSelectedAnimator=cnt
									exit
								)
								cnt+=1
							)
							if m.saveSelectedAnimator==0 then(
								append m.saveAnimators m
								m.saveSelectedAnimator=m.saveAnimators.count
							)
							select i		
							modPanel.setCurrentObject  m		
							isFound=true
							exit 
						)
					)		
				)
				if isFound then exit
			)
			if not isFound then(
				messagebox("ERROR: Could not find the assigned AWDAnimationSet-modifier!")			
			)
		)
	)			
	parameters main rollout:params
	(
		thisAWDID type:#string default:"noIDSet"
		AnimationSetID type:#string default:"noIDSet"
		saveTargetMeshes type:#maxObjectTab tabSize:0 tabSizeVariable:true
		createUnique type:#boolean ui:CBX_createUnique default:true animatable:false
		--nodeTab
	)
	rollout params "AWD Animator"
	(
		group "AWD Animation Set" (
			edittext animSetName "" readOnly:true
			button btn_newAnimSet "New"   width:60 across:2
			button btn_ChooseAnimSet "Choose"   width:60 
			button btn_SelectAnimSet "Select"  width:60 across:2
			button btn_ClearAnimSet"Clear"   width:60
		)
		checkBox CBX_createUnique "Always create unique"
		label spc_1 ""
		group "Animator Helper"(			
			button btn_selectAssignedMeshes "Select animated meshes" width:140
		)
		on btn_newAnimSet pressed do (
			spc_1.text="dsfdsfs"
			curAnimatorMod=modPanel.getCurrentObject()
			MyNewModifier = AWDAnimationSet()
			MyNewModifier.name = "AWD Anim Set"
			MyNewModifier.oldName = "AWD Anim Set" 
			addmodifier  selection[1] MyNewModifier
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
			if MyNewModifier.thisAWDID =="noAWDset" then MyNewModifier.thisAWDID=getAWDID()
			AnimationSetID=MyNewModifier.thisAWDID
			append MyNewModifier.saveAnimators curAnimatorMod
			modPanel.setCurrentObject selection[1]
			modPanel.setCurrentObject curAnimatorMod
		)
		on btn_ChooseAnimSet pressed do (
			curAnimatorMod=modPanel.getCurrentObject()
			chooseAnimationSet selection[1] curAnimatorMod AnimationSetID
		)
		on btn_SelectAnimSet pressed do (
			btn_SelectAnimSet.enabled=true
			btn_ClearAnimSet.enabled=true
			curAnimatorMod=modPanel.getCurrentObject()
			selectAnimationSet   curAnimatorMod AnimationSetID
			if AnimationSetID=="noIDSet" then(
				animSetName.text=""
				btn_SelectAnimSet.enabled=false
				btn_ClearAnimSet.enabled=false
			)
		)
		on btn_ClearAnimSet pressed do (	
			animSetName.text=""
			AnimationSetID="noIDSet"
			btn_SelectAnimSet.enabled=false
			btn_ClearAnimSet.enabled=false
		)
		
		on btn_selectAssignedMeshes pressed do(
			curAnimatorMod=modPanel.getCurrentObject()
			meshes=$*
			foundMeshes=#()
			for i in meshes do(				
				thisClassStr=classof i as string
				if thisClassStr=="Editable_mesh" or thisClassStr=="PolyMeshObject" or thisClassStr=="Editable_Poly" then (
					for m in i.modifiers do (
						if (classof m as string) == "AWDAnimator" then (
							if m==curAnimatorMod then	appendIfUnique foundMeshes i
						)
					)
				)
			)
			select foundMeshes
			if foundMeshes.count==1 then (				
			 modPanel.setCurrentObject foundMeshes[1]
			)
		)
		on params open do (
			--check the target AnimationSet	
			print ("AnimationSetID = "+AnimationSetID as string)
			btn_SelectAnimSet.enabled=true
			btn_ClearAnimSet.enabled=true
			if AnimationSetID!="noIDSet" then(
 				foundAnimSet=false
 				for m in selection[1].modifiers do(
 					if classof m as string == "AWDAnimationSet" then (
 						if m.thisAWDID==AnimationSetID then (	
 							foundAnimSet=true
 							animSetName.text=m.name
 							exit										
 						)
 					)
 				)
 				if not foundAnimSet then (
 					allMeshes=$*
 					for i in allMeshes do(
 						for m in i.modifiers do(
 							if classof m as string == "AWDAnimationSet" then (
								if m.thisAWDID==AnimationSetID then (	
 									foundAnimSet=true
 									AnimationSetName=m.name
 									exit										
 								)
 							)
 						)
 						if foundAnimSet then exit
 					)
 				)
 				if not foundAnimSet then (
 					AnimationSetName=""
 					AnimationSetID="noIDSet"
 					btn_SelectAnimSet.enabled=false
 					btn_ClearAnimSet.enabled=false
 				)
 			)
 			else(
 				AnimationSetName=""
 				btn_SelectAnimSet.enabled=false
 				btn_ClearAnimSet.enabled=false
 			)
			
		
			
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
	on create do (
		if thisAWDID=="noIDSet" then(
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
			thisAWDID=getAWDID()
		)
	)
	on clone orig do (
		--for m in thisObj.modifiers do	if (classof(m) as string)=="AWDAnimator" then exists=true
		--if exists then messagebox("A object can only have on AWDAnimator-Modifier applied.\n\This object allready contains a AWDModifier!")
		fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
		thisAWDID=getAWDID()		
	)
	-- doesnt seam to work:
	on deleted do (
		--messagebox("find all the objects that are using this animator, and delete the references")
	)
	-- doesnt seam to work:
	on isEnabled do (print "yes, is enabled")
	-- doesnt seam to work:
	on detachedFromNode thisnode do (
		--messagebox("find all the objects that are using this animator, and delete the references")
	)
)