plugin modifier AWDVertexAnimSourceClone
name:"AWDVertexAnimSourceClone"
category:"AWDModifier"
classID:#(0x7e8f1ceb, 0x24bb35d2)
extends:EmptyModifier replaceUI:false version:1
(	
	
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
					thisMod.targetRollout.btn_select.enabled=false
					thisMod.targetRollout.btn_clear.enabled=false
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
	
	
	parameters targetParams rollout:targetRollout
	(
		thisAWDID type:#string default:"noIDSet"
		jointCnt type:#string default:"valid: no target set" animatable:false
		target_ID type:#string animatable:false default:"noIDSet"
		target_Name type:#string animatable:false ui:txt_TargetSkel
		--export_mesh type:#boolean default:false animatable:false ui:cbx_export_mesh
		--auto_create_animator type:#boolean default:false animatable:false ui:cbx_auto_create_animator
		
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
		--checkbox cbx_export_mesh "Export Mesh" animatable:false
		--checkbox cbx_auto_create_animator "Auto create Anim-Setup" animatable:false
		
-- 		on cbx_export_mesh changed thisValue do(
-- 			cbx_auto_create_animator.enabled=false
-- 			if export_mesh then cbx_auto_create_animator.enabled=true
-- 		)
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
			btn_select.enabled=false
			btn_clear.enabled=false
		)
		on targetRollout open do (
			btn_select.enabled=false
			btn_clear.enabled=false
			if target_ID!="noIDSet" then (
				btn_select.enabled=true
				btn_clear.enabled=true
			)
-- 			cbx_auto_create_animator.enabled=false
-- 			if export_mesh then cbx_auto_create_animator.enabled=true
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